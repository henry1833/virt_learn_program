# page#inc
# Copyright (c) 2009-2012 mik 
# All rights reserved#


.ifndef SYSTEM_INC


#### segment register selector
.equ   code16_sel              ,     0x08
.equ   data16_sel              ,     0x10
.equ   code32_sel              ,     0x18
.equ   data32_sel              ,     0x20
.equ   kernel_code64_sel       ,     0x28
.equ   kernel_data64_sel       ,     0x30
.equ   user_code32_sel         ,     0x38
.equ   user_data32_sel         ,     0x40
.equ   user_code64_sel         ,     0x48
.equ   user_data64_sel         ,     0x50
.equ   tss64_sel               ,     0x58
   
.equ   call_gate_sel           ,     0x68
   
.equ   conforming_code_sel     ,     0x78
.equ   sysret_cs_sel           ,     0x80
.equ   data64_sel              ,     0x98
.equ   test_tss_sel            ,     0xa0
.equ   ldt_sel                 ,     0xb0
   
.equ   conforming_callgate_sel ,     0xc0
   
.equ   processor0_tss_sel      ,     tss64_sel
   
#
# processor TSS selector
#
# reserved 8 TSS descriptor area 
#
.equ   processor_tss_sel       ,     0xd0
   
.equ   reserved_sel            ,     0x150
   
   
# define TSS bit field and selector
.equ   LDT64                   ,     0x2
.equ   TSS64                   ,     0x9
.equ   TSS64_BUSY              ,     0xb
.equ   CALL_GATE64             ,     0xc
.equ   INTERRUPT_GATE64        ,     0xe
.equ   TRAP_GATE64             ,     0xf
   
.equ   KERNEL_CS               ,     kernel_code64_sel
.equ   KERNEL_SS               ,     kernel_data64_sel
.equ   USER_CS                 ,     user_code64_sel
.equ   USER_SS                 ,     user_data64_sel
.equ   USER_DS                 ,     data64_sel
   
.equ   LDT_KERNEL_CS           ,     0x08 | 4
.equ   LDT_KERNEL_SS           ,     0x10 | 4
.equ   LDT_USER32_CS           ,     0x18 | 4
.equ   LDT_USER32_SS           ,     0x20 | 4
.equ   LDT_USER_CS             ,     0x28 | 4
.equ   LDT_USER_SS             ,     0x30 | 4
   
#define interrupt vector
.equ   DE_HANDLER_VECTOR       ,     0x0
.equ   DB_HANDLER_VECTOR       ,     0x1
.equ   NMI_HANDLER_VECTOR      ,     0x2
.equ   BP_HANDLER_VECTOR       ,     0x3
.equ   OF_HANDLER_VECTOR       ,     0x4
.equ   BR_HANDLER_VECTOR       ,     0x5
.equ   UD_HANDLER_VECTOR       ,     0x6
.equ   NM_HANDLER_VECTOR       ,     0x7
.equ   DF_HANDLER_VECTOR       ,     0x8
.equ   TS_HANDLER_VECTOR       ,     0xa
.equ   NP_HANDLER_VECTOR       ,     0xb
.equ   SS_HANDLER_VECTOR       ,     0xc
.equ   GP_HANDLER_VECTOR       ,     0xd
.equ   PF_HANDLER_VECTOR       ,     0xf
.equ   MF_HANDLER_VECTOR       ,     0x10
.equ   AC_HANDLER_VECTOR       ,     0x11
.equ   MC_HANDLER_VECTOR       ,     0x12
.equ   XF_HANDLER_VECTOR       ,     0x13
.equ   SX_HANDLER_VECTOR       ,     0x1f
   
   
#deifine interrupt service vector
.equ   SYSTEM_SERVICE_VECTOR   ,     0x40
   
   
#define timer interrupt vector
.equ   PIC8259A_TIMER_VECTOR   ,     0x20
#defin  keyboard interrupt vector
.equ   KEYBOARD_VECTOR         ,     0x21
#define x87 FPU error 
.equ   FPU_VECTOR              ,     0x2d
   
   
#define APIC interrupt vector
.equ   APIC_TIMER_VECTOR       ,     0x30
.equ   APIC_LINT0_VECTOR       ,     0x31
.equ   APIC_LINT1_VECTOR       ,     0x32
.equ   APIC_PERFMON_VECTOR     ,     0x33
.equ   APIC_ERROR_VECTOR       ,     0x34
.endif
