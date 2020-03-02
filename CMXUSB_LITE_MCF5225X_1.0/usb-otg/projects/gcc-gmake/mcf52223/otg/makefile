.SUFFIXES:
.SUFFIXES: .c .o .s

AS = m68k-elf-as
ASFLAGS = -mcpu=5211

CC = m68k-elf-gcc
CFLAGS = -DON_THE_GO -DHID_DEMO -DBIG_ENDIAN=1 -I${PROJ_DIR} -I${TARGET_DIR} -I${TARGET_DIR1} -I${TARGET_DIR2} -I${COMMON_DIR} -I${TGTCOMMON_DIR} -O0 -g3 -Wall -mcpu=5211 -mdiv

LD = m68k-elf-ld
LDFLAGS = -nostartfiles -nodefaultlibs -nostdlib -T ${PROJ_DIR}/m5222xevb-rom.ld -M

RM = rm -f

INSTALL_DIR=../../../../..
#/root/workspace
PROJ_DIR=${INSTALL_DIR}/usb-otg/projects/gcc-gmake/mcf52223/${DEMO}
TARGET_DIR=${INSTALL_DIR}/usb-otg/src/${TARGET}
TARGET_DIR1=${INSTALL_DIR}/usb-peripheral/src/${TARGET}
TARGET_DIR2=${INSTALL_DIR}/usb-host/src/${TARGET}
COMMON_DIR=${INSTALL_DIR}/usb-common
TGTCOMMON_DIR=${INSTALL_DIR}/usb-common/${TARGET}
DEMO=otg
TARGET=mcf5222x

CSOURCES= ${TARGET_DIR1}/hid-demo/hid_generic.c\
  ${TARGET_DIR1}/hid-demo/hid_usb_config.c\
  ${TARGET_DIR1}/hid-demo/hid.c\
  ${TARGET_DIR1}/hid-demo/hid_kbd.c\
  ${TARGET_DIR1}/hid-demo/hid_mouse.c\
  ${TARGET_DIR1}/device_ints.c\
  $(wildcard ${TARGET_DIR1}/usb-drv/*.[c])\
  ${TARGET_DIR2}/hid/hid_parser.c\
  ${TARGET_DIR2}/hid/host_hid.c\
  ${TARGET_DIR2}/hid/host_hid_kbd.c\
  ${TARGET_DIR2}/hid/host_hid_joy.c\
  ${TARGET_DIR2}/hid/hid_demo.c\
  ${TARGET_DIR2}/hid/host_hid_mouse.c\
  $(wildcard ${TARGET_DIR2}/usb-drv/*.[c])\
  $(wildcard ${TARGET_DIR}/*.[c])\
  $(wildcard ${TARGET_DIR}/i2c-drv/*.[c])\
  $(wildcard ${TARGET_DIR}/otg-drv/*.[c])\
  $(wildcard ${TGTCOMMON_DIR}/timer-drv/*.[c])\
  $(wildcard ${TGTCOMMON_DIR}/uart-drv/*.[c])\
  $(wildcard ${TGTCOMMON_DIR}/*.[c])\
  $(wildcard ${COMMON_DIR}/terminal/*.[c])\
  $(wildcard ${COMMON_DIR}/utils/*.[c])\
  $(wildcard ${COMMON_DIR}/*.[c])\
  $(wildcard ${PROJ_DIR}/*.[c])

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
