#include <common.h>
#include <s5pc210.h>




#define	I2C_WRITE	0
#define I2C_READ	1

#define I2C_OK		0
#define I2C_NOK		1
#define I2C_NACK	2
#define I2C_NOK_LA	3		/* Lost arbitration */
#define I2C_NOK_TOUT	4		/* time out */

#define I2CSTAT_BSY	0x20		/* Busy bit */
#define I2CSTAT_NACK	0x01		/* Nack bit */
#define I2CCON_IRPND	0x10		/* Interrupt pending bit */
#define I2C_MODE_MT	0xC0		/* Master Transmit Mode */
#define I2C_MODE_MR	0x80		/* Master Receive Mode */
#define I2C_START_STOP	0x20		/* START / STOP */
#define I2C_TXRX_ENA	0x10		/* I2C Tx/Rx enable */

#define I2C_TIMEOUT 1			/* 1 second */

#define MAX8997_I2C_ADDR	(0xcc>>1)





//I2C 1
#define rIICCON		*((volatile unsigned long*) 0x13870000)
#define rIICSTAT 	*((volatile unsigned long*) 0x13870004)
#define rIICADD		*((volatile unsigned long*) 0x13870008)
#define rIICDS		*((volatile unsigned long*) 0x1387000C)
#define rIICLC		*((volatile unsigned long*) 0x13870010)



#define mdelay(n) ({unsigned long msec=(n); \
			while (msec--) udelay(1000);})



static
int WaitForXfer (void)
{
	int i;
	unsigned long status;

	i = I2C_TIMEOUT * 10000;
	status = rIICCON;
	while ((i > 0) && !(status & I2CCON_IRPND)) {
		udelay (100);
		status = rIICCON;
		i--;
	}

	return (status & I2CCON_IRPND) ? I2C_OK : I2C_NOK_TOUT;
}

static
int IsACK (void)
{
	return (!(rIICSTAT & I2CSTAT_NACK));
}

static
void ReadWriteByte (void)
{
	rIICCON &= ~I2CCON_IRPND;
}

static
void i2c_init (int speed, int slaveadd)
{
	int i, status;

	/* wait for some time to give previous transfer a chance to finish */

	i = I2C_TIMEOUT * 1000;
	status = rIICSTAT;
	while ((i > 0) && (status & I2CSTAT_BSY)) {
		udelay (1000);
		status = rIICSTAT;
		i--;
	}


	rIICCON = (1<<6) | (1<<5) | (7&0xf);


	rIICSTAT = 0;
	rIICADD = slaveadd;

	rIICSTAT = I2C_MODE_MT | I2C_TXRX_ENA;

}

/*
 * cmd_type is 0 for write, 1 for read.
 *
 * addr_len can take any value from 0-255, it is only limited
 * by the char, we could make it larger if needed. If it is
 * 0 we skip the address write cycle.
 */
static
int i2c_transfer (unsigned char cmd_type,
		  unsigned char chip,
		  unsigned char addr[],
		  unsigned char addr_len,
		  unsigned char data[], unsigned short data_len)
{
	int i, status, result;

	if (data == 0 || data_len == 0) {

		return I2C_NOK;
	}

	i = I2C_TIMEOUT * 1000;
	status = rIICSTAT;
	while ((i > 0) && (status & I2CSTAT_BSY)) {
		udelay (1000);
		status = rIICSTAT;
		i--;
	}

	if (status & I2CSTAT_BSY)
		return I2C_NOK_TOUT;

	rIICCON |= 0x80;
	result = I2C_OK;

	switch (cmd_type) {
	case I2C_WRITE:
		if (addr && addr_len) {
			rIICDS = chip;

			rIICSTAT = I2C_MODE_MT | I2C_TXRX_ENA | I2C_START_STOP;
			i = 0;
			while ((i < addr_len) && (result == I2C_OK)) {
				result = WaitForXfer ();
				rIICDS = addr[i];
				ReadWriteByte ();
				i++;
			}
			i = 0;
			while ((i < data_len) && (result == I2C_OK)) {
				result = WaitForXfer ();
				rIICDS = data[i];
				ReadWriteByte ();
				i++;
			}

			
		} else {
			rIICDS = chip;
			rIICSTAT = I2C_MODE_MT | I2C_TXRX_ENA | I2C_START_STOP;
			i = 0;
			while ((i < data_len) && (result == I2C_OK)) {
				result = WaitForXfer ();
				rIICDS = data[i];
				ReadWriteByte ();
				i++;
			}
		}

		if (result == I2C_OK)
			result = WaitForXfer ();

		rIICSTAT = I2C_MODE_MR | I2C_TXRX_ENA;
		ReadWriteByte ();
		break;
	case I2C_READ:
		
		
		rIICSTAT = I2C_MODE_MT | I2C_TXRX_ENA ;
		rIICDS = chip;
		rIICSTAT |= I2C_START_STOP;
		i = 0;
		result = WaitForXfer ();
		if (IsACK ()) {			

				i = 0;
				while ((i < addr_len) && (result == I2C_OK)) {
					rIICDS = addr[i];
					ReadWriteByte ();
					result = WaitForXfer ();
					i++;
				}

				rIICDS = chip;
				rIICSTAT =  I2C_MODE_MR | I2C_TXRX_ENA |
						I2C_START_STOP;
				
				ReadWriteByte ();

				result = WaitForXfer ();
				i = 0;

				while ((i < data_len) && (result == I2C_OK)) {
					if (i == data_len - 1)
						rIICCON &= ~0x80;
					ReadWriteByte ();
					result = WaitForXfer ();
					data[i] = rIICDS;
					i++;
				}
				
			} else {
				result = I2C_NACK;
		}
			
			rIICSTAT = I2C_MODE_MR | I2C_TXRX_ENA;
			ReadWriteByte ();

			
			
		break;
	default:
		result = I2C_NOK;
		break;
	}

	return (result);
}

static
int i2c_read (uchar chip, uint addr, int alen, uchar * buffer, int len)
{
	uchar xaddr[4];
	int ret;

	if (alen > 4) {
		return 1;
	}

	if (alen > 0) {
		xaddr[0] = (addr >> 24) & 0xFF;
		xaddr[1] = (addr >> 16) & 0xFF;
		xaddr[2] = (addr >> 8) & 0xFF;
		xaddr[3] = addr & 0xFF;
	}

	if ((ret =
	     i2c_transfer (I2C_READ, chip << 1, &xaddr[4 - alen], alen,
			   buffer, len)) != 0) {
		return 1;
	}
	return 0;
}

static
int i2c_write (uchar chip, uint addr, int alen, uchar * buffer, int len)
{
	uchar xaddr[4];

	if (alen > 4) {
		return 1;
	}

	if (alen > 0) {
		xaddr[0] = (addr >> 24) & 0xFF;
		xaddr[1] = (addr >> 16) & 0xFF;
		xaddr[2] = (addr >> 8) & 0xFF;
		xaddr[3] = addr & 0xFF;
	}
	return (i2c_transfer
		(I2C_WRITE, chip << 1, &xaddr[4 - alen], alen, buffer,
		 len) != 0);
}

static int g_flag = 0;

void max8997_init()
{
	volatile uint* gpio_reg1 = (uint*)0x114000C0;
     uint value;

	 value = *gpio_reg1;
	 value &= ~(0xff00);
	 value |= 0x2200;
	 
	*gpio_reg1 = value;

	i2c_init(0, MAX8997_I2C_ADDR);
	
}
void lowlevel_init_max8997(unsigned char Address,unsigned char *Val,int flag)
{
	unsigned char addr;
	unsigned char data[4] = {0,0,0,0};
	unsigned char write_data = 0;
	int ret;
	
	

	addr = Address;
	max8997_init();
	data[0] = *Val;
	write_data = *Val;
	if(flag == 0)
	{
		ret = i2c_read(MAX8997_I2C_ADDR, addr, 1, data, 1);
		*Val = data[0];
		//printf("max8997: addr 0x%x, value 0x%x\n",addr,data[0]);
	}
	else 
	{
		ret = i2c_write(MAX8997_I2C_ADDR, addr, 1, data, 1);
		data[0] = 0;
		ret = i2c_read(MAX8997_I2C_ADDR, addr, 1, data, 1);
		*Val = data[0];
		//printf("max8997: addr 0x%x,write value 0x%x,read value 0x%x\n",addr,write_data,data[0]);
	}
	
}


int pmic_read(uint addr, uchar * buffer, int len)
{
	return i2c_read(MAX8997_I2C_ADDR, addr, 1, buffer, len);
}

int pmic_write(uint addr, uchar * buffer, int len)
{
	return i2c_write(MAX8997_I2C_ADDR, addr, 1, buffer, len);
}

// end of file

