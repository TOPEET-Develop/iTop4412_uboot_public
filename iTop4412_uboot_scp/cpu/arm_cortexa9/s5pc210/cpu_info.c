/*
 * Copyright (C) 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
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
#include <common.h>
#include <asm/io.h>

#ifdef CONFIG_PM
#include "pmic.h"
#endif

ulong get_APLL_CLK(void);
ulong get_MPLL_CLK(void);

void check_bootmode(void);

/* Default is s5pc210 */
#define PRO_ID_OFF	0x10000000
#define S5PC210_ID	0x43210200
#define S5PV310_ID	0x43210000

//#define SMDK4212_ID 0x43220000
//#define SMDK4212_AP11_ID 0x43220210 //mj
//#define SMDK4212_AP10_ID 0x43220200 //mj

//#define SMDK4412_ID 0xE4412000
#define SECURE_PHY_BASE	0x10100018

unsigned int s5pc210_cpu_id;// = 0xc200;

char * CORE_NUM_STR="\0"; //core number information
//char * CPU_VER_STR = "\0"; //CPU VERSION

//#ifdef CONFIG_ARCH_CPU_INIT
int arch_cpu_init(void)
{
	s5pc210_cpu_id = readl(PRO_ID_OFF);

	if(s5pc210_cpu_id == S5PC210_ID) {
		unsigned int efused = *(volatile u32 *)SECURE_PHY_BASE;
		if(efused == 0x0) {
			printf("\nCPU:	S5PC210 [Samsung ASIC on SMP Platform Base on ARM CortexA9]\n");
		} else {
			printf("\nCPU:  S5PC210(Secure) [Samsung ASIC on SMP Platform Base on ARM CortexA9]\n");
		}
	}
	else if(s5pc210_cpu_id == S5PV310_ID)
		printf("\nCPU:	S5PV310 [Samsung ASIC on SMP Platform Base on ARM CortexA9]\n");


	/*added by jinzhebin*/
	else if (s5pc210_cpu_id == SMDK4212_AP10_ID)
	{
		printf("\nCPU:	SMDK4212-AP1.0 [%x]\n",s5pc210_cpu_id);
		strcpy(CORE_NUM_STR,"Dual");
	}
	else if(s5pc210_cpu_id == SMDK4212_AP11_ID)
	{
		printf("\nCPU:	SMDK4212-AP1.1 [%x]\n",s5pc210_cpu_id);
		strcpy(CORE_NUM_STR,"Dual");
	}
	else if (s5pc210_cpu_id == SMDK4412_AP10_ID)
	{
		printf("\nCPU:	SMDK4412-AP1.0 [%x]\n",s5pc210_cpu_id);
		strcpy(CORE_NUM_STR,"Quad");
	}
	else if (s5pc210_cpu_id == SMDK4412_AP11_ID)
	{
		printf("\nCPU:	SMDK4412-AP1.1 [%x]\n",s5pc210_cpu_id);
		strcpy(CORE_NUM_STR,"Quad");
	}
	
		
	printf("	APLL = %ldMHz, MPLL = %ldMHz\n", get_APLL_CLK()/1000000, get_MPLL_CLK()/1000000);
	printf("	ARM_CLOCK = %ldMHz\n", get_ARM_CLK()/1000000);
	return 0;
}
//#endif

u32 get_device_type(void)
{
	return s5pc210_cpu_id;
}

#ifdef CONFIG_DISPLAY_CPUINFO
int print_cpuinfo(void)
{
	arch_cpu_init();

#ifdef CONFIG_PM
	printf("PMIC:	");
	PMIC_InitIp();
#endif
	return 0;
}
#endif

