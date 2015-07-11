2012-6-6: Zhang Dong
        1): Modify phone_off to wait for power off to de done
        Modified files:
        lib_arm/board.c

2012-6-4: Sheng Liang
	1): Modify the bl1 / bl2 / trustzone image as the document.
	
2012-5-25:Zhang Dong
        1): corrected adc register base and value for judging power off
        Modified files:
        lib_arm/board.c
        include/s5pc210.h

2012-04-28: Ma Jun
	1): added 4212 tz bin file.

2012-04-24: Ma Jun
	1): changed the build_uboot.sh for image compile which be used in windows pc
2012-04-16: Ma Jun
	1): del the initial code of 6260
	2): changed the cpu version detect code


2012-04-10: Ma Jun
	1): fixed the SD card booting bug caused by TZ.
	2): changed the build_uboot.sh
2012-03-29: Shengliang
	1): add GPL0 relative register config.

2012-03-22:Ma Jun
	1):movi build-uboot.sh to here..



2012-03-21:Li Yang
	1):update code for 4412 trustzone enable
2012-03-20:Ma Jun
	1):delete the ext4 function
	2):added the ap1.1 function

2012-03-12:Ma Jun
	1):changed the sh file for ramdisk image fuse.
2012-03-09:Ma Jun
	1)added the logo display function including charging, boot, recovery, updating logo display.
2012-03-06:Ma Jun
	1):changed the updating function.


2012-03-04:Ma Jun
	1):fixed the fatformat bug.

2012-02-26:Ma Jun
	1):changed the configure of emmc
	2):disable the fusing protection function
	3):added the key detect function for dvt/plus
2012-02-21:Ma Jun
	1):optimized the emmc emergency code
	2):fixed the bug for partition.

2012-02-15:Ma Jun
	1):added the build_uboot.sh file. please copy this file to your 
	   "CodeSign4SecureBoot" directory for tc4 and tc4+ board compile.

2012-02-12:Ma Jun
	1):added the mmc emergency code..
	2):fixed the bug of flash system..


2012-02-06:Ma Jun **important
	1):added a new partition named "Recovery" for recovery function.
	for Flash Recovery image method, please refer the file: readme.txt
	


2012-02-02:Ma Jun
	1):added the factory rest and recovery code (to be continued..)
	2):fixed the bug of sd booting..

2012-02-01:Li Yang
	1) remove sms6260 power on code for tc4 plus version. we need power on 6260 after c2c driver init.changed file: lib_arm/board.
2012-01-29:Wang Xiebin
	1)Add config file for tc4 plus board
	Modified files:
	Makefile
	lib_arm/board.c
	include/configs/tc4_plus_android.h 
	
2012-01-18:Ma Jun
	changed the file for emmc ,sd card and fastboot support
       modified:   cmm.cmm
       modified:   common/cmd_fastboot.c
       modified:   common/cmd_mmc_fdisk.c
       modified:   common/cmd_movi.c
       modified:   cpu/arm_cortexa9/s5pc210/cpu_info.c
       modified:   cpu/arm_cortexa9/s5pc210/movi.c
       modified:   cpu/arm_cortexa9/s5pc210/setup_hsmmc.c
       modified:   drivers/mmc/mmc.c
       modified:   drivers/mmc/s3c_hsmmc.c
       modified:   drivers/mmc/s5p_mshc.c
       modified:   include/configs/tc4_android.h
       modified:   include/mmc.h
       modified:   include/movi.h
       modified:   include/s3c_hsmmc.h
       modified:   readme.txt

2012-01-16: Jin Zhebin
    1)in common/cmd_movi.c: add dvt uboot fusing protection

2012-01-11: Jin Zhebin
    1)in lib_arm/board.c: sdcard update will fail when using battery(no charge),
    the reason is that there is no button detection code here. Added it.

2012-01-10: Jin Zhebin
    1)in cpu/arm_cortexa9/s5pc210/pmic.c: add pmic version and DVT board version information
    
2012-01-09: Jin Zhebin
	1)three images will be generated, they are:
	    a. for Pegasus Dual EVT0 
            u-boot-exynos4212-evt0-nonfused.bin
	    b. for Pegasus Quad EVT0
	    u-boot-exynos4412-evt0-nonfused.bin
	    c. for Pegasus Quad EVT1
	    u-boot-exynos4412-evt1-efused.bin

	2)there are three sd_fusing files now in sdfuse_q folder, they are:
	    sd_fusing_exynos4212_evt.sh
	    sd_fusing_exynos4412_evt.sh
	    sd_fusing_exynos4412_dvt.sh
	  please use the corresponding file to fuse different uboot images
