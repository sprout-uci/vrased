#if __ATTACK == 7

.macro fill_regs mem:req
    mov r1,  \mem+0
    mov r2,  \mem+2
    mov r3,  \mem+4
    mov r4,  \mem+6
    mov r5,  \mem+8
    mov r6,  \mem+10
    mov r7,  \mem+12
    mov r8,  \mem+14
    mov r9,  \mem+16
    mov r10, \mem+18
    mov r11, \mem+20
    mov r12, \mem+22
    mov r13, \mem+24
    mov r14, \mem+26
    mov r15, \mem+28
.endm

.section .text
.global dump_regs_before
dump_regs_before:
    fill_regs &regs_before
    br #cont_dump_regs_before

.global dump_regs_after
dump_regs_after:
    fill_regs &regs_after
    br #cont_dump_regs_after

#endif
