.SUFFIXES : .c .o

CC		= gcc
ASM		=
LINK		=
LIBCC		= ar
RM		= rm

#----------------------------------------------------------------------------------------------
INCPATH		=
LIBPATH		= 
OBJPATH		= .
OUTPATH		= .
SOURCEPATH	= .

LDFLAGS		= -L$(LIBPATH) 
#LFLAGS		= -lm
ASFLAGS		=
ARFLAGS		= -ruv
CFLAGS		= -o

LIBS =

#---------------------------------------------------------------------------------------
SOURCES = $(OBJECTS:.o=.c)

all:
	$(CC) $(CFLAGS)	mk_bl1ah  mk_bl1ah.c

#---------------------------------------------------------------------------------------
.c.o:
		$(CC) $(CFLAGS) -c $< -o $@

dep:
		gccmakedep	$(SOURCES)

#---------------------------------------------------------------------------------------
clean:
		$(RM)		-rf mk_bl1ah
		$(RM)		-rf bl1ah

new:
		$(MAKE)		clean
		$(MAKE)

