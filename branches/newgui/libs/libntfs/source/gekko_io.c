/**
 * gekko_io.c - Gekko style disk io functions.
 *
 * Copyright (c) 2009 Rhys "Shareese" Koedijk
 *
 * This program/include file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program/include file is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_MATH_H
#include <math.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include "ntfs.h"
#include "types.h"
#include "logging.h"
#include "gekko_io.h"
#include "device.h"
#include "bootsect.h"

#define DEV_FD(dev) ((gekko_fd *)dev->d_private)

/* Prototypes */
static s64 ntfs_device_gekko_io_readraw(struct ntfs_device *dev, s64 offset, s64 count, void *buf);
static s64 ntfs_device_gekko_io_writeraw(struct ntfs_device *dev, s64 offset, s64 count, const void *buf);

/**
 *
 */
static int ntfs_device_gekko_io_open(struct ntfs_device *dev, int flags)
{
    ntfs_log_trace("flags %i\n", flags);

    // Get the device driver descriptor
    gekko_fd *fd = DEV_FD(dev);
    if (!fd) {
        errno = EBADF;
        return -1;
    }
    
    // Get the device interface
    const DISC_INTERFACE* interface = fd->interface;
    if (!interface) {
        errno = ENODEV;
        return -1;
    }
    
    // Start the device interface and ensure that it is inserted
    if (!interface->startup()) {
        ntfs_log_perror("device failed to start\n");
        errno = EIO;
        return -1;
    }
    if (!interface->isInserted()) {
        ntfs_log_perror("device media is not inserted\n");
        interface->shutdown();
        errno = EIO;
        return -1;
    }
    
    // Check that the device isn't already open (used by another volume?)
    if (NDevOpen(dev)) {
        ntfs_log_perror("device is busy (already open)\n");
        errno = EBUSY;
        return -1;
    }

    // Check that there is a valid NTFS boot sector at the start of the device
    NTFS_BOOT_SECTOR boot;
    if (interface->readSectors(fd->startSector, 1, &boot)) {
        if (!ntfs_boot_sector_is_ntfs(&boot)) {
            interface->shutdown();
            errno = EINVALPART;
            return -1;
        }
    } else {
        ntfs_log_perror("read failure @ sector %d\n", fd->startSector);
        interface->shutdown();
        errno = EIO;
        return -1;
    }
    
    // Parse the boot sector
    fd->sectorSize = le16_to_cpu(boot.bpb.bytes_per_sector);
    fd->sectorCount = sle64_to_cpu(boot.number_of_sectors);
    fd->pos = 0;
    fd->len = (fd->sectorCount * fd->sectorSize);
    fd->ino = le64_to_cpu(boot.volume_serial_number);
    
    // If the device sector size is not 512 bytes we cannot continue,
    // gekko disc I/O works on the assumption that sectors are always 512 bytes long.
    // TODO: Implement support for non-512 byte sector sizes through some fancy maths!?
    if (fd->sectorSize != SECTOR_SIZE) {
        ntfs_log_error("Boot sector claims there is %i bytes per sector; expected %i\n", fd->sectorSize, SECTOR_SIZE);
        interface->shutdown();
        errno = EIO;
        return -1;
    }

    // Initialise the device lock
    LWP_MutexInit(&fd->lock, false);
    
    // Mark the device as read-only (if required)
    //if (flags & O_RDWR) {
    //    NDevSetReadOnly(dev);
    //}
    
    // Mark the device as open
    NDevSetBlock(dev);
    NDevSetOpen(dev);
    
    return 0;
}

/**
 *
 */
static int ntfs_device_gekko_io_close(struct ntfs_device *dev)
{
    ntfs_log_trace("\n");
    
    // Get the device driver descriptor
    gekko_fd *fd = DEV_FD(dev);
    if (!fd) {
        errno = EBADF;
        return -1;
    }

    // Check that the device is actually open
    if (!NDevOpen(dev)) {
        ntfs_log_perror("device is not open\n");
        errno = EIO;
        return -1;
    }
    
    // Flush the device (if dirty and not read-only)
    if (NDevDirty(dev) && !NDevReadOnly(dev)) {
        ntfs_log_debug("device is dirty, will now sync\n");

        // TODO: This, if/when a cache system is implemented
        
        // Mark the device as clean
        NDevClearDirty(dev);
        
    }
    
    // Deinitialise the device lock
    LWP_MutexDestroy(fd->lock);
    
    // Shutdown the device interface
    const DISC_INTERFACE* interface = fd->interface;
    if (interface) {
        interface->shutdown();
    }
    
    // Mark the device as closed
    NDevClearBlock(dev);
    NDevClearOpen(dev);
    
    // Free the device driver private data
    ntfs_free(dev->d_private);
    dev->d_private = NULL;
    
    return 0;
}

/**
 *
 */
static s64 ntfs_device_gekko_io_seek(struct ntfs_device *dev, s64 offset, int whence)
{
    ntfs_log_trace("offset %Li, whence %i\n", offset, whence);
    
    // Get the device driver descriptor
    gekko_fd *fd = DEV_FD(dev);
    if (!fd) {
        errno = EBADF;
        return -1;
    }
    
    // Set the current position on the device (in bytes)
    switch(whence) {
        case SEEK_SET: fd->pos = MIN(MAX(offset, 0), fd->len); break;
        case SEEK_CUR: fd->pos = MIN(MAX(fd->pos + offset, 0), fd->len); break;
        case SEEK_END: fd->pos = MIN(MAX(fd->len - offset, 0), fd->len); break;
    }

    return 0;
}

/**
 *
 */
static s64 ntfs_device_gekko_io_read(struct ntfs_device *dev, void *buf, s64 count)
{
    return ntfs_device_gekko_io_readraw(dev, DEV_FD(dev)->pos, count, buf);
}

/**
 *
 */
static s64 ntfs_device_gekko_io_write(struct ntfs_device *dev, const void *buf, s64 count)
{
    return ntfs_device_gekko_io_writeraw(dev, DEV_FD(dev)->pos, count, buf);
}

/**
 *
 */
static s64 ntfs_device_gekko_io_pread(struct ntfs_device *dev, void *buf, s64 count, s64 offset)
{
    return ntfs_device_gekko_io_readraw(dev, offset, count, buf);
}

/**
 * 
 */
static s64 ntfs_device_gekko_io_pwrite(struct ntfs_device *dev, const void *buf, s64 count, s64 offset)
{
    return ntfs_device_gekko_io_writeraw(dev, offset, count, buf);
}

/**
 *
 */
static s64 ntfs_device_gekko_io_readraw(struct ntfs_device *dev, s64 offset, s64 count, void *buf)
{
    ntfs_log_trace("offset %Li, count %Li\n", offset, count);
    
    // Get the device driver descriptor
    gekko_fd *fd = DEV_FD(dev);
    if (!fd) {
        errno = EBADF;
        return -1;
    }
    
    // Get the device interface
    const DISC_INTERFACE* interface = fd->interface;
    if (!interface) {
        errno = ENODEV;
        return -1;
    }
    
    sec_t sec_start = fd->startSector;
    sec_t sec_count = 1;
    u16 buffer_offset = 0;
    u8 *buffer;
    
    // Determine the range of sectors required for this read
    if (offset > 0) {
        sec_start += floor(offset / fd->sectorSize);
        buffer_offset = offset % fd->sectorSize;
    }
    if (count > fd->sectorSize) {
        sec_count = ceil(count / fd->sectorSize);
    }
    
    // If this read happens to be on the sector boundaries then do the read straight into the destination buffer
    if((offset % fd->sectorSize == 0) && (count % fd->sectorSize == 0)) {
        
        // Read from the device
        ntfs_log_debug("direct read from sector %d (%d sector(s) long)\n", sec_start, sec_count);
        if (!interface->readSectors(sec_start, sec_count, buf)) {
            ntfs_log_perror("direct read failure @ sector %d (%d sector(s) long)\n", sec_start, sec_count);
            errno = EIO;
            return -1;
        }
        
    // Else read into a buffer and copy over only what was requested
    } else {
        
        // Allocate a buffer to hold the read data
        buffer = (u8*)ntfs_alloc(sec_count * fd->sectorSize);
        if (!buffer) {
            errno = ENOMEM;
            return -1;
        }
        
        // Read from the device
        ntfs_log_debug("buffered read from sector %d (%d sector(s) long)\n", sec_start, sec_count);
        if (!interface->readSectors(sec_start, sec_count, buffer)) {
            ntfs_log_perror("buffered read failure @ sector %d (%d sector(s) long)\n", sec_start, sec_count);
            ntfs_free(buffer);
            errno = EIO;
            return -1;
        }
        
        // Copy what was requested to the destination buffer
        memcpy(buf, buffer + buffer_offset, count);
        ntfs_free(buffer);
        
    }
    
    return count;
}

/**
 * 
 */
static s64 ntfs_device_gekko_io_writeraw(struct ntfs_device *dev, s64 offset, s64 count, const void *buf)
{
    ntfs_log_trace("offset %Li, count %Li\n", offset, count);
    
    // Get the device driver descriptor
    gekko_fd *fd = DEV_FD(dev);
    if (!fd) {
        errno = EBADF;
        return -1;
    }
    
    // Get the device interface
    const DISC_INTERFACE* interface = fd->interface;
    if (!interface) {
        errno = ENODEV;
        return -1;
    }
    
    // Check that the device can be written to
    if (NDevReadOnly(dev)) {
        errno = EROFS;
        return -1;
    }
    
    sec_t sec_start = fd->startSector;
    sec_t sec_count = 1;
    u16 buffer_offset = 0;
    u8 *buffer;
    
    // Determine the range of sectors required for this write
    if (offset > 0) {
        sec_start += floor(offset / fd->sectorSize);
        buffer_offset = offset % fd->sectorSize;
    }
    if (count > fd->sectorSize) {
        sec_count = ceil(count / fd->sectorSize);
    }
    
    // If this write happens to be on the sector boundaries then do the write straight to disc
    if((offset % fd->sectorSize == 0) && (count % fd->sectorSize == 0)) {
        
        // Write to the device
        ntfs_log_debug("direct write to sector %d (%d sector(s) long)\n", sec_start, sec_count);
        if (!interface->writeSectors(sec_start, sec_count, buf)) {
            ntfs_log_perror("direct write failure @ sector %d (%d sector(s) long)\n", sec_start, sec_count);
            errno = EIO;
            return -1;
        }
        
    // Else write from a buffer aligned to the sector boundaries
    } else {
        
        // Allocate a buffer to hold the write data
        buffer = (u8*)ntfs_alloc(sec_count * fd->sectorSize);
        if (!buffer) {
            errno = ENOMEM;
            return -1;
        }
        
        // Read the first and last sectors of the buffer from disc (if required)
        // NOTE: This is done because the data does not line up with the sector boundaries, 
        //       we just read in the buffer edges where the data overlaps with the rest of the disc
        if((offset % fd->sectorSize == 0)) {
            if (!interface->readSectors(sec_start, 1, buffer)) {
                ntfs_log_perror("read failure @ sector %d\n", sec_start);
                ntfs_free(buffer);
                errno = EIO;
                return -1;
            }
        }
        if((count % fd->sectorSize == 0)) {
            if (!interface->readSectors(sec_start + sec_count, 1, buffer + ((sec_count - 1) * fd->sectorSize))) {
                ntfs_log_perror("read failure @ sector %d\n", sec_start + sec_count);
                ntfs_free(buffer);
                errno = EIO;
                return -1;
            }
        }
        
        // Copy the data into the write buffer
        memcpy(buffer + buffer_offset, buf, count);
        
        // Write to the device
        ntfs_log_debug("buffered write to sector %d (%d sector(s) long)\n", sec_start, sec_count);
        if (!interface->writeSectors(sec_start, sec_count, buffer)) {
            ntfs_log_perror("buffered write failure @ sector %d\n", sec_start);
            ntfs_free(buffer);
            errno = EIO;
            return -1;
        }

        // Free the buffer
        ntfs_free(buffer);
        
    }
    
    // Mark the device as dirty
    NDevSetDirty(dev);
    
    return count;
}

/**
 *
 */
static int ntfs_device_gekko_io_sync(struct ntfs_device *dev)
{
    ntfs_log_trace("\n");
    
    // Check that the device can be written to
    if (NDevReadOnly(dev)) {
        errno = EROFS;
        return -1;
    }
    
    // TODO: This, if/when a cache system is implemented
    
    // Mark the device as clean
    NDevClearDirty(dev);
    
    return 0;
}

/**
 *
 */
static int ntfs_device_gekko_io_stat(struct ntfs_device *dev, struct stat *buf)
{
    ntfs_log_trace("\n");
    
    // Get the device driver descriptor
    gekko_fd *fd = DEV_FD(dev);
    if (!fd) {
        errno = EBADF;
        return -1;
    }
    
    // Build the device mode
    mode_t mode = (S_IFBLK) |
                  (S_IRUSR | S_IRGRP | S_IROTH) |
                  ((!NDevReadOnly(dev)) ? (S_IWUSR | S_IWGRP | S_IWOTH) : 0) |
                  (fd->uid ? S_ISUID : 0) |
                  (fd->gid ? S_ISGID : 0);

    // Zero out the stat buffer
    //memset(buf, 0, sizeof(struct stat));

    // Build the device stats
    buf->st_dev = fd->interface->ioType;
    buf->st_ino = fd->ino;
    buf->st_mode = mode;
    buf->st_uid = fd->uid;
    buf->st_gid = fd->gid;
    buf->st_rdev = fd->interface->ioType;
    buf->st_blksize = fd->sectorSize;
    buf->st_blocks = fd->sectorCount;
    
    return 0;
}

/**
 *
 */
static int ntfs_device_gekko_io_ioctl(struct ntfs_device *dev, int request, void *argp)
{
    ntfs_log_trace("request %i\n", request);
    
    // Operation not supported, fail silently
    
    return 0;
}

/**
 * Device operations for working with gekko style devices and files.
 */
struct ntfs_device_operations ntfs_device_gekko_io_ops = {
    .open       = ntfs_device_gekko_io_open,
    .close      = ntfs_device_gekko_io_close,
    .seek       = ntfs_device_gekko_io_seek,
    .read       = ntfs_device_gekko_io_read,
    .write      = ntfs_device_gekko_io_write,
    .pread      = ntfs_device_gekko_io_pread,
    .pwrite     = ntfs_device_gekko_io_pwrite,
    .sync       = ntfs_device_gekko_io_sync,
    .stat       = ntfs_device_gekko_io_stat,
    .ioctl      = ntfs_device_gekko_io_ioctl,
};