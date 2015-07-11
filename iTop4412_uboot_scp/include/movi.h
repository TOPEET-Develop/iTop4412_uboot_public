#ifndef __MOVI_H__
#define __MOVI_H__


#define MAGIC_NUMBER_MOVI	(0x24564236)

#define SS_SIZE			(16 * 1024)

#define eFUSE_SIZE		(1 * 512)	// 512 Byte eFuse, 512 Byte reserved

#define MOVI_BLKSIZE		(1<<9) /* 512 bytes */
//mj defined
#define FWBL1_SIZE		(8* 1024) //IROM BL1 SIZE 8KB
#define BL2_SIZE		(16 * 1024)//uboot BL1 16KB

/* partition information */
#define PART_SIZE_UBOOT		(495 * 1024)
#define PART_SIZE_KERNEL	(6 * 1024 * 1024)	//modify by cym 20140217

#define PART_SIZE_ROOTFS	(2 * 1024 * 1024)//  2M
#define RAW_AREA_SIZE		(16 * 1024 * 1024)// 16MB

	#define MOVI_RAW_BLKCNT		(RAW_AREA_SIZE / MOVI_BLKSIZE)	/* 16MB */
	#define MOVI_FWBL1_BLKCNT	(FWBL1_SIZE / MOVI_BLKSIZE)	/* FWBL1:8KB */
	#define MOVI_BL2_BLKCNT		(BL2_SIZE / MOVI_BLKSIZE)	/* BL2:16KB */
	#define MOVI_ENV_BLKCNT		(CONFIG_ENV_SIZE / MOVI_BLKSIZE)	/* ENV:16KB */
	#define MOVI_UBOOT_BLKCNT	(PART_SIZE_UBOOT / MOVI_BLKSIZE)/* UBOOT:512KB */
	#define MOVI_ZIMAGE_BLKCNT	(PART_SIZE_KERNEL / MOVI_BLKSIZE)/* 4MB */
	#define ENV_START_BLOCK		(544*1024)/MOVI_BLKSIZE

	#define MOVI_UBOOT_POS		((eFUSE_SIZE / MOVI_BLKSIZE) + MOVI_FWBL1_BLKCNT + MOVI_BL2_BLKCNT)

#define MOVI_ROOTFS_BLKCNT	(PART_SIZE_ROOTFS / MOVI_BLKSIZE)

/*
 *
 * start_blk: start block number for image
 * used_blk: blocks occupied by image
 * size: image size in bytes
 * attribute: attributes of image
 *            0x1: u-boot parted (BL1)
 *            0x2: u-boot (BL2)
 *            0x4: kernel
 *            0x8: root file system
 *            0x10: environment area
 *            0x20: reserved
 * description: description for image
 * by scsuh
 */
typedef struct member {
	uint start_blk;
	uint used_blk;
	uint size;
	uint attribute; /* attribute of image */
	char description[16];
} member_t; /* 32 bytes */

/*
 * magic_number: 0x24564236
 * start_blk: start block number for raw area
 * total_blk: total block number of card
 * next_raw_area: add next raw_area structure
 * description: description for raw_area
 * image: several image that is controlled by raw_area structure
 * by scsuh
 */
typedef struct raw_area {
	uint magic_number; /* to identify itself */
	uint start_blk; /* compare with PT on coherency test */
	uint total_blk;
	uint next_raw_area; /* should be sector number */
	char description[16];
	member_t image[15];
} raw_area_t; /* 512 bytes */

#endif /*__MOVI_H__*/
