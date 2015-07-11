#include <common.h>
#include <command.h>
#include <asm/io.h>
#include <s5pc210.h>

int do_mdmup (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret = 0;
	int val;
/*
#define HSIC_HOST_ACTIVE	EXYNOS4_GPC0(3)
#define HSIC_SLAVE_WAKEUP	EXYNOS4_GPC0(4)
#define HSIC_HOST_WAKEUP	EXYNOS4_GPX2(5)
#define HSIC_HOST_SUSREQ	EXYNOS4_GPX1(6)
all set to input
*/
	val = readl(GPC0CON);
	val &= ~(0x0f<<12 | 0x0f<<16);
	writel(val, GPC0CON);

	val = readl(GPX2CON);
	val &= ~(0x0f<<20);
	writel(val, GPX2CON);

	val = readl(GPX1CON);
	val &= ~(0x0f<<24);
	writel(val, GPX1CON);
/*
#define GPIO_MD_PWON    EXYNOS4_GPC0(0)
#define GPIO_MD_RSTN    EXYNOS4_GPC0(2)
#define GPIO_MD_RESETBB EXYNOS4_GPL2(1)
 */
	val = readl(GPC0CON);
	val &= ~(0x0f<<0 | 0x0f<<8);
	val |= (0x01<<0 | 0x01<<8);
	writel(val, GPC0CON);

	val = readl(GPL2CON);
	val &= ~(0x0f<<4);
	val |= (0x01<<4);
	writel(val, GPL2CON);

	val = readl(GPC0DAT);
	val |= (0x01<<0 | 0x01<<2); 
	writel(val,GPC0DAT);

	val = readl(GPL2DAT);
	val |= (0x01<<1); 
	writel(val,GPL2DAT);
	udelay(50000);

	val = readl(GPC0DAT);
	val &= ~(0x01<<2); 
	writel(val,GPC0DAT);
	udelay(500000);

	val = readl(GPC0DAT);
	val |= (0x1<<2); 
	writel(val,GPC0DAT);

	printf("Now you can update Modem\n");
	return ret;
}

U_BOOT_CMD(
		mdmup,	1,	1,	do_mdmup,
		"mdmup - modem upgrade support\n",
		"mdmup - modem upgrade support\n"
	  );
