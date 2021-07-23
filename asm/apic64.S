
#
#
# support_apic: check it if support APIC on chip
support_apic:
    mov $1,%rax
	cpuid
	bt $9, %rdx
	setc %al
	movzx %al, %rax
	ret
	
#
#support_x2apic: check it if support x2apic
support_x2apic:
    mov $1, %rax
	cpuid
	bt $21, %ecx
	setc %al
	movzx %al, %rax
	ret

#
#enable_x2apic
enable_apic:
    mov $IA32_APIC_BASE, %rcx
	rdmsr
	bts $11, %rax
	wrmsr
	btsl $8,(APIC_BASE + SVR)
	ret

#
#enable_x2apic
disable_apic:
    mov $IA32_APIC_BASE, %rcx
	rdmsr
	btr $11, %rax
	wrmsr
	btr $8,(APIC_BASE + SVR)
	ret

#-------------------------------------
# print ISR rigister
#-------------------------------------
dump_isr:
        mov  $inr, %rsi
        call puts
		mov $0, %rsi
        mov (APIC_BASE + ISR7), %esi
        call print_dword_value
        mov  $'_', %rsi
        call putc
        mov (APIC_BASE + ISR6), %esi
        call print_dword_value
        mov  $'_', %rsi
        call putc
        mov (APIC_BASE + ISR5), %esi
        call print_dword_value
        mov  $'_', %rsi
        call putc        
        mov (APIC_BASE + ISR4), %esi
        call print_dword_value
        mov  $'_', %rsi
        call putc        
        mov (APIC_BASE + ISR3), %esi
        call print_dword_value
        mov  $'_', %rsi
        call putc
        mov (APIC_BASE + ISR2), %esi
        call print_dword_value
        mov  $'_', %esi
        call putc                
        mov (APIC_BASE + ISR1), %esi
        call print_dword_value
        mov  $'_', %rsi
        call putc        
        mov (APIC_BASE + ISR0), %esi
        call print_dword_value
        call println
        ret
        
#-----------------------------
# print IRR register
#-----------------------------        
dump_irr:
        mov  $irr, %rsi
        call puts
		mov $0, %rsi
        mov (APIC_BASE + IRR7), %esi
        call print_dword_value
        mov $'_', %rsi
        call putc
        mov (APIC_BASE + IRR6), %esi
        call print_dword_value
        mov $'_', %rsi
        call putc
        mov (APIC_BASE + IRR5), %esi
        call print_dword_value
        mov $'_', %esi
        call putc        
        mov (APIC_BASE + IRR4), %esi
        call print_dword_value
        mov $'_', %rsi
        call putc        
        mov (APIC_BASE + IRR3), %esi
        call print_dword_value
        mov $'_', %rsi
        call putc
        mov (APIC_BASE + IRR2), %esi
        call print_dword_value
        mov $'_', %rsi
        call putc                
        mov (APIC_BASE + IRR1), %esi
        call print_dword_value
        mov $'_', %esi
        call putc        
        mov (APIC_BASE + IRR0), %esi
        call print_dword_value
        call println
        ret
                
#--------------------------
# Print TMR register
#-------------------------                
dump_tmr:
        mov $tmr, %rsi
        call puts
		mov $0, %esi
        mov (APIC_BASE + TMR7), %rsi
        call print_dword_value
        mov $'_', %rsi
        call putc
        mov (APIC_BASE + TMR6), %esi
        call print_dword_value
        mov $'_', %esi
        call putc
        mov (APIC_BASE + TMR5), %esi
        call print_dword_value
        mov $'_', %esi
        call putc        
        mov (APIC_BASE + TMR4), %esi
        call print_dword_value
        mov $'_', %rsi
        call putc        
        mov (APIC_BASE + TMR3), %esi
        call print_dword_value
        mov $'_', %esi
        call putc
        mov (APIC_BASE + TMR2), %esi
        call print_dword_value
        mov $'_', %esi
        call putc                
        mov (APIC_BASE + TMR1), %esi
        call print_dword_value
        mov $'_', %esi
        call putc        
        mov (APIC_BASE + TMR0), %esi
        call print_dword_value
        call println
        ret                
        
        
#------------------------------
# dump_apic()：打印 apic寄存器信息
#--------------------------------
dump_apic:
        mov $apicid, %rsi
        call puts
		mov $0, %esi
        mov (APIC_BASE + APIC_ID), %esi                
        call print_dword_value
        mov $apicver, %rsi
        call puts
        mov (APIC_BASE + APIC_VER), %esi
        call print_dword_value
        call println
        mov $tpr, %rsi
        call puts
		mov $0, %rsi
        mov (APIC_BASE + TPR), %esi
        call print_dword_value
        mov $apr, %rsi
        call puts
        mov (APIC_BASE + APR), %esi
        call print_dword_value
        mov $ppr, %rsi
        call puts
        mov (APIC_BASE + PPR), %esi
        call print_dword_value
        call println        
        mov $eoi, %rsi
        call puts
        mov (APIC_BASE + EOI), %esi
        call print_dword_value
        mov $rrd, %rsi
        call puts
        mov (APIC_BASE + RRD), %esi
        call print_dword_value
        mov $ldr,%rsi
        call puts
        mov (APIC_BASE + LDR), %esi
        call print_dword_value        
        call println
        mov $dfr, %rsi
        call puts
        mov (APIC_BASE + DFR), %esi
        call print_dword_value        
        mov $svr, %rsi
        call puts
        mov (APIC_BASE + SVR), %esi
        call print_dword_value        
        call println

        call dump_irr
                      
        call dump_isr

        call dump_tmr

        mov $esr, %rsi
        call puts
        mov (APIC_BASE + ESR), %esi        
        call print_dword_value
        mov $icr, %rsi
        call puts
        mov (APIC_BASE + ICR0),%rsi        
#        movl (APIC_BASE + ICR1),%rdi        
        call print_qword_value
        call println
        
        mov $lvt_msg, %rsi
        call puts
        mov $lvt_cmci, %rsi
        call puts
		mov $0,%rsi
        mov (APIC_BASE + LVT_CMCI), %esi
        call print_dword_value
        mov $lvt_timer, %rsi
        call puts
        mov (APIC_BASE + LVT_TIMER), %esi
        call print_dword_value        
        mov  $lvt_thermal, %rsi
        call puts
        mov (APIC_BASE + LVT_THERMAL), %esi
        call print_dword_value        
        call println
        mov $lvt_perfmon, %rsi
        call puts
        mov (APIC_BASE + LVT_PERFMON), %esi
        call print_dword_value        
        mov $lvt_lint0, %rsi
        call puts
        mov (APIC_BASE + LVT_LINT0), %esi
        call print_dword_value        
        mov $lvt_lint1, %rsi
        call puts
        mov (APIC_BASE + LVT_LINT1), %esi
        call print_dword_value        
        call println
        mov $lvt_error, %rsi
        call puts
        mov (APIC_BASE + LVT_ERROR), %esi
        call print_dword_value        
        call println
        mov $init_count, %rsi
        call puts
        mov (APIC_BASE + TIMER_ICR), %esi
        call print_dword_value
        mov $current_count, %rsi
        call puts
        mov (APIC_BASE + TIMER_CCR), %esi
        call print_dword_value
        mov $dcr, %rsi
        call puts
        mov (APIC_BASE + TIMER_DCR), %esi
        call print_dword_value        
        ret
		
#
# define multi-processor enviromenet
#


# logical processor number
processor_index:                 .byte        -1
processor_count:                 .byte        0
apic_id:                         .byte        0, 0, 0, 0, 0, 0, 0, 0
vacant:                          .byte        0        
ap_init_done:                    .byte        0

instruction_count:               .byte        0, 0, 0, 0
clocks:                          .byte        0, 0, 0, 0


# define  variable related x2APIC ID
x2apic_smt_mask_width:           .byte        0, 0, 0, 0
x2apic_smt_select_mask:          .byte        0, 0, 0, 0
x2apic_core_mask_width:          .byte        0, 0, 0, 0
x2apic_core_select_mask:         .byte        0, 0, 0, 0
x2apic_package_mask_width:       .byte        0, 0, 0, 0
x2apic_package_select_mask:      .byte        0, 0, 0, 0

x2apic_id:                       .byte        0, 0, 0, 0
x2apic_package_id:               .byte        0, 0, 0, 0
x2apic_core_id:                  .byte        0, 0, 0, 0
x2apic_smt_id:                   .byte        0, 0, 0, 0


# define variable related xAPIC ID
xapic_smt_mask_width:            .byte        0, 0, 0, 0
xapic_smt_select_mask:           .byte        0, 0, 0, 0
xapic_core_mask_width:           .byte        0, 0, 0, 0
xapic_core_select_mask:          .byte        0, 0, 0, 0
xapic_package_mask_width:        .byte        0, 0, 0, 0
xapic_package_select_mask:       .byte        0, 0, 0, 0

xapic_id:                        .byte        0, 0, 0, 0
xapic_package_id:                .byte        0, 0, 0, 0
xapic_core_id:                   .byte        0, 0, 0, 0
xapic_smt_id:                    .byte         0, 0, 0, 0



#**** data *****
apicid:          .asciz        "apic ID: 0x"
apicver:         .asciz        "    apic version: 0x"
tpr:             .asciz        "TPR: 0x"
apr:             .asciz        "  APR: 0x"
ppr:             .asciz        "  PPR: 0x"
eoi:             .asciz        "EOI: 0x"        
rrd:             .asciz        "  RRD: 0x"
ldr:             .asciz        "  LDR: 0x"
dfr:             .asciz        "DFR: 0x"
svr:             .asciz        "  SVR: 0x"
inr:             .asciz        "ISR(7-0) : 0x"
tmr:             .asciz        "TMR: 0x"        
irr:             .asciz        "IRR: 0x"
esr:             .asciz        "ESR: 0x"
icr:             .asciz        "  ICR: 0x"
lvt_msg:         .asciz        "---------------------- Local Vector Table -----------------"
lvt_cmci:        .asciz        "CMCI: 0x"
lvt_timer:       .asciz        "  TIMER: 0x"
lvt_thermal:     .asciz        "  THERMAL: 0x"
lvt_perfmon:     .asciz        "PERFMON: 0x"
lvt_lint0:       .asciz        "  LINT0: 0x"
lvt_lint1:       .asciz        "  LINT1: 0x"
lvt_error:       .asciz        "ERROR: 0x"
init_count:      .asciz        "timer_ICR: 0x"
current_count:   .asciz        "  timer_CCR: 0x"
dcr:             .asciz        "  timer_DCR: 0x"
