/*
 * Memory Setup stuff - taken from blob memsetup.S
 *
 * Copyright (C) 1999 2000 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl) and
 *                     Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
 *
 * Modified for the Samsung SMDK2410 by
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
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


#include <config.h>
#include <version.h>

#include <s5pc210.h>
#include "smdk4x12_val.h" //mj

#define check_mem
#define MEM_DLLl_ON

_TEXT_BASE:
	.word	TEXT_BASE

	.globl lowlevel_init
lowlevel_init:
#if 1//*****ly
	/* use iROM stack in bl2 */
	ldr	sp, =0x02060000
#endif
	push	{lr}

	/* check reset status  */
	ldr     r0, =(INF_REG_BASE + INF_REG1_OFFSET)
        ldr     r1, [r0]

	/* AFTR wakeup reset */
	ldr	r2, =S5P_CHECK_DIDLE
	cmp	r1, r2
	beq	exit_wakeup

	/* Sleep wakeup reset */
	ldr	r2, =S5P_CHECK_SLEEP
	cmp	r1, r2
	beq	wakeup_reset

        /* PS-Hold high */
        ldr r0, =0x1002330c
        ldr r1, [r0]
        orr r1, r1, #0x300
        str r1, [r0]

        ldr     r0, =0x11000c08
        ldr r1, =0x0
        str r1, [r0]

        /* Clear  MASK_WDT_RESET_REQUEST  */
        ldr r0, =0x1002040c
        ldr r1, =0x00
        str r1, [r0]
        
#ifdef check_mem /*liyang 20110822*/
	/* when we already run in ram, we don't need to relocate U-Boot.
	 * and actually, memory controller must be configured before U-Boot
	 * is running in ram.
	 */
	ldr	r0, =0xff000fff
	bic	r1, pc, r0		/* r0 <- current base addr of code */
	ldr	r2, _TEXT_BASE		/* r1 <- original base addr in ram */
	bic	r2, r2, r0		/* r0 <- current base addr of code */
	cmp     r1, r2                  /* compare r0, r1                  */
	beq     1f			/* r0 == r1 then skip sdram init   */
#endif

/*	liyang 20110822
	
	ldr	r0, =CHIP_ID_BASE
	ldr	r1, [r0]
	lsr	r1, r1, #8
	and	r1, r1, #3
	cmp	r1, #2
	bne v310_1

*/

	/* Memory initialize */
	bl mem_ctrl_asm_init


	/* init system clock */
	bl system_clock_init


	bl tzpc_init
	
	b	1f

v310_1:
	/* init system clock */
	bl	system_clock_init

	/* Memory initialize	*/
	bl	mem_ctrl_asm_init

1:

#ifdef CONFIG_EVT1___
	/* store DMC density information in DRAM */
	/* mem_ctrl_asm_init returns dmc_density in r6 */
	ldr	r0, =CFG_UBOOT_BASE
	sub	r0, r0, #4
	str	r6, [r0]
#endif
	/*wenpin.cui: headphone and sw uart switch init*/
	ldr	r0, =0x11000C44
	ldr	r1, [r0]
	and	r1, r1, #0x4 	
	cmp	r1, #0x4	/*uart*/	
	beq	out	

	ldr     r0, =0x11400084  /* GPC1(0)  */
	ldr     r1, [r0]	/* read GPC1DAT status*/
	orr	r1, r1, #0x1	/* GPC1(0) output high  */
	str     r1, [r0]

	ldr     r0, =0x11400080  /* GPC1(0)  */
	ldr	r1, [r0]
	and	r1, r1, #0xfffffff0
	orr     r1, r1, #0x1	/* GPC1(0) output  */
	str     r1, [r0]
out:
	/* for UART */
	bl uart_asm_init

       

#if defined(CONFIG_ONENAND)
	bl onenandcon_init
#endif

#if defined(CONFIG_NAND)
	/* simple init for NAND */
	bl nand_asm_init
#endif

	/* Print 'K' */
	ldr	r0, =ELFIN_UART_CONSOLE_BASE
	ldr	r1, =0x4b4b4b4b
	str	r1, [r0, #UTXH_OFFSET]

	/* 2010.08.27 by icarus : for temporary 3D clock fix */
	ldr	r1, =0x1
	ldr	r2, =0x1003C22C
	str	r1, [r2]
	ldr	r1, =0x2
	ldr	r2, =0x1003C52C
	str	r1, [r2]

	/* 2010.10.17 by icarus : for temporary MFC clock fix */
	ldr	r1, =0x3
	ldr	r2, =0x1003C528
	str	r1, [r2]

	pop	{pc}

wakeup_reset:
	ldr	r0, =CHIP_ID_BASE
	ldr	r1, [r0]
	lsr	r1, r1, #8
	and	r1, r1, #3
	cmp	r1, #2
	bne	wake_v310
	
	//mj 
	/* check C2C_CTRL enable bit */
	ldr r3, =SYSC_PMU_BASE
	ldr r1, [r3, #C2C_CTRL_OFFSET]
	and r1, r1, #1
	cmp r1, #0
	bne skip_dmc
	
	/* init system clock */
	bl	mem_ctrl_asm_init
skip_dmc:
	bl	system_clock_init
	bl	tzpc_init
	b	exit_wakeup

wake_v310:
	bl system_clock_init
	bl mem_ctrl_asm_init
	bl tzpc_init

exit_wakeup:
	/*Load return address and jump to kernel*/
	ldr	r0, =(INF_REG_BASE+INF_REG0_OFFSET)
	ldr	r1, [r0]	/* r1 = physical address of s5pc110_cpu_resume function*/

	mov	pc, r1		/*Jump to kernel */
	nop
	nop

/*
 * system_clock_init: Initialize core clock and bus clock.
 * void system_clock_init(void)
 */
system_clock_init:
	push	{lr}
	
	ldr	r0, =CMU_BASE	@0x1003_0000

@ CMU_CPU MUX / DIV
	ldr	r1, =0x0
	ldr	r2, =CLK_SRC_CPU_OFFSET
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x10000
1:	subs	r1, r1, #1
	bne	1b
/* liyang removed it
	ldr	r1, =CLK_DIV_CPU0_VAL
	ldr	r2, =CLK_DIV_CPU0_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =CLK_DIV_CPU1_VAL
	ldr	r2, =CLK_DIV_CPU1_OFFSET
	str	r1, [r0, r2]

*/
@ CMU_DMC MUX / DIV
#ifdef CONFIG_EVT1
	ldr	r1, =0x10000
#else
	ldr	r1, =CLK_SRC_DMC_VAL
#endif
	ldr	r2, =CLK_SRC_DMC_OFFSET
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x10000
2:	subs	r1, r1, #1
	bne	2b

	ldr	r1, =0x00111113//CLK_DIV_DMC0_VAL
	ldr	r2, =CLK_DIV_DMC0_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =0x00551113//CLK_DIV_DMC1_VAL
	ldr	r2, =CLK_DIV_DMC1_OFFSET
	str	r1, [r0, r2]

	ldr	r1, =0x01010100
	ldr	r2, =CLK_DIV_DMC1_OFFSET
	str	r1, [r0, r2]
	
///////////////////////////////////////////////
@ CMU_TOP MUX / DIV
	ldr	r1, =0x0//CLK_SRC_TOP0_VAL
	ldr	r2, =CLK_SRC_TOP0_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =0x0//CLK_SRC_TOP1_VAL
	ldr	r2, =CLK_SRC_TOP1_OFFSET
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x10000
3:	subs	r1, r1, #1
	bne	3b

	ldr	r1, =0x01315474//for ISP,ACLK_200,ACLK_400_MCUISP,0x03315473//CLK_DIV_TOP_VAL
	ldr	r2, =CLK_DIV_TOP_OFFSET
	str	r1, [r0, r2]

@ CMU_LEFTBUS MUX / DIV
	ldr	r1, =0x10//CLK_SRC_LEFTBUS_VAL
	ldr	r2, =CLK_SRC_LEFTBUS_OFFSET
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x10000
4:	subs	r1, r1, #1
	bne	4b

	ldr	r1, =0x00000013//CLK_DIV_LEFRBUS_VAL
	ldr	r2, =CLK_DIV_LEFTBUS_OFFSET
	str	r1, [r0, r2]

@ CMU_RIGHTBUS MUX / DIV
	ldr	r1, =0x10//CLK_SRC_RIGHTBUS_VAL
	ldr	r2, =CLK_SRC_RIGHTBUS_OFFSET
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x10000
5:	subs	r1, r1, #1
	bne	5b

	ldr	r1, =0x00000013//CLK_DIV_RIGHTBUS_VAL
	ldr	r2, =CLK_DIV_RIGHTBUS_OFFSET
	str	r1, [r0, r2]

@ Set PLL locktime
	ldr	r1, =APLL_LOCK_VAL
	ldr	r2, =APLL_LOCK_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =MPLL_LOCK_VAL
	ldr	r2, =MPLL_LOCK_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =EPLL_LOCK_VAL
	ldr	r2, =EPLL_LOCK_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =VPLL_LOCK_VAL
	ldr	r2, =VPLL_LOCK_OFFSET
	str	r1, [r0, r2]

	

	ldr	r1, =CLK_DIV_CPU0_VAL
	ldr	r2, =CLK_DIV_CPU0_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =CLK_DIV_CPU1_VAL
	ldr	r2, =CLK_DIV_CPU1_OFFSET
	str	r1, [r0, r2]


	  
@ Set PLL P,M,S ON
@ Set APLL
	ldr	r1, =APLL_CON1_VAL
	ldr	r2, =APLL_CON1_OFFSET
	str	r1, [r0, r2]

	ldr	r1, =APLL_CON0_VAL
	ldr	r2, =APLL_CON0_OFFSET
	str	r1, [r0, r2]

@ Set MPLL//liyang
	ldr	r1, =MPLL_CON1_VAL
	ldr	r2, =MPLL_CON1_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =MPLL_CON0_VAL
	ldr	r2, =MPLL_CON0_OFFSET
	str	r1, [r0, r2]

@ Set EPLL//liyang
	ldr	r1, =EPLL_CON2_VAL
	ldr	r2, =EPLL_CON2_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =EPLL_CON1_VAL
	ldr	r2, =EPLL_CON1_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =EPLL_CON0_VAL
	ldr	r2, =EPLL_CON0_OFFSET
	str	r1, [r0, r2]

@ Set VPLL//liyang
	ldr	r1, =VPLL_CON2_VAL
	ldr	r2, =VPLL_CON2_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =VPLL_CON1_VAL
	ldr	r2, =VPLL_CON1_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =VPLL_CON0_VAL
	ldr	r2, =VPLL_CON0_OFFSET
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x30000
6:	subs	r1, r1, #1
	bne	6b

	ldr	r1, =CLK_SRC_CPU_VAL_MOUTMPLLFOUT
	ldr	r2, =CLK_SRC_CPU_OFFSET
	str	r1, [r0, r2]




	//d.s SD:0x10040200 %LE %LONG 0x00111000 ;CLK_SRC_DMC	MUX_MPLL out => PLL out, DMC & C2C
	//d.s SD:0x10040504 %LE %LONG 0x01011171 ;CLK_SRC_DMC1
	//d.s SD:0x1003C210 %LE %LONG 0x00000110 ;CLK_SRC_TOP0	MUX_E/VPLL out => PLL out, BUS(Main BUS)
	//d.s SD:0x1003C214 %LE %LONG 0x01101000 ;CLK_SRC_TOP1	BUS(ISP,GPS)


	ldr	r1, =0x00111000//CLK_SRC_CPU_VAL_MOUTMPLLFOUT
	ldr	r2, =0x10040200
	str	r1, [r2]

	
	ldr	r1, =0x01011171//CLK_SRC_CPU_VAL_MOUTMPLLFOUT
	ldr	r2, =0x10040504
	str	r1, [r2]

		
	ldr	r1, =0x00000110//CLK_SRC_CPU_VAL_MOUTMPLLFOUT
	ldr	r2, =0x1003C210
	str	r1, [r2]

	ldr	r1, =0x01101000//CLK_SRC_CPU_VAL_MOUTMPLLFOUT
	ldr	r2, =0x1003C214
	str	r1, [r2]
	
	
	/* wait ?us */
	mov	r1, #0x10000
7:	subs	r1, r1, #1
	bne	7b
	//d.s SD:0x1003C52C %LE %LONG 0x00000003 ;CLK_DIV_G3D  
	//d.s SD:0x1003C22C %LE %LONG 0x00000000 ;CLK_SRC_G3D	SCLKMPLL

	//d.s SD:0x1003C528 %LE %LONG 0x00000003 ;CLK_DIV_MFC  
	//d.s SD:0x1003C228 %LE %LONG 0x00000000 ;CLK_SRC_G3D	SCLKMPLL	


	ldr	r1, =0x00000003//CLK_SRC_CPU_VAL_MOUTMPLLFOUT
	ldr	r2, =0x1003C52C
	str	r1, [r2]

	
	ldr	r1, =0x00000000//CLK_SRC_CPU_VAL_MOUTMPLLFOUT
	ldr	r2, =0x1003C22C
	str	r1, [r2]

		
	ldr	r1, =0x00000003//CLK_SRC_CPU_VAL_MOUTMPLLFOUT
	ldr	r2, =0x1003C528
	str	r1, [r2]

	ldr	r1, =0x00000000//CLK_SRC_CPU_VAL_MOUTMPLLFOUT
	ldr	r2, =0x1003C228
	str	r1, [r2]





/* liyang removed
	ldr	r0, =CHIP_ID_BASE
	ldr	r1, [r0]
	lsr	r1, r1, #8
	and	r1, r1, #3
	cmp	r1, #2
	bne	v310_2
*/
@ ConControl
#ifdef MEM_DLLl_ON
	ldr	r0, =APB_DMC_0_BASE
#ifdef CONFIG_EVT1__	
	ldr	r1, =0x6910100A
#else
	ldr	r1, =0x7f10100A //ly
#endif
	ldr	r2, =DMC_PHYCONTROL0
	str	r1, [r0, r2]
	ldr	r1, =0xe0000084//0x00000084
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]
#ifdef CONFIG_EVT1__
	ldr	r1, =0x6910100B
#else
	ldr	r1, =0x7f10100B
#endif
	ldr	r2, =DMC_PHYCONTROL0
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x20000
8:	subs	r1, r1, #1
	bne	8b

	ldr	r1, =0x0000008C
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]
	ldr	r1, =0x00000084
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x20000
9:	subs	r1, r1, #1
	bne	9b

	ldr	r0, =APB_DMC_1_BASE
#ifdef CONFIG_EVT1___	
	ldr	r1, =0xE910100A
#else
	ldr	r1, =0x7f10100A//ly
#endif
	ldr	r2, =DMC_PHYCONTROL0
	str	r1, [r0, r2]
	ldr	r1, =0xE0000084//ly
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]
#ifdef CONFIG_EVT1___
	ldr	r1, =0xE910100B
#else
	ldr	r1, =0x7f10100B
#endif
	ldr	r2, =DMC_PHYCONTROL0
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x20000
10:	subs	r1, r1, #1
	bne	10b

	ldr	r1, =0x0000008C
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]
	ldr	r1, =0x00000084
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x20000
11:	subs	r1, r1, #1
	bne	11b
#endif

	ldr	r0, =APB_DMC_0_BASE
	ldr	r1, =0x0FFF30fa
	ldr	r2, =DMC_CONCONTROL
	str	r1, [r0, r2]
	ldr	r0, =APB_DMC_1_BASE
	ldr	r1, =0x0FFF30fa
	ldr	r2, =DMC_CONCONTROL
	str	r1, [r0, r2]

	ldr	r0, =APB_DMC_0_BASE
#ifdef CONFIG_EVT1_____
	ldr	r1, =0x00202523
#else
	ldr	r1, =0x00202533//ly
#endif
	ldr	r2, =DMC_MEMCONTROL
	str	r1, [r0, r2]
	ldr	r0, =APB_DMC_1_BASE
#ifdef CONFIG_EVT1____
	ldr	r1, =0x00202523
#else
	ldr	r1, =0x00202533
#endif
	ldr	r2, =DMC_MEMCONTROL
	str	r1, [r0, r2]
v310_2:
	pop	{pc}

/*
 *	Check clock until stable.
 */
	
	/* Check devider change state */
wait_div_state1:
	ldr	r1, =(ELFIN_CLOCK_POWER_BASE + CLK_DIV_STAT1_OFFSET)
        ldr     r2, [r1]
	tst	r2, #(0x1<<15)
	bne	wait_div_state1
	mov	pc, lr
	
	/* Check source change state */
wait_mux_state1:
	ldr	r1, =(ELFIN_CLOCK_POWER_BASE + CLK_MUX_STAT1_OFFSET)
        ldr     r2, [r1]
	tst	r2, #(0x1<<31)
	bne	wait_mux_state1
	mov	pc, lr
	
	/* Check source change state */
wait_mux_state0:
	ldr	r1, =(ELFIN_CLOCK_POWER_BASE + CLK_MUX_STAT0_OFFSET)
        ldr     r2, [r1]
	tst	r2, #(0x4)
	bne	wait_mux_state0
	mov	pc, lr

/*
 * uart_asm_init: Initialize UART in asm mode, 115200bps fixed.
 * void uart_asm_init(void)
 */
	.globl uart_asm_init
uart_asm_init:

	/* set GPIO to enable UART */
	@ GPIO setting for UART for UART0/1
	ldr	r0, =0x11400000
	ldr	r1, =0x22222222
	str   	r1, [r0]
	ldr	r0, =0x11400020
	ldr	r1, =0x222222
	str	r1, [r0]

	ldr	r0, =CMU_BASE	@0x1003_0000
	ldr	r1, =CLK_SRC_PERIL0_VAL		@0x666666
	ldr	r2, =CLK_SRC_PERIL0_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =CLK_DIV_PERIL0_VAL		@0x777777
	ldr	r2, =CLK_DIV_PERIL0_OFFSET
	str	r1, [r0, r2]

	ldr	r0, =ELFIN_UART_CONSOLE_BASE		@0xEC000000
	ldr	r1, =0x111
	str	r1, [r0, #UFCON_OFFSET]

	mov	r1, #0x3
	str	r1, [r0, #ULCON_OFFSET]

	ldr	r1, =0x3c5
	str	r1, [r0, #UCON_OFFSET]

	ldr	r1, =UART_UBRDIV_VAL	@0x2B /*  UDIV(43) = ((82,500,000 / 16 / 115200) - 1) */
	str	r1, [r0, #UBRDIV_OFFSET]

	ldr	r1, =UART_UDIVSLOT_VAL	@0xC /* UFRACVAL(12) = ((((82,500,000 / 16 / 115200) - 1) - 43) * 16) */
	str	r1, [r0, #UDIVSLOT_OFFSET]

	ldr	r1, =0x4f4f4f4f
	str	r1, [r0, #UTXH_OFFSET]		@'O'

	mov	pc, lr

/*
 * OneNAND Interface Init
 */
onenandcon_init:
wait_orwb:
	@; Read ONENAND_IF_STATUS
	ldr	r0, =CFG_ONENANDXL_BASE		@; 0x0C600000
	ldr	r1, [r0, #0x100]		@; ONENAND_IF_STATUS_OFFSET(0x100)
	bic	r1, r1, #0xFFFFFFFE
	cmp	r1, #0x0

	@; ORWB != 0x0
	bne	wait_orwb

	@; write new configuration to onenand system configuration1 register
	ldr	r1, =0xF006			@; Sync.
	ldr	r2, =(CFG_ONENAND_BASE+0x1E442)	@; 0x1E442(REG_SYS_CONF1)
	strh	r1, [r2]

	@; read one dummy halfword
	ldrh	r1, [r2]
	ldrh	r1, [r2]

	@; write new configuration to ONENAND_IF_CTRL
	ldr	r0, =CFG_ONENANDXL_BASE		@; 0x0C600000
	@;ldr	r1, =0x2F006			@; ONENAND_IF_CTRL_REG_VAL (GCE off)
	ldr	r1, =0x402F006			@; ONENAND_IF_CTRL_REG_VAL (GCE on)
	str	r1, [r0, #0x100]		@; ONENAND_IF_STATUS_OFFSET(0x100)

	mov	pc, lr

/*
 * Nand Interface Init for SMDKC100
 */
	.globl nand_asm_init
nand_asm_init:

	/* Setting GPIO for NAND */
	/* This setting is NAND initialze code at booting time in iROM. */

	ldr	r0, =0x11000000
	
	ldr     r1, [r0, #MP01CON_OFFSET]
	bic     r1, r1, #(0xf<<8)
	orr     r1, r1, #(0x3<<8)
	str     r1, [r0, #MP01CON_OFFSET]

@	ldr	r1, [r0, #MP01CON_OFFSET]
@	bic	r1, r1, #(0xf<<8)
@       orr     r1, r1, #((0x3<<8)|(0x2<<16)|(0x2<<20))
@       orr     r1, r1, #0x220300
@       ldr     r2, =0x220300
@       bic     r1, r1, r2
@	str	r1, [r0, #MP01CON_OFFSET]

        ldr     r1, [r0, #MP01PUD_OFFSET]
        bic     r1, r1, #(0x3<<4)
        str     r1, [r0, #MP01PUD_OFFSET]

	ldr     r1, [r0, #MP03CON_OFFSET]
	bic     r1, r1, #0xFFFFFF

	ldr     r2, =0x222
	orr     r1, r1, r2
	str     r1, [r0, #MP03CON_OFFSET]

	ldr	r0, =ELFIN_NAND_BASE
	ldr     r1, [r0, #NFCONF_OFFSET]

	ldr     r2, =0x7772
	bic	r1, r1, r2

	ldr	r2, =NFCONF_VAL
	orr	r1, r1, r2
	str	r1, [r0, #NFCONF_OFFSET]
	ldr	r1, [r0, #NFCONT_OFFSET]
	ldr     r2, =0x6
	bic	r1, r1, r2

	ldr	r2, =NFCONT_VAL

	orr	r1, r1, r2
	str	r1, [r0, #NFCONT_OFFSET]
	ldr     r1, [r0, #MP01CON_OFFSET]
	bic     r1, r1, #(0xf<<8)
	orr     r1, r1, #(0x3<<8)
	str     r1, [r0, #MP01CON_OFFSET]
	mov	pc, lr

/*
 * Setting TZPC[TrustZone Protection Controller]
 */

tzpc_init:

	ldr	r0, =ELFIN_TZPC0_BASE
 	mov	r1, #0x0
 	str	r1, [r0]
 	mov	r1, #0xff
 	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
 	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	@@@;;str	  r1, [r0, #TZPC_DECPROT2SET_OFFSET] 	
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

 	ldr 	r0, =ELFIN_TZPC1_BASE
 	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
 	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET] 	
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

 	ldr	r0, =ELFIN_TZPC2_BASE
 	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
 	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET] 

 	ldr	r0, =ELFIN_TZPC3_BASE
 	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
 	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET] 	
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

 	ldr	r0, =ELFIN_TZPC4_BASE
 	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
 	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

 	ldr	r0, =ELFIN_TZPC5_BASE
 	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
 	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET]

 	mov	pc, lr


#ifdef CONFIG_ENABLE_MMU

/*
 * MMU Table for SMDKC210
 * 0x0000_0000 -- 0x1FFF_FFFF => A:0x0000_0000 -- 0x1FFF_FFFF
 * 0x2000_0000 -- 0x3FFF_FFFF => Not Allowed
 * 0x4000_0000 -- 0x5FFF_FFFF => A:0x4000_0000 -- 0x5FFF_FFFF
 * 0x6000_0000 -- 0xBFFF_FFFF => Not Allowed
 * 0xC000_0000 -- 0xDFFF_FFFF => A:0x4000_0000 -- 0X5FFF_FFFF
 * 0xE000_0000 -- 0xFFFF_FFFF => Not Allowed
 */

	/* form a first-level section entry */
.macro FL_SECTION_ENTRY base,ap,d,c,b
	.word (\base << 20) | (\ap << 10) | \
	      (\d << 5) | (1<<4) | (\c << 3) | (\b << 2) | (1<<1)
.endm

.section .mmudata, "a"
	.align 14
	// the following alignment creates the mmu table at address 0x4000.
	.globl mmu_table
mmu_table:
	.set __base,0
	// Access for iRAM
	.rept 0x200
	FL_SECTION_ENTRY __base,3,0,0,0
	.set __base,__base+1
	.endr

	// Not Allowed
	.rept 0x400 - 0x200
	.word 0x00000000
	.endr

	.set __base,0x400
	// 512MB for SDRAM with cacheable
	.rept 0x600 - 0x400
	FL_SECTION_ENTRY __base,3,0,1,1
	.set __base,__base+1
	.endr

	// access is not allowed.
	.rept 0xc00 - 0x600
	.word 0x00000000
	.endr

	.set __base,0x400
	// 512MB for SDRAM with cacheable
	.rept 0xE00 - 0xC00
	FL_SECTION_ENTRY __base,3,0,1,1
	.set __base,__base+1
	.endr

	// access is not allowed.
	.rept 0x1000 - 0xE00
	.word 0x00000000
	.endr

#endif

