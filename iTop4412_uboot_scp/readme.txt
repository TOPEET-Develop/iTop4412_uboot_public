
1. building image 
   The CodeSign4SecureBoot tool has two versions:
   one is for linux pc, the other one is for windows pc
   if you use the linux version, please do as followings:
	[1]: put CodeSign4SecureBoot at the same directory as TC4_uboot
	
	[2]:
	tc4 board
		 ./build_uboot.sh 
	
	tc4 plus board (to use this command, please update the build-uboot.sh file)
		 ./build_uboot.sh tc4_plus
	[3]:

   	three images will be generated, they are:
	(1) for Pegasus Dual EVT0 

		u-boot-exynos4212-evt0-nonfused.bin

	(2) for Pegasus Quad EVT0

		u-boot-exynos4412-evt0-nonfused.bin

	(3) for Pegasus Quad EVT1

	    	u-boot-exynos4412-evt1-efused.bin
   if you use the windows version, please do as followings:
   	[1]: ./build_uboot.sh windows
	[2]: please copy "checksum_bl2_14k.bin" and "u-boot.bin" to CodeSign4SecureBoot directory witch
	     located in your windows pc
	[3]: executed the command "make_uboot.bat" in windows pc

2. flashing image

	flashing uboot image to sd/mmc card or inand use u-boot-config_fused.bin.

	flashing uboot to inand:
	a) use JTAG:
		run cmm file in uboot directory,after uboot booting,use this command:
		movi write emmc-uboot 40000000
	b) use sd/mmc card booting:
		tc4 can boot form sd/mmc card if there are no image in inand.after 
		booting from sd/mmc card:
		dnw c0000000 (download u-boot-config_fused.bin)
		movi write emmc-uboot 40000000

	flashing uboot to sd card, have two way:
	a). use linux PC,:
	insert card to linux PC, enter uboot folder: 
	for 4212 evt board: ./mkuboot 4212 /dev/sdb
	for 4412 evt board: ./mkuboot 4412 /dev/sdb
	for 4412 dvt board: ./mkuboot 4412s /dev/sdb
	b). use uboot :
	dnw c0000000 (download u-boot-config_fused.bin)
	movi write u-boot c0000000

3. images upgrade
	if we want to flashing other images to inand or sd/mmc card except for uboot, 
	first of all we need to format inand:
		fdisk -c 0
		fatformat mmc 0:1
		ext3format mmc 0:2
		ext3format mmc 0:3
		ext3format mmc 0:4
	if tc4 board have images in inand, we can upgrade images using these ways:

	a) fastboot: 
	fastboot flash bootloader u-boot-config_fused.bin
	fastboot flash Recovery ramdisk-recovery-uboot.img
	fastboot flash kernel zimage
	fastboot flash ramdisk ramdisk-uboot.img
	fastboot flash logo logo_resource.bin
	fastboot flash system system.img

	b) sd upgrade:
	copy images to sdcard root directory,then use uboot command:
	sdfuse flashall
	after upgrade all the images in sdcard, system will reset auto.

	c) dnw
	dnw can flashing uboot ,kernel and ramdisk ,but can't flashing system.img.
	dnw c0000000
	movi write emmc-uboot c0000000
	dnw c0000000
	movi write kernel c0000000
	dnw c0000000
	movi write rootfs c0000000 100000
4. Erase the image in eMMC
	Format:mmc erase <boot|user> <device num> <start block> <block count>
	[1]:eraloader
		 mmc erase boot 0 0 512
	[2]:erael
		 mmc erase boot 0 1120 8192
               