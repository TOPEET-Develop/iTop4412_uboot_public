/*
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BL1_SIZE (16 * 1024)
#define HEAD_SIZE 16

static void movi_write_bl1(char *fn_uboot, char *fn_bl1ah)
{
        int     i;
        ulong   checksum=0;
        unsigned char      *ptmp1 = NULL;
        unsigned char      *ptmp2 = NULL;
        unsigned char      *addr = NULL;
        unsigned char      *src = NULL;

	FILE *fp_uboot = NULL;
	FILE *fp_bl1ah = NULL;

	if (NULL == fn_uboot)
		fn_uboot = "../u-boot.bin";
	if (NULL == fn_bl1ah)
		fn_bl1ah = "bl1ah";

	fp_uboot = fopen(fn_uboot, "rb");
	if (NULL == fp_uboot)
	{
		printf("can't open uboot file for read\n");
		return ;
	}
	fp_bl1ah = fopen(fn_bl1ah, "wb");
	if (NULL == fp_bl1ah)
	{
		printf("can't open bl1ah file for write\n");
		goto err;
	}

        src = (unsigned char *) malloc(BL1_SIZE);
        memset(src, 0, BL1_SIZE);
	ptmp1 = src;

        addr = (unsigned char *) malloc(BL1_SIZE - HEAD_SIZE);
	memset(addr, 0, BL1_SIZE - HEAD_SIZE);
	ptmp2 = addr;

	fread(addr, 1, BL1_SIZE - HEAD_SIZE, fp_uboot);

        for(i = 0;i < BL1_SIZE - HEAD_SIZE;i++)
        {
                src[i+16]=*(unsigned char*)addr++;
                checksum += src[i+16];
        }

        *(ulong*)src = 0x1f;
        *(ulong*)(src+4) = checksum;

        src[ 0] ^= 0x53;
        src[ 1] ^= 0x35;
        src[ 2] ^= 0x50;
        src[ 3] ^= 0x43;
        src[ 4] ^= 0x32;
        src[ 5] ^= 0x31;
        src[ 6] ^= 0x30;
        src[ 7] ^= 0x20;
        src[ 8] ^= 0x48;
        src[ 9] ^= 0x45;
        src[10] ^= 0x41;
        src[11] ^= 0x44;
        src[12] ^= 0x45;
        src[13] ^= 0x52;
        src[14] ^= 0x20;
        src[15] ^= 0x20;

        for(i=1;i<16;i++)
        {
                src[i] ^= src[i-1];
        }

	fwrite(src, 1, BL1_SIZE, fp_bl1ah);

        free(ptmp1);
        free(ptmp2);

	fclose(fp_bl1ah);
err:
	fclose(fp_uboot);
}


int main(int argc, char *argv[])
{
	FILE		*fp;

	if (argc < 3)
		movi_write_bl1(NULL, NULL);
	else
		movi_write_bl1(argv[1], argv[2]);
	return 0;
}

