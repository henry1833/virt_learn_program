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
	bts $0, %eax            # set cr0.PE, open protected mode 
	mov %eax,%cr0
	
	jmp $code32_sel,$entry32

.include "pre_run.S"

.fill 510-(.-_start),1,0
boot_magic: .byte 0x55,0xaa

#.fill 16,1,0


		
.section .text.protected_mode
		.code32
entry32:
##set protected mode run environment
##
	mov $data32_sel, %ax
	mov %ax,%ds
	mov %ax,%ss
	mov $entry32,%esp
	
	lgdt gdt_pointer64
	lidt idt_pointer64
##open SSE instruction set
    mov %cr4, %eax
	bts $0x9, %eax
	mov %eax, %cr4

#set page PAE
    call pae_enable	
	
#init paging environment
    call init_paging
	
    mov $PML4T_BASE,%eax
	mov %eax,%cr3	

#set EFER register EFER.LME = 1
    mov $IA32_EFER,%ecx
	rdmsr
	bts $8,%eax
	wrmsr

#open long mode 
    mov %cr0,%eax
    bts $31, %eax
    mov %eax,%cr0	

	jmp $kernel_code64_sel,$entry64 

#define 32-bit gdt entry
    .align 16
.include "table_desc.S"
    .align 16
.include "page64.S"

.section .text.long_mode		
		.code64
entry64:
	mov $KERNEL_SS,%ax
    mov %ax,%ds
    mov %ax,%es
    mov %ax,%ss
    mov $PROCESSOR0_KERNEL_RSP ,%rsp


#set Gdt and Idt
    

    mov GP_HANDLER_VECTOR, %rsi
	mov GP_handler,%rdi
	call set_interrupt_handler
	
	mov PF_HANDLER_VECTOR, %rsi
	mov PF_handler,%rdi
	call set_interrupt_handler
	
	mov DB_HANDLER_VECTOR, %rsi
	mov DB_handler,%rdi
	call set_interrupt_handler
	
	
    push $(USER_SS|0x3)
	mov  $USER_RSP, %rax
	push %rax
	push $(USER_CS|0x3)
	push $user_entry
#	retf64

user_entry: 
    jmp .

.include "libs64.S"
	