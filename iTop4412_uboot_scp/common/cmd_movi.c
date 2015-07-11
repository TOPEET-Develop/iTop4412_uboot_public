#include <common.h>
#include <command.h>
#include <movi.h>
#include <mmc.h>
#include <part.h>

//#define DEBUG_CMD_MOVI
#ifdef DEBUG_CMD_MOVI
#define dbg(x...)	printf(x)
#else
#define dbg(x...)	do { } while (0)
#endif

//#ifndef SMDK4212_ID
//#define SMDK4212_ID 0x43220000
//#endif
//#ifndef SMDK4412_ID
//#define SMDK4412_ID 0xE4412000
//#endif

extern unsigned int OmPin;
extern unsigned int s5pc210_cpu_id;
extern int dev_number_write;
extern int fuse_by_fastboot;
extern int Is_TC4_Dvt;


raw_area_t raw_area_control;

int init_raw_area_table (block_dev_desc_t * dev_desc)
{
	struct mmc *host = find_mmc_device(dev_desc->dev);

	/* when last block does not have raw_area definition. */
	if (raw_area_control.magic_number != MAGIC_NUMBER_MOVI) {
		int  i = 0;
		member_t *image;
		u32 capacity;
		
		dbg("The host name is %s\n",host->name);
		
		if(host->high_capacity) {
			capacity = host->capacity;
			}
		else {
			capacity = host->capacity * (host->read_bl_len / MOVI_BLKSIZE);
		}

		dbg("Warning: cannot find the raw area table(%p) %08x\n",
			&raw_area_control, raw_area_control.magic_number);
		
		/* add magic number */
		raw_area_control.magic_number = MAGIC_NUMBER_MOVI;

		/* init raw_area will be 16MB */
		raw_area_control.start_blk = 16*1024*1024/MOVI_BLKSIZE;
		raw_area_control.total_blk = capacity;
		raw_area_control.next_raw_area = 0;
		strcpy(raw_area_control.description, "initial raw table");

		image = raw_area_control.image;

		/* image 0 should be fwbl1 */
		if(strcmp(host->name, "S5P_MSHC4") == 0)
			image[0].start_blk = 0;
		else
			image[0].start_blk = (eFUSE_SIZE/MOVI_BLKSIZE);
		
		image[0].used_blk = MOVI_FWBL1_BLKCNT;
		image[0].size = FWBL1_SIZE;
		image[0].attribute = 0x0;
		strcpy(image[0].description, "fwbl1");
		dbg("fwbl1: %d\n", image[0].start_blk);

		/* image 1 should be bl2 */
		image[1].start_blk = image[0].start_blk + image[0].used_blk;
		image[1].used_blk = MOVI_BL2_BLKCNT;
		image[1].size = BL2_SIZE;
		image[1].attribute = 0x3;
		strcpy(image[1].description, "bl2");
		dbg("bl2: %d\n", image[1].start_blk);

		#if 0
		/* image 2 should be uboot */
		image[2].start_blk = image[1].start_blk + image[1].used_blk;
		image[2].used_blk = MOVI_UBOOT_BLKCNT;
		image[2].size = PART_SIZE_UBOOT;
		image[2].attribute = 0x2;
		strcpy(image[2].description, "bootloader");
		dbg("u-boot: %d\n", image[2].start_blk);
		#else
		/*BL1,BL2,u-boot have been combined together when compiling for EMMC*/
		if(strcmp(host->name, "S5P_MSHC4") == 0)
			image[2].start_blk = 0;
		else
			image[2].start_blk = (eFUSE_SIZE/MOVI_BLKSIZE);
		
			image[2].used_blk = MOVI_FWBL1_BLKCNT+MOVI_UBOOT_BLKCNT+MOVI_BL2_BLKCNT;
			image[2].size = PART_SIZE_UBOOT+FWBL1_SIZE+BL2_SIZE;
			image[2].attribute = 0x2;
			strcpy(image[2].description, "bootloader");
			dbg("u-boot: %d\n", image[2].start_blk);
		#endif
		/* image 3 should be environment */
		image[3].start_blk = (544*1024)/MOVI_BLKSIZE;
		image[3].used_blk = MOVI_ENV_BLKCNT;
		image[3].size = CONFIG_ENV_SIZE;
		image[3].attribute = 0x10;
		strcpy(image[3].description, "environment");
		dbg("env: %d\n", image[3].start_blk);

		/* image 4 should be kernel */
		image[4].start_blk = image[3].start_blk + image[3].used_blk;
		image[4].used_blk = MOVI_ZIMAGE_BLKCNT;
		image[4].size = PART_SIZE_KERNEL;
		image[4].attribute = 0x4;
		strcpy(image[4].description, "kernel");
		dbg("knl: %d\n", image[4].start_blk);

		/* image 5 should be RFS */
		image[5].start_blk = image[4].start_blk + image[4].used_blk;
		image[5].used_blk = MOVI_ROOTFS_BLKCNT;
		image[5].size = PART_SIZE_ROOTFS;
		image[5].attribute = 0x8;
		strcpy(image[5].description, "ramdisk");
		dbg("rfs: %d\n", image[5].start_blk);
		
		#ifdef CONFIG_RECOVERY
		/* image 6 should be Recovery */
		image[6].start_blk = image[5].start_blk + image[5].used_blk;
		image[6].used_blk = RAW_AREA_SIZE/MOVI_BLKSIZE - image[5].start_blk;
		image[6].size = image[6].used_blk * MOVI_BLKSIZE;
		image[6].attribute = 0x6;
		strcpy(image[6].description, "Recovery");
		dbg("recovery: %d\n", image[6].start_blk);
		#endif

		
		/* image 7 should be disk */
		image[7].start_blk = RAW_AREA_SIZE/MOVI_BLKSIZE;
		image[7].size = capacity-RAW_AREA_SIZE;
		image[7].used_blk = image[7].size/MOVI_BLKSIZE;
		image[7].attribute = 0xff;
		strcpy(image[7].description, "disk");
		dbg("disk: %d\n", image[7].start_blk);
		for (i=8; i<15; i++) {
			raw_area_control.image[i].start_blk = 0;
			raw_area_control.image[i].used_blk = 0;
		}
		
		
	}

	return 0;
}

int do_movi(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	char *cmd;
	ulong addr, start_blk, blkcnt;
	char *addr_img_compare;         //used to compare the image between 4212 & 4412
	uint rfs_size;
	char run_cmd[100];
	uint rw = 0, attribute = 0;
	int i;
	member_t *image;
	struct mmc *mmc;
	int boot_dev = 0;

	cmd = argv[1];

	switch (cmd[0]) {
	case 'i':
		raw_area_control.magic_number = 0;
		run_command("mmcinfo", 0);
		return 1;	
	case 'r':
		rw = 0;	/* read case */
		break;
	case 'w':
		rw = 1; /* write case */
		break;
	default:
		goto usage;
	}
	
	cmd = argv[2];

	switch (cmd[0]) {
	
	case 'f':
		if (argc != 4)
			goto usage;
		attribute = 0x0;
		addr = simple_strtoul(argv[3], NULL, 16);
		break;
	case 'u':
		if (argc != 4)
			goto usage;
		attribute = 0x2;
		addr = simple_strtoul(argv[3], NULL, 16);
		break;
	case 'k':
		if (argc != 4)
			goto usage;
		attribute = 0x4;
		addr = simple_strtoul(argv[3], NULL, 16);
		break;
		
	#ifdef CONFIG_RECOVERY
	case 'R':
		if (argc != 5)
			goto usage;
		attribute = 0x6;
		addr = simple_strtoul(argv[3], NULL, 16);
		break;
	#endif
	
	case 'r':
		if (argc != 5)
			goto usage;
		attribute = 0x8;
		addr = simple_strtoul(argv[3], NULL, 16);
		break;
	case 'e':
		if (argc != 4)
			goto usage;
		attribute = 0x20;
		addr = simple_strtoul(argv[3], NULL, 16);
		break;		

	
	default:
		goto usage;
	}
	
	mmc = find_mmc_device(0);
	

	if(OmPin == BOOT_EMMC441)
	{
		boot_dev = 0;		
	}
	else if(OmPin == BOOT_MMCSD)
	{
		if(strcmp(mmc->name, "S5P_MSHC4") == 0)//emmc exist..
		{
			boot_dev = 1;	
		}
		else
		{
			boot_dev = 0; 
		}

	}
	else
	{
		boot_dev = 0; 
		printf("[ERROR]Undefined booting mode\n");
	}

	
//	init_raw_area_table();

	/* firmware BL1 r/w */
	if (attribute == 0x0) {
		/* on write case we should write BL1 1st. */
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}
		start_blk = image[i].start_blk;
		blkcnt = image[i].used_blk;
		printf("%s FWBL1 .. %ld, %ld ", rw ? "writing":"reading",
				start_blk, blkcnt);
		sprintf(run_cmd,"mmc %s 0 0x%lx 0x%lx 0x%lx",
				rw ? "write":"read",
				addr, start_blk, blkcnt);
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}

#if defined(CONFIG_SECURE)
	
	/* u-boot r/w */
	if (attribute == 0x2) {
		/* on write case we should write BL2 1st. */
		if (rw) {
			start_blk = raw_area_control.image[1].start_blk;
			blkcnt = raw_area_control.image[1].used_blk;
			printf("Writing BL1 to sector %ld (%ld sectors).. ",
					start_blk, blkcnt);
			movi_calc_checksum_bl1(addr);
		}
		
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}
		start_blk = image[i].start_blk;
		blkcnt = image[i].used_blk;
		printf("%s bootloader.. %ld, %ld ", rw ? "writing":"reading",
				start_blk, blkcnt);
		sprintf(run_cmd,"mmc %s 0 0x%lx 0x%lx 0x%lx",
				rw ? "write":"read",
				addr, start_blk, blkcnt);
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;		
		
#else	/* NOT CONFIG_SECURE */
	/* u-boot r/w */
	if (attribute == 0x2) {
		#if 0//ly: we don't need it 
		/* on write case we should write BL2 1st. */
		if (rw) {
			start_blk = raw_area_control.image[1].start_blk;
			blkcnt = raw_area_control.image[1].used_blk;
			printf("Writing BL1 to sector %ld (%ld sectors).. ",
					start_blk, blkcnt);
			movi_write_bl1(addr);
		}

		#endif
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
				
		}
		start_blk =1;// image[i].start_blk;
		blkcnt = image[i].used_blk;
		printf("%s bootloader.. %ld, %ld ", rw ? "writing":"reading",
				start_blk, blkcnt);
		sprintf(run_cmd,"mmc %s 1 0x%lx 0x%lx 0x%lx", //0-->1
				rw ? "write":"read",
				addr, start_blk, blkcnt);
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}
#endif		
	/* u-boot r/w for emmc*/
	if (attribute == 0x20) {

		//printf("******u-boot r/w for emmc!!\n");

	
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == 0x2) /* the same with uboot setting */
				break;
		}

		/* switch mmc to boot partition */

		/* transfer u-boot-config-fused */

		/*added by jzb
		   check whether the uboot img matchs the board or not
		*/
		#if 0
		addr_img_compare = addr + 4;//there is a difference at the fifth byte between 4212 & 4412 image
		
		if ((s5pc210_cpu_id & 0xfffff000) == SMDK4212_ID) {
			if (*addr_img_compare != 0x66) {
				printf("\n\n*** Incorrect u-boot image!!!\n");
				printf("*** Please fuse the %s!\n",CONFIG_4212_BOOTLOADER);
				return -1;
			}
		}
		else if ((s5pc210_cpu_id & 0xfffff000) == SMDK4412_ID && Is_TC4_Dvt == 0) {
			if (*addr_img_compare != 0xfe) {
				printf("\n\n*** Incorrect u-boot image!!!\n");
				printf("*** Please fuse the %s!\n",CONFIG_4412_BOOTLOADER);
				return -1;
			}
		}
		else if ((s5pc210_cpu_id & 0xfffff000) == SMDK4412_ID && Is_TC4_Dvt != 0) {
			if (*addr_img_compare != 0x8a) {
				printf("\n\n*** Incorrect u-boot image!!!\n");
				printf("*** Please fuse the %s!\n",CONFIG_4412_DVT_BOOTLOADER);
				return -1;
			}
		}
		#endif
		
		start_blk =0;// image[i].start_blk ;//+ MOVI_BL1_BLKCNT;
		blkcnt = image[i].used_blk ;//- MOVI_BL1_BLKCNT;
		printf("%s bootloader.. %ld, %ld \n", rw ? "writing":"reading",
				start_blk, blkcnt);
		
		sprintf(run_cmd,"emmc open 0");
		run_command(run_cmd, 0);
		
		sprintf(run_cmd,"mmc %s 0 0x%lx 0x%lx 0x%lx",
				rw ? "write":"read",
				addr, start_blk, blkcnt);


		run_command(run_cmd, 0);

		/* switch mmc to normal paritition */
		sprintf(run_cmd,"emmc close 0");
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}
	
	/* kernel r/w */
	if (attribute == 0x4) {
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}
		start_blk = image[i].start_blk;
		blkcnt = image[i].used_blk;
		printf("%s kernel.. %ld, %ld ", rw ? "writing" : "reading",
				start_blk, blkcnt);
		if (1 == fuse_by_fastboot)
			sprintf(run_cmd, "mmc %s %s 0x%lx 0x%lx 0x%lx",
					rw ? "write" : "read", dev_number_write ? "1" : "0",
					addr, start_blk, blkcnt);
		else
		{
			#if 0
			if (key1_pulldown == 1)
				sprintf(run_cmd,"mmc %s 1 0x%lx 0x%lx 0x%lx",
						rw ? "write":"read",
						addr, start_blk, blkcnt);
			else 
			#endif
				sprintf(run_cmd,"mmc %s %d 0x%lx 0x%lx 0x%lx",
						rw ? "write":"read",boot_dev,
						addr, start_blk, blkcnt);
		}
		
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}


	
	/* root file system r/w */
	if (attribute == 0x8) {
		rfs_size = simple_strtoul(argv[4], NULL, 16);
		
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}
		start_blk = image[i].start_blk;
		blkcnt = rfs_size/MOVI_BLKSIZE +
			((rfs_size&(MOVI_BLKSIZE-1)) ? 1 : 0);
		image[i].used_blk = blkcnt;
		printf("%s RFS.. %ld, %ld ", rw ? "writing":"reading",
				start_blk, blkcnt);
		if (1 == fuse_by_fastboot)
			sprintf(run_cmd, "mmc %s %s 0x%lx 0x%lx 0x%lx",
					rw ? "write" : "read", dev_number_write ? "1" : "0",
					addr, start_blk, blkcnt);
		else
		{
			#if 0
			if (key1_pulldown == 1)
				sprintf(run_cmd,"mmc %s 1 0x%lx 0x%lx 0x%lx",
						rw ? "write":"read",
						addr, start_blk, blkcnt);
			else
			#endif
				sprintf(run_cmd,"mmc %s %d 0x%lx 0x%lx 0x%lx",
						rw ? "write":"read",boot_dev,
						addr, start_blk, blkcnt);
		}

		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}

	/* Recovery image.. r/w */
	if (attribute == 0x6) {
		rfs_size = simple_strtoul(argv[4], NULL, 16);
		
		for (i=0, image = raw_area_control.image; i<15; i++) {
			if (image[i].attribute == attribute)
				break;
		}
		start_blk = image[i].start_blk;
		blkcnt = rfs_size/MOVI_BLKSIZE +
			((rfs_size&(MOVI_BLKSIZE-1)) ? 1 : 0);
		image[i].used_blk = blkcnt;
		
		printf("%s recovery-img.. %ld, %ld ", rw ? "writing":"reading",
				start_blk, blkcnt);
		
		if (1 == fuse_by_fastboot)
			sprintf(run_cmd, "mmc %s %s 0x%lx 0x%lx 0x%lx",
					rw ? "write" : "read", dev_number_write ? "1" : "0",
					addr, start_blk, blkcnt);
		else
		{
			#if 0
			if (key1_pulldown == 1)
				sprintf(run_cmd,"mmc %s 1 0x%lx 0x%lx 0x%lx",
						rw ? "write":"read",
						addr, start_blk, blkcnt);
			else
			#endif
				sprintf(run_cmd,"mmc %s %d 0x%lx 0x%lx 0x%lx",
						rw ? "write":"read",boot_dev,
						addr, start_blk, blkcnt);
		}
		run_command(run_cmd, 0);
		printf("completed\n");
		return 1;
	}

usage:
	printf("Usage:\n%s\n", cmdtp->usage);
	return -1;
}

U_BOOT_CMD(
		movi,	5,	0,	do_movi,
		"movi\t- sd/mmc r/w sub system for SMDK board\n",
		"init - Initialize moviNAND and show card info\n"
		"movi read  {u-boot | kernel} {addr} - Read data from sd/mmc\n"
		"movi write {fwbl1 | u-boot | kernel} {addr} - Write data to sd/mmc\n"
		"movi read  rootfs {addr} [bytes(hex)] - Read rootfs data from sd/mmc by size\n"
		"movi write rootfs {addr} [bytes(hex)] - Write rootfs data to sd/mmc by size\n"
		"movi read  {sector#} {bytes(hex)} {addr} - instead of this, you can use \"mmc read\"\n"
		"movi write {sector#} {bytes(hex)} {addr} - instead of this, you can use \"mmc write\"\n"
	  );
