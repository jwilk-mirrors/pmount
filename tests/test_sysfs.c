/* 
 * Copyright (c) 2009 Vincent Fourmond <fourmond@debian.org>
 *
 * This software is distributed under the terms and conditions of the 
 * GNU General Public License. See file GPL for the full text of the license.
 */

/**
   This program tries uses find_sysfs_device from src/policy.c to find
   sysfs device information for the device given as argument.

   This program is only intended for development purposes.

   DO NOT INSTALL IT SUID ROOT !

   DO NOT RUN IT WITH ROOT PRIVILEGES ! (there's no need for that)
 */


/* #include "policy.h" */
#include <stdio.h>

/* For stat: */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* The very functions we want to test ! */
int find_sysfs_device( const char* dev, char* blockdevpath, 
		       size_t blockdevpathsize );

int is_blockdev_attr_true( const char* blockdevpath, 
			   const char* attr );

const char * bus_has_ancestry(const char * blockdevpath, 
			      char** buses);

static char* hotplug_buses[] = { "usb", "ieee1394", 
				 "mmc", "pcmcia", NULL };



int main(int argc, char *argv[])
{
  char device_path[512];
  const char * bus;
  struct stat devstat;
  if(argc != 2) {
    fprintf(stderr, "Usage: %s device\n", argv[0]);
    return 1;
  }
  if(stat(argv[1], &devstat)) {
    perror("Could not open file");
    return 1;
  }
  if(! (devstat.st_mode & S_IFBLK)) {
    fprintf(stderr, "Sorry, `%s' is not a block device.\n", argv[1]);
    return 1;
  }

  if(find_sysfs_device(argv[1], device_path, sizeof(device_path))) {
    fprintf(stdout, "Found sysfs device for %s : %s\n", 
	    argv[1], device_path);
    fprintf(stdout, "Device %s is removable: %s\n",
	    argv[1], is_blockdev_attr_true(device_path,"removable") ?
	    "yes" :"no");
    bus = bus_has_ancestry(device_path, hotplug_buses);
    if(bus) 
      fprintf(stdout, "Found whitelisted bus: %s\n", bus);
    else 
      fprintf(stdout, "No whitelisted bus found\n", bus);
    return 0;
  }
  else {
    fprintf(stderr, "find_sysfs_device failed for %s\n", argv[1]);
    return 1;
  }

}
