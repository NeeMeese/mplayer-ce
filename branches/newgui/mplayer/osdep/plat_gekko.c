/*
   MPlayer Wii port

   Copyright (C) 2008 dhewg

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the
   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301 USA.

   Improved by MplayerCE Team
*/


#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ogcsys.h>
#include <ogc/lwp_watchdog.h>
#include <ogc/lwp.h>
#include <debug.h>
#include <wiiuse/wpad.h>

#include <fat.h>
#include <smb.h>

#include <network.h>
#include <errno.h>
#include <di/di.h>
#include "libdvdiso.h"


#include "log_console.h"
#include "gx_supp.h"
#include "plat_gekko.h"

#include "mload.h"
#include "ehcmodule_elf.h"

#include "../m_option.h"
#include "../parser-cfg.h"
#include "../get_path.h"

#undef abort

#define MPCE_VERSION "0.63"

extern int stream_cache_size;

//bool reset_pressed = false;
//bool power_pressed = false;
//bool playing_usb = false;
bool playing_dvd = false;
//int network_inited = 0;
//int mounting_usb=0;
static bool dvd_mounted = false;
static bool dvd_mounting = false;
static int dbg_network = false;
static int component_fix = false;
static float gxzoom=358;
static float hor_pos=3;
static float vert_pos=0;
static float stretch=0;
static bool exit_automount_thread = false;
static bool usb_init=false;

#define CE_DEBUG 1

struct SMBSettings {
	char	ip[16];
	char	share[20];
	char	user[20];
	char	pwd[20];
};

struct SMBSettings smbConf[5];

/*
static char *default_args[] = {
	"sd:/apps/mplayer_ce/mplayer.dol",
	"-bgvideo", NULL,
	"-idle", NULL,
#ifndef CE_DEBUG
	"-really-quiet",
#endif
	"-vo","gekko","-ao","gekko",
	"-menu","-menu-startup"
	//,"sd:/thriller.mp4"
};

static void reset_cb (void) {
	reset_pressed = true;
}

static void power_cb (void) {
	power_pressed = true;
}

static void wpad_power_cb (void) {
	power_pressed = true;
}

#include <sys/time.h>
#include <sys/timeb.h>
void gekko_gettimeofday(struct timeval *tv, void *tz) {
//	u64 us = ticks_to_microsecs(gettime());

//	tv->tv_sec = us / TB_USPERSEC;
//	tv->tv_usec = us % TB_USPERSEC;
	u64 t;
	t=gettime();
	tv->tv_sec = ticks_to_secs(t);
	tv->tv_usec = ticks_to_microsecs(t);


}

void gekko_abort(void) {
	//printf("abort() called\n");
	plat_deinit(-1);
	exit(-1);
}

void __dec(char *cad){int i;for(i=0;cad[i]!='\0';i++)cad[i]=cad[i]-50;}
void sp(){sleep(5);}

void trysmb();

// Mounting code
#include <sdcard/wiisd_io.h>
#include <sdcard/gcsd.h>
#include <ogc/usbstorage.h>

const DISC_INTERFACE* sd_mp = &__io_wiisd;
const DISC_INTERFACE* usb_mp = &__io_usbstorage;

static lwp_t mainthread;

static s32 initialise_network()
{
    s32 result;
    int cnt=0;
    while ((result = net_init()) == -EAGAIN)
	{
		usleep(500);
		printf("err net\n");
	}
    return result;
}

int wait_for_network_initialisation()
{
	if(network_inited) return 1;

	while(1)
	{
	    if (initialise_network() >= 0) {
	        char myIP[16];
	        if (if_config(myIP, NULL, NULL, true) < 0)
			{
			  if(dbg_network)
			  {
			  	printf("Error getting ip\n");
			  	return 0;
			  }
			  sleep(5);
			  continue;
			}
	        else
			{
			  network_inited = 1;
			  if(dbg_network) printf("Network initialized. IP: %s\n",myIP);
			  return 1;
			}
	    }
	    if(dbg_network)
		{
			printf("Error initializing network\n");
			return 0;
		}
		sleep(10);
    }

	return 0;
}
*/

bool DVDGekkoMount()
{
	if(playing_dvd) return true;
		  set_osd_msg(1, 1, 10000, "Mounting DVD, please wait");
	  force_osd();

	//if(dvd_mounted) return true;
	dvd_mounting=true;
	if(WIIDVD_DiscPresent())
	{
		int ret;
		//printf("WIIDVD_Unmount\n");
		WIIDVD_Unmount();
		//printf("WIIDVD_mount\n");
		ret = WIIDVD_Mount();
		dvd_mounted=true;
		dvd_mounting=false;
		if(ret==-1) return false;
		return true;
	}
	dvd_mounting=false;
	dvd_mounted=false;
	return false;
}
/*
static void * networkthreadfunc (void *arg)
{

	while(1){
	if(wait_for_network_initialisation())
	{
		trysmb();
		break;
	}
	sleep(10);
	}
	LWP_JoinThread(mainthread,NULL);
	return NULL;
}

#include <sys/iosupport.h>
bool DeviceMounted(const char *device)
{
  devoptab_t *devops;
  int i,len;
  char *buf;

  len = strlen(device);
  buf=(char*)malloc(sizeof(char)*len+2);
  strcpy(buf,device);
  if ( buf[len-1] != ':')
  {
    buf[len]=':';
    buf[len+1]='\0';
  }
  devops = (devoptab_t*)GetDeviceOpTab(buf);
  if (!devops) return false;
  for(i=0;buf[i]!='\0' && buf[i]!=':';i++);
  if (!devops || strncasecmp(buf,devops->name,i)) return false;
  return true;
}

static void * mountthreadfunc (void *arg)
{
	int dp, dvd_inserted=0,usb_inserted=0;


	//initialize usb
	//if(!usb_init)usb->startup();

#ifdef CE_DEBUG
	LWP_JoinThread(mainthread,NULL);
	return NULL;
#endif
	sleep(1);
  if(!usb_init)usb_mp->startup();
	while(!exit_automount_thread)
	{
		if(!playing_usb)
		{
			mounting_usb=1;

			dp=usb_mp->isInserted();
			usleep(500); // needed, I don't know why, but hang if it's deleted

			if(dp!=usb_inserted)
			{
				usb_inserted=dp;
				if(!dp)
				{
					fatUnmount("usb:");
				}else
				{
					fatMount("usb",usb_mp,0,2,128);
				}
			}
			mounting_usb=0;
		}
		if(dvd_mounting==false)
		{
			dp=WIIDVD_DiscPresent();
			if(dp!=dvd_inserted)
			{
				dvd_inserted=dp;
				if(!dp)dvd_mounted=false; // eject
			}
		}

		sleep(1);
	}
	LWP_JoinThread(mainthread,NULL);
	return NULL;
}

void mount_smb(int number)
{

	char* smb_ip=NULL;
	char* smb_share=NULL;
	char* smb_user=NULL;
	char* smb_pass=NULL;

	m_config_t *smb_conf;
	m_option_t smb_opts[] =
	{
	    {   NULL, &smb_ip, CONF_TYPE_STRING, 0, 0, 0, NULL },
	    {   NULL, &smb_share, CONF_TYPE_STRING, 0, 0, 0, NULL },
	    {   NULL, &smb_user, CONF_TYPE_STRING, 0, 0, 0, NULL },
	    {   NULL, &smb_pass, CONF_TYPE_STRING, 0, 0, 0, NULL },
	    {   NULL, NULL, 0, 0, 0, 0, NULL }
	};
	char cad[10];
	sprintf(cad,"ip%d",number);smb_opts[0].name=strdup(cad);
	sprintf(cad,"share%d",number);smb_opts[1].name=strdup(cad);
	sprintf(cad,"user%d",number);smb_opts[2].name=strdup(cad);
	sprintf(cad,"pass%d",number);smb_opts[3].name=strdup(cad);

	// read configuration
	smb_conf = m_config_new();
	m_config_register_options(smb_conf, smb_opts);
	char file[100];
	sprintf(file,"%s/smb.conf",MPLAYER_DATADIR);
	m_config_parse_config_file(smb_conf, file);

	if(smb_ip==NULL || smb_share==NULL) return;

	if(smb_user==NULL) smb_user=strdup("");
	if(smb_pass==NULL) smb_pass=strdup("");
	sprintf(cad,"smb%d",number);
	if(dbg_network)
	{
	 printf("Mounting SMB Share.. ip:%s  smb_share: '%s'  device: %s ",smb_ip,smb_share,cad);
	 if(!smbInitDevice(cad,smb_user,smb_pass,smb_share,smb_ip)) printf("error \n");
	 else printf("ok \n");
	}
	else
	  smbInitDevice(cad,smb_user,smb_pass,smb_share,smb_ip);

}

void trysmb()
{
	int i;
	for(i=1;i<=5;i++) mount_smb(i);

	//to force connection, I don't understand why is needed
	char cad[20];
	DIR_ITER *dp;
	for(i=1;i<=5;i++)
	{
		dp=diropen(cad);
		if(dp!=NULL) dirclose(dp);
	}
}

static bool CheckPath(char *path)
{
	char *filename;
	FILE *f;

	filename=malloc(sizeof(char)*(strlen(path)+15));
	strcpy(filename,path);
	strcat(filename,"/mplayer.conf");

	f=fopen(filename,"r");
	free(filename);
	if(f==NULL) return false;
	fclose(f);

	sprintf(MPLAYER_DATADIR,"%s",path);
	sprintf(MPLAYER_CONFDIR,"%s",path);
	sprintf(MPLAYER_LIBDIR,"%s",path);

	return true;
}
static bool DetectValidPath()
{

	if(sd_mp->startup())
	{
		if(fatMount("sd",sd_mp,0,2,256))
		{
			if(CheckPath("sd:/apps/mplayer_ce")) return true;
			if(CheckPath("sd:/mplayer")) return true;
		}
	}
	usb_mp->startup();
	usb_init=true;
	//if (usb->startup())
	{
		if(fatMount("usb",usb_mp,0,2,256))
		{
			if(CheckPath("usb:/apps/mplayer_ce")) return true;
			if(CheckPath("usb:/mplayer")) return true;
		}
	}
	return false;
}

static bool load_echi_module()
{
	int ret;
	data_elf my_data_elf;

	//usleep(500);
	ret=mload_init();
	if(ret<0) return false;

	if(((u32) ehcmodule_elf) & 3) return false;

	mload_elf((void *) ehcmodule_elf, &my_data_elf);

	mload_run_thread(my_data_elf.start, my_data_elf.stack, my_data_elf.size_stack, my_data_elf.prio);
	//usleep(500);
	//usleep(100000);
	return true;
}

static void * mloadthreadfunc (void *arg)
{
	sleep(2);
	//if(!load_echi_module()) DisableUSB2(true);
	LWP_JoinThread(mainthread,NULL);
	return NULL;
}
*/
/****************************************************************************
 * LoadConfig
 *
 * Loads all MPlayer .conf files
 ***************************************************************************/
void LoadConfig(char * path)
{
	int i;
	char filepath[1024];
	char tmp[20];
	char* smb_ip;
	char* smb_share;
	char* smb_user;
	char* smb_pass;

	// mplayer.conf
	m_config_t *comp_conf;
	m_option_t comp_opts[] =
	{
	{   "component_fix", &component_fix, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{   "debug_network", &dbg_network, CONF_TYPE_FLAG, 0, 0, 1, NULL},
	{   "gxzoom", &gxzoom, CONF_TYPE_FLOAT, CONF_RANGE, 200, 500, NULL},
	{   "hor_pos", &hor_pos, CONF_TYPE_FLOAT, CONF_RANGE, -400, 400, NULL},
	{   "vert_pos", &vert_pos, CONF_TYPE_FLOAT, CONF_RANGE, -400, 400, NULL},
	{   "horizontal_stretch", &stretch, CONF_TYPE_FLOAT, CONF_RANGE, -400, 400, NULL},
	{   NULL, NULL, 0, 0, 0, 0, NULL }
	};

	comp_conf = m_config_new();
	m_config_register_options(comp_conf, comp_opts);
	sprintf(filepath,"%s/mplayer.conf", path);
	m_config_parse_config_file(comp_conf, filepath);

	// smb.conf
	memset(&smbConf, 0, sizeof(smbConf));

	m_config_t *smb_conf;
	m_option_t smb_opts[] =
	{
		{   NULL, &smb_ip, CONF_TYPE_STRING, 0, 0, 0, NULL },
		{   NULL, &smb_share, CONF_TYPE_STRING, 0, 0, 0, NULL },
		{   NULL, &smb_user, CONF_TYPE_STRING, 0, 0, 0, NULL },
		{   NULL, &smb_pass, CONF_TYPE_STRING, 0, 0, 0, NULL },
		{   NULL, NULL, 0, 0, 0, 0, NULL }
	};

	for(i=1; i<=5; i++)
	{
		smb_ip = NULL;
		smb_share = NULL;
		smb_user = NULL;
		smb_pass = NULL;

		sprintf(tmp,"ip%d",i); smb_opts[0].name=strdup(tmp);
		sprintf(tmp,"share%d",i); smb_opts[1].name=strdup(tmp);
		sprintf(tmp,"user%d",i); smb_opts[2].name=strdup(tmp);
		sprintf(tmp,"pass%d",i); smb_opts[3].name=strdup(tmp);

		smb_conf = m_config_new();
		m_config_register_options(smb_conf, smb_opts);
		sprintf(filepath,"%s/smb.conf", path);
		m_config_parse_config_file(smb_conf, filepath);

		if(smb_ip!=NULL && smb_share!=NULL)
		{
			if(smb_user==NULL) smb_user=strdup("");
			if(smb_pass==NULL) smb_pass=strdup("");

			snprintf(smbConf[i-1].ip, 16, "%s", smb_ip);
			snprintf(smbConf[i-1].share, 20, "%s", smb_share);
			snprintf(smbConf[i-1].user, 20, "%s", smb_user);
			snprintf(smbConf[i-1].pwd, 20, "%s", smb_pass);
		}
	}

	sprintf(MPLAYER_DATADIR,"%s",path);
	sprintf(MPLAYER_CONFDIR,"%s",path);
	sprintf(MPLAYER_LIBDIR,"%s",path);
}

void plat_init (int *argc, char **argv[])
{
	GX_SetCamPosZ(gxzoom);
	GX_SetScreenPos((int)hor_pos,(int)vert_pos,(int)stretch);

	//chdir(MPLAYER_DATADIR);
	setenv("HOME", MPLAYER_DATADIR, 1);
	setenv("DVDCSS_CACHE", "off", 1);
	setenv("DVDCSS_VERBOSE", "0", 1);
	setenv("DVDREAD_VERBOSE", "0", 1);
	setenv("DVDCSS_RAW_DEVICE", "/dev/di", 1);

	stream_cache_size=8*1024; //default cache size (8MB)

	//if (!*((u32*)0x80001800)) sp();
}