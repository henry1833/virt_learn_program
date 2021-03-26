#------------------------------------------------------
# clear_screen()
# description:
#                clear the screen & set cursor position at (0,0)
#------------------------------------------------------
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

read_sector_with_chs:
       call do_lba_to_chs
	   mov driver_number,%dl
	   mov buffer_selector,%es
	   mov buffer_offset, %bx
	   movb %al, read_sector
	   mov $0x2,%ah
	   int $0x13
	   movzx %ah,%ax
read_sector_with_chs_done:
       ret


read_sector:
       push %es
	   mov buffer_offset,%es
	   
	   pop %es
	   ret

get_driver_parameters:
       push %dx
	   push %cx
	   push %bx
	   mov %0x08,%ah
	   mov driver_number,%dl
	   mov parameter_table,%di
	   int $0x13
	   jc get_driver_parameters_done
       movb %bl,driver_type
       int %dh
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


disk_address_packet:
    size:               .byte   0x10    # size of packet
    read_sectors:	    .byte   0x0     # unused
    buffer_offset:	    .word   0x0     # read numbers of sectors
    dst_segment:	    .word   0x0     # dst segment
    dst_offset:		    .word   0x0     # dst offset
    start_sector:	    .quad   0x0     # start sectors for read 


driver_parameters_table:
    driver_type:        .byte 0x0
    driver_number:		.byte 0x0
    cylinder_maximum:   .word 0x0
    header_maximum:     .word 0x0
    sector_maximum:     .word 0x0
    parameter_table:    .word 0x0