#include <config.h>
#include <s5pc210.h>

//#define SET_MIU

@ MIU (Memory Interleaving Unit)
//#define MIU_LINEAR
//#define MIU_1BIT_INTERLEAVED
//#define MIU_2BIT_INTERLEAVED

//#define MIU_1BIT_12_INTERLEAVED
//#define MIU_1BIT_7_INTERLEAVED
//#define MIU_2BIT_21_12_INTERLEAVED
#define MIU_2BIT_21_7_INTERLEAVED

#define MEM_DLL

#ifdef CONFIG_CLK_800_330_165
#define DRAM_CLK_330
#endif
#ifdef CONFIG_CLK_1000_200_200
#define DRAM_CLK_200
#endif
#ifdef CONFIG_CLK_1000_330_165
#define DRAM_CLK_330
#endif
#ifdef CONFIG_CLK_1000_400_200
#define DRAM_CLK_400
#endif

	.globl mem_ctrl_asm_init
mem_ctrl_asm_init:

	/* Async bridge configuration at CPU_core */
	/* 1: half_sync */
	/* 0: full_sync */
	ldr r0, =0x10010350
	mov r1, #1
	str r1, [r0]
	
/* liyang 20110822 removed it for tc4
	ldr	r0, =CHIP_ID_BASE
	ldr	r1, [r0]
	lsr	r1, r1, #8
	and	r1, r1, #3
	cmp	r1, #2
	bne	v310
*/

	/* CLK_DIV_DMC0 on iROM DMC=50MHz for Init DMC */
	ldr	r0, =CMU_BASE	@0x1003_0000
	ldr	r1, =0x13113113
	ldr	r2, =CLK_DIV_DMC0_OFFSET
	str	r1, [r0, r2]


#ifdef SET_MIU
	ldr	r0, =MIU_BASE
#ifdef CONFIG_EVT1
	#ifdef MIU_1BIT_12_INTERLEAVED
	ldr	r1, =0x0000000c
	str	r1, [r0, #MIU_INTLV_CONFIG]
	ldr	r1, =0x00000001
	str	r1, [r0, #MIU_REG_UPDATE]
	#endif
	#ifdef MIU_1BIT_7_INTERLEAVED
	ldr     r1, =0x00000007
	str     r1, [r0, #MIU_INTLV_CONFIG]
	ldr     r1, =0x00000001
	str     r1, [r0, #MIU_REG_UPDATE]
	#endif
	#ifdef MIU_2BIT_21_12_INTERLEAVED
	ldr     r1, =0x2000150C
	str     r1, [r0, #MIU_INTLV_CONFIG]
	ldr     r1, =0x00000001
	str     r1, [r0, #MIU_REG_UPDATE]
	#endif
	#ifdef MIU_2BIT_21_7_INTERLEAVED
	ldr     r1, =0x20001507
	str     r1, [r0, #MIU_INTLV_CONFIG]
	ldr     r1, =0x00000001
	str     r1, [r0, #MIU_REG_UPDATE]
	#endif
#else

#ifdef MIU_1BIT_INTERLEAVED
	ldr	r1, =0x0000000c				
	str	r1, [r0, #MIU_INTLV_CONFIG]
	ldr	r1, =0x40000000				
	str	r1, [r0, #MIU_INTLV_START_ADDR]
	ldr	r1, =0x5fffffff				
	str	r1, [r0, #MIU_INTLV_END_ADDR]
	ldr	r1, =0x00000001          				
	str	r1, [r0, #MIU_MAPPING_UPDATE]
#elif MIU_2BIT_INTERLEAVED
	ldr	r1, =0x2000150c				
	str	r1, [r0, #MIU_INTLV_CONFIG]
	ldr	r1, =0x40000000				
	str	r1, [r0, #MIU_INTLV_START_ADDR]
	ldr	r1, =0x5fffffff				
	str	r1, [r0, #MIU_INTLV_END_ADDR]
	ldr	r1, =0x00000001          				
	str	r1, [r0, #MIU_MAPPING_UPDATE]
#else MIU_LINEAR
	ldr	r1, =0x40000000				
	str	r1, [r0, #MIU_SINGLE_MAPPING0_START_ADDR]
	ldr	r1, =0x4fffffff				
	str	r1, [r0, #MIU_SINGLE_MAPPING0_END_ADDR]
	ldr	r1, =0x50000000				
	str	r1, [r0, #MIU_SINGLE_MAPPING1_START_ADDR]
	ldr	r1, =0x5fffffff          				
	str	r1, [r0, #MIU_SINGLE_MAPPING1_END_ADDR]
	ldr	r1, =0x00000006          				
	str	r1, [r0, #MIU_MAPPING_UPDATE]
#endif
#endif

#endif


//ly added

	ldr   r0, =0x10040500
	ldr	r1, =0x00117713				
	str	r1, [r0]

		ldr   r0, =0x10020A00
	ldr	r1, =0x00000000				
	str	r1, [r0]

			ldr   r0, =0x10040A00
	ldr	r1, =0x00010905				
	str	r1, [r0]


/*****************************************************************/
/*DREX0***********************************************************/
/*****************************************************************/

	ldr	r0, =APB_DMC_0_BASE 

	ldr	r1, =0xE3855403 //0xE3855503				
	str	r1, [r0, #DMC_PHYZQCONTROL]

	ldr	r1, =0x71101008				
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x7110100A				
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x00000084				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x71101008				
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x0000008C				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x00000084				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x0000008C				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x00000084				
	str	r1, [r0, #DMC_PHYCONTROL1]

	//ldr	r1, =0x00000000			
	//str	r1, [r0, #DMC_PHYCONTROL2]

	ldr	r1, =0x0FFF30CA//0x0FFF30da				
	str	r1, [r0, #DMC_CONCONTROL]
	ldr	r1, =0x00202500				
	str	r1, [r0, #DMC_MEMCONTROL]
	//ldr	r1, =0x20f01223				
	ldr	r1, =0x40C01323
	str	r1, [r0, #DMC_MEMCONFIG0]


	/*liyang 20110822 Interleave on 512M*/
	ldr	r1, =0x8000001D				
	str	r1, [r0, #DMC_IvControl]

	
	ldr	r1, =0xff000000			
	str	r1, [r0, #DMC_PRECHCONFIG]

	ldr	r1, =0x0000005D    				
	str	r1, [r0, #DMC_TIMINGAREF] @TimingAref

#ifdef DRAM_CLK_200
111
	ldr	r1, =0x1A254349				
	str	r1, [r0, #DMC_TIMINGROW]
	ldr	r1, =0x23230306			
	str	r1, [r0, #DMC_TIMINGDATA]
	ldr	r1, =0x281c0235				
	str	r1, [r0, #DMC_TIMINGPOWER]
#endif
#ifdef DRAM_CLK_330
111
	ldr	r1, =0x2b47654e				
	str	r1, [r0, #DMC_TIMINGROW]
	ldr	r1, =0x35330306			
	str	r1, [r0, #DMC_TIMINGDATA]
	ldr	r1, =0x442f0365				
	str	r1, [r0, #DMC_TIMINGPOWER]
#endif
#ifdef DRAM_CLK_400

	ldr	r1, =0x34498691    				
	str	r1, [r0, #DMC_TIMINGROW] @TimingRow
	ldr	r1, =0x36330306    				
	str	r1, [r0, #DMC_TIMINGDATA] @TimingData
	ldr	r1, =0x50380365    				
	str	r1, [r0, #DMC_TIMINGPOWER] @TimingPower
#endif

	mov	r2, #0x100000
2:	subs	r2, r2, #1
	bne	2b
	ldr	r1, =0x07000000       				
	str	r1, [r0, #DMC_DIRECTCMD] 
	mov	r2, #0x100000
3:	subs	r2, r2, #1
	bne	3b
	ldr	r1, =0x00071C00       				
	str	r1, [r0, #DMC_DIRECTCMD] 
	mov	r2, #0x100000
4:	subs	r2, r2, #1
	bne	4b

	ldr	r1, =0x00010BFC       				
	str	r1, [r0, #DMC_DIRECTCMD] 
	mov	r2, #0x100000
5:	subs	r2, r2, #1
	bne	5b
	ldr	r1, =0x00000488       				
	str	r1, [r0, #DMC_DIRECTCMD] 
	ldr	r1, =0x00000810       				
	str	r1, [r0, #DMC_DIRECTCMD] 
	ldr	r1, =0x00000C08       				
	str	r1, [r0, #DMC_DIRECTCMD] 

#ifdef CONFIG_EVT1__
	/* get DMC density information */
	ldr	r1, =0x09010000
	mov	r3, #10
6:	str	r1, [r0, #DMC_DIRECTCMD]
	mov	r2, #0x10000
7:	subs	r2, r2, #1
	bne	7b
	ldr	r6, [r0, #DMC_MRSTATUS]
	subs	r3, r3, #1
	bne	6b
	and	r6, r6, #0x3c
	lsr	r6, r6, #2
	cmp	r6, #6
	ldreq	r1, =0x20e01323
	ldrne	r1, =0x20f01223   
	str	r1, [r0, #DMC_MEMCONFIG0]
#endif



//#ifdef MEM_1

/*****************************************************************/
/*DREX1***********************************************************/
/*****************************************************************/
	ldr	r0, =APB_DMC_1_BASE 
	ldr	r1, =0xE3855403 //0xE3855503					
	str	r1, [r0, #DMC_PHYZQCONTROL]

	ldr	r1, =0x71101008				
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x7110100A				
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x00000084				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x71101008				
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x0000008C				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x00000084				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x0000008C				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x00000084				
	str	r1, [r0, #DMC_PHYCONTROL1]

	//ldr	r1, =0x00000000			
	//str	r1, [r0, #DMC_PHYCONTROL2]

	ldr	r1, =0x0FFF30CA//0x0FFF30da	
	str	r1, [r0, #DMC_CONCONTROL]
	ldr	r1, =0x00202500				
	str	r1, [r0, #DMC_MEMCONTROL]
	//ldr	r1, =0x20f01223
	ldr	r1, =0x40C01323
	str	r1, [r0, #DMC_MEMCONFIG0]

	/*liyang 20110822 Interleave on 512M*/
	ldr	r1, =0x8000001D				
	str	r1, [r0, #DMC_IvControl]


	
	ldr	r1, =0xff000000			
	str	r1, [r0, #DMC_PRECHCONFIG]

	ldr	r1, =0x0000005D    				
	str	r1, [r0, #DMC_TIMINGAREF] @TimingAref

#ifdef DRAM_CLK_200
	ldr	r1, =0x1A254349				
	str	r1, [r0, #DMC_TIMINGROW]
	ldr	r1, =0x23230306			
	str	r1, [r0, #DMC_TIMINGDATA]
	ldr	r1, =0x281c0235				
	str	r1, [r0, #DMC_TIMINGPOWER]
#endif
#ifdef DRAM_CLK_330
	ldr	r1, =0x2b47654e				
	str	r1, [r0, #DMC_TIMINGROW]
	ldr	r1, =0x35330306			
	str	r1, [r0, #DMC_TIMINGDATA]
	ldr	r1, =0x442f0365
	str	r1, [r0, #DMC_TIMINGPOWER]
#endif
#ifdef DRAM_CLK_400
	ldr	r1, =0x34498691    				
	str	r1, [r0, #DMC_TIMINGROW] @TimingRow
	ldr	r1, =0x36330306    				
	str	r1, [r0, #DMC_TIMINGDATA] @TimingData
	ldr	r1, =0x50380365
	str	r1, [r0, #DMC_TIMINGPOWER] @TimingPower
#endif

	mov	r2, #0x100000
2:	subs	r2, r2, #1
	bne	2b
	ldr	r1, =0x07000000       				
	str	r1, [r0, #DMC_DIRECTCMD] 
	mov	r2, #0x100000
3:	subs	r2, r2, #1
	bne	3b
	ldr	r1, =0x00071C00       				
	str	r1, [r0, #DMC_DIRECTCMD] 
	mov	r2, #0x100000
4:	subs	r2, r2, #1
	bne	4b

	ldr	r1, =0x00010BFC       				
	str	r1, [r0, #DMC_DIRECTCMD] 
	mov	r2, #0x100000
5:	subs	r2, r2, #1
	bne	5b
	ldr	r1, =0x00000488       				
	str	r1, [r0, #DMC_DIRECTCMD] 
	ldr	r1, =0x00000810       				
	str	r1, [r0, #DMC_DIRECTCMD] 
	ldr	r1, =0x00000C08
	str	r1, [r0, #DMC_DIRECTCMD] 


#ifdef CONFIG_EVT1__
	/* get DMC density information */
	ldr	r1, =0x09010000
	mov	r3, #10
6:	str	r1, [r0, #DMC_DIRECTCMD]
	mov	r2, #0x10000
7:	subs	r2, r2, #1
	bne	7b
	ldr	r7, [r0, #DMC_MRSTATUS]
	subs	r3, r3, #1
	bne	6b
	and	r7, r7, #0x3c
	lsr	r7, r7, #2
	cmp	r7, #6
	ldreq	r1, =0x20e01323
	ldrne	r1, =0x20f01223   
	str	r1, [r0, #DMC_MEMCONFIG0]
#endif
//#endif //ifdef MEM_1


	mov	pc, lr

/*****************************************************/
v310:

#ifdef SET_MIU
	ldr	r0, =MIU_BASE
#ifdef CONFIG_MIU_1BIT_INTERLEAVED
	ldr	r1, =0x0000000c				
	str	r1, [r0, #MIU_INTLV_CONFIG]
	ldr	r1, =0x40000000				
	str	r1, [r0, #MIU_INTLV_START_ADDR]
	ldr	r1, =0xbfffffff				
	str	r1, [r0, #MIU_INTLV_END_ADDR]
	ldr	r1, =0x00000001          				
	str	r1, [r0, #MIU_MAPPING_UPDATE]
#endif
#ifdef CONFIG_MIU_2BIT_INTERLEAVED
	ldr	r1, =0x2000150c				
	str	r1, [r0, #MIU_INTLV_CONFIG]
	ldr	r1, =0x40000000
	str	r1, [r0, #MIU_INTLV_START_ADDR]
	ldr	r1, =0xbfffffff				
	str	r1, [r0, #MIU_INTLV_END_ADDR]
	ldr	r1, =0x00000001          				
	str	r1, [r0, #MIU_MAPPING_UPDATE]
#endif
#ifdef CONFIG_MIU_LINEAR
	ldr	r1, =0x40000000				
	str	r1, [r0, #MIU_SINGLE_MAPPING0_START_ADDR]
	ldr	r1, =0x7fffffff				
	str	r1, [r0, #MIU_SINGLE_MAPPING0_END_ADDR]
	ldr	r1, =0x80000000				
	str	r1, [r0, #MIU_SINGLE_MAPPING1_START_ADDR]
	ldr	r1, =0xbfffffff          				
	str	r1, [r0, #MIU_SINGLE_MAPPING1_END_ADDR]
	ldr	r1, =0x00000006          				
	str	r1, [r0, #MIU_MAPPING_UPDATE]
#endif
#endif

/*****************************************************************/
/*DREX0***********************************************************/
/*****************************************************************/

	ldr	r0, =APB_DMC_0_BASE

	ldr	r1, =0xe0000086				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0xE3855703				
	str	r1, [r0, #DMC_PHYZQCONTROL]

	mov	r2, #0x100000
1:	subs	r2, r2, #1
	bne	1b

	ldr	r1, =0xe000008e
	str	r1, [r0, #DMC_PHYCONTROL1]
	ldr	r1, =0xe0000086				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x71101008				
	str	r1, [r0, #DMC_PHYCONTROL0]
	ldr	r1, =0x7110100A				
	str	r1, [r0, #DMC_PHYCONTROL0]
	ldr	r1, =0xe0000086				
	str	r1, [r0, #DMC_PHYCONTROL1]
	ldr	r1, =0x7110100B				
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x00000000				
	str	r1, [r0, #DMC_PHYCONTROL2]

	ldr	r1, =0x0FFF301A
	str	r1, [r0, #DMC_CONCONTROL]
	ldr	r1, =0x00312640
	str	r1, [r0, #DMC_MEMCONTROL]

#ifdef MIU_LINEAR
	ldr	r1, =0x40e01323
	str	r1, [r0, #DMC_MEMCONFIG0]
	ldr	r1, =0x60e01323
	str	r1, [r0, #DMC_MEMCONFIG1]
#else	@MIU_1BIT_INTERLEAVED | MIU_2BIT_INTERLEAVED
	ldr	r1, =0x20e01323
	str	r1, [r0, #DMC_MEMCONFIG0]
	ldr	r1, =0x40e01323
	str	r1, [r0, #DMC_MEMCONFIG1]
#endif

	ldr	r1, =0xff000000
	str	r1, [r0, #DMC_PRECHCONFIG]

	ldr	r1, =0x000000BC
	str	r1, [r0, #DMC_TIMINGAREF] @TimingAref

#ifdef DRAM_CLK_330
	ldr	r1, =0x3545548d
	str	r1, [r0, #DMC_TIMINGROW]
	ldr	r1, =0x45430506
	str	r1, [r0, #DMC_TIMINGDATA]
	ldr	r1, =0x46000A3c
	str	r1, [r0, #DMC_TIMINGPOWER]
#endif
#ifdef DRAM_CLK_400
	ldr	r1, =0x4046654f
	str	r1, [r0, #DMC_TIMINGROW] @TimingRow
	ldr	r1, =0x56500506
	str	r1, [r0, #DMC_TIMINGDATA] @TimingData
	ldr	r1, =0x5444033d
	str	r1, [r0, #DMC_TIMINGPOWER] @TimingPower
#endif

	ldr	r1, =0x07000000
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
2:	subs	r2, r2, #1
	bne	2b

	ldr	r1, =0x00020000
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00030000
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00010002
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00000328
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
3:	subs	r2, r2, #1
	bne	3b

	ldr	r1, =0x0a000000
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
4:	subs	r2, r2, #1
	bne	4b

	ldr	r1, =0x07100000
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
5:	subs	r2, r2, #1
	bne	5b

	ldr	r1, =0x00120000
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00130000
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00110002
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00100328
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
6:	subs	r2, r2, #1
	bne	6b

	ldr	r1, =0x0a100000
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
7:	subs	r2, r2, #1
	bne	7b

	ldr	r1, =0xe000008e
	str	r1, [r0, #DMC_PHYCONTROL1]
	ldr	r1, =0xe0000086
	str	r1, [r0, #DMC_PHYCONTROL1]

	mov	r2, #0x100000
8:	subs	r2, r2, #1
	bne	8b

/*****************************************************************/
/*DREX1***********************************************************/
/*****************************************************************/

	ldr	r0, =APB_DMC_1_BASE

	ldr	r1, =0xe0000086				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0xE3855703				
	str	r1, [r0, #DMC_PHYZQCONTROL]

	mov	r2, #0x100000
1:	subs	r2, r2, #1
	bne	1b

	ldr	r1, =0xe000008e
	str	r1, [r0, #DMC_PHYCONTROL1]
	ldr	r1, =0xe0000086				
	str	r1, [r0, #DMC_PHYCONTROL1]

	ldr	r1, =0x71101008				
	str	r1, [r0, #DMC_PHYCONTROL0]
	ldr	r1, =0x7110100A				
	str	r1, [r0, #DMC_PHYCONTROL0]
	ldr	r1, =0xe0000086				
	str	r1, [r0, #DMC_PHYCONTROL1]
	ldr	r1, =0x7110100B				
	str	r1, [r0, #DMC_PHYCONTROL0]

	ldr	r1, =0x00000000				
	str	r1, [r0, #DMC_PHYCONTROL2]

	ldr	r1, =0x0fff301a
	str	r1, [r0, #DMC_CONCONTROL]
	ldr 	r1, =0x00312640
	str	r1, [r0, #DMC_MEMCONTROL]

#ifdef MIU_LINEAR
	ldr	r1, =0x40e01323
	str	r1, [r0, #DMC_MEMCONFIG0]
	ldr	r1, =0x60e01323
	str	r1, [r0, #DMC_MEMCONFIG1]
#else	@MIU_1BIT_INTERLEAVED | MIU_2BIT_INTERLEAVED
	ldr	r1, =0x20e01323				
	str	r1, [r0, #DMC_MEMCONFIG0]
	ldr	r1, =0x40e01323
	str	r1, [r0, #DMC_MEMCONFIG1]
#endif

	ldr	r1, =0xff000000			
	str	r1, [r0, #DMC_PRECHCONFIG]

	ldr	r1, =0x000000BC
	str	r1, [r0, #DMC_TIMINGAREF] @TimingAref

#ifdef DRAM_CLK_330
	ldr	r1, =0x3545548d
	str	r1, [r0, #DMC_TIMINGROW]
	ldr	r1, =0x45430506
	str	r1, [r0, #DMC_TIMINGDATA]
	ldr	r1, =0x4439033c
	str	r1, [r0, #DMC_TIMINGPOWER]
#endif
#ifdef DRAM_CLK_400
	ldr	r1, =0x4046654f
	str	r1, [r0, #DMC_TIMINGROW] @TimingRow
	ldr	r1, =0x56500506
	str	r1, [r0, #DMC_TIMINGDATA] @TimingData
	ldr	r1, =0x5444033d
	str	r1, [r0, #DMC_TIMINGPOWER] @TimingPower
#endif

	ldr	r1, =0x07000000
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
2:	subs	r2, r2, #1
	bne	2b

	ldr	r1, =0x00020000
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00030000
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00010002
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00000328
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
3:	subs	r2, r2, #1
	bne	3b

	ldr	r1, =0x0a000000
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
4:	subs	r2, r2, #1
	bne	4b

	ldr	r1, =0x07100000
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
5:	subs	r2, r2, #1
	bne	5b

	ldr	r1, =0x00120000
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00130000
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00110002
	str	r1, [r0, #DMC_DIRECTCMD]
	ldr	r1, =0x00100328
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
6:	subs	r2, r2, #1
	bne	6b

	ldr	r1, =0x0a100000
	str	r1, [r0, #DMC_DIRECTCMD]

	mov	r2, #0x100000
7:	subs	r2, r2, #1
	bne	7b

	ldr	r1, =0xe000008e
	str	r1, [r0, #DMC_PHYCONTROL1]
	ldr	r1, =0xe0000086
	str	r1, [r0, #DMC_PHYCONTROL1]

	mov	r2, #0x100000
8:	subs	r2, r2, #1
	bne	8b

/*****************************************************************/
/* turn on DREX0, DREX1 ******************************************/
/*****************************************************************/

	ldr	r0, =APB_DMC_0_BASE
	ldr	r1, =0x0FFF303a
	str	r1, [r0, #DMC_CONCONTROL]

	ldr	r0, =APB_DMC_1_BASE
	ldr	r1, =0x0FFF303a
	str	r1, [r0, #DMC_CONCONTROL]

	mov	pc, lr

