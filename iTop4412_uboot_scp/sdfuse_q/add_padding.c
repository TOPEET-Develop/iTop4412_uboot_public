/*
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com/
 *   
 * 20120315:  yang81.li@samsung.com for add trustzone image
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  PART_SIZE_UBOOT		(328 * 1024) 
//#define PART_SIZE_TZSW		(160 * 1024)




static void do_add_padding(char* uboot_file)
{
    FILE *fp_src_uboot = NULL;
    char *fn_src_uboot = uboot_file;


    unsigned char *src = NULL;
    int fs = 0;

    fp_src_uboot = fopen(fn_src_uboot, "ab+");
    if (NULL == fp_src_uboot)
    {
        printf("can't open uboot file for read(%s)\n", fn_src_uboot);
        return;
    }
    
    fseek(fp_src_uboot, 0L, SEEK_END);
    fs = ftell(fp_src_uboot);
    printf("before padding uboot.bin file size= %dB\n", fs);

   if (fs > PART_SIZE_UBOOT){
      printf("error: too large uboot !!!");
	return ;
   }
   src = (unsigned char *)calloc((size_t)(PART_SIZE_UBOOT-fs), 1);
	
   memset(src,0,(size_t)(PART_SIZE_UBOOT-fs));

   if (!src){
	      printf("memory alloc failed !!!");
	return ;
   }
		
    fs=fwrite(src, 1, (size_t)(PART_SIZE_UBOOT-fs), fp_src_uboot);

   // fs = ftell(fp_src_uboot);
    printf("%d B written \n", fs);
	
    free(src);
	
    fclose(fp_src_uboot);
}


int main(int argc, char *argv[])
{

    do_add_padding("u-boot.bin");
	return 0;
}