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




        .align 4
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
		