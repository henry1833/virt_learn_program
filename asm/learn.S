.include "system_data.h"
.include "page.h"
.include "CPU.h"

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
	mov $BASE,%sp
    sti
	
    call clear_screen
	call get_driver_parameters
	movl $70,total_sectors
    call load_module
	


	lgdt gdt_pointer32 		# load GDT
		
	lidt idt_pointer32      # load IDT
	
	mov %cr0,%eax
	bts $0, %eax
	mov %eax,%cr0
	
	jmp $code32_sel,$entry32

.include "pre_run.S"

.fill 510-(.-_start),1,0
boot_magic: .byte 0x55,0xaa

.fill 16,1,0

.include "table_desc.S"
		
		.align 8
gdt_pointer32:
       .word      (__global_descriptor_table_end - __global_descriptor_table) - 1
       .long      __global_descriptor_table
idt_pointer32:
       .word      (__interrupt_descriptor_table_end - __interrupt_descriptor_table) - 1
       .long       __interrupt_descriptor_table


       .align 8
gdt_pointer64:
       .word      (__global_descriptor_table_end - __global_descriptor_table) - 1
       .quad      __global_descriptor_table

idt_pointer64:
       .word      (__interrupt_descriptor_table_end - __interrupt_descriptor_table) - 1
       .quad       __interrupt_descriptor_table


.section .text.protected_mode
		.code32
entry32:
##set protected mode run environment
##

##open sse instruction set
    mov %cr4, %eax
	bts $0x9, %eax
	mov %eax, %cr4
	
#init paging environment
    call int_paging

#set page PAE
    call pae_enable

#open XD function
    call execution_disable_enable

    mov $PML4T_BASE,%eax
	mov %eax,%cr3
	


#set EFER register EFER.LME = 1
    mov IA32_EFER,%ecx
	rdmsr
	bts $8,%eax
	wrmsr

#open long mode 
    mov %cr0,%eax
    bts $31, %eax
    mov %eax,%cr0	

	jmp $kernel_code64_sel,$entry64

#define 32-bit gdt entry

    .align 8
pae_enable:
    mov $0x1,%eax
	cpuid
	bt  $0x6,%edx
	jnc pae_enable_done
	mov %cr4,%eax
	bts $PAE_BIT, %eax
	mov %eax, %cr4
pae_enable_done:
    ret

.section .text.long_mode		
		.code64
entry64:
	mov $KERNEL_SS,%ax
    mov %ax,%ds
    mov %ax,%es
    mov %ax,%ss
    mov %rsp, $PROCESSOR0_KERNEL_RSP	
#    mov GP_HANDLER_VECTOR, %rsi
#	mov GP_handler,%rdi
#	call set_interrupt_handler
	
#	mov PF_HANDLER_VECTOR, %rsi
#	mov PF_handler,%rdi
#	call set_interrupt_handler
	
#	mov DB_HANDLER_VECTOR, %rsi
#	mov DB_handler,%rdi
#	call set_interrupt_handler
    push USER_SS | 0x3
	mov  USER_RSP, %rax
	push %rax
	push USER_CS | 0x3
	push $user_entry
	retf64

user_entry:
    
    jmp $