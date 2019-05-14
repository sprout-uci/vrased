# makfile configuration
NAME            = vrased
OBJECTS         = vrased.o rom_wrapper.o hacl_mac.lib

# Choose GCC toolchain prefix ('msp430' for MSPGCC / 'msp430-elf' for GCC RedHat/TI)
MSPGCC_PFX      = msp430

# Select flags depending on selected toolchain
CFLAGS          = -D PFX_MSP430  -std=c99   -O1 -Wall -g -mcpu=430    -mmpy=none    -mivcnt=16

# Toolchain executables
CC              = ${MSPGCC_PFX}-gcc
OBJCOPY		= ${MSPGCC_PFX}-objcopy
OBJDUMP         = ${MSPGCC_PFX}-objdump
OBJSIZE         = ${MSPGCC_PFX}-size

.PHONY: all FORCE clean download download-jtag download-bsl dist

#all should be the first target. it's built when make is runwithout args
#all: srom_libc.a ${NAME}.o ${NAME}.elf ${NAME}.a43 ${NAME}.lst ${NAME}.size
all: srom_libc.a hacl_mac.lib rom_wrapper.o ${NAME}.o ${NAME}.elf ${NAME}.a43 ${NAME}.lst ${NAME}.size

#additional rules for files
${NAME}.elf: ${OBJECTS}
	${CC} ${CFLAGS} -T pmem.def -o $@ ${OBJECTS} ./srom_libc.a #./hacl.lib

${NAME}.a43: ${NAME}.elf
	${OBJCOPY} -O ihex $^ $@

${NAME}.lst: ${NAME}.elf
	${OBJDUMP} -dSt $^ >$@

${NAME}.size: ${NAME}.elf
	${OBJSIZE}  $^ >$@

${NAME}.o: main.c
	${CC} ${CFLAGS} -c -o $@ main.c

rom_wrapper.o:
	${CC} ${CFLAGS} -c -o $@ wrapper.c #./hacl_mac.lib

hacl_mac.lib: 
	cd hacl-c ; \
	${CC} ${CFLAGS} -c -o hacl.lib.o Hacl_HMAC_SHA2_256.c ; \
	msp430-objcopy --rename-section .text=.do_mac.lib hacl.lib.o ; \
	ar crs ../$@ hacl.lib.o ; \
	rm hacl.lib.o 

srom_libc.a:
	mkdir -p tmp_srom_lib 
	cd tmp_srom_lib; \
        ar x /usr/msp430/lib/libc.a ; \
        for i in mem*.o ; do  msp430-objcopy --rename-section .text=.srom.libc $$i ; done ; \
        ar crs  ../srom_libc.a *.o;
	rm -rf tmp_srom_lib

clean:
	rm -f ${NAME} ${NAME}.a43 ${NAME}.lst ${NAME}.elf ${NAME}.size *.o srom_libc.a hacl_mac.lib rom_wrapper.o pmem.def

#project dependencies

