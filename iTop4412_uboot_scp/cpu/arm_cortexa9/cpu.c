/*
 * (C) Copyright 2004 Texas Insturments
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
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

/*
 * CPU specific code
 */

#include <common.h>
#include <command.h>
#include <s5pc210.h>

#ifdef CONFIG_S5PC210S
#include "s5pc210/UBOOT_SB20_S5PC210S.h"
#define	BL1_SIGNATURE_ADDR			(0x02023800)
#endif

void clear_hsmmc_clock_div(void);

#ifdef CONFIG_USE_IRQ
DECLARE_GLOBAL_DATA_PTR;
#endif

/* read co-processor 15, register #1 (control register) */
static unsigned long read_p15_c1 (void)
{
	unsigned long value;

	__asm__ __volatile__(
						"mrc	p15, 0, %0, c1, c0, 0   @ read control reg\n"
						: "=r" (value)
						:
						: "memory");
	return value;
}

/* write to co-processor 15, register #1 (control register) */
static void write_p15_c1 (unsigned long value)
{
	__asm__ __volatile__(
						"mcr	p15, 0, %0, c1, c0, 0   @ write it back\n"
						:
						: "r" (value)
						: "memory");

	read_p15_c1 ();
}

static void cp_delay (void)
{
	volatile int i;

	/* Many OMAP regs need at least 2 nops  */
	for (i = 0; i < 100; i++);
}

/* See also ARM Ref. Man. */
#define C1_MMU		(1<<0)		/* mmu off/on */
#define C1_ALIGN	(1<<1)		/* alignment faults off/on */
#define C1_DC		(1<<2)		/* dcache off/on */
#define C1_WB		(1<<3)		/* merging write buffer on/off */
#define C1_BIG_ENDIAN	(1<<7)	/* big endian off/on */
#define C1_SYS_PROT	(1<<8)		/* system protection */
#define C1_ROM_PROT	(1<<9)		/* ROM protection */
#define C1_BRANCH	(1<<11)		/* branch prediction off/on */
#define C1_IC		(1<<12)		/* icache off/on */
#define C1_HIGH_VECTORS	(1<<13)	/* location of vectors: low/high addresses */
#define RESERVED_1	(0xf << 3)	/* must be 111b for R/W */

//------------------------------------------------------
#define CSn_0 0
#define CSn_1 1
#define CSn_2 2
#define CSn_3 3
#define Inp32(x) *((unsigned int *)(x))
#define Outp32(x, y) *((unsigned int *)(x)) = y

#define rMP0_0CON 0x11000120
#define rMP0_1CON 0x11000140
#define rMP0_2CON 0x11000160
#define rMP0_5CON 0x110001C0
#define rMP0_0DRV_SR 0x1100012C
#define rMP0_1DRV_SR 0x1100014C
#define rMP0_2DRV_SR 0x1100016C
#define rMP0_5DRV_SR 0x110001CC


void InitBaseHwForNfc(void)
{
	int eCS = 0;
	unsigned int uReg;
	unsigned char uShift;
 
	// set control signal
 	switch(eCS)
	{
		case CSn_0:
			uShift = 8;
			break;

		case CSn_1:
			uShift = 12;
			break;

		case CSn_2:
			uShift = 0;
			break;

		case CSn_3:
			uShift = 4;
			break;

		default:
		       return;
			break;
	}
	uReg = Inp32(rMP0_0CON);
	uReg &= ~((0xf<<uShift)|(0xf<<20)|(0xf<<16));
	uReg |= ((0x3<<uShift)|(0x2<<20)|(0x2<<16)); //NF_CSn[0], NF_CSn[1], NF_CSn[2], NF_CSn[3], EBI_OEn, EBI_WEn
	Outp32(rMP0_0CON , uReg);
	
	uReg = Inp32(rMP0_1CON);
	uReg &= ~((0xf<<12));
	uReg |= (0x2<<12);
	Outp32(rMP0_1CON , uReg);		//EBI_DATA_RDn to select NF_CSn0,1,2,3

 	uShift = (4*eCS + 8);
	uReg = Inp32(rMP0_2CON);
	uReg &= ~((0xf<<uShift)|(0xf<<4)|(0xf<<0));
	uReg |= ((0x2<<uShift)|(0x2<<4)|(0x2<<0));		//NF_CLE, NF_ALE, NF_RnB[0], NF_RnB[1], NF_RnB[2], NF_RnB[3]
	Outp32(rMP0_2CON , uReg);	
	
#if 0
	uShift = (2*eCS + 4);
	uReg = Inp32(rMP0_2PUD);
	uReg &= ~((0x3<<uShift));
	uReg |= ((0x3<<uShift));			//NF_RnB Pull-up Enable		
	Outp32(rMP0_2PUD , uReg);	
#endif
	// set data signal
	Outp32(rMP0_5CON , 0x22222222);	


	// set drive strength	 => x3
 	switch(eCS)
	{
		case CSn_0:
			uShift = 4;
			break;

		case CSn_1:
			uShift = 6;
			break;

		case CSn_2:
			uShift = 0;
			break;

		case CSn_3:
			uShift = 2;
			break;

		default:
			break;
	}
	uReg = Inp32(rMP0_0DRV_SR);
	uReg &= ~((0x3<<uShift));
	uReg &= ((0x1<<uShift));
	Outp32(rMP0_0DRV_SR , uReg);
	
	uReg = Inp32(rMP0_1DRV_SR);
	uReg &= ~((0x3<<6));
	uReg |= (0x1<<6);
	Outp32(rMP0_1DRV_SR , uReg);		//EBI_DATA_RDn => x3

 	uShift = (2*eCS + 4);
	uReg = Inp32(rMP0_2DRV_SR);
	uReg &= ~((0x3<<uShift));
	uReg |= ((0x1<<uShift) | (0x1<<2) | (0x1<<0));
	Outp32(rMP0_2DRV_SR , uReg);			//NF_CLE, NF_ALE, NF_RnB[0], NF_RnB[1], NF_RnB[2], NF_RnB[3] => x3
	
	Outp32(rMP0_5DRV_SR , 0x5555);	// EBI_DATA [   7:0] 	 => x3
	
}

#define CLOCK_TICK_RATE 10000000

static unsigned int uart_base = 0x13810000;

void _console_write(const char *s,
			     unsigned int count)
{
	int i;
	volatile unsigned int *base_addr = (volatile unsigned int *)uart_base;
	for(i=0;i<count;i++)
	{
		//while(!(base_addr[4] & (1<<1))){} //if fifo is empty, let's break
		while((base_addr[6] & (1<<24))){} // if fifo is not full, let's break
		base_addr[8]  = s[i];	
		while((base_addr[6] & (1<<24))){} // if fifo is not full, let's break
		if(s[i]=='\n')base_addr[8]='\r';
	}
}

int _console_setup(void)
{
	volatile unsigned int *base_addr=(volatile unsigned int *)uart_base;

	base_addr[0] = 3;
	base_addr[2]=0x1;
	base_addr[3]=0;
	base_addr[4]=0;
	base_addr[7]=0;

	base_addr[10] = CLOCK_TICK_RATE/(115200*16) -1;
	//base_addr[11] = getSlot((((CLOCK_TICK_RATE*10/(115200*16) -10))%10)*16/10);
	base_addr[11] = (((CLOCK_TICK_RATE*10/(115200*16) -10))%10)*16/10;

	base_addr[1]=5;
	base_addr[2]=0x7; //reset tx/rx fifo
	base_addr[0xc] = base_addr[0xc]; //pending clear	

	base_addr[0xe] = 0xf; // mask all interrupt source

	
	return 0;
}
//------------------------------------------------------

int cpu_init (void)
{
//------------------------------------------------------
//	_console_setup();

//	_console_write("uart", 4);
//------------------------------------------------------
	
	/*
	 * setup up stacks if necessary
	 */
#ifdef CONFIG_USE_IRQ
	IRQ_STACK_START = _armboot_start - CFG_MALLOC_LEN - CFG_GBL_DATA_SIZE - 4;
	FIQ_STACK_START = IRQ_STACK_START - CONFIG_STACKSIZE_IRQ;
#endif
	return 0;
}

int cleanup_before_linux (void)
{
	/*
	 * this function is called just before we call linux
	 * it prepares the processor for linux
	 *
	 * we turn off caches etc ...
	 */

	unsigned long i;

	disable_interrupts ();

	/* turn off I/D-cache */
	asm ("mrc p15, 0, %0, c1, c0, 0":"=r" (i));
	i &= ~(C1_DC | C1_IC);
	asm ("mcr p15, 0, %0, c1, c0, 0": :"r" (i));

	/* invalidate I-cache */
	arm_cache_flush();

	i = 0;
	/* mem barrier to sync up things */
	asm("mcr p15, 0, %0, c7, c10, 4": :"r"(i));

	clear_hsmmc_clock_div();

	return(0);
}


/* * reset the cpu by setting up the watchdog timer and let him time out */
void reset_cpu(ulong ignored)
{
	printf("reset... \n\n\n");

	SW_RST_REG = 0x1;

	/* loop forever and wait for reset to happen */
	while (1)
	{
		if (serial_tstc())
		{
			serial_getc();
			break;
		}
	}
	/*NOTREACHED*/
}

int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	disable_interrupts ();
	reset_cpu (0);
	/*NOTREACHED*/
	return(0);
}

void icache_enable (void)
{
	ulong reg;

	reg = read_p15_c1 ();	/* get control reg. */
	cp_delay ();
	write_p15_c1 (reg | C1_IC);
}

void icache_disable (void)
{
	ulong reg;

	reg = read_p15_c1 ();
	cp_delay ();
	write_p15_c1 (reg & ~C1_IC);
}

int icache_status (void)
{
	return(read_p15_c1 () & C1_IC) != 0;
}

/* It makes no sense to use the dcache if the MMU is not enabled */
void dcache_enable (void)
{
	ulong reg;

	reg = read_p15_c1 ();
	cp_delay ();
	write_p15_c1 (reg | C1_DC);
}

void dcache_disable (void)
{
	ulong reg;

	reg = read_p15_c1 ();
	cp_delay ();
	reg &= ~C1_DC;
	write_p15_c1 (reg);
}

int dcache_status (void)
{
	return (read_p15_c1 () & C1_DC) != 0;
}

void branch_enable (void)
{
	ulong reg;

	reg = read_p15_c1 ();
	cp_delay ();
	write_p15_c1 (reg | C1_BRANCH);
}

void branch_disable (void)
{
	ulong reg;

	reg = read_p15_c1 ();
	cp_delay ();
	reg &= ~C1_BRANCH;
	write_p15_c1 (reg);
}

int branch_status (void)
{
	return (read_p15_c1 () & C1_BRANCH) != 0;
}

#ifdef CONFIG_SECURE_BOOT
void security_check(void)
{
	/* do security check */
	if(Check_Signature( (SB20_CONTEXT *)BL1_SIGNATURE_ADDR,
			(unsigned char*)CONFIG_SECURE_KERNEL_BASE, 
			CONFIG_SECURE_KERNEL_SIZE-256,
			(unsigned char*)(CONFIG_SECURE_KERNEL_BASE+CONFIG_SECURE_KERNEL_SIZE-256),
			256 ) != 0) {
		printf("Kernel Integrity check fail\nSystem Halt....");
		while(1);
	}
	printf("Kernel Integirty check success.\n");

#ifdef CONFIG_SECURE_ROOTFS
	if(Check_Signature( (SB20_CONTEXT *)BL1_SIGNATURE_ADDR,
			(unsigned char*)CONFIG_SECURE_ROOTFS_BASE,
			CONFIG_SECURE_ROOTFS_SIZE-256,
			(unsigned char*)(CONFIG_SECURE_ROOTFS_BASE+CONFIG_SECURE_ROOTFS_SIZE-256),
			256 ) != 0) {
		printf("rootfs Integrity check fail\nSystem Halt....");
		while(1);
	}
	printf("rootfs Integirty check success.\n");
#endif
}
#endif

/*
 * Initializes on-chip MMC controllers.
 * to override, implement board_mmc_init()
 */
int cpu_mmc_init(bd_t *bis)
{
	int ret;
#if defined(CONFIG_S3C_HSMMC) || defined(CONFIG_S5P_MSHC)
	setup_hsmmc_clock();
	setup_hsmmc_cfg_gpio();
#endif
#ifdef USE_MMC4
	ret = smdk_s5p_mshc_init();
#endif
#ifdef USE_MMC2
	ret = smdk_s3c_hsmmc_init();
#endif
	return ret;
}

