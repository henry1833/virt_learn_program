
    .code64

strlen:                    jmp __strlen
putc:                      jmp __putc
println:                   jmp __println
puts:                      jmp __puts
set_interrupt_handler:
set_interrupt_descriptor:  jmp __set_interrupt_handler

video_current: .quad 0xb8000

__strlen:
    mov $-1,%rax
	test %rsi,%rsi
	jz strlen_done
strlen_loop:
    inc %rax
	cmpb $0x0,(%rsi,%rax)
	jnz strlen_loop
strlen_done:
    ret

__memcpy:
    push %rcx
	pop %rcx
	ret

#
#get current row
#
__get_current_row:
    push %rbx
	mov  video_current,%rax
	sub $0xb8000,%rax
	mov $(80*2),
	div
	pop %rbx
	ret

#
#get current column
#
__get_current_row:
    push %rbx
	mov  video_current,%rax
	sub $0xb8000,%rax
	mov $(80*2),
	div
	pop %rbx
	ret


#write a character to video
#
#input:
#     %esi character

__write_char:
    push %rbx
	mov video_current,%rbx
	or $0x0f00,%si
	cmp $0xf0a,%si
	jnz do_write_char
	call __get_current_column
	neg %rax
	add $(80*2),%rax
	add (%rbx),%rax
	jmp do_write_char_done
	
do_write_char:
    mov (%rbx),%rax
    cmp $0xb9ff0,%rax
    ja do_write_char_done
	mov %si,(%rax)
	add $2,%rax
do_write_char_done:
    mov %rax,(%rbx)	
	pop %rbx
    ret

#putc: print a char
#input:
#      %esi a char
#
__putc:
    and $0x00ff,%esi
	call __write_char
	ret


#change a line
#
__println:
    mov $10,%si
	call __putc
	ret

#print blank	
#
#	
__printblank:
    mov ' ',%si
	call __putc
	ret
	
	
#print string message
#
#input:
#     rsi  string message address
__puts:
    push %rbx
	mov %rsi,%rbx
	test %rbx,%rbx
	jz do_puts_done

do_puts_done:
    mov (%rbx),%al
	test %al,%al
	jz do_puts_done
	mov %rax,%rsi
	call __putc
    inc %rbx
    jmp do_puts_loop	
	
do_puts_done:
	pop %rbx
    ret

#set interrupt handler
#
#input:
#     %rsi: vector %rdi: handler
__set_interrupt_handler:
    sidt [idt_pointer]
	mov  (idt_pointer+2),%rax
	shl  $4,%rsi
	mov  %rdi,(%rax,%rsi)
	mov  %rdi,4(%rax,%rsi)
    movw $kernel_code64_sel,2(%rax,%rsi)
	movb $(0x80|INTERRUPT_GATE64),5(%rax,%rsi)
	ret

#set user interrupt handler
#
#input:
#     %rsi: vector %rdi: handler
__set_interrupt_handler:
    sidt [idt_pointer]
	mov  (idt_pointer+2),%rax
	shl  $4,%rsi
	mov  %rdi,(%rax,%rsi)
	mov  %rdi,4(%rax,%rsi)
    movw $kernel_code64_sel,2(%rax,%rsi)
	movb $(0xE0|INTERRUPT_GATE64),5(%rax,%rsi)
	ret


    .align 8
gdt_pointer:
    .word 0
	.quad 0

    .align 8
idt_pointer:
    .word 0
	.quad 0	
	