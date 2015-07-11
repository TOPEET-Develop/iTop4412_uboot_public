#
# (C) Copyright 2000-2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# (C) Copyright 2008
# Guennadi Liakhovetki, DENX Software Engineering, <lg@denx.de>
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	= $(obj)lib$(SOC).a

#SOBJS	= cache.o
SOBJS	= cpu_init.o
#SOBJS	+= reset.o

COBJS	+= nand.o
COBJS	+= nand_cp.o
COBJS	+= serial.o usb_ohci.o usbd-otg-hs.o
COBJS	+= interrupts.o
COBJS	+= cpu_info.o
COBJS	+= onenand.o
COBJS	+= onenand_cp.o
COBJS	+= fastboot.o
COBJS	+= pmic.o
COBJS	+= gpio.o
COBJS	+= i2c.o
COBJS	+= cmu.o
COBJS	+= speed.o
COBJS	+= max8997.o
ifdef CONFIG_CMD_MOVINAND
COBJS	+= setup_hsmmc.o
COBJS	+= movi.o
endif
ifdef CONFIG_SECURE
COBJS	+= UBOOT_SB20_S5PC210S.o
COBJS	+= cmd_movi.o
endif
#ifdef CONFIG_EMMC
#COBJS	+= cmd_movi.o
#endif
ifdef CONFIG_RECOVERY
COBJS	+= recovery.o
endif

ifdef CONFIG_LOGO_DISPLAY
COBJS	+= fimd.o
endif

SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS) $(SOBJS))

all:	 $(obj).depend $(LIB)

$(LIB):	$(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
