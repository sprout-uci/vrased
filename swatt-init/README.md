Minimal C compilation module, based on the relevant fragment of the SW-Att
source code, to show that the redundant key zero-initialization (while not
optimized away with the default, older `msp430-gcc` compiler),
may be optimized out by more recent MSP430 compilers.

Binaries were generated using the compiler versions provided in the sample
output below.

```
rm -f *.o
--------------------------------------------------------------------------------
msp430-gcc --version
msp430-gcc (GCC) 4.6.3 20120301 (mspgcc LTS 20120406 unpatched)
Copyright (C) 2011 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

msp430-gcc -O1 -c swatt-init.c -o swatt-init-msp430-gcc.o
msp430-objdump -d swatt-init-msp430-gcc.o

swatt-init-msp430-gcc.o:     file format elf32-msp430


Disassembly of section .text:

00000000 <Hacl_HMAC_SHA2_256_hmac_entry>:
   0:	31 50 c0 ff 	add	#-64,	r1	;#0xffc0
   4:	3d 40 40 00 	mov	#64,	r13	;#0x0040
   8:	0e 43       	clr	r14		
   a:	0f 41       	mov	r1,	r15	
   c:	b0 12 00 00 	call	#0x0000	
  10:	3d 40 40 00 	mov	#64,	r13	;#0x0040
  14:	3e 40 00 6a 	mov	#27136,	r14	;#0x6a00
  18:	0f 41       	mov	r1,	r15	
  1a:	b0 12 00 00 	call	#0x0000	
  1e:	03 12       	push	#0		;r3 As==00
  20:	30 12 20 00 	push	#32		;#0x0020
  24:	30 12 30 02 	push	#560		;#0x0230
  28:	3c 40 40 00 	mov	#64,	r12	;#0x0040
  2c:	0d 43       	clr	r13		
  2e:	0e 41       	mov	r1,	r14	
  30:	3e 50 06 00 	add	#6,	r14	;#0x0006
  34:	0f 4e       	mov	r14,	r15	
  36:	b0 12 00 00 	call	#0x0000	
  3a:	31 50 46 00 	add	#70,	r1	;#0x0046
  3e:	30 41       	ret			
msp430-nm swatt-init-msp430-gcc.o
00000000 T Hacl_HMAC_SHA2_256_hmac_entry
         U hmac
         U memcpy
         U memset
msp430-nm swatt-init-msp430-gcc.o | grep memset
         U memset
--------------------------------------------------------------------------------
clang -target msp430-elf --version
clang version 4.0.1 
Target: msp430---elf
Thread model: posix
InstalledDir: /usr/local/bin
clang -target msp430-elf -I/usr/msp430/include -O1 -c swatt-init.c -o swatt-init-clang.o
msp430-objdump -d swatt-init-clang.o

swatt-init-clang.o:     file format elf32-msp430


Disassembly of section .text:

00000000 <Hacl_HMAC_SHA2_256_hmac_entry>:
   0:	04 12       	push	r4		
   2:	04 41       	mov	r1,	r4	
   4:	0b 12       	push	r11		
   6:	31 80 46 00 	sub	#70,	r1	;#0x0046
   a:	0b 44       	mov	r4,	r11	
   c:	3b 80 42 00 	sub	#66,	r11	;#0x0042
  10:	0f 4b       	mov	r11,	r15	
  12:	3e 40 00 6a 	mov	#27136,	r14	;#0x6a00
  16:	3d 40 40 00 	mov	#64,	r13	;#0x0040
  1a:	b0 12 00 00 	call	#0x0000	
  1e:	81 43 04 00 	mov	#0,	4(r1)	;r3 As==00, 0x0004(r1)
  22:	b1 40 20 00 	mov	#32,	2(r1)	;#0x0020, 0x0002(r1)
  26:	02 00 
  28:	b1 40 30 02 	mov	#560,	0(r1)	;#0x0230, 0x0000(r1)
  2c:	00 00 
  2e:	0f 4b       	mov	r11,	r15	
  30:	0e 4b       	mov	r11,	r14	
  32:	3c 40 40 00 	mov	#64,	r12	;#0x0040
  36:	0d 43       	clr	r13		
  38:	b0 12 00 00 	call	#0x0000	
  3c:	31 50 46 00 	add	#70,	r1	;#0x0046
  40:	3b 41       	pop	r11		
  42:	34 41       	pop	r4		
  44:	30 41       	ret			
	...
msp430-nm swatt-init-clang.o
00000000 T Hacl_HMAC_SHA2_256_hmac_entry
         U hmac
         U memcpy
! msp430-nm swatt-init-clang.o | grep memset
```
