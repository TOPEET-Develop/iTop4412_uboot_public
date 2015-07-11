/*
 * (C) Copyright 2001-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, d.mueller@elsoft.ch
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

/* This code should work for both the S5PC110
 * as they seem to have the same PLL and clock machinery inside.
 * The different address mapping is handled by the s5pc110.h files below.
 */

#include <common.h>
#include <s5pc210.h>

#define APLL 0
#define MPLL 1
#define EPLL 2
#define VPLL 3


/* ------------------------------------------------------------------------- */
/* NOTE: This describes the proper use of this file.
 *
 * CONFIG_SYS_CLK_FREQ should be defined as the input frequency of the PLL.
 *
 * get_FCLK(), get_HCLK(), get_PCLK() and get_UCLK() return the clock of
 * the specified bus in HZ.
 */
/* ------------------------------------------------------------------------- */

static ulong get_PLLCLK(int pllreg)
{
	ulong r, m, p, s;

	if (pllreg == APLL) {
		r = APLL_CON0_REG;
		m = (r>>16) & 0x3ff;
	} else if (pllreg == MPLL) {
		r = MPLL_CON0_REG;
		m = (r>>16) & 0x3ff;
	} else
		hang();

	p = (r>>8) & 0x3f;
	s = r & 0x7;

	//if ((pllreg == APLL) || (pllreg == MPLL)) 
	//	s= s-1;
	
	return (m * (CONFIG_SYS_CLK_FREQ / (p * (1 << s))));
}

ulong get_APLL_CLK(void)
{
	return (get_PLLCLK(APLL));
}

ulong get_MPLL_CLK(void)
{
	return (get_PLLCLK(MPLL));
}

ulong get_ARM_CLK(void)//mj
{
	u32 apll_out;
	u32 reg,core_ratio,core2_ratio;

	reg = CLK_DIV_CPU0;
	core_ratio = reg&0x00000007;
	core2_ratio = (reg>>28)&0x7;
	apll_out=get_PLLCLK(APLL);
	//printf("cpu ratio: %d,%d",core_ratio,core2_ratio);
	return ((apll_out/(core_ratio+1))/(core2_ratio+1));
}


