/*
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com/
 *
 * EXYNOS4x12 - RECOVERY Driver for U-Boot
 * jun.ma@samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <s5pc210.h>
#include <command.h>
#include "recovery.h"
#include <movi.h>

int factory_reset(int mode)
{
	block_dev_desc_t *dev_desc=NULL;
	dev_desc = get_dev("mmc", 0);
	if (dev_desc==NULL) {
		printf("\n** Invalid boot device **\n");
		return -1;
	}
	/*for t34h, 1:fat, 2,3,4:ext4*/
	if (ext2fs_format(dev_desc, 3) != 0)
	{
		printf("ext2format failed for data\n");
		return -1;
	}
	
	if(ext2fs_format(dev_desc, 4))
	{
		printf("ext2format failed for cache\n");
		return -1;
	}

	if(!mode)
	{
		//printf("FAT format failed for cache\n");
		//format SD card...
		if (fat_format_device(dev_desc, 1) != 0) 
		{	
			printf("FAT Format EMMC  Failure!!!\n");
			return -1;
		}
		
		//mj : there is need some emergency..for example no sd card....
		dev_desc = get_dev("mmc", 1); //find the sd card..
		if (dev_desc==NULL) 
		{
			printf("\n** Invalid boot device **\n");
			return -1;
		}
		if (fat_format_device(dev_desc, 1) != 0) 
		{	
			printf("FAT Format SD  Failure!!!\n");
			return -1;
		}

	}
	return 0;
}

int recovery_preboot(void)
{
	unsigned int reset_mode;
	reset_mode = INF_REG5_REG;
		
	if(reset_mode ==FACTORY_RESET_MODE)
	{
			printf("SYSTEM ENTER FACTORY RESET MODE[0x%x]\n",reset_mode);	
			INF_REG5_REG = reset_mode&(~0xff);
			if(factory_reset(1))
			{
				printf("[ERROR]: Factory Reset Fail..");
				return -1;
			}
			return 0;
	}	
	else
	{
		/* add by cym 20141211 GPX1_1 */
		int value = 0;

		char run_cmd[50];

		value = __REG(GPX1DAT);

		if(0x2 == (value & 0x2))//not press
		{
			printf("SYSTEM ENTER NORMAL BOOT MODE\n");
		}
		else	//press
		{
			printf("SYSTEM ENTER Updating MODE\n");

			sprintf(run_cmd, "sdfuse flashall");
			run_command(run_cmd, 0);
		}
		/* end add */

		return 0;
	}
}
