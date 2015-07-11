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

#define BL1_SIZE    (8 * 1024)
#define BL2_SIZE    (14 * 1024)
#define CS_SIZE     (4)
#define CS_START    (BL2_SIZE - CS_SIZE)


static void calc_bl2_chksum(unsigned long addr)
{
    int i;
    unsigned long checksum;
    unsigned long src;

   // addr = addr + BL1_SIZE;

    for(i = 0, checksum = 0;i < CS_START;i++)
    {
        checksum += *(unsigned char*)addr++;
    }

    *(unsigned long*)addr = checksum;
}


static void add_sign(char* src_file,char* dst_file, int symbol)
{
    	FILE *fp_src_uboot = NULL;
    	FILE *fp_dst_uboot = NULL;
	unsigned char      *ptmp1 = NULL;
	unsigned char      *addr = NULL;
	
    	char *fn_src_uboot = src_file;
    	char *fn_dst_uboot = dst_file;
    	unsigned char *src = NULL;
    	int fs = 0;
    	int i = 0;
    	fp_src_uboot = fopen(fn_src_uboot, "rb");
    	if (NULL == fp_src_uboot)
    	{
    	    printf("can't open uboot file for read(%s)\n", fn_src_uboot);
    	    return;
    	}
    	
    	fp_dst_uboot = fopen(fn_dst_uboot, "wb");
    	if (NULL == fp_src_uboot)
    	{
    	    printf("can't open uboot file for read(%s)\n", fn_src_uboot);
    	    return;
    	}
    	
    	fseek(fp_src_uboot, 0L, SEEK_END);
    	fs = ftell(fp_src_uboot);
    	printf("u-boot.bin file size= %dB\n", fs);
    	rewind(fp_src_uboot);
    	
    	
    	src = (unsigned char *)calloc((size_t)fs, 1);
    	fread(src, 1, (size_t)fs, fp_src_uboot);
    	
    	ptmp1=src;
    	ptmp1+=8;
    	
    	//printf("the symbol is = %x\n", symbol);
	#if 0
    	addr = (unsigned char*)symbol;
    	for(i=0;i<4;i++)
    	{
    		*(ptmp1+i)=*(addr+i);	//*(unsigned char*)addr++;    
    	}
    	#else
    	#if(symbol==1)//add the sign for 4212 uboot.bin 
    	{
    		*(ptmp1+0)=0x00;
    		*(ptmp1+1)=0x00;
    		*(ptmp1+2)=0x22;
    		*(ptmp1+3)=0x43;
    	}
    	#else//add the sign for 4412 uboot.bin
    	{
    		*(ptmp1+0)=0x00;
    		*(ptmp1+1)=0x20;
    		*(ptmp1+2)=0x41;
    		*(ptmp1+3)=0xE4;
    	}
    	#endif
    	
	#endif
    	fwrite(src, 1, (size_t)fs, fp_dst_uboot);
    	
    	free(src);
    	fclose(fp_src_uboot);
    	fclose(fp_dst_uboot);
    	
}


int main(int argc, char *argv[])
{
	//int *symbol = 0x12345678;
	
   	add_sign("u-boot.bin","u-boot-4212.bin",1);
   	add_sign("u-boot.bin","u-boot-4412.bin",1);
	return 0;
}

