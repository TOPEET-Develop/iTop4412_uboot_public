/*
 *  armboot - Startup Code for S5PC110/ARM-Cortex CPU-core
 *
 *  Copyright (c) 2009	Samsung Electronics
 *
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Base codes by scsuh (sc.suh)
 */

#include <config.h>
#include <version.h>
#if defined(CONFIG_ENABLE_MMU)
#include <asm/proc/domain.h>
#endif
#if defined(CONFIG_S5PV310)
#include <s5pv310.h>
#endif
#if defined(CONFIG_S5PC210)
#include <s5pc210.h>
#endif

#ifndef CONFIG_ENABLE_MMU
#ifndef CFG_PHY_UBOOT_BASE
#define CFG_PHY_UBOOT_BASE	CFG_UBOOT_BASE
#endif
#endif

/*
 *************************************************************************
 *
 * Jump vector table as in table 3.1 in [1]
 *
 *************************************************************************
 */
	.word 0x2000
	.word 0x0
	.word 0x0
	.word 0x0
.globl _start
_start: b	reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

_undefined_instruction:
	.word undefined_instruction
_software_interrupt:
	.word software_interrupt
_prefetch_abort:
	.word prefetch_abort
_data_abort:
	.word data_abort
_not_used:
	.word not_used
_irq:
	.word irq
_fiq:
	.word fiq
_pad:
	.word 0x12345678 /* now 16*4=64 */
.global _end_vect
_end_vect:

	.balignl 16,0xdeadbeef
/*
 *************************************************************************
 *
 * Startup Code (reset vector)
 *
 * do important init only if we don't start from memory!
 * setup Memory and board specific bits prior to relocation.
 * relocate armboot to ram
 * setup stack
 *
 *************************************************************************
 */

_TEXT_BASE:
	.word	TEXT_BASE

/*
 * Below variable is very important because we use MMU in U-Boot.
 * Without it, we cannot run code correctly before MMU is ON.
 * by scsuh.
 */
_TEXT_PHY_BASE:
	.word	CFG_PHY_UBOOT_BASE

.globl _armboot_start
_armboot_start: 
	.word _start

/*
 * These are defined in the board-specific linker script.
 */
.globl _bss_start
_bss_start:
	.word __bss_start

.globl _bss_end
_bss_end:
	.word _end

    .macro  cache_invalidate_dcache_v7
    MRC     p15, 1, r0, c0, c0, 1      @ read Cache Level ID register (clidr)
    ANDS    r3, r0, #0x7000000         @ extract level of coherency from clidr
    MOV     r3, r3, lsr #23            @ left align level of coherency bit field
    BEQ     finished_inval             @ if loc is 0, then no need to clean

    MOV     r10, #0                    @ start clean at cache level 0 (in r10)
loop_1:
    ADD     r2, r10, r10, lsr #1       @ work out 3x current cache level
    MOV     r1, r0, lsr r2             @ extract cache type bits from clidr
    AND     r1, r1, #7                 @ mask of the bits for current cache only
    CMP     r1, #2                     @ see what cache we have at this level
    BLT     skip_inval                 @ skip if no cache, or just i-cache
    MCR     p15, 2, r10, c0, c0, 0     @ select current cache level in cssr
    MOV     r1, #0
    MCR     p15, 0, r1, c7, c5, 4      @ prefetchflush to synch the new cssr&csidr
    MRC     p15, 1, r1, c0, c0, 0      @ read the new csidr
    AND     r2, r1, #7                 @ extract the length of the cache lines
    ADD     r2, r2, #4                 @ add 4 (line length offset)
    LDR     r6, =0x3ff
    ANDS    r6, r6, r1, lsr #3         @ find maximum number on the way size
    CLZ     r5,r6                       @ DCI 0xE16F5F16 , find bit position of way size increment
    LDR     r7, =0x7fff
    ANDS    r7, r7, r1, lsr #13        @ extract max number of the index size
loop_2:
    MOV     r8, r6                     @ create working copy of max way size
	loop_3:
    ORR     r11, r10, r8, lsl r5       @ factor way and cache number into r11
    ORR     r11, r11, r7, lsl r2       @ factor index number into r11
    MCR     p15, 0, r11, c7, c6, 2     @ invalidate by set/way
    SUBS    r8, r8, #1                 @ decrement the way
    BGE     loop_3

    SUBS    r7, r7, #1                 @ decrement the index
    BGE     loop_2
skip_inval:
    ADD     r10, r10, #2               @ increment cache number
    CMP     r3, r10
    BGT     loop_1
finished_inval:
    .endm

#if defined(CONFIG_USE_IRQ)
/* IRQ stack memory (calculated at run-time) */
.globl IRQ_STACK_START
IRQ_STACK_START:
	.word	0x0badc0de

/* IRQ stack memory (calculated at run-time) */
.globl FIQ_STACK_START
FIQ_STACK_START:
	.word 0x0badc0de
#endif

/*
 * the actual reset code
 */

reset:
#if 0
	/*
	 * set the cpu to SVC32 mode and IRQ & FIQ disable
	 */
	mrs	r0, cpsr
	bic	r0, r0, #0x3f
	orr	r0, r0, #0xd3
	msr	cpsr, r0
#else//*****ly
	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd3
	msr	cpsr,r0
#endif
//	cache_invalidate_dcache_v7			@ call cache macro
#if 1 //*****ly
cache_init:
	mrc	p15, 0, r0, c0, c0, 0	@ read main ID register
	and	r1, r0, #0x00f00000	@ variant
	and	r2, r0, #0x0000000f	@ revision
	orr	r2, r2, r1, lsr #20-4	@ combine variant and revision
	cmp	r2, #0x30
	mrceq	p15, 0, r0, c1, c0, 1	@ read ACTLR
	orreq	r0, r0, #0x6		@ Enable DP1(2), DP2(1)
	mcreq	p15, 0, r0, c1, c0, 1	@ write ACTLR
	/*
	 * Invalidate L1 I/D
	 */
	mov	r0, #0			@ set up for MCR
	mcr	p15, 0, r0, c8, c7, 0	@ invalidate TLBs
	mcr	p15, 0, r0, c7, c5, 0	@ invalidate icache

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002000	@ clear bits 13 (--V-)
	bic	r0, r0, #0x00000007	@ clear bits 2:0 (-CAM)
	orr	r0, r0, #0x00001000	@ set bit 12 (---I) Icache
	orr	r0, r0, #0x00000002	@ set bit 1  (--A-) Align
	orr	r0, r0, #0x00000800	@ set bit 11 (Z---) BTB
	mcr	p15, 0, r0, c1, c0, 0
#endif
	

        /* LED Test Code */
		#if 0
        ldr r0, =0x11000080
        ldr r1, [r0]
        bic r1, r1, #0xf00
        orr r1, r1, #0x100
        str r1, [r0]
        #endif


	/* Read booting information */
        ldr	r0, =POWER_BASE
        ldr	r1, [r0,#OMR_OFFSET]
        bic	r2, r1, #0xffffffc1

	/* NAND BOOT */
@	cmp	r2, #0x0		@ 512B 4-cycle
@	moveq	r3, #BOOT_NAND

@	cmp	r2, #0x2		@ 2KB 5-cycle
@	moveq	r3, #BOOT_NAND

@	cmp	r2, #0x4		@ 4KB 5-cycle	8-bit ECC
@	moveq	r3, #BOOT_NAND

	cmp     r2, #0xA
        moveq   r3, #BOOT_ONENAND

@	cmp	r2, #0x6		@ 4KB 5-cycle	16-bit ECC
@	moveq	r3, #BOOT_NAND

	/* SD/MMC BOOT */
	cmp     r2, #0x4
        moveq   r3, #BOOT_MMCSD	

	/* eMMC4.3 BOOT */
	cmp		r2, #0x6
	moveq	r3, #BOOT_EMMC43

	/* eMMC441 BOOT */
	cmp		r2, #0x28
	moveq	r3, #BOOT_EMMC441
	
        /* NOR BOOT */
@	cmp     r2, #0x14
@	moveq   r3, #BOOT_NOR	

	/* For second device booting */
	/* OneNAND BOOTONG failed */
@	cmp     r2, #0x8
@	moveq   r3, #BOOT_SEC_DEV

	/* Uart BOOTONG failed */
@	cmp     r2, #(0x1<<4)
@	moveq   r3, #BOOT_SEC_DEV
	
	ldr	r0, =INF_REG_BASE
	str	r3, [r0, #INF_REG3_OFFSET]     

	/*
	 * Go setup Memory and board specific bits prior to relocation.
	 */
	//b     .
	bl	lowlevel_init	/* go setup pll,mux,memory */


	ldr	r0, =0x1002330C  /* PS_HOLD_CONTROL register */
	ldr	r1, =0x00005300	 /* PS_HOLD output high	*/
	str	r1, [r0]


	/* get ready to call C functions */
	ldr	sp, _TEXT_PHY_BASE	/* setup temp stack pointer */
	sub	sp, sp, #12
	mov	fp, #0			/* no previous frame, so fp=0 */


	/* I-Cache & D-Cache Enabled    */  
//	mrc p15, 0, r0, c1, c0, 0
//	orr r0, r0, #(0x1 << 2)
//	orr r0, r0, #(0x1 << 12)
//	mcr p15, 0, r0, c1, c0, 0

	/* when we already run in ram, we don't need to relocate U-Boot.
	 * and actually, memory controller must be configured before U-Boot
	 * is running in ram.
	 */
	ldr	r0, =0xff000fff
	bic	r1, pc, r0		/* r0 <- current base addr of code */
	ldr	r2, _TEXT_BASE		/* r1 <- original base addr in ram */
	bic	r2, r2, r0		/* r0 <- current base addr of code */
	cmp     r1, r2                  /* compare r0, r1                  */
	beq     after_copy		/* r0 == r1 then skip flash copy   */



	ldr	r0, =0x11000104  /* GPL2(0)  */
	ldr	r1, =0x00000001	 /* GPL2(0 output high	*/
	str	r1, [r0]
	
	ldr	r0, =0x11000100  /* GPL2(0)  */
	ldr	r1, =0x00000001	 /* GPL2(0 output high	*/
	str	r1, [r0]

	
	/* wait ?us */
	mov	r1, #0x10000
9:	subs	r1, r1, #1
	bne	9b

//	ldr	r0, =0x11000C44 /* GPX2(0)  */
//	ldr	r1, [r0]
//	bic	r2, r1, #0xfe
//	ldr	r1, =0x1



//	ldr	r0, =INF_REG_BASE
//	ldr	r3, [r0, #INF_REG3_OFFSET]  
	
//	cmp	r1, r2
//	moveq   r3, #BOOT_MMCSD	
//	ldr	r0, =INF_REG_BASE
//	str	r3, [r0, #INF_REG3_OFFSET]
//	cmp	r1, r2
//	beq     mmcsd_boot



	ldr	r0, =INF_REG_BASE
	ldr	r1, [r0, #INF_REG3_OFFSET]
	cmp	r1, #BOOT_NAND		/* 0x0 => boot device is nand */
	beq		nand_boot
	
	cmp	r1, #BOOT_ONENAND	/* 0x1 => boot device is onenand */
	beq		onenand_boot
	
	cmp     r1, #BOOT_EMMC441
	beq     emmc441_boot
	
	cmp     r1, #BOOT_EMMC43
	beq     emmc_boot
	
	cmp     r1, #BOOT_MMCSD
	beq     mmcsd_boot
	
	cmp     r1, #BOOT_NOR
	beq     nor_boot
	
	cmp     r1, #BOOT_SEC_DEV
	beq     mmcsd_boot

nand_boot:
	mov	r0, #0x1000
	bl	copy_from_nand
	b	after_copy

onenand_boot:
	bl	onenand_bl2_copy  /*goto 0x1010*/
	b	after_copy
//ly
second_mmcsd_boot:
	ldr   r3, =BOOT_MMCSD	
	ldr	r0, =INF_REG_BASE
	str	r3, [r0, #INF_REG3_OFFSET]
	
mmcsd_boot:
#ifdef CONFIG_CLK_1000_400_200
	ldr	r0, =CMU_BASE
	ldr	r2, =CLK_DIV_FSYS2_OFFSET
	ldr	r1, [r0, r2]
	orr r1, r1, #0xf
	str r1, [r0, r2]
#endif
	bl      movi_uboot_copy
	b       after_copy

emmc_boot:
#if defined(CONFIG_CLK_1000_400_200) || defined(CONFIG_CLK_1000_200_200) || defined(CONFIG_CLK_800_400_200)
	ldr	r0, =CMU_BASE
	ldr	r2, =CLK_DIV_FSYS1_OFFSET
	ldr	r1, [r0, r2]
	orr r1, r1, #0x3
	str r1, [r0, r2]
#endif
	bl		emmc_uboot_copy
	b	after_copy

emmc441_boot:
#if defined(CONFIG_CLK_1000_400_200) || defined(CONFIG_CLK_1000_200_200) || defined(CONFIG_CLK_800_400_200)
	ldr	r0, =CMU_BASE
	ldr	r2, =CLK_DIV_FSYS3_OFFSET
	ldr	r1, [r0, r2]
	orr r1, r1, #0x3
	str r1, [r0, r2]
#endif
	bl		emmc441_uboot_copy
//ly 20110824
	ldr   r0, =0x43e00000
	ldr   r1, [r0]
	ldr   r2, =0x2000
	cmp r1, r2
	bne  second_mmcsd_boot
	b	after_copy


nor_boot:
@	bl	read_hword
	b	after_copy


after_copy:


#if defined(CONFIG_ENABLE_MMU)
enable_mmu:
	/* enable domain access */
	ldr	r5, =0x0000ffff
	mcr	p15, 0, r5, c3, c0, 0		@load domain access register

	/* Set the TTB register */
	ldr	r0, _mmu_table_base
	ldr	r1, =CFG_PHY_UBOOT_BASE
	ldr	r2, =0xfff00000
	bic	r0, r0, r2
	orr	r1, r0, r1
	mcr	p15, 0, r1, c2, c0, 0

	/* Enable the MMU */
mmu_on:
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #1
	mcr	p15, 0, r0, c1, c0, 0
	nop
	nop
	nop
	nop
#endif


#ifdef CONFIG_EVT1
	/* store DMC density information in u-boot C level variable */
	ldr	r0, = CFG_UBOOT_BASE
	sub	r0, r0, #4
	ldr	r1, [r0]
	ldr	r0, _dmc_density
	str	r1, [r0]
#endif

skip_hw_init:
	/* Set up the stack						    */
stack_setup:
#if defined(CONFIG_MEMORY_UPPER_CODE)
	ldr	sp, =(CFG_UBOOT_BASE + CFG_UBOOT_SIZE - 0x1000)
#else
	ldr	r0, _TEXT_BASE		/* upper 128 KiB: relocated uboot   */
	sub	r0, r0, #CONFIG_SYS_MALLOC_LEN	/* malloc area                      */
	sub	r0, r0, #CFG_GBL_DATA_SIZE /* bdinfo                        */
#if defined(CONFIG_USE_IRQ)
	sub	r0, r0, #(CONFIG_STACKSIZE_IRQ+CONFIG_STACKSIZE_FIQ)
#endif
	sub	sp, r0, #12		/* leave 3 words for abort-stack    */

#endif

clear_bss:
	ldr	r0, _bss_start		/* find start of bss segment        */
	ldr	r1, _bss_end		/* stop here                        */
	mov 	r2, #0x00000000		/* clear                            */

clbss_l:
	str	r2, [r0]		/* clear loop...                    */
	add	r0, r0, #4
	cmp	r0, r1
	ble	clbss_l
	
	ldr	pc, _start_armboot

_start_armboot:
	.word start_armboot

#ifdef CONFIG_EVT1
_dmc_density:
	.word dmc_density
#endif

#if defined(CONFIG_ENABLE_MMU)
_mmu_table_base:
	.word mmu_table
#endif


/*
 * copy U-Boot to SDRAM and jump to ram (from NAND or OneNAND)
 * r0: size to be compared
 * Load 1'st 2blocks to RAM because U-boot's size is larger than 1block(128k) size
 */
	.globl copy_from_nand
copy_from_nand:
	push	{lr}		/* save return address */

	mov	r9, r0
	
	mov	r9, #0x100		/* Compare about 8KB */
	bl	copy_uboot_to_ram

	tst 	r0, #0x0
	bne	copy_failed

	ldr	r0, =0xd0020000

	ldr	r1, _TEXT_PHY_BASE	/* 0x23e00000 */
1:	ldr	r3, [r0], #4
	ldr	r4, [r1], #4
	teq	r3, r4
	bne	compare_failed	/* not matched */
	subs	r9, r9, #4
	bne	1b

	pop	{pc}		/* all is OK */

copy_failed:
	nop			/* copy from nand failed */
	b	copy_failed

compare_failed:
	nop			/* compare failed */
	b	compare_failed

/*
 * we assume that cache operation is done before. (eg. cleanup_before_linux())
 * actually, we don't need to do anything about cache if not use d-cache in U-Boot
 * So, in this function we clean only MMU. by scsuh
 *
 * void	theLastJump(void *kernel, int arch_num, uint boot_params);
 */
#if defined(CONFIG_ENABLE_MMU)
	.globl theLastJump
theLastJump:
	mov	r9, r0
	ldr	r3, =0xfff00000
	ldr	r4, _TEXT_PHY_BASE
	adr	r5, phy_last_jump
	bic	r5, r5, r3
	orr	r5, r5, r4
	mov	pc, r5
phy_last_jump:
	/*
	 * disable MMU stuff
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002300	/* clear bits 13, 9:8 (--V- --RS) */
	bic	r0, r0, #0x00000087	/* clear bits 7, 2:0 (B--- -CAM) */
	orr	r0, r0, #0x00000002	/* set bit 2 (A) Align */
	orr	r0, r0, #0x00001000	/* set bit 12 (I) I-Cache */
	mcr	p15, 0, r0, c1, c0, 0

	mcr	p15, 0, r0, c8, c7, 0	/* flush v4 TLB */

	mov	r0, #0
	mov	pc, r9
#endif
/*
 *************************************************************************
 *
 * Interrupt handling
 *
 *************************************************************************
 */
@
@ IRQ stack frame.
@
#define S_FRAME_SIZE	72

#define S_OLD_R0	68
#define S_PSR		64
#define S_PC		60
#define S_LR		56
#define S_SP		52

#define S_IP		48
#define S_FP		44
#define S_R10		40
#define S_R9		36
#define S_R8		32
#define S_R7		28
#define S_R6		24
#define S_R5		20
#define S_R4		16
#define S_R3		12
#define S_R2		8
#define S_R1		4
#define S_R0		0

#define MODE_SVC 0x13
#define I_BIT	 0x80

/*
 * use bad_save_user_regs for abort/prefetch/undef/swi ...
 * use irq_save_user_regs / irq_restore_user_regs for IRQ/FIQ handling
 */

	.macro	bad_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE		@ carve out a frame on current user stack
	stmia	sp, {r0 - r12}			@ Save user registers (now in svc mode) r0-r12

	ldr	r2, _armboot_start
	sub	r2, r2, #(CONFIG_SYS_MALLOC_LEN)
	sub	r2, r2, #(CFG_GBL_DATA_SIZE+8)	@ set base 2 words into abort stack
	ldmia	r2, {r2 - r3}			@ get values for "aborted" pc and cpsr (into parm regs)
	add	r0, sp, #S_FRAME_SIZE		@ grab pointer to old stack

	add	r5, sp, #S_SP
	mov	r1, lr
	stmia	r5, {r0 - r3}			@ save sp_SVC, lr_SVC, pc, cpsr
	mov	r0, sp				@ save current stack into r0 (param register)
	.endm

	.macro	irq_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	add	r8, sp, #S_PC			@ !!!! R8 NEEDS to be saved !!!! a reserved stack spot would be good.
	stmdb	r8, {sp, lr}^			@ Calling SP, LR
	str	lr, [r8, #0]			@ Save calling PC
	mrs	r6, spsr
	str	r6, [r8, #4]			@ Save CPSR
	str	r0, [r8, #8]			@ Save OLD_R0
	mov	r0, sp
	.endm

	.macro	irq_restore_user_regs
	ldmia	sp, {r0 - lr}^			@ Calling r0 - lr
	mov	r0, r0
	ldr	lr, [sp, #S_PC]			@ Get PC
	add	sp, sp, #S_FRAME_SIZE
	subs	pc, lr, #4			@ return & move spsr_svc into cpsr
	.endm

	.macro get_bad_stack
	ldr	r13, _armboot_start		@ setup our mode stack (enter in banked mode)
	sub	r13, r13, #(CONFIG_SYS_MALLOC_LEN)	@ move past malloc pool
	sub	r13, r13, #(CFG_GBL_DATA_SIZE+8) @ move to reserved a couple spots for abort stack

	str	lr, [r13]			@ save caller lr in position 0 of saved stack
	mrs	lr, spsr			@ get the spsr
	str	lr, [r13, #4]			@ save spsr in position 1 of saved stack

	mov	r13, #MODE_SVC			@ prepare SVC-Mode
	@ msr	spsr_c, r13
	msr	spsr, r13			@ switch modes, make sure moves will execute
	mov	lr, pc				@ capture return pc
	movs	pc, lr				@ jump to next instruction & switch modes.
	.endm

	.macro get_bad_stack_swi
	sub	r13, r13, #4			@ space on current stack for scratch reg.
	str	r0, [r13]			@ save R0's value.
	ldr	r0, _armboot_start		@ get data regions start
	sub	r0, r0, #(CONFIG_SYS_MALLOC_LEN)	@ move past malloc pool
	sub	r0, r0, #(CFG_GBL_DATA_SIZE+8)	@ move past gbl and a couple spots for abort stack
	str	lr, [r0]			@ save caller lr in position 0 of saved stack
	mrs	r0, spsr			@ get the spsr
	str	lr, [r0, #4]			@ save spsr in position 1 of saved stack
	ldr	r0, [r13]			@ restore r0
	add	r13, r13, #4			@ pop stack entry
	.endm

	.macro get_irq_stack			@ setup IRQ stack
	ldr	sp, IRQ_STACK_START
	.endm

	.macro get_fiq_stack			@ setup FIQ stack
	ldr	sp, FIQ_STACK_START
	.endm

/*
 * exception handlers
 */
	.align	5
undefined_instruction:
	get_bad_stack
	bad_save_user_regs
	bl	do_undefined_instruction

	.align	5
software_interrupt:
	get_bad_stack_swi
	bad_save_user_regs
	bl	do_software_interrupt

	.align	5
prefetch_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_prefetch_abort

	.align	5
data_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_data_abort

	.align	5
not_used:
	get_bad_stack
	bad_save_user_regs
	bl	do_not_used

#if defined(CONFIG_USE_IRQ)

	.align	5
irq:
	get_irq_stack
	irq_save_user_regs
	bl	do_irq
	irq_restore_user_regs

	.align	5
fiq:
	get_fiq_stack
	/* someone ought to write a more effiction fiq_save_user_regs */
	irq_save_user_regs
	bl	do_fiq
	irq_restore_user_regs

#else

	.align	5
irq:
	get_bad_stack
	bad_save_user_regs
	bl	do_irq

	.align	5
fiq:
	get_bad_stack
	bad_save_user_regs
	bl	do_fiq

#endif
	.align 5
.global arm_cache_flush
arm_cache_flush:
       mcr     p15, 0, r1, c7, c5, 0           @ invalidate I cache
       mov     pc, lr                          @ back to caller

/*
 *     v7_flush_dcache_all()
 *
 *     Flush the whole D-cache.
 *
 *     Corrupted registers: r0-r5, r7, r9-r11
 *
 *     - mm    - mm_struct describing address space
 */
       .align 5
.global v7_flush_dcache_all
v7_flush_dcache_all:

	ldr	r0, =0xffffffff
	mrc	p15, 1, r0, c0, c0, 1 		@ Read CLIDR
	ands	r3, r0, #0x7000000
	mov	r3, r3, LSR #23       		@ Cache level value (naturally aligned)
	beq 	Finished
	mov	r10, #0
Loop1:         
	add	r2, r10, r10, LSR #1  		@ Work out 3xcachelevel
	mov	r1, r0, LSR r2        		@ bottom 3 bits are the Ctype for this level
	and	r1, r1, #7            		@ get those 3 bits alone
	cmp	r1, #2
	blt	Skip                   		@ no cache or only instruction cache at this level
	mcr	p15, 2, r10, c0, c0, 0 		@ write the Cache Size selection register
	mov	r1, #0
	mcr	p15, 0, r1, c7, c5, 4 		@ PrefetchFlush to sync the change to the CacheSizeID reg
	mrc	p15, 1, r1, c0, c0, 0 		@ reads current Cache Size ID register
	and	r2, r1, #0x7           		@ extract the line length field
	add	r2, r2, #4            		@ add 4 for the line length offset (log2 16 bytes)
	ldr	r4, =0x3FF
	ands	r4, r4, r1, LSR #3   		@ R4 is the max number on the way size (right aligned)
	clz	r5, r4                		@ R5 is the bit position of the way size increment
	ldr	r7, =0x00007FFF
	ands	r7, r7, r1, LSR #13  		@ R7 is the max number of the index size (right aligned)
Loop2:         
	mov	r9, r4                      	@ R9 working copy of the max way size (right aligned)
Loop3:         
	orr	r11, r10, r9, LSL r5        	@ factor in the way number and cache number into R11
	orr	r11, r11, r7, LSL r2        	@ factor in the index number
	mcr	p15, 0, r11, c7, c6, 2 		@ invalidate by set/way
	subs	r9, r9, #1                 	@ decrement the way number
	bge	Loop3
	subs	r7, r7, #1                 	@ decrement the index
	bge	Loop2
Skip:          
	add	r10, r10, #2                	@ increment the cache number
	cmp	r3, r10
	bgt	Loop1
Finished:
	mov	pc, lr
	
       .align  5
.global disable_l2cache
disable_l2cache:
	mrc     p15, 0, r0, c1, c0, 1
	bic     r0, r0, #(1<<1)
	mcr     p15, 0, r0, c1, c0, 1
	mov	pc, lr


       .align  5
.global enable_l2cache
enable_l2cache:
	mrc     p15, 0, r0, c1, c0, 1
	orr     r0, r0, #(1<<1)
	mcr     p15, 0, r0, c1, c0, 1
	mov     pc, lr

       .align  5
.global set_l2cache_auxctrl
set_l2cache_auxctrl:
	mov	r0, #0x0
	mcr     p15, 1, r0, c9, c0, 2
	mov     pc, lr

       .align  5
.global set_l2cache_auxctrl_cycle
set_l2cache_auxctrl_cycle:
	mrc 	p15, 1, r0, c9, c0, 2
	bic 	r0, r0, #(0x1<<29)
	bic 	r0, r0, #(0x1<<21)
	bic 	r0, r0, #(0x7<<6)
	bic 	r0, r0, #(0x7<<0)
	mcr 	p15, 1, r0, c9, c0, 2
	mov     pc,lr

	.align 5
CoInvalidateDCacheIndex:
	;/* r0 = index */
	mcr     p15, 0, r0, c7, c6, 2
	mov     pc,lr


#if defined(CONFIG_INTEGRATOR) && defined(CONFIG_ARCH_CINTEGRATOR)
/* Use the IntegratorCP function from board/integratorcp/platform.S */
#elif defined(CONFIG_S5PC11X) || defined(CONFIG_S5PC210)|| defined(CONFIG_S5PV310)
/* For future usage of S3C64XX*/
#else
	.align	5
.globl reset_cpu
reset_cpu:
	ldr	r1, rstctl	/* get addr for global reset reg */
	mov	r3, #0x2	/* full reset pll+mpu */
	str	r3, [r1]	/* force reset */
	mov	r0, r0
_loop_forever:
	b	_loop_forever
rstctl:
	.word	PM_RSTCTRL_WKUP

#endif
