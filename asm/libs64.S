
    .code64

strlen:                    jmp __strlen
putc:                      jmp __putc
println:                   jmp __println
puts:                      jmp __puts
print_byte_value:          jmp __print_byte_value
print_word_value:          jmp __print_word_value
print_dword_value:         jmp __print_dword_value 
print_qword_value:         jmp __print_qword_value
get_byte_hex_string:       jmp __get_byte_hex_string
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
	push %rdx
	mov $0,%rdx
	mov  video_current,%rax
	sub $0xb8000,%rax
	mov $(80*2),%rbx
	div %rbx
	pop %rdx
	pop %rbx
	ret

#
#get current column
#
__get_current_column:
    push %rbx
	push %rdx
	mov $0,%rdx
	mov  video_current,%rax
	sub $0xb8000,%rax
	mov $(80*2),%rbx
	div %rbx
	mov %rdx,%rax
	pop %rdx
	pop %rbx
	ret


#write a character to video
#
#input:
#     %rsi character

__write_char:
    push %rbx
	mov $video_current,%rbx
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
#      %rsi a char
#
__putc:
    and $0x00ff,%rsi
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
	push %rax
	mov %rsi,%rbx
	test %rbx,%rbx
	jz do_puts_done

do_puts_loop:
    mov (%rbx),%al
	test %al,%al
	jz do_puts_done
	mov %rax,%rsi
	call __putc
    inc %rbx
    jmp do_puts_loop	
	
do_puts_done:
    pop %rax
	pop %rbx
    ret

#--------------------------------------------
#hex_to_char: 
#input: 
#      %rsi: Hex number
#output:
#      %rax: char 
#--------------------------------------------
number_character:          .asciz "0123456789ABCDEF"
__hex_to_char:
    push %rsi
	push %rbx
    and $0x0f,%rsi
	mov $number_character,%rbx
	movzbq (%rbx,%rsi),%rax
    pop  %rbx	
    pop  %rsi
    ret

#   
#%rsi:value %rdx: hex count
#
__print_hex_by_byte:
   push %rbx
   push %rsi
   mov %rsi,%rbx
hex_loop:
   dec %rdx
   mov %rbx,%rsi
   mov %dl,%cl
   shl $2,%cl
   shr %cl,%rsi
   call __hex_to_char
   mov %rax,%rsi   
   call __putc
   test %rdx,%rdx
   jnz hex_loop
   pop %rsi
   pop %rbx
   ret

#------------
#print one byte
#input: %rsi
#------------
__print_byte_value:
   push %rdx
   mov $2,%rdx
   call __print_hex_by_byte
   pop %rdx
   ret

#------------
#print two byte
#input: %rsi
#------------

__print_word_value:
   push %rdx
   mov $4,%rdx
   call __print_hex_by_byte
   pop %rdx
   ret


#------------
#print four byte
#input: %rsi
#------------
__print_dword_value:
   push %rdx
   mov $8,%rdx
   call __print_hex_by_byte
   pop %rdx
   ret

#------------
#print eight byte
#input: %rsi
#------------
__print_qword_value:
    push %rdx
    mov $16,%rdx
    call __print_hex_by_byte
    pop %rdx
    ret

#
#__get_byte_hex_string: convert byte to sting
#     %rsi: input BYTE value, %rdi: buffer 
__get_byte_hex_string:
    push %rcx
	push %rsi
	mov %rsi, %rcx
	shr $4, %rsi
	call __hex_to_char
	movb %al,(%rdi)
	inc %rdi
	mov %rcx, %rsi
	call __hex_to_char
	mov %al, (%rdi)
	inc %rdi
	movb $0,(%rdi)
	pop %rsi
	pop %rcx
	ret

#set interrupt handler
#
#input:
#     %rsi: vector %rdi: handler
__set_interrupt_handler:
    sidt (idt_pointer)
	mov  (idt_pointer+2),%rax
	shl  $4,%rsi
	mov  %rdi,(%rax,%rsi)
	mov  %rdi,4(%rax,%rsi)
    movw $kernel_code64_sel,2(%rax,%rsi)
	movw $0,4(%rax,%rsi)
	movb $(0x80|INTERRUPT_GATE64),5(%rax,%rsi)
	ret

#set user interrupt handler
#
#input:
#     %rsi: vector %rdi: handler
__set_user_interrupt_handler:
    sidt idt_pointer
	mov  (idt_pointer+2),%rax
	shl  $4,%rsi
	mov  %rdi,(%rax,%rsi)
	mov  %rdi,4(%rax,%rsi)
    movw $kernel_code64_sel,2(%rax,%rsi)
	movw $0,4(%rax,%rsi)
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
	