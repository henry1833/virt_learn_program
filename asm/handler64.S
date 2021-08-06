         .align 8
#-------------------------------------------
# DE_handler:
#------------------------------------------

DE_handler:
#        STORE_CONTEXT64
do_DE_handler:        
        mov $demsg1,%rsi
		call puts
		mov 8(%rsp),%rsi
		call print_qword_value
        call println
        
		mov $demsg2,%rsi
		call puts
		mov (%rsp),%rsi
		call print_qword_value
		call println
		
		mov $demsg3,%rsi
		call puts
		mov 32(%rsp),%rsi
        call print_qword_value
		call println
		
#		mov $gmsg4,%rsi
#		call puts
#		call dump_reg64
		jmp .
		iret
demsg1:   .asciz "---> Now, enter #DE handler, occur at: 0x"
demsg2:   .asciz "error code = 0x"
demsg3:   .asciz "rsp = 0x"

         .align 8
#-------------------------------------------
# NMI_handler:
#------------------------------------------

NMI_handler:
#        STORE_CONTEXT64
do_NMI_handler:        
        mov $nmimsg1,%rsi
		call puts
		mov 8(%rsp),%rsi
		call print_qword_value
        call println
        
		mov $nmimsg2,%rsi
		call puts
		mov (%rsp),%rsi
		call print_qword_value
		call println
		
		mov $nmimsg3,%rsi
		call puts
		mov 32(%rsp),%rsi
        call print_qword_value
		call println
		
#		mov $gmsg4,%rsi
#		call puts
#		call dump_reg64
		jmp .
		iret
nmimsg1:   .asciz "---> Now, enter #NMI handler, occur at: 0x"
nmimsg2:   .asciz "error code = 0x"
nmimsg3:   .asciz "rsp = 0x"


         .align 8
#-------------------------------------------
# OF_handler:
#------------------------------------------

OF_handler:
#        STORE_CONTEXT64
do_OF_handler:        
        mov $ofmsg1,%rsi
		call puts
		mov 8(%rsp),%rsi
		call print_qword_value
        call println
        
		mov $ofmsg2,%rsi
		call puts
		mov (%rsp),%rsi
		call print_qword_value
		call println
		
		mov $ofmsg3,%rsi
		call puts
		mov 32(%rsp),%rsi
        call print_qword_value
		call println
		
#		mov $gmsg4,%rsi
#		call puts
#		call dump_reg64
		jmp .
		iret
ofmsg1:   .asciz "---> Now, enter #OF handler, occur at: 0x"
ofmsg2:   .asciz "error code = 0x"
ofmsg3:   .asciz "rsp = 0x"

         .align 8
#-------------------------------------------
# UD_handler:
#------------------------------------------

UD_handler:
#        STORE_CONTEXT64
do_UD_handler:        
        mov $udmsg1,%rsi
		call puts
		mov 8(%rsp),%rsi
		call print_qword_value
        call println
        
		mov $udmsg2,%rsi
		call puts
		mov (%rsp),%rsi
		call print_qword_value
		call println
		
		mov $udmsg3,%rsi
		call puts
		mov 32(%rsp),%rsi
        call print_qword_value
		call println
		
#		mov $gmsg4,%rsi
#		call puts
#		call dump_reg64
		jmp .
		iret
udmsg1:   .asciz "---> Now, enter #UD handler, occur at: 0x"
udmsg2:   .asciz "error code = 0x"
udmsg3:   .asciz "rsp = 0x"


         .align 8
#-------------------------------------------
# NM_handler:
#------------------------------------------

NM_handler:
#        STORE_CONTEXT64
do_NM_handler:        
        mov $nmmsg1,%rsi
		call puts
		mov 8(%rsp),%rsi
		call print_qword_value
        call println
        
		mov $nmmsg2,%rsi
		call puts
		mov (%rsp),%rsi
		call print_qword_value
		call println
		
		mov $udmsg3,%rsi
		call puts
		mov 32(%rsp),%rsi
        call print_qword_value
		call println
		
#		mov $gmsg4,%rsi
#		call puts
#		call dump_reg64
		jmp .
		iret
nmmsg1:   .asciz "---> Now, enter #NM handler, occur at: 0x"
nmmsg2:   .asciz "error code = 0x"
nmmsg3:   .asciz "rsp = 0x"

         .align 8
#-------------------------------------------
# DF_handler:
#------------------------------------------

DF_handler:
#        STORE_CONTEXT64
do_DF_handler:        
        mov $dfmsg1,%rsi
		call puts
		mov 4(%rsp),%rsi
		call print_qword_value
        call println
		jmp .
		iret
dfmsg1: .asciz "---> now, enter #DF handler, occur at: 0x"

         .align 8
#-------------------------------------------
# TS_handler:
#------------------------------------------

TS_handler:
#        STORE_CONTEXT64
do_TS_handler:        
        mov $tsmsg1,%rsi
		call puts
		mov 8(%rsp),%rsi
		call print_qword_value
        call println
        
		mov $tsmsg2,%rsi
		call puts
		mov (%rsp),%rsi
		call print_qword_value
		call println
		
		mov $tsmsg3,%rsi
		call puts
		mov 32(%rsp),%rsi
        call print_qword_value
		call println
		
#		mov $gmsg4,%rsi
#		call puts
#		call dump_reg64
		jmp .
		iret
tsmsg1:   .asciz "---> Now, enter #TS handler, occur at: 0x"
tsmsg2:   .asciz "error code = 0x"
tsmsg3:   .asciz "rsp = 0x"

         .align 8
#-------------------------------------------
# NP_handler:
#------------------------------------------

NP_handler:
#        STORE_CONTEXT64
do_NP_handler:        
        mov $npmsg1,%rsi
		call puts
		mov 8(%rsp),%rsi
		call print_qword_value
        call println
        
		mov $npmsg2,%rsi
		call puts
		mov (%rsp),%rsi
		call print_qword_value
		call println
		
		mov $npmsg3,%rsi
		call puts
		mov 32(%rsp),%rsi
        call print_qword_value
		call println
		
#		mov $gmsg4,%rsi
#		call puts
#		call dump_reg64
		jmp .
		iret
npmsg1:   .asciz "---> Now, enter #NP handler, occur at: 0x"
npmsg2:   .asciz "error code = 0x"
npmsg3:   .asciz "rsp = 0x"

         .align 8
#-------------------------------------------
# gp_handler:
#------------------------------------------

GP_handler:
#        STORE_CONTEXT64
do_GP_handler:        
        mov $gmsg1,%rsi
		call puts
		mov 8(%rsp),%rsi
		call print_qword_value
        call println
        
		mov $gmsg2,%rsi
		call puts
		mov (%rsp),%rsi
		call print_qword_value
		call println
		
		mov $gmsg3,%rsi
		call puts
		mov 32(%rsp),%rsi
        call print_qword_value
		call println
		
#		mov $gmsg4,%rsi
#		call puts
#		call dump_reg64
		jmp .
		iret
gmsg1:   .asciz "---> Now, enter #GP handler, occur at: 0x"
gmsg2:   .asciz "error code = 0x"
gmsg3:   .asciz "rsp = 0x"
#gmsg4:   .byte '--------------- register context ------------', 10, 0


         .align 8
#-------------------------------------------
# MC_handler:
#------------------------------------------

MC_handler:
#        STORE_CONTEXT64
do_MC_handler:        
        mov $mcmsg1,%rsi
		call puts
		mov 8(%rsp),%rsi
		call print_qword_value
        call println
        
		mov $mcmsg2,%rsi
		call puts
		mov (%rsp),%rsi
		call print_qword_value
		call println
		
		mov $mcmsg3,%rsi
		call puts
		mov 32(%rsp),%rsi
        call print_qword_value
		call println
		
#		mov $gmsg4,%rsi
#		call puts
#		call dump_reg64
		jmp .
		iret
mcmsg1:   .asciz "---> Now, enter #MC handler, occur at: 0x"
mcmsg2:   .asciz "error code = 0x"
mcmsg3:   .asciz "rsp = 0x"
#gmsg4:   .byte '--------------- register context ------------', 10, 0


        .align 8
#-------------------------------------------
# #PF handler:
#------------------------------------------
PF_handler:        
        add $8,%rsp
		mov $pf_msg1,%rsi
		call puts
		mov $pf_msg2,%rsi
		call puts
		mov (%rsp),%rsi
		mov 4(%rsp),%rdi
		call print_qword_value
        call println
        mov $pf_msg3,%rsi
        call puts
        mov %cr2,%rsi
        mov %rsi,%rdi
        shr $32,%rdi
        call print_qword_value
        call println
        jmp . 
        iret
		
pf_msg1: .ascii "\10>>> Now, enter #PF handler\10\0"
pf_msg2: .asciz ">>>>>>> occur at: 0x"       
pf_msg3: .asciz ">>>>>>> page fault address: 0x"




        .align 8
#*********************************
# #DB handler
#*********************************
DB_handler:      
        mov $db_msg1,%rsi
        call puts
        
# 关掉 L0 enable 位
        mov %dr7,%rax
        btr $0,%rax
        mov %rax,%dr7
        
#        call dump_debugctl
#        call dump_lbr_stack
        
do_db_handler_done:        
        btsl $16,16(%rsp)                ; RF=1
        mov $db_msg2,%rsi
        call puts
        iret
		
db_msg1: .asciz ">>> now, enter #DB handler"
db_msg2: .ascii "now, exit #DB handler <<<\10\0"

##
##apic timer interrupt handler		
		.align 8
apic_timer_handler:
        mov $at_msg1, %rsi
		call puts
		call println
		call dump_apic
		call println
		mov $at_msg2, %rsi
		call puts
		call println
		movl $0, (APIC_BASE + EOI) 
        iret

at_msg1: .asciz ">>>>>>> now: enter APIC timer handler"
at_msg2: .asciz "exit the APIC timer handler"
