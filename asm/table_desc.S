# system_data64.asm
# Copyright (c) 2009-2012 mik 
# All rights reserved.

PROCESSOR_IDT_RSP    = 0x0FFFFFFFFFFE04800
PROCESSOR0_IDT_RSP   = 0x0FFFFFFFFFFE04800

PROCESSOR_IST1_RSP   = 0x0FFFFFFFFFFE10800
PROCESSOR0_IST1_RSP  = 0x0FFFFFFFFFFE10800

system_data64_entry:

#-----------------------------------------
# long-mode 的 GDT 表
#-----------------------------------------
__global_descriptor_table:
		.quad 0                            # NULL descriptor

		.quad 0x00009a000000ffff           # for real mode code segment
		.quad 0x000092000000ffff           # for real mode data segment

		.quad 0x00cf9a000000ffff           # for protected(or compatibility) mode code segment								                        
		.quad 0x00cf92000000ffff           # for protected(or compatibility) mode data segment
								                  

        .quad 0x0020980000000000		    # 64-bit code segment
		.quad 0x0000920000000001		    # 64-bit data segment

		.quad 0x00cffa000000ffff           # for protected(or compatibility) mode code segment
		.quad 0x00cff2000000ffff           # for protected(or compatibility) mode data segment	
                                                
		.quad 0x0020f80000000000		    # 64-bit non-conforming
		.quad 0x0000f20000000000		    # 64-bit data segment

		.word 0x67                         # 64bit TSS
		.long 0
		.word 0
		.quad 0

        .quad 0, 0

        .quad 0

## for syscall/sysret environment					
		.quad 0				             # reserved
        .quad 0x0000f20000000000
        .quad 0x0020f80000000000

        .quad 0x0000f00000000000            # 64-bit data segment

#test_kernel_data64_desc		.quad 0x0000920000000001		# 64-bit data segment
        .rept 5                                     				
        .quad 0						                 # reserved 40  descriptor entries
        .endr
__global_descriptor_table_end:




#--------------------------------------
#  define for long-mode LDT descriptor
#--------------------------------------

__local_descriptor_table:
	    .quad 0
		.quad 0x0020980000000000		# 64-bit code segment
        .quad 0x0000920000000000		# 64-bit data segment
        .quad 0x00cffa000000ffff       # for protected mode code segment
				                                        # or for compatibility mode code segmnt
        .quad 0x00cff2000000ffff           # for protected mode data segment	
        .quad 0x0020f80000000000		# 64-bit non-conforming
        .quad 0x0000f20000000000		# 64-bit data segment
__local_descriptor_table_end:



#-------------------------------------------
# 下面定义 long-mode 的 IDT 表
#-------------------------------------------
__interrupt_descriptor_table:
		   .rept 0x50
		   .quad 0, 0			# 保留 0x50 个 vector
           .endr
__interrupt_descriptor_table_end:



#-------------------------------------------
# TSS64 for long mode
#-------------------------------------------
__processor0_task_status_segment:
__task_status_segment:
	.long 0							        # reserved
	.quad PROCESSOR0_IDT_RSP					# rsp0
	.quad 0							        # rsp1
	.quad 0							        # rsp2
	.quad 0							        # reserved
	.quad PROCESSOR0_IST1_RSP       			# IST1

	.rept 0x3c
    .byte 0
    .endr
__task_status_segment_end:
__processor0_task_status_segment_end:


#*
#* 为 7 个处理器定义 7 个 TSS 区域
#*
__processor_task_status_segment:
        .rept 104* 8
		.byte 0                                      # 保留 8 个 TSS 空间
        .endr

#--------------------------------------------
# TEST_TSS SEGMENT
#-------------------------------------------
__test_tss:
	.long 0							# reserved
	.quad PROCESSOR0_IDT_RSP		# rsp0
	.quad 0							# rsp1
	.quad 0							# rsp2
	.quad 0							# reserved
	.quad PROCESSOR0_IST1_RSP       # IST1

#    .rept 0x3c
#    .byte 0
#    .endr
__test_tss_end:

system_data64_end:


