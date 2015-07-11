/*
 * Copyright 2008, Freescale Semiconductor, Inc
 * Andy Fleming
 *
 * Based vaguely on the Linux code
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
#include <common.h>
#include <command.h>
#include <mmc.h>
#include <part.h>
#include <malloc.h>
#include <linux/list.h>
#include <mmc.h>
#include <div64.h>

#ifdef CONFIG_CMD_MOVINAND
extern int init_raw_area_table (block_dev_desc_t * dev_desc);
#endif
extern int emmc_mode_change(struct mmc *mmc); //mj


#define CAPACITY_THEREHOLD 0x400000 //only sector bigger than this, the card is high capacity
static struct list_head mmc_devices;
static int cur_dev_num = -1;

struct mmc *mmc_global[3] = {NULL, NULL, NULL};
struct mmc *mmc_default = NULL;

int mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd, struct mmc_data *data)
{
	return mmc->send_cmd(mmc, cmd, data);
}

int mmc_set_blocklen(struct mmc *mmc, int len)
{
	struct mmc_cmd cmd;

	cmd.cmdidx = MMC_CMD_SET_BLOCKLEN;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = len;
	cmd.flags = 0;

	return mmc_send_cmd(mmc, &cmd, NULL);
}

struct mmc *find_mmc_device(int dev_num)
{
	struct mmc *m;
	struct list_head *entry;

	list_for_each(entry, &mmc_devices) {
		m = list_entry(entry, struct mmc, link);

		if (m->block_dev.dev == dev_num)
			return m;
	}

	printf("MMC Device %d not found\n", dev_num);

	return NULL;
}

static ulong
mmc_bread_primitive(int dev_num, ulong start, lbaint_t blkcnt, void *dst)
{
	struct mmc *host = find_mmc_device(dev_num);
	int err;
	struct mmc_cmd cmd;
	struct mmc_data data;

	if (!host)
		return 0;

#if 0
	/* We always do full block reads from the card */
	err = mmc_set_blocklen(host, (1<<9));
	if (err) {
		return 0;
	}
#endif

	if (blkcnt > 1)
		cmd.cmdidx = MMC_CMD_READ_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_READ_SINGLE_BLOCK;

	if (host->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * (1<<9);

	cmd.resp_type = MMC_RSP_R1;
	cmd.flags = 0;

	data.dest = dst;
	data.blocks = blkcnt;
	data.blocksize = (1<<9);
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(host, &cmd, &data);
	if (err) {
		printf("mmc read failed\n");
		return err;
	}

	return blkcnt;
}

#define MAXBLKCNT	0xFFFF
static ulong
mmc_bread(int dev_num, ulong start, lbaint_t blkcnt, void *dst)
{
	lbaint_t blkcnt_tmp = blkcnt;
	ulong ret = 0;

	while (blkcnt_tmp >= MAXBLKCNT)
	{
		ret += mmc_bread_primitive(dev_num, start, MAXBLKCNT, dst);

		start += MAXBLKCNT;
		dst += MAXBLKCNT * 512;
		blkcnt_tmp -= MAXBLKCNT;
	}
	ret += mmc_bread_primitive(dev_num, start, blkcnt_tmp, dst);

	return ret;
}

static ulong
mmc_bwrite(int dev_num, ulong start, lbaint_t blkcnt, const void*src)
{
	struct mmc *host = find_mmc_device(dev_num);
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err;
	printf(".");

	if (!host)
		return 0;
/*
	err = mmc_set_blocklen(host, (1<<9));
	if (err) {
		printf("set write bl len failed\n");
		return err;
	}
*/
	if (blkcnt > 1)
		cmd.cmdidx = MMC_CMD_WRITE_MULTIPLE_BLOCK;
	else
		cmd.cmdidx = MMC_CMD_WRITE_SINGLE_BLOCK;

	if (host->high_capacity)
		cmd.cmdarg = start;
	else
		cmd.cmdarg = start * (1<<9);

	cmd.resp_type = MMC_RSP_R1;
	cmd.flags = 0;

	data.src = src;
	data.blocks = blkcnt;
	data.blocksize = (1<<9);
	data.flags = MMC_DATA_WRITE;

	err = mmc_send_cmd(host, &cmd, &data);
	if (err) {
		printf("mmc write failed\n");
		return err;
	}

#if defined(CONFIG_VOGUES)
	mmc_bread(dev_num, start, blkcnt, src);
#endif
	return blkcnt;
}

ulong movi_write(ulong start, lbaint_t blkcnt, void *src)
{
	return mmc_bwrite(0, start, blkcnt, src);
}

ulong movi_read(ulong start, lbaint_t blkcnt, void *dst)
{
	return mmc_bread(0, start, blkcnt, dst);
}

int mmc_go_idle(struct mmc* host)
{
	struct mmc_cmd cmd;
	int err;

	udelay(1000);

	cmd.cmdidx = MMC_CMD_GO_IDLE_STATE;
	cmd.cmdarg = 0;
	cmd.resp_type = MMC_RSP_NONE;
	cmd.flags = 0;

	err = mmc_send_cmd(host, &cmd, NULL);

	if (err)
		return err;

	udelay(2000);

	return 0;
}

int mmc_send_app_op_cond(struct mmc *host)
{
	int timeout = 100;
	int err;
	struct mmc_cmd cmd;

	do {
		cmd.cmdidx = MMC_CMD_APP_CMD;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = 0;
		cmd.flags = 0;

		err = mmc_send_cmd(host, &cmd, NULL);

		if (err)
			return err;

		cmd.cmdidx = SD_CMD_APP_SEND_OP_COND;	/* when C110 : SD_APP_OP_COND	*/
		cmd.resp_type = MMC_RSP_R3;
		cmd.cmdarg = host->voltages;

		if (host->version == SD_VERSION_2)
			cmd.cmdarg |= OCR_HCS;

		err = mmc_send_cmd(host, &cmd, NULL);

		if (err)
			return err;

		udelay(10000);
	} while ((!(cmd.response[0] & OCR_BUSY)) && timeout--);

	if (timeout <= 0)
		return UNUSABLE_ERR;

	if (host->version != SD_VERSION_2)
		host->version = SD_VERSION_1_0;

	host->ocr = cmd.response[0];
	host->high_capacity = ((host->ocr & OCR_HCS) == OCR_HCS);
	host->rca = 0;

	return 0;
}

int mmc_send_op_cond(struct mmc *mmc)
{
	int timeout = 1000;
	struct mmc_cmd cmd;
	int err;

	/* Some cards seem to need this */
	mmc_go_idle(mmc);

	do {
		cmd.cmdidx = MMC_CMD_SEND_OP_COND;
		cmd.resp_type = MMC_RSP_R3;
		cmd.cmdarg = OCR_HCS | mmc->voltages;
		cmd.flags = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);

		if (err)
			return err;

		udelay(1000);
	} while (!(cmd.response[0] & OCR_BUSY) && timeout--);

	if (timeout <= 0)
		return UNUSABLE_ERR;

	mmc->version = MMC_VERSION_UNKNOWN;
	mmc->ocr = cmd.response[0];

	mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
	mmc->rca = 0;

	return 0;
}

int mmc_send_ext_csd(struct mmc *mmc, u8 *ext_csd)
{
	struct mmc_cmd cmd;
	struct mmc_data data;
	int err;

	/* Get the Card Status Register */
	cmd.cmdidx = MMC_CMD_SEND_EXT_CSD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	data.dest = ext_csd;
	data.blocks = 1;
	data.blocksize = 512;
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);

	return err;
}

int mmc_set_relative_addr(struct mmc *host)
{
	int err;
	struct mmc_cmd cmd;

	cmd.cmdidx = MMC_CMD_SET_RELATIVE_ADDR;	/* C110 : MMC_SET_RELATIVE_ADDR	*/
	cmd.cmdarg = host->rca << 16;
	cmd.flags = MMC_RSP_R1;

	err = mmc_send_cmd(host, &cmd, NULL);

	return err;
}

int mmc_switch(struct mmc *host, u8 set, u8 index, u8 value)
{
	struct mmc_cmd cmd;

	cmd.cmdidx = MMC_CMD_SWITCH;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
		(index << 16) |
		(value << 8);
	cmd.flags = 0;

	return mmc_send_cmd(host, &cmd, NULL);
}

int mmc_change_freq(struct mmc *mmc)
{
	u8 ext_csd[512];
	char cardtype;
	int err;
	u8 high_speed = 0;

	mmc->card_caps = 0;

	/* Only version 4 supports high-speed */
	if (mmc->version < MMC_VERSION_4)
		return 0;


	err = mmc_send_ext_csd(mmc, ext_csd);

	if (err)
		return err;
		
	#if 0 //mj temp
	if (ext_csd[212] || ext_csd[213] || ext_csd[214] || ext_csd[215])
		mmc->high_capacity = 1;
	#endif
	
	cardtype = ext_csd[196] & 0xf;
	emmcdbg("cardtype: 0x%08x\n", cardtype);
	/* Now check to see that it worked */

	err = mmc_send_ext_csd(mmc, ext_csd);

	if (err)
		return err;

	/* High Speed is set, there are three types: DDR 52Mhz, 
	   52MHz and 26MHz */
	if (cardtype & (MMC_HS_52MHZ_1_8V_3V_IO)) {
	emmcdbg("MMC_HS_52MHZ_1_8V_3V_IO\n");
		mmc->card_caps |= MMC_MODE_HS_52MHz_DDR_18_3V | 
						MMC_MODE_HS_52MHz | MMC_MODE_HS |
						MMC_MODE_8BIT_DDR | MMC_MODE_4BIT_DDR |
						MMC_MODE_4BIT | MMC_MODE_8BIT;
		high_speed = 1;
	}
	else if (cardtype & MMC_HS_52MHZ) {
	emmcdbg("MMC_HS_52MHZ\n");
		mmc->card_caps |= MMC_MODE_HS_52MHz | MMC_MODE_HS |
						MMC_MODE_4BIT | MMC_MODE_8BIT;
		high_speed = 1;
	}
	else {
	emmcdbg("MMC_MODE_HS\n");
		mmc->card_caps |= MMC_MODE_HS |
						MMC_MODE_4BIT | MMC_MODE_8BIT;
		high_speed = 0;
	}

	/* Set High speed mode */
	err = mmc_switch(mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_HS_TIMING, high_speed);		
	if (err)
		return err;

	return 0;
}

int sd_switch(struct mmc *mmc, int mode, int group, u8 value, u8 *resp)
{
	struct mmc_cmd cmd;
	struct mmc_data data;

	/* Switch the frequency */
	cmd.cmdidx = SD_CMD_SWITCH_FUNC;	/* C110 : SD_SWITCH	*/
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = (mode << 31) | 0xffffff;
	cmd.cmdarg &= ~(0xf << (group * 4));
	cmd.cmdarg |= value << (group * 4);
	cmd.flags = 0;

	data.dest = resp;
	data.blocksize = 64;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	return mmc_send_cmd(mmc, &cmd, &data);
}


int sd_change_freq(struct mmc *mmc)
{
	int err;
	struct mmc_cmd cmd;
	uint scr[2];
	uint switch_status[16];
	struct mmc_data data;
	int timeout;

	mmc->card_caps = 0;

	/* Read the SCR to find out if this card supports higher speeds */
	cmd.cmdidx = MMC_CMD_APP_CMD;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = mmc->rca << 16;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);

	if (err)
		return err;

	cmd.cmdidx = SD_CMD_APP_SEND_SCR;	/* C110 : SD_APP_SEND_SCR	*/
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	timeout = 3;

retry_scr:
	data.dest = (u8 *)&scr;
	data.blocksize = 8;
	data.blocks = 1;
	data.flags = MMC_DATA_READ;

	err = mmc_send_cmd(mmc, &cmd, &data);

	if (err) {
		if (timeout--)
			goto retry_scr;

		return err;
	}

	mmc->scr[0] = __be32_to_cpu(scr[0]);
	mmc->scr[1] = __be32_to_cpu(scr[1]);

	switch ((mmc->scr[0] >> 24) & 0xf) {
		case 0:
			mmc->version = SD_VERSION_1_0;
			break;
		case 1:
			mmc->version = SD_VERSION_1_10;
			break;
		case 2:
			mmc->version = SD_VERSION_2;
			break;
		default:
			mmc->version = SD_VERSION_1_0;
			break;
	}

	/* Version 1.0 doesn't support switching */
	if (mmc->version == SD_VERSION_1_0)
		return 0;

	timeout = 4;
	while (timeout--) {
		err = sd_switch(mmc, SD_SWITCH_CHECK, 0, 1,
				(u8 *)&switch_status);

		if (err)
			return err;

		/* The high-speed function is busy.  Try again */
		if (!(__be32_to_cpu(switch_status[7]) & SD_HIGHSPEED_BUSY))
			break;
	}

//	if (mmc->scr[0] & SD_DATA_4BIT)
		mmc->card_caps |= MMC_MODE_4BIT;

	/* If high-speed isn't supported, we return */
	if (!(__be32_to_cpu(switch_status[3]) & SD_HIGHSPEED_SUPPORTED))
		return 0;

	err = sd_switch(mmc, SD_SWITCH_SWITCH, 0, 1, (u8 *)&switch_status);

	if (err)
		return err;

	if ((__be32_to_cpu(switch_status[4]) & 0x0f000000) == 0x01000000) {
		mmc->card_caps |= MMC_MODE_HS;
		mmc->clock = 50000000;
	} else {
		mmc->clock = 25000000;
	}

	return 0;
}

static const unsigned int tran_exp[] = {
	10000,		100000,		1000000,	10000000,
	0,		0,		0,		0
};

static const unsigned char tran_mant[] = {
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};

static const unsigned int tacc_exp[] = {
	1,	10,	100,	1000,	10000,	100000,	1000000, 10000000,
};

static const unsigned int tacc_mant[] = {
	0,	10,	12,	13,	15,	20,	25,	30,
	35,	40,	45,	50,	55,	60,	70,	80,
};

#define UNSTUFF_BITS(resp,start,size)					\
	({								\
		const int __size = size;				\
		const u32 __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
		const int __off = 3 - ((start) / 32);			\
		const int __shft = (start) & 31;			\
		u32 __res;						\
									\
		__res = resp[__off] >> __shft;				\
		if (__size + __shft > 32)				\
			__res |= resp[__off-1] << ((32 - __shft) % 32);	\
		__res & __mask;						\
	})

/*
 * Given a 128-bit response, decode to our card CSD structure. for SD
 * every
 */
static int sd_decode_csd(struct mmc *host)
{
	struct mmc_csd csd_org, *csd;
	unsigned int e, m, csd_struct;
	u32 *resp = host->csd;
	csd = &csd_org;

	csd_struct = UNSTUFF_BITS(resp, 126, 2);
	emmcdbg(" %s, struct:%d \n \n", __func__,csd_struct);
	switch (csd_struct) {
	case 0:
		m = UNSTUFF_BITS(resp, 115, 4);
		e = UNSTUFF_BITS(resp, 112, 3);
		csd->tacc_ns	 = (tacc_exp[e] * tacc_mant[m] + 9) / 10;
		csd->tacc_clks	 = UNSTUFF_BITS(resp, 104, 8) * 100;

		m = UNSTUFF_BITS(resp, 99, 4);
		e = UNSTUFF_BITS(resp, 96, 3);
		csd->max_dtr	  = tran_exp[e] * tran_mant[m];
		csd->cmdclass	  = UNSTUFF_BITS(resp, 84, 12);

		e = UNSTUFF_BITS(resp, 47, 3);
		m = UNSTUFF_BITS(resp, 62, 12);
		csd->capacity	  = (1 + m) << (e + 2);

		csd->read_blkbits = UNSTUFF_BITS(resp, 80, 4);
		csd->read_partial = UNSTUFF_BITS(resp, 79, 1);
		csd->write_misalign = UNSTUFF_BITS(resp, 78, 1);
		csd->read_misalign = UNSTUFF_BITS(resp, 77, 1);
		csd->r2w_factor = UNSTUFF_BITS(resp, 26, 3);
		csd->write_blkbits = UNSTUFF_BITS(resp, 22, 4);
		csd->write_partial = UNSTUFF_BITS(resp, 21, 1);
		host->read_bl_len = (1<<9);
		host->write_bl_len = (1<<9);
		host->capacity = csd->capacity<<(csd->read_blkbits - 9);
		break;
	case 1:
		/*
		 * This is a block-addressed SDHC card. Most
		 * interesting fields are unused and have fixed
		 * values. To avoid getting tripped by buggy cards,
		 * we assume those fixed values ourselves.
		 */

		csd->tacc_ns	 = 0; /* Unused */
		csd->tacc_clks	 = 0; /* Unused */

		m = UNSTUFF_BITS(resp, 99, 4);
		e = UNSTUFF_BITS(resp, 96, 3);
		csd->max_dtr	  = tran_exp[e] * tran_mant[m];
		csd->cmdclass	  = UNSTUFF_BITS(resp, 84, 12);

		m = UNSTUFF_BITS(resp, 48, 22);
		csd->capacity     = (1 + m) << 10;

		csd->read_blkbits = 9;
		csd->read_partial = 0;
		csd->write_misalign = 0;
		csd->read_misalign = 0;
		csd->r2w_factor = 4; /* Unused */
		csd->write_blkbits = 9;
		csd->write_partial = 0;

		host->high_capacity = 1;
		host->read_bl_len = (1<<9);
		host->write_bl_len = (1<<9);
		host->capacity = csd->capacity;
		break;
	default:
		printf("unrecognised CSD structure version %d\n"
			, csd_struct);
		return -1;
	}

	return 0;
}

#if 0
/*
 * Given the decoded CSD structure, decode the raw CID to our CID structure.
 */
static int mmc_decode_cid(struct mmc_card *card)
{
	u32 *resp = card->raw_cid;

	/*
	 * The selection of the format here is based upon published
	 * specs from sandisk and from what people have reported.
	 */
	switch (card->csd.mmca_vsn) {
	case 0: /* MMC v1.0 - v1.2 */
	case 1: /* MMC v1.4 */
		card->cid.manfid	= UNSTUFF_BITS(resp, 104, 24);
		card->cid.prod_name[0]	= UNSTUFF_BITS(resp, 96, 8);
		card->cid.prod_name[1]	= UNSTUFF_BITS(resp, 88, 8);
		card->cid.prod_name[2]	= UNSTUFF_BITS(resp, 80, 8);
		card->cid.prod_name[3]	= UNSTUFF_BITS(resp, 72, 8);
		card->cid.prod_name[4]	= UNSTUFF_BITS(resp, 64, 8);
		card->cid.prod_name[5]	= UNSTUFF_BITS(resp, 56, 8);
		card->cid.prod_name[6]	= UNSTUFF_BITS(resp, 48, 8);
		card->cid.hwrev		= UNSTUFF_BITS(resp, 44, 4);
		card->cid.fwrev		= UNSTUFF_BITS(resp, 40, 4);
		card->cid.serial	= UNSTUFF_BITS(resp, 16, 24);
		card->cid.month		= UNSTUFF_BITS(resp, 12, 4);
		card->cid.year		= UNSTUFF_BITS(resp, 8, 4) + 1997;
		break;

	case 2: /* MMC v2.0 - v2.2 */
	case 3: /* MMC v3.1 - v3.3 */
	case 4: /* MMC v4 */
		card->cid.manfid	= UNSTUFF_BITS(resp, 120, 8);
		card->cid.oemid		= UNSTUFF_BITS(resp, 104, 16);
		card->cid.prod_name[0]	= UNSTUFF_BITS(resp, 96, 8);
		card->cid.prod_name[1]	= UNSTUFF_BITS(resp, 88, 8);
		card->cid.prod_name[2]	= UNSTUFF_BITS(resp, 80, 8);
		card->cid.prod_name[3]	= UNSTUFF_BITS(resp, 72, 8);
		card->cid.prod_name[4]	= UNSTUFF_BITS(resp, 64, 8);
		card->cid.prod_name[5]	= UNSTUFF_BITS(resp, 56, 8);
		card->cid.serial	= UNSTUFF_BITS(resp, 16, 32);
		card->cid.month		= UNSTUFF_BITS(resp, 12, 4);
		card->cid.year		= UNSTUFF_BITS(resp, 8, 4) + 1997;
		break;

	default:
		printk(KERN_ERR "%s: card has unknown MMCA version %d\n",
			mmc_hostname(card->host), card->csd.mmca_vsn);
		return -EINVAL;
	}

	return 0;
}
#endif

/*
 * Given a 128-bit response, decode to our card CSD structure.
 */
static int mmc_decode_csd(struct mmc *host)
{
	struct mmc_csd csd_org, *csd;
	unsigned int e, m, csd_struct;
	u32 *resp = host->csd;

	csd = &csd_org;

	/*
	 * We only understand CSD structure v1.1 and v1.2.
	 * v1.2 has extra information in bits 15, 11 and 10.
	 */
	csd_struct = UNSTUFF_BITS(resp, 126, 2);
	if (csd_struct > 3) {
		printf("unrecognised CSD structure version %d\n",
			csd_struct);
		return -1;
	}

	csd->mmca_vsn	 = UNSTUFF_BITS(resp, 122, 4);
	switch (csd->mmca_vsn) {
	case 0:
		host->version = MMC_VERSION_1_2;
		break;
	case 1:
		host->version = MMC_VERSION_1_4;
		break;
	case 2:
		host->version = MMC_VERSION_2_2;
		break;
	case 3:
		host->version = MMC_VERSION_3;
		break;
	case 4:
		host->version = MMC_VERSION_4;
		break;
	}

	m = UNSTUFF_BITS(resp, 115, 4);
	e = UNSTUFF_BITS(resp, 112, 3);
	csd->tacc_ns	 = (tacc_exp[e] * tacc_mant[m] + 9) / 10;
	csd->tacc_clks	 = UNSTUFF_BITS(resp, 104, 8) * 100;

	m = UNSTUFF_BITS(resp, 99, 4);
	e = UNSTUFF_BITS(resp, 96, 3);
	csd->max_dtr	  = tran_exp[e] * tran_mant[m];
	csd->cmdclass	  = UNSTUFF_BITS(resp, 84, 12);

	e = UNSTUFF_BITS(resp, 47, 3);
	m = UNSTUFF_BITS(resp, 62, 12);
	csd->capacity	  = (1 + m) << (e + 2);

	csd->read_blkbits = UNSTUFF_BITS(resp, 80, 4);
	csd->read_partial = UNSTUFF_BITS(resp, 79, 1);
	csd->write_misalign = UNSTUFF_BITS(resp, 78, 1);
	csd->read_misalign = UNSTUFF_BITS(resp, 77, 1);
	csd->r2w_factor = UNSTUFF_BITS(resp, 26, 3);
	csd->write_blkbits = UNSTUFF_BITS(resp, 22, 4);
	csd->write_partial = UNSTUFF_BITS(resp, 21, 1);

	host->read_bl_len = 1 << csd->read_blkbits;
	host->write_bl_len = 1 << csd->write_blkbits;
	host->capacity = csd->capacity;
	host->clock = 20000000;
	emmcdbg("csd->read_partial:%d \n",csd->read_partial);
	emmcdbg("host->read_bl_len:%d \n",host->read_bl_len);
	emmcdbg("host->write_bl_len :%d \n",host->write_bl_len );
	emmcdbg("host->capacity:%x \n",host->capacity);
	/*---chang the readlen----*/
	#if 0
	if((host->read_bl_len>512)||(host->write_bl_len>512))
	{
		printf("Change the read/write length and capacity");
		host->capacity = host->capacity*2;
		host->read_bl_len = 512;
		emmcdbg("host->capacity:%x \n",host->capacity);
		emmcdbg("host->read_bl_len:%d \n",host->read_bl_len);
	}
	#endif
	return 0;
}

/*
 * Read and decode extended CSD.
 */
static int mmc_read_ext_csd(struct mmc *host)
{
	int err;
	u8 *ext_csd;
	unsigned int ext_csd_struct;

	if (host->version < (MMC_VERSION_4 | MMC_VERSION_MMC))
		return 0;

	/*
	 * As the ext_csd is so large and mostly unused, we don't store the
	 * raw block in mmc_card.
	 */
	ext_csd = malloc(512);
	if (!ext_csd) {
		printf("could not allocate a buffer to "
			"receive the ext_csd.\n");
		return -1;
	}

	err = mmc_switch(host, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_HS_TIMING, 0);
	if (err)
		return err;

	err = mmc_switch(host, EXT_CSD_CMD_SET_NORMAL,
				EXT_CSD_BUS_WIDTH,
				EXT_CSD_BUS_WIDTH_1);	
	if (err)
		return err;
	

	err = mmc_send_ext_csd(host, ext_csd);
	if (err) {
		/*
		 * High capacity cards should have this "magic" size
		 * stored in their CSD.
		 */
		if (host->capacity == (4096 * 512)) {
			printf("unable to read EXT_CSD "
				"on a possible high capacity card. "
				"Card will be ignored.\n");
		} else {
			printf("unable to read "
				"EXT_CSD, performance might suffer.\n");
			err = 0;
		}

		goto out;
	}

	ext_csd_struct = ext_csd[EXT_CSD_REV];
	host->ext_csd.boot_size_multi = ext_csd[BOOT_SIZE_MULTI];//mj
	
	/* modify by cym 20140416 */
#if 0
	if (ext_csd_struct > 5) {
#else
	if ((ext_csd_struct > 5) && (7 != ext_csd_struct)) {
#endif
	/* end modify */
		printf("unrecognised EXT_CSD structure "
			"version %d\n", ext_csd_struct);
		err = -1;
		goto out;
	}

	if (ext_csd_struct >= 2) {
		host->ext_csd.sectors =
			ext_csd[EXT_CSD_SEC_CNT + 0] << 0 |
			ext_csd[EXT_CSD_SEC_CNT + 1] << 8 |
			ext_csd[EXT_CSD_SEC_CNT + 2] << 16 |
			ext_csd[EXT_CSD_SEC_CNT + 3] << 24;
		if ((host->ext_csd.sectors)&&(host->ext_csd.sectors>CAPACITY_THEREHOLD)) {//mj
			emmcdbg("this is high capacity card\n");
			host->high_capacity = 1;
			host->capacity = host->ext_csd.sectors;
		}
		else
		{

			host->high_capacity = 0;
			// the host->capacity use the csd value.  
			emmcdbg("Becareful, this is not high capacity card\n");
		}
	}
	emmcdbg("boot_size_multi:%x \n",host->ext_csd.boot_size_multi);
	emmcdbg("ext_csd.sectors:%x \n",host->ext_csd.sectors);
	emmcdbg(" the card tyep is:%x \n",ext_csd[EXT_CSD_CARD_TYPE]);
	emmcdbg("boot configer[179]:%x \n",ext_csd[179]);
	emmcdbg("boot configer[177]:%x \n",ext_csd[177]);
	if ((ext_csd[EXT_CSD_CARD_TYPE] & 0xf) & (EXT_CSD_CARD_TYPE_52 |
			EXT_CSD_CARD_TYPE_52_DDR_18_30 | EXT_CSD_CARD_TYPE_52_DDR_12)) {
		host->ext_csd.hs_max_dtr = 52000000;
		host->clock = 50000000;
		emmcdbg(" clock 1:%d \n",host->clock);
	} else if (ext_csd[EXT_CSD_CARD_TYPE] & EXT_CSD_CARD_TYPE_26) {
		emmcdbg(" clock 2:%d \n",host->clock);
		host->ext_csd.hs_max_dtr = 26000000;
		host->clock = 26000000;
	} else {
		/* MMC v4 spec says this cannot happen */
		printf("card is mmc v4 but doesn't "
				"support any high-speed modes.\n");
		goto out;
	}

out:
	free(ext_csd);

	return err;
}

void mmc_set_ios(struct mmc *mmc)
{
	mmc->set_ios(mmc);
}

void mmc_set_clock(struct mmc *mmc, uint clock)
{
	if (clock > mmc->f_max)
		clock = mmc->f_max;

	if (clock < mmc->f_min)
		clock = mmc->f_min;

	mmc->clock = clock;

	mmc_set_ios(mmc);
}

void mmc_set_bus_width(struct mmc *mmc, uint width)
{
	emmcdbg("mmc set bus width:%d\n",width);
	mmc->bus_width = width;

	mmc_set_ios(mmc);
}

int mmc_startup(struct mmc *host)
{
	int err;
	struct mmc_cmd cmd;
	emmcdbg(" mmc_startup\n");
	/* Put the Card in Identify Mode */
	cmd.cmdidx = MMC_CMD_ALL_SEND_CID;	/* C110 : MMC_ALL_SEND_CID	*/
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	err = mmc_send_cmd(host, &cmd, NULL);
	if (err)
		return err;

	memcpy(host->cid, cmd.response, 16);

	/*
	 * For MMC cards, set the Relative Address.
	 * For SD cards, get the Relatvie Address.
	 * This also puts the cards into Standby State
	 */
	cmd.cmdidx = SD_CMD_SEND_RELATIVE_ADDR;
	cmd.cmdarg = host->rca << 16;
	cmd.resp_type = MMC_RSP_R6;
	cmd.flags = 0;

	err = mmc_send_cmd(host, &cmd, NULL);

	if (err)
		return err;

	if (IS_SD(host))
		host->rca = (cmd.response[0] >> 16) & 0xffff;

	/* Get the Card-Specific Data */
	cmd.cmdidx = MMC_CMD_SEND_CSD;
	cmd.resp_type = MMC_RSP_R2;
	cmd.cmdarg = host->rca << 16;
	cmd.flags = 0;

	err = mmc_send_cmd(host, &cmd, NULL);

	if (err)
		return err;

	memcpy(host->csd, cmd.response, 4*4);
	if (IS_SD(host))
		sd_decode_csd(host);
	else
		mmc_decode_csd(host);

	/* Select the card, and put it into Transfer Mode */
	cmd.cmdidx = MMC_CMD_SELECT_CARD;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = host->rca << 16;
	cmd.flags = 0;
	err = mmc_send_cmd(host, &cmd, NULL);
	if (err)
		return err;

	if (IS_SD(host)) {
		err = sd_change_freq(host);
	}
	else {
		//mmc_set_clock(host,10000000);  //mj: why ?
		err = mmc_read_ext_csd(host);
		if (err)
			return err;
		err = mmc_change_freq(host);
	}

	if (err)
		return err;

	/* Restrict card's capabilities by what the host can do */
	host->card_caps &= host->host_caps;
	emmcdbg("\n Final host->card_caps: %x \n",host->card_caps);
	if (IS_SD(host)) { //0x20020  SD_VERSION_2

		emmcdbg("\n the host version is1: %x \n",host->version);
		if (host->card_caps & MMC_MODE_4BIT) //host->card_caps:101
		{
			emmcdbg("[mjdbg]host->card_caps:%x \n",host->card_caps);
			cmd.cmdidx = MMC_CMD_APP_CMD;
			cmd.resp_type = MMC_RSP_R1;
			cmd.cmdarg = host->rca << 16;
			cmd.flags = 0;

			err = mmc_send_cmd(host, &cmd, NULL);
			if (err)
				return err;

			cmd.cmdidx = SD_CMD_APP_SET_BUS_WIDTH;
			cmd.resp_type = MMC_RSP_R1;
			cmd.cmdarg = 2;
			cmd.flags = 0;
			err = mmc_send_cmd(host, &cmd, NULL);
			if (err)
				return err;

			mmc_set_bus_width(host, MMC_BUS_WIDTH_4);
		}

		if (host->card_caps & (MMC_MODE_HS | MMC_MODE_HS_52MHz_DDR_12V |
								MMC_MODE_HS_52MHz_DDR_18_3V))
			host->clock = 50000000;
		else
			host->clock = 25000000;
	}
	else {
		emmcdbg(" >>>This is not SD card host->version: %x \n",host->version);
		if (host->card_caps & MMC_MODE_8BIT_DDR) {
			/* Set the card to use 4 bit*/
			err = mmc_switch(host, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_BUS_WIDTH,
					EXT_CSD_BUS_WIDTH_8_DDR);
			if (err)
				return err;
			mmc_set_bus_width(host, MMC_BUS_WIDTH_8_DDR);			
		} else if (host->card_caps & MMC_MODE_4BIT_DDR) {
			/* Set the card to use 4 bit*/
			err = mmc_switch(host, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_BUS_WIDTH,
					EXT_CSD_BUS_WIDTH_4_DDR);
			if (err)
				return err;
			mmc_set_bus_width(host, MMC_BUS_WIDTH_4_DDR);			
		} else if (host->card_caps & MMC_MODE_8BIT) {
			/* Set the card to use 8 bit*/
			err = mmc_switch(host, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_BUS_WIDTH,
					EXT_CSD_BUS_WIDTH_8);
			if (err)
				return err;
			mmc_set_bus_width(host, MMC_BUS_WIDTH_8);
		} else if (host->card_caps & MMC_MODE_4BIT) {
			/* Set the card to use 4 bit*/
			err = mmc_switch(host, EXT_CSD_CMD_SET_NORMAL,
					EXT_CSD_BUS_WIDTH,
					EXT_CSD_BUS_WIDTH_4);
			if (err)
				return err;
			mmc_set_bus_width(host, MMC_BUS_WIDTH_4);
		} else {
			mmc_set_bus_width(host, EXT_CSD_BUS_WIDTH_1);
		}
	}

	mmc_set_ios(host);  

	/* fill in device description */
	host->block_dev.lun = 0;
	host->block_dev.type = 0;
	host->block_dev.blksz = (1<<9);
	host->block_dev.lba = host->capacity;
	sprintf(host->block_dev.vendor, "Man %06x Snr %08x", host->cid[0] >> 8,
			(host->cid[2] << 8) | (host->cid[3] >> 24));
	sprintf(host->block_dev.product, "%c%c%c%c%c", host->cid[0] & 0xff,
			(host->cid[1] >> 24), (host->cid[1] >> 16) & 0xff,
			(host->cid[1] >> 8) & 0xff, host->cid[1] & 0xff);
	sprintf(host->block_dev.revision, "%d.%d", host->cid[2] >> 28,
			(host->cid[2] >> 24) & 0xf);
	err = init_part(&host->block_dev);
	if(err<0)  //mj:mmc read faill..
	{
		printf("[ERROR] mmc read part type error..!!\n");
		return err;
	}
	else if (err>0) //get part type fail
	{
		printf(" >>>get part_type fail : %x \n",host->block_dev.part_type);
	}

	//printf(" >>>part_type is : %x \n",host->block_dev.part_type);

#ifdef CONFIG_CMD_MOVINAND
	init_raw_area_table(&host->block_dev);
#endif
	return 0;
}

int mmc_send_if_cond(struct mmc *host)
{
	struct mmc_cmd cmd;
	int err;
	static const u8 test_pattern = 0xAA;

	/*
	 * To support SD 2.0 cards, we must always invoke SD_SEND_IF_COND
	 * before SD_APP_OP_COND. This command will harmlessly fail for
	 * SD 1.0 cards.
	 */
	cmd.cmdidx = SD_CMD_SEND_IF_COND;
	cmd.cmdarg = ((host->voltages & 0xFF8000) != 0) << 8 | test_pattern;
	cmd.resp_type = MMC_RSP_R7;
	cmd.flags = 0;

	err = mmc_send_cmd(host, &cmd, NULL);

	if (err)
		return err;

	if ((cmd.response[0] & 0xff) != 0xaa)
		return UNUSABLE_ERR;
	else
		host->version = SD_VERSION_2;

	return 0;
}

int mmc_register(struct mmc *mmc)
{
	/* Setup the universal parts of the block interface just once */
	mmc->block_dev.if_type = IF_TYPE_MMC;
	mmc->block_dev.dev = cur_dev_num++;
	mmc->block_dev.removable = 1;
	mmc->block_dev.block_read = mmc_bread;
	mmc->block_dev.block_write = mmc_bwrite;

	INIT_LIST_HEAD (&mmc->link);

	list_add_tail (&mmc->link, &mmc_devices);

	return 0;
}

block_dev_desc_t *mmc_get_dev(int dev)
{
	struct mmc *mmc = find_mmc_device(dev);

	return mmc ? &mmc->block_dev : NULL;
}

int mmc_init(struct mmc *host)
{
	int err;

	err = host->init(host);
	if (err)
		return err;

	/* Reset the Card */
	err = mmc_go_idle(host);

	if (err)
		return err;

	/* Test for SD version 2 */
	err = mmc_send_if_cond(host);

	/* Now try to get the SD card's operating condition */
	err = mmc_send_app_op_cond(host);

	/* If the command timed out, we check for an MMC card */
	if (err == TIMEOUT) {
		err = mmc_send_op_cond(host);

		if (err) {
			return UNUSABLE_ERR;
		}
	}
	else if(err) {
		return UNUSABLE_ERR;
	}

	return mmc_startup(host);
}

/*
 * CPU and board-specific MMC initializations.  Aliased function
 * signals caller to move on
 */
static int __def_mmc_init(bd_t *bis)
{
	return -1;
}

int cpu_mmc_init(bd_t *bis) __attribute__((weak, alias("__def_mmc_init")));
int board_mmc_init(bd_t *bis) __attribute__((weak, alias("__def_mmc_init")));

void print_mmc_devices(char separator)
{
	struct mmc *m;
	struct list_head *entry;

	list_for_each(entry, &mmc_devices) {
		m = list_entry(entry, struct mmc, link);

		m->init(m);
		
		printf("%s_dev%d", m->name, m->block_dev.dev);

		if (entry->next != &mmc_devices)
			printf("%c ", separator);
	}

	printf("\n");
}

int mmc_initialize(bd_t *bis)
{
	struct mmc *mmc;
	int err,dev;
	
	INIT_LIST_HEAD (&mmc_devices);
	cur_dev_num = 0;

	if (board_mmc_init(bis) < 0)
		cpu_mmc_init(bis);

#if defined(DEBUG_S3C_HSMMC)
	print_mmc_devices(',');
#endif
	for (dev = 0; dev < cur_dev_num; dev++) 
	{
		mmc = find_mmc_device(dev);

		if (mmc) 
		{
			err = mmc_init(mmc);
			if (err && !strcmp(mmc->name, "S5P_MSHC4")) //mj for emergency
			{	
				char run_cmd[40];
				printf("[ERROR]Card init fail,so Change mode and try again..!\n");
				sprintf(run_cmd,"emmc mode 0 0 0"); //change dev 0 to 1<<0 bit mode
				err = run_command(run_cmd, 0); //

				if(1)//(!(1-err))
				{
					printf("[ERROR]go to inital  again..!\n");
					err = mmc_init(mmc);
					
					if(err)
						printf("[ERROR]Card init fail again..So give up mmc initial!\n");
				}
				else
				{
					printf("[ERROR] 1bit mode is also not working..please find other way!!\n");
				}
			}
		}
		else
		{
			//cur_dev_num = dev;	//mj fix me
			break;
		}
		printf("MMC%d:	%ld MB\n",dev,(mmc->capacity/(1024*1024/mmc->read_bl_len)));
	}
	return err;
}
#if defined (CONFIG_EMMC)
int emmc_boot_erase(struct mmc *mmc, uint from, uint to)
{//mj: not used ...
	int err;
	struct mmc_cmd cmd;

	cmd.cmdidx = MMC_CMD_ERASE_GROUP_START;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = from;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	udelay(1000);
	if (err)
		return err;

	cmd.cmdidx = MMC_CMD_ERASE_GROUP_END;
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = to;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	udelay(1000);
	if (err)
		return err;

	cmd.cmdidx = MMC_CMD_ERASE;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = 0;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	udelay(1000);
	if (err)
		return err;

	do
	{
		cmd.cmdidx = MMC_CMD_SEND_STATUS;
		cmd.resp_type = MMC_RSP_R1;
		cmd.cmdarg = mmc->rca << 16;
		cmd.flags = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);
		udelay(1000);
		if (err)
			return err;
	} while (!(cmd.response[0] & (1<<8)) || (cmd.response[0]&(0xf<<9)) == (7<<9) );

	return 0;
}
int mmc_erase(struct mmc *mmc, int part, u32 start, u32 block)
{
	int err, erase_time;
	u32 count, dis, blk_hc;
	struct mmc_cmd cmd;

	printf("START: %d BLOCK: %d\n", start, block);
	printf("high_capacity: %d\n", mmc->high_capacity);
	printf("Capacity: %d\n", mmc->capacity);

	/* ERASE boot partition */
	if (part == 0) {
		/* Boot ack enable, boot partition enable , boot partition access */
		cmd.cmdidx = MMC_CMD_SWITCH;
		cmd.resp_type = MMC_RSP_R1b;
		cmd.cmdarg = ((3<<24)|(179<<16)|(((1<<6)|(1<<3)|(1<<0))<<8));
		cmd.flags = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);
		if (err)
			return err;
	}


	/* Byte addressing */
	if (mmc->high_capacity == 0) {
		start = start * 512;
		block = block * 512;
	} else {
		if (!IS_SD(mmc)) {
			blk_hc = block;
		/* MMC High Capacity erase minimum size is 512KB */
			if (block < 1024) {
				if (block < 512)
					block = 1;
				else
					block = 2;
			} else {
				if (0 == (block%1024)) {
					block = (block / 1024);
				} else {
					block = (block / 1024) + 1;
				}
			}
		}
	}

	/* Set ERASE start group */
	if (IS_SD(mmc)) {
		cmd.cmdidx = 32;
	} else {
		cmd.cmdidx = 35;
	}
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = start;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err)
		return err;

	/* Set ERASE start group */
	if (IS_SD(mmc)) {
		cmd.cmdidx = 33;
	} else {
		cmd.cmdidx = 36;
	}
	cmd.resp_type = MMC_RSP_R1;
	cmd.cmdarg = (start + block);
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err)
		return err;

	/* ERASE */
	cmd.cmdidx = 38;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = 0x0;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	if (err)
		return err;

	/* Byte addressing */
	if (mmc->high_capacity == 0) {
		start = start / 512;
		block = block / 512;
		erase_time = block;
	} else {
		if (!IS_SD(mmc)) {
			erase_time = blk_hc;
		} else {
			erase_time = block;
		}
	}
	if (erase_time < 2048*1024) {
		dis = 1;
	} else {
		dis = 100;
	}
	printf("\nErase\n");
	for (count = 1; count < erase_time ; count++) {

		if (!(count%(100*dis))) {
			printf(".");
		}
		if (!(count%(4000*dis))) {
			printf("\n");
		}
	}

	if (mmc->high_capacity && !IS_SD(mmc)) {
		printf("\n\t\t\t*** NOTICE ***\n");
		printf("*** High Capacity(higher than 2GB) MMC's erase "
		"minimum size is 512KB ***\n");
		if (block < 2) {
			printf("\n %d KB erase Done\n", block*512);
		} else if ((block >= 2)&&(block < 2048)){
			printf("\n %d.%d MB erase Done\n", (block/2),
			(block%2)*5);
		} else {
			printf("\n %d.%d GB erase Done\n",
			(block/2048),
			((block*1000)/2048) -
			((block/2048)*1000));
		}
	} else {
		if (block < 2) {
			printf("\n %d B erase Done\n", block*512);
		} else if ((block >= 2)&&(block < 2048)){
			printf("\n %d KB erase Done\n", (block/2));
		} else if ((block >= 2048)&&(block < (2048 * 1024))){
			printf("\n %d.%d MB erase Done\n",
			(block/2048),
			((block*1000)/2048) -
			((block/2048)*1000));
		} else {
			printf("\n %d.%d GB erase Done\n",
			(block/(2048*1024)),
			((block*10)/(2048*1024)) -
			((block/(2048*1024))*10));
		}
	}
	/* ERASE boot partition */
	if (part == 0) {
		/* Boot ack enable, boot partition enable , boot partition access */
		cmd.cmdidx = MMC_CMD_SWITCH;
		cmd.resp_type = MMC_RSP_R1b;
		//cmd.cmdarg = ((3<<24)|(179<<16)|(((1<<6)|(1<<3)|(0<<0))<<8));
		cmd.cmdarg = ((3<<24)|(179<<16)|(((1<<6)|(0<<3)|(0<<0))<<8));//mj
		cmd.flags = 0;

		err = mmc_send_cmd(mmc, &cmd, NULL);
		if (err)
			return err;
	}
	return 0;
}

int emmc_boot_partition_size_change(struct mmc *mmc, u32 bootsize, u32 rpmbsize)
{
	int err;
	struct mmc_cmd cmd;

	/* Only use this command for raw eMMC moviNAND */
	/* Enter backdoor mode */
	cmd.cmdidx = 62;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = 0xefac62ec;
	cmd.flags = 0;
	
	err = mmc_send_cmd(mmc, &cmd, NULL);
	udelay(1000);
	if (err)
		return err;
	
	/* Boot partition changing mode */
	cmd.cmdidx = 62;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = 0xcbaea7;
	cmd.flags = 0;
	
	err = mmc_send_cmd(mmc, &cmd, NULL);
	udelay(1000);
	if (err)
		return err;

	bootsize = ((bootsize*512)/128);//ly form 2470

	/* Arg: boot partition size */
	cmd.cmdidx = 62;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = bootsize;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	udelay(1000);
	if (err)
		return err;

	rpmbsize = ((rpmbsize*512)/128);//ly form 2470

	/* Arg: RPMB partition size */
	cmd.cmdidx = 62;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = rpmbsize;
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	udelay(1000);
	if (err)
		return err;



	return 0;	
}
int emmc_boot_open(struct mmc *mmc)
{
	int err;
	struct mmc_cmd cmd;

	/* Boot ack enable, boot partition enable , boot partition access */
	cmd.cmdidx = MMC_CMD_SWITCH;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = ((3<<24)|(179<<16)|(((1<<6)|(1<<3)|(1<<0))<<8));
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	udelay(1000);
	if (err)
		return err;

	/* 4bit transfer mode at booting time. */
	cmd.cmdidx = MMC_CMD_SWITCH;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = ((3<<24)|(177<<16)|(((1<<0)|(0<<2))<<8));//ly
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	udelay(1000);
	if (err)
		return err;

	return 0;
}

int emmc_boot_close(struct mmc *mmc)
{
	int err;
	struct mmc_cmd cmd;

	/* Boot ack enable, boot partition enable , boot partition access */
	cmd.cmdidx = MMC_CMD_SWITCH;
	cmd.resp_type = MMC_RSP_R1b;
	cmd.cmdarg = ((3<<24)|(179<<16)|(((1<<6)|(1<<3)|(0<<0))<<8));
	cmd.flags = 0;

	err = mmc_send_cmd(mmc, &cmd, NULL);
	udelay(1000);
	if (err)
		return err;

	return 0;
}


#endif
