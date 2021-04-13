# page#inc
# Copyright (c) 2009-2012 mik 
# All rights reserved#


.ifndef PAGE_INC


#*** 32-bit paging base address ***
.equ PDT32_BASE        ,      0x100000

#*** PAE paging base address ***
.equ PDPT_BASE         ,      0x100000


# IA-32e paging base address
.equ PML4T_BASE        ,      0x200000
.equ INIT_SEG          ,      0x0FFFFFFF800000000


## define  table entry control bit filed

.equ P                 ,      0x1
.equ RW                ,      0x2
.equ US                ,      0x4
.equ PWT               ,      0x8
.equ PCD               ,      0x10
.equ A                 ,      0x20
.equ D                 ,      0x40
.equ PS                ,      0x80
.equ G                 ,      0x100
.equ PAT               ,      0x1000
.equ XD                ,      0x80000000


## Use user stack when off paging
.equ USER_ESP          ,      0x08FF0

## each logical processor has themself stack pointer
.equ PROCESSOR0_ESP                        ,        0x3ff000
.equ PROCESSOR1_ESP                        ,        0x3fe000
.equ PROCESSOR2_ESP                        ,        0x3fd000
.equ PROCESSOR3_ESP                        ,        0x3fc000


# The default stack size is 2K
.equ PROCESSOR_STACK_SIZE   ,         0x800


#--------------------------------------------------------------------------------------------------------
# Every logical cpu define themself kernel stack  pointer
#      processor #0 = PROCESSOR_KERNEL_ESP + PROCESSOR_STACK_SIZE + (PROCESSOR_STACK_SIZE * 0)
#      processor #1 = PROCESSOR_KERNEL_ESP + PROCESSOR_STACK_SIZE + (PROCESSOR_STACK_SIZE * 1)
#      processor #2 = PROCESSOR_KERNEL_ESP + PROCESSOR_STACK_SIZE + (PROCESSOR_STACK_SIZE * 2)
#      processor #3 = PROCESSOR_KERNEL_ESP + PROCESSOR_STACK_SIZE + (PROCESSOR_STACK_SIZE * 3)
#------------------------------------------------------------------------------------------------------------
.equ PROCESSOR_KERNEL_ESP    ,        0x0FFE00800

# use for interrupt handler RSP
.equ PROCESSOR_IDT_ESP       ,        0x0FFE04800

# user stack
.equ PROCESSOR_USER_ESP      ,        0x7FE00800


.ifdef NON_PAGING
        .equ USER_ESP         ,       0x08FF0
        .equ KERNEL_ESP       ,       0x3FF000
.else
        .equ USER_ESP         ,       (PROCESSOR_USER_ESP)
        .equ KERNEL_ESP       ,       (PROCESSOR_KERNEL_ESP)
.endif



# 64-bit mode
.equ SYSTEM_DATA64_BASE       ,       0x0FFFFFFF800000000

#  uesed for compatibility mode
.equ COMPATIBILITY_USER_ESP    ,      0x018FF0
.equ COMPATIBILITY_KERNEL_ESP  ,      0x0FFE003F0
.equ LIB32_ESP                 ,      0x0FFE01FF0


# 64-bit mode kernel stack
#
.equ PROCESSOR_KERNEL_RSP      ,      0x0FFFFFFFFFFE00800
.equ PROCESSOR0_KERNEL_RSP     ,      0x0FFFFFFFFFFE00800

# used for interrupt handler  RSP
.equ PROCESSOR_IDT_RSP         ,      0x0FFFFFFFFFFE04800
.equ PROCESSOR0_IDT_RSP        ,      0x0FFFFFFFFFFE04800

# used for sysenter  RSP
.equ PROCESSOR_SYSENTER_RSP    ,      0x0FFFFFFFFFFE08800


# used for syscall  RSP
.equ PROCESSOR_SYSCALL_RSP     ,      0x0FFFFFFFFFFE0C800


# used for IST table RSP value
.equ PROCESSOR_IST1_RSP        ,      0x0FFFFFFFFFFE10800
.equ PROCESSOR0_IST1_RSP       ,      0x0FFFFFFFFFFE10800


# 64-bit mode user stack
.equ PROCESSOR_USER_RSP        ,      0x00007FFFFFE00800



.equ USER_RSP                  ,      (PROCESSOR_USER_RSP)
.equ KERNEL_RSP                ,      (PROCESSOR_KERNEL_RSP)


.endif
