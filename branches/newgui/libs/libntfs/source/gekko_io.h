/*
* gekko_io.h - Platform specifics for device io.
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

#ifndef _GEKKO_IO_H
#define _GEKKO_IO_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "types.h"
#include <gccore.h>
#include <ogc/disc_io.h>

#define SECTOR_SIZE     512

/**
 * gekko_fd - Gekko device driver descriptor
 */
typedef struct _gekko_fd {
    const DISC_INTERFACE* interface;
    sec_t startSector;
    sec_t hiddenSectors;
    u16 sectorSize;
    u64 sectorCount;
    u64 pos;
    u64 len;
    ino_t ino;
} gekko_fd;

/* Forward declaration. */
struct ntfs_device_operations;

extern struct ntfs_device_operations ntfs_device_gekko_io_ops;

#endif /* _GEKKO_IO_H */
