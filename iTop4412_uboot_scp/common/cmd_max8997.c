/*
 * common/cmd_max17040.c
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */


#include <common.h>
#include <command.h>


extern void lowlevel_init_max8997(unsigned char Address,unsigned char *Val,int flag);


int do_max8997 ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
  int index = 0;
	unsigned char Address;
	unsigned char  Val;
	int readflag;
	
	switch (argc) {
	case 4:
		Address = simple_strtoul(argv[1], NULL, 16);
	    Val = simple_strtoul(argv[2], NULL, 16);
	    readflag = simple_strtoul(argv[3], NULL, 16);
		break;
	default:
		Address = 0;
		Val = 0;
		readflag = 0;
		return 1;
	}
	

	lowlevel_init_max8997(Address,&Val,readflag);
	return 0;
}


U_BOOT_CMD(
	max8997, 4, 0, do_max8997,
	"max8997     - Test the MAX8997 (I2C Interface.)\n",	
	"[addr value flag(read-0/write-1)]\n"
);


