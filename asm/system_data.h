# page#inc
# Copyright (c) 2009-2012 mik 
# All rights reserved#


.ifndef SYSTEM_INC


#### segment register selector
code16_sel              =     0x08
data16_sel              =     0x10
code32_sel              =     0x18
data32_sel              =     0x20
kernel_code64_sel       =     0x28
kernel_data64_sel       =     0x30
user_code32_sel         =     0x38
user_data32_sel         =     0x40
user_code64_sel         =     0x48
user_data64_sel         =     0x50
tss64_sel               =     0x58

call_gate_sel           =     0x68

conforming_code_sel     =     0x78
sysret_cs_sel           =     0x80
data64_sel              =     0x98
test_tss_sel            =     0xa0
ldt_sel                 =     0xb0

conforming_callgate_sel =     0xc0

processor0_tss_sel      =     tss64_sel

#
# processor TSS selector
#
# reserved 8 TSS descriptor area 
#
processor_tss_sel       =     0xd0

reserved_sel            =     0x150

.endif
