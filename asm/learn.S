.include "system_data.h"

BASE            =   0x7c00

SECTOR_SIZE     =   512



.section .text.real_mode
		.global _start
		.code16        
_start:
	mov %cs,%ax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%ss
	mov BASE,%sp
    sti
	
    call clear_screen
	call get_driver_parameters
	movl $70,total_sectors
    call load_module
	


	lgdt gdt_pointer32 		# load GDT
		
	lidt idt_pointer32      # load IDT
	
	mov %cr0,%eax
	mov $0, %eax
	mov %eax,%cr0
	
	jmp $code32_sel,$entry32

.include "pre_run.S"

.fill 510-(.-_start),1,0
boot_magic: .byte 0x55,0xaa


.include "table_desc.S"
		
		.align 4
gdt_pointer32:
       .word      (__global_descriptor_table_end - __global_descriptor_table) - 1
       .long      __global_descriptor_table
idt_pointer32:
       .word      (__interrupt_descriptor_table_end - __interrupt_descriptor_table) - 1
       .long       __interrupt_descriptor_table


       .align 4
gdt_pointer64:
       .word      (__global_descriptor_table_end - __global_descriptor_table) - 1
       .quad      __global_descriptor_table

idt_pointer64:
       .word      (__interrupt_descriptor_table_end - __interrupt_descriptor_table) - 1
       .quad       __interrupt_descriptor_table


.section .text.protected_mode
		.code32
entry32:
    mov %cr4,%eax
	bts $5, %eax
	mov %eax,%cr4
    mov $0x1000,%esp

#set page PAE
#    call pae_enable

#open XD function
#    call execution_disable_enable

#init paging environment
#    call int_pae_paging

#open paging
#    mov %cr0,%eax
#    bts $31, %eax
#    mov %eax,%cr0	

	jmp $kernel_code64_sel,$entry64

#define 32-bit gdt entry

.section .text.long_mode

		.code64
entry64:		
        mov %rax,%r9
		mov $0xfffff,%rdi


#.section .long_mode
#		.text
#		.code64
#        .org LONG_BASE	
#entry64:
		
#    mov GP_HANDLER_VECTOR, %rsi
#	mov GP_handler,%rdi
#	call set_interrupt_handler
	
#	mov PF_HANDLER_VECTOR, %rsi
#	mov PF_handler,%rdi
#	call set_interrupt_handler
	
#	mov DB_HANDLER_VECTOR, %rsi
#	mov DB_handler,%rdi
#	call set_interrupt_handler
	
