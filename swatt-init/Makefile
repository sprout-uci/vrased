CFLAGS  = -O1

all: swatt-init-msp430-gcc.o swatt-init-clang.o

swatt-init-msp430-gcc.o: swatt-init.c
	@echo "--------------------------------------------------------------------------------"
	msp430-gcc --version
	msp430-gcc $(CFLAGS) -c $< -o $@
	msp430-objdump -d $@
	msp430-nm $@
	msp430-nm $@ | grep memset

swatt-init-clang.o: swatt-init.c
	@echo "--------------------------------------------------------------------------------"
	clang -target msp430-elf --version
	clang -target msp430-elf -I/usr/msp430/include $(CFLAGS) -c $< -o $@
	msp430-objdump -d $@
	msp430-nm $@
	! msp430-nm $@ | grep memset

clean:
	rm -f *.o
