.include "system_data.h"
.include "page.h"
.include "CPU.h"
.include "apic.h"

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

    mov $SYSTEM_DATA64_BASE, %rdi
	mov $system_data64_entry, %rsi
	mov $(system_data64_end - system_data64_entry), %rcx
	rep movsb
	
#set Gdt and Idt
    mov $SYSTEM_DATA64_BASE, %rbx
    add $(gdt_pointer64 - system_data64_entry), %rbx
	mov $(SYSTEM_DATA64_BASE + (__global_descriptor_table - system_data64_entry)), %rax
	mov %rax, 2(%rbx)
    lgdt (%rbx)
	
    mov $SYSTEM_DATA64_BASE, %rbx
	add $(idt_pointer64 - system_data64_entry), %rbx
	mov $(SYSTEM_DATA64_BASE + (__interrupt_descriptor_table - system_data64_entry)), %rax
	mov %rax, 2(%rbx)
	lidt (%rbx)

#set GP handler
    mov $GP_HANDLER_VECTOR, %rsi
	mov $GP_handler,%rdi
	call set_interrupt_handler

#set PF handler	
	mov $PF_HANDLER_VECTOR, %rsi
	mov $PF_handler,%rdi
	call set_interrupt_handler

#set DB handler	
	mov $DB_HANDLER_VECTOR, %rsi
	mov $DB_handler,%rdi
	call set_interrupt_handler

    mov $IA32_APIC_BASE,%rax
	rdmsr
	mov $0xfffff000, %rbx
	and %rbx, %rax
	
	
	mov $0xfffff7c000000,%rax
	mov (%rax),%rbx
	
    push $(USER_SS|0x3)
	mov  $USER_RSP, %rax
	push %rax
	push $(USER_CS|0x3)
	push $user_entry
#	retf64

user_entry: 
    jmp .

msg1: .asciz "support local apic: "
msg2: .asciz "support x2apic    :"
msg3: .asciz "yes"
msg4: .asciz "no"

.include "handler64.S"
.include "libs64.S"
.include "apic64.S"
