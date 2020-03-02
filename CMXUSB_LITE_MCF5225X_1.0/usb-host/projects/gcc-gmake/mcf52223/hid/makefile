.SUFFIXES:
.SUFFIXES: .c .o .s

AS = m68k-elf-as
ASFLAGS = -mcpu=5211

CC = m68k-elf-gcc
CFLAGS = -DHID_DEMO -DBIG_ENDIAN=1 -I${PROJ_DIR} -I${TARGET_DIR} -I${TARGET_DIR}/${DEMO} -I${COMMON_DIR} -I${TGTCOMMON_DIR} -O0 -g3 -Wall -mcpu=5211 -mdiv

LD = m68k-elf-ld
LDFLAGS = -nostartfiles -nodefaultlibs -nostdlib -T ${PROJ_DIR}/m5222xevb-rom.ld -M

RM = rm -f

INSTALL_DIR=../../../../..
#/root/workspace

PROJ_DIR=${INSTALL_DIR}/usb-host/projects/gcc-gmake/mcf52223/${DEMO}
TARGET_DIR=${INSTALL_DIR}/usb-host/src/${TARGET}
COMMON_DIR=${INSTALL_DIR}/usb-common
TGTCOMMON_DIR=${INSTALL_DIR}/usb-common/${TARGET}
DEMO=hid
TARGET=mcf5222x

CSOURCES = $(wildcard ${PROJ_DIR}/*.[c])\
          $(wildcard ${TARGET_DIR}/*.[c])\
          $(wildcard ${TARGET_DIR}/${DEMO}/*.[c])\
          $(wildcard ${TARGET_DIR}/usb-drv/*.[c])\
          $(wildcard ${TGTCOMMON_DIR}/uart-drv/*.[c])\
          $(wildcard ${TGTCOMMON_DIR}/timer-drv/*.[c])\
          $(wildcard ${COMMON_DIR}/utils/*.[c])\
          $(wildcard ${TGTCOMMON_DIR}/*.[c])

ASOURCES = $(wildcard ${PROJ_DIR}/*.[s])

OBJS = $(CSOURCES:.c=.o) $(ASOURCES:.s=.o)

all: ${DEMO}

#.o.c:
#	@echo Compiling ${OBJS}
#	$(CC) -c $(CFLAGS) -o $*.o $@

%.o : %s
	$(AS) $(ASFLAGS) -o $@ $*.s

%.o : %c
	$(CC) -c $(CFLAGS) -o $@ $*.c
   
${DEMO}: ${OBJS}
	${LD} ${LDFLAGS} ${OBJS} -o $@.elf > $@.map

clean:
	${RM} ${OBJS}
