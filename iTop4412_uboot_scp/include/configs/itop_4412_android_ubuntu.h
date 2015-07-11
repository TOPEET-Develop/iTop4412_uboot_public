/*
 *
 * Configuation settings for the iTOP-4412 board.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_S5PC210      1       /* in a SAMSUNG S5PC210 SoC */
#define CONFIG_SMDKC210     1
#define CONFIG_DEVICE_STRING    "iTOP-4412"

#define CONFIG_EVT1     1       /* EVT1 */

#define TC4_PLUS		
/*----CPU ID Defination----*/
#define SMDK4212_ID 		0x43220000
#define SMDK4212_AP11_ID 	0x43220210 //mj
#define SMDK4212_AP10_ID 	0x43220200 //mj

#define SMDK4412_ID 		0xE4412000
#define SMDK4412_AP11_ID 	0xE4412211 //mj
#define SMDK4412_AP10_ID 	0xE4412210 //mj

/* add by cym 20131206 */
#define SMDK4412_SUPPORT_UBUNTU	1
/* end add */

/*
 * SECURE BOOT
 * */
//#define CONFIG_SECURE

#ifdef CONFIG_SECURE

/* BL1 size */
#ifdef CONFIG_EVT1
#define CONFIG_SECURE_BL1_SIZE			0x2000
#define CONFIG_SECURE_BL1_ONLY
//#define CONFIG_SECURE_BOOT			/* Signed Kernel, RFS */
#else
#define CONFIG_SECURE_BL1_SIZE			0x4000
#endif

#ifdef CONFIG_SECURE_BOOT
#define CONFIG_S5PC210S
#define CONFIG_SECURE_ROOTFS
#define CONFIG_SECURE_KERNEL_BASE       0xC0008000
#define CONFIG_SECURE_KERNEL_SIZE       0x300000
#define CONFIG_SECURE_ROOTFS_BASE       0xC0800000
#define CONFIG_SECURE_ROOTFS_SIZE       0x100000
#endif

#endif

/* add by cym 20130218 */
/* IV_SIZE: 128 byte, 2 port(1 Gbyte), open page, trrd: 4 */
#define CONFIG_EVT0_PERFORMANCE
/* IV_SIZE: 512 Mbyte, 1 port(512 Mbyte), open page, trrd: 4 */
//#define CONFIG_EVT0_STABLE
/* IV_SIZE: 128 byte, 2 port(1 Gbyte), close page, trrd: 0xA */
//#define CONFIG_EVT0_RECOMMEND

/* (Memory Interleaving Size = 1 << IV_SIZE) */
#ifdef CONFIG_EVT0_STABLE
#define CONFIG_IV_SIZE 0x1D
#else
#define CONFIG_IV_SIZE 0x7
#endif

#define S5PV310_POWER_BASE	0x10020000

/*
 * CLOCK
 */
#define ELFIN_CLOCK_BASE		0x10030000

#define CLK_SRC_LEFTBUS_OFFSET		0x04200
#define CLK_MUX_STAT_LEFTBUS_OFFSET	0x04400
#define CLK_DIV_LEFTBUS_OFFSET		0x04500

#define CLK_SRC_RIGHTBUS_OFFSET		0x08200
#define CLK_MUX_STAT_RIGHTBUS_OFFSET	0x08400
#define CLK_DIV_RIGHTBUS_OFFSET		0x08500

#define EPLL_LOCK_OFFSET		0x0C010
#define VPLL_LOCK_OFFSET		0x0C020
#define EPLL_CON0_OFFSET		0x0C110
#define EPLL_CON1_OFFSET		0x0C114
#ifdef CONFIG_SMDKC220
#define EPLL_CON2_OFFSET		0x0C118
#endif
#define VPLL_CON0_OFFSET		0x0C120
#define VPLL_CON1_OFFSET		0x0C124
#ifdef CONFIG_SMDKC220
#define VPLL_CON2_OFFSET		0x0C128
#endif

#define CLK_SRC_TOP0_OFFSET		0x0C210
#define CLK_SRC_TOP1_OFFSET		0x0C214
#define CLK_SRC_FSYS_OFFSET		0x0C240
#define CLK_SRC_PERIL0_OFFSET		0x0C250
#define CLK_MUX_STAT_TOP_OFFSET		0x0C410
#define CLK_MUX_STAT_TOP1_OFFSET	0x0C414
#define CLK_DIV_TOP_OFFSET		0x0C510
#define CLK_DIV_FSYS1_OFFSET		0x0C544
#define CLK_DIV_FSYS2_OFFSET		0x0C548
#define CLK_DIV_FSYS3_OFFSET		0x0C54C
#define CLK_DIV_PERIL0_OFFSET		0x0C550

#define CLK_SRC_DMC_OFFSET		0x10200
#define CLK_MUX_STAT_DMC_OFFSET		0x10400
#define CLK_DIV_DMC0_OFFSET		0x10500
#define CLK_DIV_DMC1_OFFSET		0x10504

#define CLK_GATE_IP_DMC_OFFSET		0x10900

#if defined(CONFIG_SMDKC220) || defined(CONFIG_EXYNOS4412)
#define CLK_GATE_IP_PERIR_OFFSET	0x08960
#elif defined(CONFIG_SMDKC210)
#define CLK_GATE_IP_PERIR_OFFSET	0x0C960
#endif

#define APLL_LOCK_OFFSET		0x14000
#define APLL_CON0_OFFSET		0x14100
#define APLL_CON1_OFFSET		0x14104

#if defined(CONFIG_SMDKC220) || defined(CONFIG_EXYNOS4412)
#define MPLL_LOCK_OFFSET		0x10008
#define MPLL_CON0_OFFSET		0x10108
#define MPLL_CON1_OFFSET		0x1010C
#elif defined(CONFIG_SMDKC210)
#define MPLL_LOCK_OFFSET		0x14008
#define MPLL_CON0_OFFSET		0x14108
#define MPLL_CON1_OFFSET		0x1410C
#endif

#define CLK_SRC_CPU_OFFSET		0x14200
#define CLK_MUX_STAT_CPU_OFFSET		0x14400
#define CLK_DIV_CPU0_OFFSET		0x14500
#define CLK_DIV_CPU1_OFFSET		0x14504

#define CLK_SRC_FSYS		__REG(ELFIN_CLOCK_BASE+CLK_SRC_FSYS_OFFSET)
#define CLK_DIV_FSYS1		__REG(ELFIN_CLOCK_BASE+CLK_DIV_FSYS1_OFFSET)
#define CLK_DIV_FSYS2		__REG(ELFIN_CLOCK_BASE+CLK_DIV_FSYS2_OFFSET)
#define CLK_DIV_FSYS3		__REG(ELFIN_CLOCK_BASE+CLK_DIV_FSYS3_OFFSET)
#define APLL_CON0_REG		__REG(ELFIN_CLOCK_BASE+APLL_CON0_OFFSET)
#define MPLL_CON0_REG		__REG(ELFIN_CLOCK_BASE+MPLL_CON0_OFFSET)
#define EPLL_CON0_REG		__REG(ELFIN_CLOCK_BASE+EPLL_CON0_OFFSET)
#define VPLL_CON0_REG		__REG(ELFIN_CLOCK_BASE+VPLL_CON0_OFFSET)

#define USB_PHY_CONTROL_OFFSET		0x0704
#define USB_PHY_CONTROL            (0x10020000+USB_PHY_CONTROL_OFFSET)//(ELFIN_CLOCK_POWER_BASE+USB_PHY_CONTROL_OFFSET)

/* end add */

/*********************************************
 *  Configure clock
 *********************************************/
/* APLL : 800Mhz	*/
//#define CONFIG_CLK_800_330_165
/* APLL : 1GHz	*/
#define CONFIG_CLK_1000_400_200


#define CONFIG_SPARSEMEM	1		/* Sparsemem for 32 kernel */

#define BOOT_ONENAND	0x1
#define BOOT_NAND		0x40000
#define BOOT_MMCSD		0x3
#define BOOT_NOR		0x4
#define BOOT_SEC_DEV	0x5
#define BOOT_EMMC43		0x6
#define BOOT_EMMC441	0x7


#define CONFIG_BOOTM_LINUX      1

/* skip to load BL2 */
#define FAST_BOOT		1

#define CONFIG_SYS_NO_FLASH
#define MEMORY_BASE_ADDRESS	0x40000000

/* input clock of PLL */
#define CONFIG_SYS_CLK_FREQ	24000000	/* the SMDKC210 has 24MHz input clock */

/* MMU Setting  */
#define CONFIG_ENABLE_MMU

#ifdef CONFIG_ENABLE_MMU
#define virt_to_phys(x)	virt_to_phy_smdkc210(x)
#else
#define virt_to_phys(x)	(x)
#endif

#define CONFIG_MEMORY_UPPER_CODE

#undef CONFIG_USE_IRQ				/* we don't need IRQ/FIQ stuff */

#define CONFIG_INCLUDE_TEST

#define CONFIG_ZIMAGE_BOOT
#define CONFIG_IMAGE_BOOT

#define BOARD_LATE_INIT

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG

/*
 * Architecture magic and machine type
 */

#define MACH_TYPE		2838//(S5PC210:2838, S5PV310:2925)
#define UBOOT_MAGIC		(0x43090000 | MACH_TYPE)
#define CHIP_ID_BASE            0x10000000
#define PKG_ID_BASE		0x10000004

/* Power Management is enabled */
#define CONFIG_PM

#define CONFIG_TA4

//#define CONFIG_SD_UPDATE
#define CONFIG_SD_AUTOUPDATE

#ifdef CONFIG_SD_AUTOUPDATE
#define CONFIG_SD_UPDATE
#endif

//@@Robin
#ifndef CONFIG_TA4

#ifdef CONFIG_CLK_800_300_150
#define CONFIG_PM_11V   /* ARM 1.1v, INT 1.1v */
#else   /* default or CONFIG_CLK_1000_400_200 */
//#define CONFIG_PM_12V   /* ARM 1.2v, INT 1.2v */
#define CONFIG_PM_13V_12V	/* ARM 1.3v, INT 1.2v */
#endif
//@@Robin
#endif

#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

#undef CONFIG_SKIP_RELOCATE_UBOOT
#undef CONFIG_USE_NOR_BOOT

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + 1024*1024)
#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

#define CFG_STACK_SIZE		512*1024

/*
 * select serial console configuration
 */
#ifndef CONFIG_TA4
#define CONFIG_SERIAL2          1	/* we use UART1 on SMDKC210 */
#else
#define CONFIG_SERIAL3          1	/* we use UART2 on TA4 */
#endif
//#define CONFIG_SERIAL4		1

#define CFG_HUSH_PARSER			/* use "hush" command parser	*/
#ifdef CFG_HUSH_PARSER
#define CFG_PROMPT_HUSH_PS2	"> "
#endif

#define CONFIG_CMDLINE_EDITING

#undef CONFIG_S3C64XX_I2C		/* this board has H/W I2C */
#ifdef CONFIG_S3C64XX_I2C
#define CONFIG_HARD_I2C		1
#define CFG_I2C_SPEED		50000
#define CFG_I2C_SLAVE		0xFE
#endif

#define CONFIG_DOS_PARTITION
#define CONFIG_SUPPORT_VFAT

#define CONFIG_USB_OHCI
#undef CONFIG_USB_STORAGE
#define CONFIG_S3C_USBD

#define USBD_DOWN_ADDR		0xc0000000

/************************************************************
 * RTC
 ************************************************************/
/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_BAUDRATE		115200

/***********************************************************
 * Command definition
 ***********************************************************/
//#define CONFIG_CMD_CACHE
//#define CONFIG_CMD_USB
//#define CONFIG_CMD_REGINFO

//#define	CONFIG_CMD_NAND
//#define	CONFIG_CMD_FLASH

//#define CONFIG_CMD_ONENAND
#define CONFIG_CMD_MOVINAND

//#define CONFIG_CMD_PING
//#define CONFIG_CMD_DATE

#include <config_cmd_default.h>

#define CONFIG_CMD_ELF
//#define CONFIG_CMD_I2C

#define CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_FAT

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	115200		/* speed to run kgdb serial port */
/* what's this ? it's not used anywhere */
#define CONFIG_KGDB_SER_INDEX	1		/* which serial port to use */
#endif

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP                             /* undef to save memory         */
#define CONFIG_SYS_PROMPT              "iTOP-4412 # "    /* Monitor Command Prompt       */

#define CONFIG_SYS_CBSIZE		256		/* Console I/O Buffer Size	*/
#define CONFIG_SYS_PBSIZE		384		/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS		16		/* max number of command args	*/
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size	*/

#define CONFIG_SYS_MEMTEST_START	MEMORY_BASE_ADDRESS	/* memtest works on	*/

#define CONFIG_SYS_MEMTEST_END		MEMORY_BASE_ADDRESS + 0x3E00000		/* 256 MB in DRAM	*/

#undef CFG_CLKS_IN_HZ		/* everything, incl board info, in Hz */

#define CONFIG_SYS_LOAD_ADDR		MEMORY_BASE_ADDRESS + 0x00100000	/* default load address	*/

/* the PWM TImer 4 uses a counter of 41687 for 10 ms, so we need */
/* it to wrap 100 times (total 4168750) to get 1 sec. */
#define CONFIG_SYS_HZ			4168750		// at PCLK 66MHz

/* valid baudrates */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */

/* MIU Setting  */
//#define CONFIG_MIU_1BIT_INTERLEAVED

#define CONFIG_STACKSIZE	0x40000		/* regular stack 256KB */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif

// SMDKC210 POP-A(512MB)   : 2 banks
// SMDKC210 POP-B(1024MB)  : 4 banks
#define CONFIG_NR_DRAM_BANKS    8          	/* 8 banks of DRAM at maximum */
#define SDRAM_BANK_SIZE         0x10000000/2	/* each bank has 256 MB */
#define PHYS_SDRAM_1            (unsigned long)MEMORY_BASE_ADDRESS /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE       (unsigned long)SDRAM_BANK_SIZE
#define PHYS_SDRAM_2            (unsigned long)(MEMORY_BASE_ADDRESS + SDRAM_BANK_SIZE) /* SDRAM Bank #2 */
#define PHYS_SDRAM_2_SIZE       (unsigned long)SDRAM_BANK_SIZE
#define PHYS_SDRAM_3            (unsigned long)(MEMORY_BASE_ADDRESS + 2 * SDRAM_BANK_SIZE) /* SDRAM Bank #2 */
#define PHYS_SDRAM_3_SIZE       (unsigned long)SDRAM_BANK_SIZE
#define PHYS_SDRAM_4            (unsigned long)(MEMORY_BASE_ADDRESS + 3 * SDRAM_BANK_SIZE) /* SDRAM Bank #2 */
#define PHYS_SDRAM_4_SIZE       (unsigned long)SDRAM_BANK_SIZE
#define PHYS_SDRAM_5            (unsigned long)(MEMORY_BASE_ADDRESS + 4 * SDRAM_BANK_SIZE) /* SDRAM Bank #2 */
#define PHYS_SDRAM_5_SIZE       (unsigned long)SDRAM_BANK_SIZE
#define PHYS_SDRAM_6            (unsigned long)(MEMORY_BASE_ADDRESS + 5 * SDRAM_BANK_SIZE) /* SDRAM Bank #2 */
#define PHYS_SDRAM_6_SIZE       (unsigned long)SDRAM_BANK_SIZE
#define PHYS_SDRAM_7            (unsigned long)(MEMORY_BASE_ADDRESS + 6 * SDRAM_BANK_SIZE) /* SDRAM Bank #2 */
#define PHYS_SDRAM_7_SIZE       (unsigned long)SDRAM_BANK_SIZE
#define PHYS_SDRAM_8            (unsigned long)(MEMORY_BASE_ADDRESS + 7 * SDRAM_BANK_SIZE) /* SDRAM Bank #2 */
#define PHYS_SDRAM_8_SIZE       (unsigned long)SDRAM_BANK_SIZE




#define CFG_FLASH_BASE		0x80000000

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CONFIG_MX_LV640EB		/* MX29LV640EB */
//#define CONFIG_AMD_LV800		/* AM29LV800BB */

#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */

#if	defined(CONFIG_MX_LV640EB)
#define CFG_MAX_FLASH_SECT	135
#define PHYS_FLASH_SIZE		0x800000	/* 8MB */
#elif	defined(CONFIG_AMD_LV800)
#define CFG_MAX_FLASH_SECT	19
#define PHYS_FLASH_SIZE		0x100000	/* 1MB */
#else
#define CFG_MAX_FLASH_SECT	512
#define PHYS_FLASH_SIZE		0x100000	/* 1MB */
#endif

#define CFG_FLASH_LEGACY_512Kx16
//#define CONFIG_FLASH_CFI_LEGACY
#define CFG_FLASH_CFI

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT	(5*CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(5*CFG_HZ) /* Timeout for Flash Write */

#define CFG_ENV_ADDR		0
#define CONFIG_ENV_SIZE		0x4000	/* Total Size of Environment Sector */

/*
 * SMDKC210 board specific data
 */

#define CONFIG_IDENT_STRING	" for iTOP-4412 Android"

/* total memory required by uboot */
#define CFG_UBOOT_SIZE		(2*1024*1024)

 /* base address for uboot */
//#ifdef CONFIG_ENABLE_MMU
//#define CFG_UBOOT_BASE		0xc3e00000
//#else
#define CFG_UBOOT_BASE		0x43e00000
//#endif

#define CFG_PHY_UBOOT_BASE	MEMORY_BASE_ADDRESS + 0x3e00000
#define CFG_PHY_KERNEL_BASE	MEMORY_BASE_ADDRESS + 0x8000

#define CONFIG_ENV_OFFSET	0x0007C000

/* nand copy size from nand to DRAM.*/
#define	COPY_BL2_SIZE		0x80000

/* NAND configuration */
#define CONFIG_SYS_MAX_NAND_DEVICE     1
#define CONFIG_SYS_NAND_BASE           (0x0CE00000)
#define NAND_MAX_CHIPS          1

#define NAND_DISABLE_CE()	(NFCONT_REG |= (1 << 1))
#define NAND_ENABLE_CE()	(NFCONT_REG &= ~(1 << 1))
#define NF_TRANSRnB()		do { while(!(NFSTAT_REG & (1 << 0))); } while(0)

#define CFG_NAND_SKIP_BAD_DOT_I	1  /* ".i" read skips bad blocks   */
#define	CFG_NAND_WP		1
#define CFG_NAND_YAFFS_WRITE	1  /* support yaffs write */

#define CFG_NAND_HWECC
#undef	CFG_NAND_FLASH_BBT

/* IROM specific data */
#define SDMMC_BLK_SIZE        (0xD003A500)
#define COPY_SDMMC_TO_MEM     (0xD003E008)

/*
 *  Fast Boot 
*/
/* Fastboot variables */
#define CFG_FASTBOOT_TRANSFER_BUFFER            (0x48000000)//(0x50000000)
#define CFG_FASTBOOT_TRANSFER_BUFFER_SIZE       (0x36000000)//(0x10000000)   /* 256MB */
#define CFG_FASTBOOT_ADDR_KERNEL                (0x40008000)
#define CFG_FASTBOOT_ADDR_RAMDISK               (0x40800000)
#define CFG_FASTBOOT_PAGESIZE                   (2048)  // Page size of booting device
#define CFG_FASTBOOT_SDMMC_BLOCKSIZE            (512)   // Block size of sdmmc

/* Just one BSP type should be defined. */
#if defined(CONFIG_CMD_ONENAND) | defined(CONFIG_CMD_NAND) | defined(CONFIG_CMD_MOVINAND)
#define CONFIG_FASTBOOT
#endif

#if defined(CONFIG_CMD_NAND)
#define CFG_FASTBOOT_NANDBSP
#endif
#if defined(CONFIG_CMD_ONENAND)
#define CFG_FASTBOOT_ONENANDBSP
#endif
#if defined(CONFIG_CMD_MOVINAND)
#define CFG_FASTBOOT_SDMMCBSP
#endif

#if defined(CONFIG_CMD_MOVINAND)
/* SD/MMC configuration */
#define CONFIG_MMC
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
 

#ifdef CONFIG_EVT1
//Select one among 3 mode, default is CONFIG_SDMMC_CH2
#define CONFIG_SDMMC_CH2 //SDMMC_CH2 (OMPIN[5:1] = 2
//#define CONFIG_EMMC43_CH0 //eMMC43_CH0 (OMPIN[5:1] = 3)
#define CONFIG_EMMC44_CH4 //eMMC44_CH4 (OMPIN[5:1] = 4)

#ifdef CONFIG_SDMMC_CH2
#define CONFIG_S3C_HSMMC
#undef DEBUG_S3C_HSMMC
#define USE_MMC2  
#endif

#ifdef CONFIG_EMMC44_CH4
#define CONFIG_S5P_MSHC
#define CONFIG_EMMC		1		
#define USE_MMC4  
//#define CONFIG_EMMC_8Bit
#define CONFIG_EMMC_EMERGENCY
#endif

#endif //config_evt1

#define MMC_MAX_CHANNEL                5
 
#endif

#define CONFIG_FASTBOOT_SDFUSE 

/*
 * SD/MMC detection takes a little long time
 * So, you can ignore detection process for SD/MMC card
 */
#undef	CONFIG_NO_SDMMC_DETECTION

#define CONFIG_MTDPARTITION	"80000 400000 3000000"
#define CONFIG_BOOTDELAY	2

//#if defined (CONFIG_CMD_MOVINAND)
//#define CONFIG_BOOTCOMMAND      "movi read kernel 40008000;movi read rootfs 40800000 100000;bootm 40008000 40800000"
//#else
#define CONFIG_BOOTCOMMAND	"onenand read 40008000 600000 300000;onenand read 40800000 b00000 100000;bootm 40008000 40800000"//"movi read kernel c0008000;bootm c0008000"
//#endif

/* OneNAND configuration */
#define CFG_ONENAND_BASE 	(0x0C000000)
#define CFG_ONENANDXL_BASE      (0x0C600000)
#define CFG_MAX_ONENAND_DEVICE	1
#define CONFIG_SYS_ONENAND_BASE CFG_ONENAND_BASE
   
#define CONFIG_BOOT_ONENAND_IROM
//#define CONFIG_NAND
//#define CONFIG_BOOT_NAND
#define CONFIG_ONENAND
#define ONENAND_REG_DBS_DFS_WIDTH 	(0x160)
#define ONENAND_REG_FLASH_AUX_CNTRL     (0x300)

#define GPNCON_OFFSET		0x830
#define GPNDAT_OFFSET		0x834
#define GPNPUD_OFFSET		0x838

#define CONFIG_ENV_IS_IN_AUTO
//#define CONFIG_ENV_IS_IN_NAND


#define CONFIG_4212_AP10_BOOTLOADER "u-boot-exynos4212-evt0-nonfused.bin"
#define CONFIG_4212_AP11_BOOTLOADER "u-boot-exynos4212-evt1-efused.bin"
#define CONFIG_4412_BOOTLOADER "u-boot-iTOP-4412.bin"
#define CONFIG_4412_DVT_BOOTLOADER "u-boot-iTOP-4412.bin"

#define CONFIG_RECOVERY
#define FACTORY_RESET_MODE 	0xC1	
#define CHARGING_RESET_MODE	0xC2 
//#define RECOVERY_MODE 		0xC3

#define CONFIG_TRUSTZONE

#define DEBUG_TC4
#undef DEBUG_TC4
#ifdef DEBUG_TC4

#define emmcdbg(fmt,args...) printf(fmt ,##args) //for emmc dbg
#define sddbg(fmt,args...) printf(fmt ,##args) //for emmc dbg
#else
#define emmcdbg(fmt,args...)
#define sddbg(fmt,args...)
#endif
#endif	/* __CONFIG_H */
