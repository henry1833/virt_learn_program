#------------------------------------------------------
# clear_screen()
# description:
#                clear the screen & set cursor position at (0,0)
#------------------------------------------------------
#        .align 8
clear_screen:
        pusha
        mov $0x0600,%ax
        xor %cx, %cx
        xor $0x0f,%bh           # white
        mov $24,%dh
        mov $79,%dl
        int $0x10
        mov $2,%ah
        mov $0,%bh
        mov $0,%dx
        int $0x10        
        popa
        ret

#
#
#
#       .align 8
do_lba_to_chs:
	   mov sector_maximum,%cx
	   mov header_maximum,%ax
	   mul %cx
	   mov %eax,%ecx             #ecx = head_maximum * sector_maximum
       mov start_sector,%eax
	   xor %edx,%edx
       div %ecx
	
       mov %eax,%ebx             #ebx = cylinder

	   mov sector_maximum,%di
	   movzx %di,%ecx
	   mov %edx,%eax
	   xor %edx,%edx
	   div %ecx  
	   inc %edx                  #edx = sector, eax = head

	   mov %dl,%cl
	   mov %bl,%ch
	   shr $2,%bx
	   and $0x0c0,%bx
	   or %bl,%cl
	   mov %al,%dh
	   ret

#       .align 8
read_sector_with_chs:
       call do_lba_to_chs
	   mov driver_number,%dl
	   mov buffer_selector,%es
	   mov buffer_offset, %bx
	   movb read_sectors,%al
	   mov $0x2,%ah
	   int $0x13
	   movzx %ah,%ax
read_sector_with_chs_done:
       ret


#	   .align 8

load_module:
       movw $0x7e00,(buffer_offset)
       movw $0x0,   (buffer_selector)	   
       movw $0x1,read_sectors
	   movw $0x1,start_sector
	   call read_sector_with_chs
	   test %ax,%ax
	   jnz do_load_module_done
	   
	   add $0x1,(start_sector)
	   sub $0x1,(total_sectors)
       movw $0x800,(buffer_selector)
	   movw $0x0,(buffer_offset)
loop_read:	   
	   movw $0x20,read_sectors
	   call read_sector_with_chs
	   test %ax,%ax
	   jnz do_load_module_done
	   addw $0x20,start_sector
	   addw $0x400,buffer_selector
	   subw $0x20,total_sectors
	   jz do_load_module_done
	   cmpl $0x20,total_sectors
	   jnc loop_read
	   
	   movw total_sectors,%bx
	   movw %bx,read_sectors
	   call read_sector_with_chs
do_load_module_done:
	   ret

#       .align 8
get_driver_parameters:
       push %dx
	   push %cx
	   push %bx
	   mov $0x08,%ah
	   mov driver_number,%dl
	   mov parameter_table,%di
	   int $0x13
	   jc get_driver_parameters_done
       movb %bl,driver_type
       inc %dh
       movb %dh, header_maximum
       movb %cl, sector_maximum
       and $0x3f, sector_maximum
       shr $6,%cl
       rol $8,%cx
       and $0x3ff,%cx
       inc %cx
       mov %cx, cylinder_maximum	   
get_driver_parameters_done:
       movzx %ah,%ax
	   pop %bx
	   pop %cx
	   pop %dx
	   ret


int13_ext_check:  
       mov $0x41, %ah  
       mov $0x55aa, %bx  
       mov $0x00, %dl  
       int $0x13  
	   ret

read_sector_with_extension:
        mov disk_address_packet,%si
		mov $0x0, %dl
		mov $0x42,%ah
		int $0x13
		movzx %ah,%ax
		ret

		.align 4
disk_address_packet:
    size:               .byte   0x10    # size of packet
    read_sectors:	    .word   0x0     # number of sector
    buffer_offset:	    .word   0x0     # dst offset
    buffer_selector:	.word   0x0     # dst segment
    start_sector:	    .long   0x0     # start sectors for read 
                        .long   0x0
	total_sectors:      .long   0x0
	
read_chs_parameter:
    CL_P:    .byte   0x0
	DH_P:    .byte   0x0
	CH_P:    .byte   0x0

driver_parameters_table:
    driver_type:        .byte 0x0
    driver_number:		.byte 0x0
    cylinder_maximum:   .word 0x0
    header_maximum:     .word 0x0
    sector_maximum:     .word 0x0
    parameter_table:    .word 0x0
