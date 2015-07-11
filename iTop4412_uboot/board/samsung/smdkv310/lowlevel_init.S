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

#include <s5pv310.h>
#include "smdkv310_val.h"

#define ARMCLK_800
//#define ARMCLK_1000
#define CLK_BUS_DMC_CLK_165_330
//#define CLK_BUS_DMC_CLK_200_400
#define MEM_DLLl_ON
_TEXT_BASE:
	.word	TEXT_BASE

	.globl lowlevel_init
lowlevel_init:
	push	{lr}

	/* check reset status  */
@	ldr     r0, =(ELFIN_CLOCK_POWER_BASE+RST_STAT_OFFSET)
@        ldr     r1, [r0]
	/* Sleep wakeup reset */
@	tst	r1, #(0x1<<16)
@        bne	wakeup_reset_pre
	/* Deepidle wakeup reset */
@	tst	r1, #(0x1<<19)
@	bne	wakeup_reset_from_didle

	/* IO Retention release */
/*	ldr	r0, =(ELFIN_CLOCK_POWER_BASE + OTHERS_OFFSET)
	ldr	r1, [r0]
	ldr	r2, =IO_RET_REL
	orr	r1, r1, r2
	str	r1, [r0]
*/
	/* PS_HOLD pin(GPH0_0) set to high */
/*	ldr	r0, =(ELFIN_CLOCK_POWER_BASE + PS_HOLD_CONTROL_OFFSET)
	ldr	r1, [r0]
	orr	r1, r1, #0x300	
	orr	r1, r1, #0x1	
	str	r1, [r0]
*/
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
	
	/* Memory initialize */
	bl mem_ctrl_asm_init
	
	/* init system clock */
	bl system_clock_init

	/* Internal voltage set 1.1V */
@	bl PMIC_InitIp


	
1:
	/* for UART */
	bl uart_asm_init

@	bl tzpc_init

#if defined(CONFIG_ONENAND)
	bl onenandcon_init
#endif

#if defined(CONFIG_NAND)
	/* simple init for NAND */
@	bl nand_asm_init
#endif

	/* ABB disable */
/*	ldr	r0, =0xE010C300
	mov	r1, #0x800000
	str	r1, [r0]
*/
	/* Print 'K' */
	ldr	r0, =ELFIN_UART_CONSOLE_BASE
	ldr	r1, =0x4b4b4b4b
	str	r1, [r0, #UTXH_OFFSET]

	pop	{pc}

@wakeup_reset_from_didle:
@	/* Wait when APLL is locked */
@	ldr	r0, =ELFIN_CLOCK_POWER_BASE
@lockloop:
@	ldr	r1, [r0, #APLL_CON0_OFFSET]
@	tst	r1, #(1<<29)
@	beq 	lockloop
@	b	exit_wakeup

@wakeup_reset_pre:
@	mrc	p15, 0, r1, c1, c0, 1	@Read CP15 Auxiliary control register
@	tst	r1, #(0x1<<31)		@Check L2RD is disable or not
@	beq	wakeup_reset		@if L2RD is not disable jump to wakeup_reset 
	
	bl	disable_l2cache
	bl	v7_flush_dcache_all
	bl	enable_l2cache

wakeup_reset:
	/* init system clock */
	bl system_clock_init
@	bl mem_ctrl_asm_init
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

	ldr	r1, =0x00133730
	ldr	r2, =CLK_DIV_CPU0_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =0x00000003
	ldr	r2, =CLK_DIV_CPU1_OFFSET
	str	r1, [r0, r2]

@ CMU_DMC MUX / DIV
	ldr	r1, =0
	ldr	r2, =CLK_SRC_DMC_OFFSET
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x10000
2:	subs	r1, r1, #1
	bne	2b

	ldr	r1, =0x13111113
	ldr	r2, =CLK_DIV_DMC0_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =0x01010100
	ldr	r2, =CLK_DIV_DMC1_OFFSET
	str	r1, [r0, r2]

@ CMU_TOP MUX / DIV
	ldr	r1, =0
	ldr	r2, =CLK_SRC_TOP0_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =0
	ldr	r2, =CLK_SRC_TOP1_OFFSET
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x10000
3:	subs	r1, r1, #1
	bne	3b

	ldr	r1, =0x00015473
	ldr	r2, =CLK_DIV_TOP_OFFSET
	str	r1, [r0, r2]

@ CMU_LEFTBUS MUX / DIV
	ldr	r1, =0
	ldr	r2, =CLK_SRC_LEFTBUS_OFFSET
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x10000
4:	subs	r1, r1, #1
	bne	4b

	ldr	r1, =0x00000013
	ldr	r2, =CLK_DIV_LEFTBUS_OFFSET
	str	r1, [r0, r2]

@ CMU_LEFTBUS MUX / DIV
	ldr	r1, =0
	ldr	r2, =CLK_SRC_RIGHTBUS_OFFSET
	str	r1, [r0, r2]
	
	/* wait ?us */
	mov	r1, #0x10000
5:	subs	r1, r1, #1
	bne	5b

	ldr	r1, =0x00000013
	ldr	r2, =CLK_DIV_RIGHTBUS_OFFSET
	str	r1, [r0, r2]

@ Set PLL locktime
	ldr	r1, =0x00001C20
	ldr	r2, =APLL_LOCK_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =0x00001C20
	ldr	r2, =MPLL_LOCK_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =0x00001C20
	ldr	r2, =EPLL_LOCK_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =0x00001C20
	ldr	r2, =VPLL_LOCK_OFFSET
	str	r1, [r0, r2]

@ Set PLL P,M,S ON
	ldr	r1, =0x8000001C
	ldr	r2, =APLL_CON1_OFFSET
	str	r1, [r0, r2]

#ifdef ARMCLK_1000
	ldr	r1, =0x80FA0601
#else
	ldr	r1, =0x80C80601
#endif
	ldr	r2, =APLL_CON0_OFFSET
	str	r1, [r0, r2]

#ifdef CLK_BUS_DMC_CLK_200_400
	ldr	r1, =0x8000001C
	ldr	r2, =MPLL_CON1_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =0x80C80601
	ldr	r2, =MPLL_CON0_OFFSET
	str	r1, [r0, r2]
#else
	ldr	r1, =0x8000000D
	ldr	r2, =MPLL_CON1_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =0x806E0401
	ldr	r2, =MPLL_CON0_OFFSET
	str	r1, [r0, r2]
#endif

	ldr	r1, =0
	ldr	r2, =EPLL_CON1_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =0x80300302
	ldr	r2, =EPLL_CON0_OFFSET
	str	r1, [r0, r2]

	ldr	r1, =0x11000400
	ldr	r2, =VPLL_CON1_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =0x80350302
	ldr	r2, =VPLL_CON0_OFFSET
	str	r1, [r0, r2]
	
	/* wait ?us */
	mov	r1, #0x30000
6:	subs	r1, r1, #1
	bne	6b

	ldr	r1, =0x00000101
	ldr	r2, =CLK_SRC_CPU_OFFSET
	str	r1, [r0, r2]

	/* wait ?us */
	mov	r1, #0x10000
7:	subs	r1, r1, #1
	bne	7b

@ ConControl
#ifdef MEM_DLLl_ON
	ldr	r0, =APB_DMC_0_BASE

	ldr	r1, =0x7110100A
	ldr	r2, =DMC_PHYCONTROL0
	str	r1, [r0, r2]
	ldr	r1, =0x00000084
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]
	ldr	r1, =0x7110100B
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

	ldr	r1, =0x7110100A
	ldr	r2, =DMC_PHYCONTROL0
	str	r1, [r0, r2]
	ldr	r1, =0x00000084
	ldr	r2, =DMC_PHYCONTROL1
	str	r1, [r0, r2]
	ldr	r1, =0x7110100B
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
#endif
	
	ldr	r0, =APB_DMC_0_BASE
	ldr	r1, =0x0FFF30fa
	ldr	r2, =DMC_CONCONTROL
	str	r1, [r0, r2]
	ldr	r0, =APB_DMC_1_BASE
	ldr	r1, =0x0FFF30fa
	ldr	r2, =DMC_CONCONTROL
	str	r1, [r0, r2]

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

	ldr	r0, =CMU_BASE	@0x1003_0000
	ldr	r1, =0x666666
	ldr	r2, =CLK_SRC_PERIL0_OFFSET
	str	r1, [r0, r2]
	ldr	r1, =0x777777
	ldr	r2, =CLK_DIV_PERIL0_OFFSET
	str	r1, [r0, r2]

	ldr	r0, =ELFIN_UART_CONSOLE_BASE		@0xEC000000
	ldr	r1, =0x111
	str	r1, [r0, #UFCON_OFFSET]

	mov	r1, #0x3
	str	r1, [r0, #ULCON_OFFSET]

	ldr	r1, =0x3c5
	str	r1, [r0, #UCON_OFFSET]

	ldr	r1, =0x2B /*  UDIV(43) = ((82,500,000 / 16 / 115200) - 1) */
	str	r1, [r0, #UBRDIV_OFFSET]

	ldr	r1, =0xC /* UFRACVAL(12) = ((((82,500,000 / 16 / 115200) - 1) - 43) * 16) */
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
	
@	ldr	r1, [r0, #MP01CON_OFFSET]
@	bic	r1, r1, #(0xf<<8)
@	orr	r1, r1, #(0x3<<8)
@	str	r1, [r0, #MP01CON_OFFSET]

@	ldr	r1, [r0, #MP01PUD_OFFSET]
@	bic	r1, r1, #(0x3<<4)
@	str	r1, [r0, #MP01PUD_OFFSET]

@	ldr	r1, [r0, #MP03CON_OFFSET]
@	bic	r1, r1, #0xFFFFFF
@	ldr	r2, =0x22222222
@	orr	r1, r1, r2
@	str	r1, [r0, #MP03CON_OFFSET]

@	ldr	r1, [r0, #MP03PUD_OFFSET]
@	ldr	r2, =0x3fff
@	bic	r1, r1, r2
@	str	r1, [r0, #MP03PUD_OFFSET]

	ldr	r0, =ELFIN_NAND_BASE

	ldr	r1, [r0, #NFCONF_OFFSET]
	ldr	r2, =0x777F
	bic	r1, r1, r2
	ldr	r2, =NFCONF_VAL
	orr	r1, r1, r2
	str	r1, [r0, #NFCONF_OFFSET]

	ldr	r1, [r0, #NFCONT_OFFSET]
	ldr	r2, =0x707C7
	bic	r1, r1, r2
	ldr	r2, =NFCONT_VAL
	orr	r1, r1, r2
	str	r1, [r0, #NFCONT_OFFSET]

	ldr	r1, [r0, #NFCONF_OFFSET]
	orr	r1, r1, #0x70
	orr	r1, r1, #0x7700
	str     r1, [r0, #NFCONF_OFFSET]

	ldr	r1, [r0, #NFCONT_OFFSET]
	orr	r1, r1, #0x03
	str     r1, [r0, #NFCONT_OFFSET]

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
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET] 	

 	ldr 	r0, =ELFIN_TZPC1_BASE
 	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
 	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET] 	

 	ldr	r0, =ELFIN_TZPC2_BASE
 	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
 	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT3SET_OFFSET] 

 	ldr	r0, =ELFIN_TZPC3_BASE
 	str	r1, [r0, #TZPC_DECPROT0SET_OFFSET]
 	str	r1, [r0, #TZPC_DECPROT1SET_OFFSET]
	str	r1, [r0, #TZPC_DECPROT2SET_OFFSET] 	

 	mov	pc, lr


#ifdef CONFIG_ENABLE_MMU

	#ifdef CONFIG_MCP_SINGLE
/*
 * MMU Table for SMDKC110
 * 0x0000_0000 -- 0xBFFF_FFFF => Not Allowed
 * 0xB000_0000 -- 0xB7FF_FFFF => A:0xB000_0000 -- 0xB7FF_FFFF
 * 0xC000_0000 -- 0xC7FF_FFFF => A:0x3000_0000 -- 0x37FF_FFFF
 * 0xC800_0000 -- 0xDFFF_FFFF => Not Allowed
 * 0xE000_0000 -- 0xFFFF_FFFF => A:0xE000_0000 -- 0XFFFF_FFFF
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
	.rept 0x100
	FL_SECTION_ENTRY __base,3,0,0,0
	.set __base,__base+1
	.endr

	// Not Allowed
	.rept 0x200 - 0x100
	.word 0x00000000
	.endr

	.set __base,0x200
	// should be accessed
	.rept 0x600 - 0x200
	FL_SECTION_ENTRY __base,3,0,1,1
	.set __base,__base+1
	.endr

	.rept 0x800 - 0x600
	.word 0x00000000
	.endr

	.set __base,0x800
	// should be accessed
	.rept 0xb00 - 0x800
	FL_SECTION_ENTRY __base,3,0,0,0
	.set __base,__base+1
	.endr

/*	.rept 0xc00 - 0xb00
	.word 0x00000000
	.endr */

	.set __base,0xB00
	.rept 0xc00 - 0xb00
	FL_SECTION_ENTRY __base,3,0,0,0
	.set __base,__base+1
	.endr

	.set __base,0x200
	// 256MB for SDRAM with cacheable
	.rept 0xD00 - 0xC00
	FL_SECTION_ENTRY __base,3,0,1,1
	.set __base,__base+1
	.endr

	// access is not allowed.
	@.rept 0xD00 - 0xC80
	@.word 0x00000000
	@.endr

	.set __base,0xD00
	// 1:1 mapping for debugging with non-cacheable
	.rept 0x1000 - 0xD00
	FL_SECTION_ENTRY __base,3,0,0,0
	.set __base,__base+1
	.endr	
	
	#else	// CONFIG_MCP_AC, CONFIG_MCP_H, CONFIG_MCP_B

/*
 * MMU Table for SMDKC110
 * 0x0000_0000 -- 0xBFFF_FFFF => Not Allowed
 * 0xB000_0000 -- 0xB7FF_FFFF => A:0xB000_0000 -- 0xB7FF_FFFF
 * 0xC000_0000 -- 0xC7FF_FFFF => A:0x3000_0000 -- 0x37FF_FFFF
 * 0xC800_0000 -- 0xDFFF_FFFF => Not Allowed
 * 0xE000_0000 -- 0xFFFF_FFFF => A:0xE000_0000 -- 0XFFFF_FFFF
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
	.rept 0x100
	FL_SECTION_ENTRY __base,3,0,0,0
	.set __base,__base+1
	.endr

	// Not Allowed
	.rept 0x300 - 0x100
	.word 0x00000000
	.endr

	.set __base,0x300
	// should be accessed
	.rept 0x350 - 0x300
	FL_SECTION_ENTRY __base,3,0,1,1
	.set __base,__base+1
	.endr

	// Not Allowed
	.rept 0x400 - 0x350
	.word 0x00000000
	.endr

	.set __base,0x400
	// should be accessed
	.rept 0x500 - 0x400
	FL_SECTION_ENTRY __base,3,0,1,1
	.set __base,__base+1
	.endr

	.rept 0x800 - 0x500
	.word 0x00000000
	.endr

	.set __base,0x800
	// should be accessed
	.rept 0xb00 - 0x800
	FL_SECTION_ENTRY __base,3,0,0,0
	.set __base,__base+1
	.endr

	.set __base,0xB00
	.rept 0xc00 - 0xb00
	FL_SECTION_ENTRY __base,3,0,0,0
	.set __base,__base+1
	.endr

	.set __base,0x300
	// 80MB for SDRAM with cacheable
	.rept 0xC50 - 0xC00
	FL_SECTION_ENTRY __base,3,0,1,1
	.set __base,__base+1
	.endr

	// Not Allowed
	.rept 0xD00 - 0xC50
	.word 0x00000000
	.endr

	.set __base,0xD00
	// 1:1 mapping for debugging with non-cacheable
	.rept 0x1000 - 0xD00
	FL_SECTION_ENTRY __base,3,0,0,0
	.set __base,__base+1
	.endr
	#endif
#endif

