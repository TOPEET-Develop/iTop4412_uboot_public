#include <common.h>
#include <s5pc210.h>
#include <asm/io.h>
#include <mmc.h>
#include <s3c_hsmmc.h>


#ifdef CONFIG_S5P_MSHC
extern void mshci_fifo_deinit(struct mshci_host *host);
extern struct mmc *find_mmc_device(int dev_num);
#endif

void clear_hsmmc_clock_div(void)
{
	/* set sdmmc clk src as mpll */
	u32 tmp;
#ifdef CONFIG_S5P_MSHC
	struct mmc *mmc = find_mmc_device(0);

	mshci_fifo_deinit(mmc);
#endif

	tmp = CLK_SRC_FSYS & ~(0x000fffff);
	CLK_SRC_FSYS = tmp | 0x00066666;


	CLK_DIV_FSYS1 = 0x00080008;
	CLK_DIV_FSYS2 = 0x00080008;
	CLK_DIV_FSYS3 = 0x4;


}

void set_hsmmc_pre_ratio(uint clock)
{
	u32 div;
	u32 tmp;
	u32 clk,sclk_mmc,doutmmc;
	u32 i;
	
	/* XXX: we assume that clock is between 40MHz and 50MHz */
#ifdef USE_MMC0
	tmp = CLK_DIV_FSYS1 & ~(0x0000ff00);
	CLK_DIV_FSYS1 = tmp | (div << 8);
#endif

#ifdef USE_MMC2
	/* MMC2 clock div */
	div = CLK_DIV_FSYS2 & ~(0x0000ff00);
	tmp = CLK_DIV_FSYS2 & (0x0000000f);

	clk = get_MPLL_CLK();
	doutmmc = clk / (tmp + 1);
	
	for(i=0 ; i<=0xff; i++)
	{
		if((doutmmc /(i+1)) <= clock) {
			CLK_DIV_FSYS2 = tmp | i<<8;
			break;
		}
	}
	sclk_mmc = doutmmc/(i+1);
	sddbg("MPLL Clock %dM HZ\n",clk/1000000);
	sddbg("SD DOUTMMC Clock %dM HZ\n",doutmmc/1000000);
	sddbg("SD Host pre-ratio is %x\n",i);
	printf("SD sclk_mmc is %dK HZ\n",sclk_mmc/1000);
#endif
	

}
u32 sclk_mmc4;  //clock source for emmc controller
void setup_hsmmc_clock(void)
{
	u32 tmp;
	u32 clock;
	u32 i;


#ifdef USE_MMC0
#endif	

#ifdef USE_MMC1
#endif	

#ifdef USE_MMC2
	/* MMC2 clock src = SCLKMPLL */
	tmp = CLK_SRC_FSYS & ~(0x00000f00);
	CLK_SRC_FSYS = tmp | 0x00000600;

	/* MMC2 clock div */
	tmp = CLK_DIV_FSYS2 & ~(0x0000000f);
	clock = get_MPLL_CLK()/1000000;
	for(i=0 ; i<=0xf; i++)
	{
		if((clock /(i+1)) <= 50) {
			CLK_DIV_FSYS2 = tmp | i<<0;
			break;
		}
	}
	
	sddbg("[mjdbg] the sd clock ratio is %d,%d\n",i,clock);

#endif

#ifdef USE_MMC3
#endif

#ifdef USE_MMC4
	/* MMC4 clock src = SCLKMPLL */
	tmp = CLK_SRC_FSYS & ~(0x000f0000);
	CLK_SRC_FSYS = tmp | 0x00060000;
	/* MMC4 clock div */
	tmp = CLK_DIV_FSYS3 & ~(0x0000ff0f);
	clock = get_MPLL_CLK()/1000000;
	
	for(i=0 ; i<=0xf; i++)	{
		sclk_mmc4=(clock/(i+1));
		
		if(sclk_mmc4 <= 160) //200
		{
			CLK_DIV_FSYS3 = tmp | (i<<0);
			break;
		}
	}
	emmcdbg("[mjdbg] sclk_mmc4:%d MHZ; mmc_ratio: %d\n",sclk_mmc4,i);
	sclk_mmc4 *= 1000000;
#endif

}

/*
 * this will set the GPIO for hsmmc ch0
 * GPG0[0:6] = CLK, CMD, CDn, DAT[0:3]
 */
void setup_hsmmc_cfg_gpio(void)
{
	ulong reg;

#ifdef USE_MMC0
	writel(0x02222222, 0x11000040);
	writel(0x00003FFC, 0x11000048);
	writel(0x00003FFF, 0x1100004c);
	writel(0x03333000, 0x11000060);
	writel(0x00003FC0, 0x11000068);
	writel(0x00003FC0, 0x1100006c);	
#endif

#ifdef USE_MMC1
#endif

#ifdef USE_MMC2
	writel(0x02222222, 0x11000080);
	writel(0x00003FF0, 0x11000088);
	writel(0x00003FFF, 0x1100008C);
#endif

#ifdef USE_MMC3
#endif

#ifdef USE_MMC4


	
	#if 0
//reset
	writel(0x03333100, 0x11000040);
	writel(0x0, 0x11000044);
	writel(0x00003FF5, 0x11000048);
	udelay(100*1000);
	writel(0x03333333, 0x11000040);
	#else//mj
	writel(readl(0x11000048)&~(0xf),0x11000048); //SD_4_CLK/SD_4_CMD pull-down enable
	writel(readl(0x11000040)&~(0xff),0x11000040);//cdn set to be output

	writel(readl(0x11000048)&~(3<<4),0x11000048); //cdn pull-down disable
	writel(readl(0x11000044)&~(1<<2),0x11000044); //cdn output 0 to shutdown the emmc power
	writel(readl(0x11000040)&~(0xf<<8)|(1<<8),0x11000040);//cdn set to be output
	udelay(100*1000);
	writel(readl(0x11000044)|(1<<2),0x11000044); //cdn output 1

	
	writel(0x03333133, 0x11000040);
	#endif
	writel(0x00003FF0, 0x11000048);
	writel(0x00002AAA, 0x1100004C);
	
	#ifdef CONFIG_EMMC_8Bit
	writel(0x04444000, 0x11000060);
	writel(0x00003FC0, 0x11000068);
	writel(0x00002AAA, 0x1100006C);
	#endif
	
#endif

}

void setup_sdhci0_cfg_card(struct sdhci_host *host)
{
#if 0 //mj del
	u32 ctrl2;
	u32 ctrl3;
	/* don't need to alter anything acording to card-type */
	writel(S3C_SDHCI_CONTROL4_DRIVE_9mA, host->ioaddr + S3C_SDHCI_CONTROL4);
	ctrl2 = readl(host->ioaddr + S3C_SDHCI_CONTROL2);

	ctrl2 |= (S3C_SDHCI_CTRL2_ENSTAASYNCCLR |
		  S3C_SDHCI_CTRL2_ENCMDCNFMSK |
#if defined(CONFIG_MCP_SINGLE)
                  S3C_SDHCI_CTRL2_ENFBCLKRX |
#endif
		S3C_SDHCI_CTRL2_ENFBCLKTX |
		S3C_SDHCI_CTRL2_DFCNT_NONE	|
		S3C_SDHCI_CTRL2_ENCLKOUTHOLD);

	ctrl3 = 0;

	writel(ctrl2, host->ioaddr + S3C_SDHCI_CONTROL2);
	writel(ctrl3, host->ioaddr + S3C_SDHCI_CONTROL3);
#endif
}
