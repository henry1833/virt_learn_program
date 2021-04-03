
#
# define opcode
#
.equ REX_W         ,          .byte 0x48
.equ O16           ,          .byte 0x66
.equ O32           ,          .byte 0x66
.equ O64           ,          .byte 0x48
.equ A16           ,          .byte 0x67
.equ A32           ,          .byte 0x67

.equ iret64        ,           iretq
.equ retf64        ,          .word 0x0CB48
.equ sysexit64     ,          .byte 0x48, 0x0F, 0x35
.equ sysret64      ,          .byte 0x48h, 0x0Fh, 0x07


# define lea rax, [rip] instruction encode
.equ GET_RIP       ,          .byte 48h, 8Dh, 05h, 00h, 00h, 00h, 00h



# CR0 control bits
.equ PE_BIT        ,          0
.equ MP_BIT        ,          1
.equ EM_BIT        ,          2
.equ TS_BIT        ,          3
.equ ET_BIT        ,          4
.equ NE_BIT        ,          5
.equ WP_BIT        ,          16
.equ AM_BIT        ,          18
.equ NW_BIT        ,          29
.equ CD_BIT        ,          30
.equ PG_BIT        ,          31

#CR4 control bits
.equ WME_BIT       ,          0
.equ PVI_BIT       ,          1
.equ TSD_BIT       ,          2
.equ DE_BIT        ,          3
.equ PSE_BIT       ,          4
.equ PAE_BIT       ,          5
.equ MCE_BIT       ,          6
.equ PGE_BIT       ,          7
.equ PCE_BIT       ,          8
.equ OSFXSR_BIT    ,          9
.equ OSXMMEXCPT_BIT,          10
.equ VMXE_BIT      ,          13
.equ SMXE_BIT      ,          14
.equ PCIDE_BIT     ,          17
.equ OSXSAVE_BIT   ,          18
.equ SMEP_BIT      ,          20

# SMM mode base address
.equ SMBASE        ,          0x30000


.equ IA32_APIC_BASE                 , 0x1B
.equ IA32_FEATURE_CONTROL           , 0x3A
.equ IA32_SMM_MONITOR_CTL           , 0x9B
.equ IA32_MISC_ENABLE               , 0x1A0
.equ IA32_CPU_DCA_CAP               , 0x1F9
.equ IA32_MONITOR_FILTER_LINE_SIZE  , 0x06
.equ IA32_TIME_STAMP_COUNTER        , 0x10
.equ IA32_TSC_COUNTER               , 0x10


#***** define MTRR register address******

# Fixed-rang MTRR 　
.equ IA32_MTRR_FIX64K_00000         ,                            0x250
.equ IA32_MTRR_FIX16K_80000         ,                            0x258
.equ IA32_MTRR_FIX16K_A0000         ,                            0x259
.equ IA32_MTRR_FIX4K_C0000          ,                            0x268
.equ IA32_MTRR_FIX4K_C8000          ,                            0x269
.equ IA32_MTRR_FIX4K_D0000          ,                            0x26A
.equ IA32_MTRR_FIX4K_D8000          ,                            0x26B
.equ IA32_MTRR_FIX4K_E0000          ,                            0x26C
.equ IA32_MTRR_FIX4K_E8000          ,                            0x26D
.equ IA32_MTRR_FIX4K_F0000          ,                            0x26E
.equ IA32_MTRR_FIX4K_F8000          ,                            0x26F
															   
# MTRR function register                                       
.equ IA32_MTRRCAP                   ,                            0x0FE
.equ IA32_MTRR_DEF_TYPE             ,                            0x2FF

# defint MTRR  PHYSBASE/PHYSMASK register
.equ IA32_MTRR_PHYSBASE0            ,                            0x200
.equ IA32_MTRR_PHYSMASK0            ,                            0x201
.equ IA32_MTRR_PHYSBASE1            ,                            0x202
.equ IA32_MTRR_PHYSMASK1            ,                            0x203
.equ IA32_MTRR_PHYSBASE2            ,                            0x204
.equ IA32_MTRR_PHYSMASK2            ,                            0x205
.equ IA32_MTRR_PHYSBASE3            ,                            0x206
.equ IA32_MTRR_PHYSMASK3            ,                            0x207
.equ IA32_MTRR_PHYSBASE4            ,                            0x208
.equ IA32_MTRR_PHYSMASK4            ,                            0x209
.equ IA32_MTRR_PHYSBASE5            ,                            0x20A
.equ IA32_MTRR_PHYSMASK5            ,                            0x20B
.equ IA32_MTRR_PHYSBASE6            ,                            0x20C
.equ IA32_MTRR_PHYSMASK6            ,                            0x20D
.equ IA32_MTRR_PHYSBASE7            ,                            0x20E
.equ IA32_MTRR_PHYSMASK7            ,                            0x20F
.equ IA32_MTRR_PHYSBASE8            ,                            0x210
.equ IA32_MTRR_PHYSMASK8            ,                            0x211
.equ IA32_MTRR_PHYSBASE9            ,                            0x212
.equ IA32_MTRR_PHYSMASK9            ,                            0x213

# SMRR registers
.equ IA32_SMRR_PHYSBASE             ,                            0x1F2
.equ IA32_SMRR_PHYSMASK             ,                            0x1F3

# PAT register
.equ IA32_PAT                       ,                            0x277


# define Special instructions MSR registers
.equ IA32_SYSENTER_CS               ,                            0x174
.equ IA32_SYSENTER_ESP              ,                            0x175
.equ IA32_SYSENTER_EIP              ,                            0x176


#  support x64 mode register
.equ IA32_EFER                      ,                            0x0C0000080
.equ IA32_STAR                      ,                            0x0C0000081
.equ IA32_LSTAR                     ,                            0x0C0000082
.equ IA32_FMASK                     ,                            0x0C0000084
.equ IA32_FS_BASE                   ,                            0x0C0000100
.equ IA32_GS_BASE                   ,                            0x0C0000101
.equ IA32_KERNEL_GS_BASE            ,                            0x0C0000102



# ##### define for AMD #######
.equ MSR_SMI_ON_IO_TRAP0            ,                            0x0C0010050
.equ MSR_SMI_ON_IO_TRAP1            ,                            0x0C0010051
.equ MSR_SMI_ON_IO_TRAP2            ,                            0x0C0010052
.equ MSR_SMI_ON_IO_TRAP3            ,                            0x0C0010053
.equ MSR_SMI_ON_IO_TRAP_CTL_STS     ,                            0x0C0010054


# define  dubug/performance monitoring  MSR registers

.equ IA32_PEBS_ENABLE               ,                            0x3F1
# general-purpose counter 寄存器
.equ IA32_PERFEVTSEL0               ,                            0x186
.equ IA32_PERFEVTSEL1               ,                            0x187
.equ IA32_PERFEVTSEL2               ,                            0x188
.equ IA32_PERFEVTSEL3               ,                            0x189
.equ IA32_PMC0                      ,                            0x0C1
.equ IA32_PMC1                      ,                            0x0C2
.equ IA32_PMC2                      ,                            0x0C3
.equ IA32_PMC3                      ,                            0x0C4

# fixed-counter 寄存器
.equ IA32_FIXED_CTR0                                            309H
.equ MSR_PERF_FIXED_CTR0                                        309H
.equ IA32_FIXED_CTR1                                            30AH
.equ MSR_PERF_FIXED_CTR1                                        30AH
.equ IA32_FIXED_CTR2                                            30BH
.equ MSR_PERF_FIXED_CTR2                                        30BH

.equ IA32_FIXED_CTR_CTRL                                        38DH
.equ MSR_PERF_FIXED_CTR_CTRL                                    38DH

.equ IA32_PERF_STATUS                                           198H
.equ IA32_PERF_CTL                                              199H

.equ IA32_PERF_CAPABILITIES                                     345H
.equ IA32_PERF_GLOBAL_STATUS                                    38EH
.equ MSR_PERF_GLOBAL_STATUS                                     38EH
.equ MSR_PERF_GLOBAL_CTRL                                       38FH
.equ IA32_PERF_GLOBAL_CTRL                                      38FH
.equ IA32_PERF_GLOBAL_OVF_CTRL                                  390H
.equ MSR_PERF_GLOBAL_OVF_CTRL                                   390H

.equ MSR_PEBS_LD_LAT                                            3F6H

.equ IA32_DS_AREA                                               600H
.equ IA32_TSC_DEADLINE                                          6E0H


# 下面是 06_25H(DisplayFamily_DisplayModel) 处理器的 MSRs， 适用于 Nehalem/Westmere 微架构

.equ MSR_LBR_SELECT                                             1C8H
.equ MSR_LASTBRANCH_TOS                                         1C9H
.equ IA32_DEBUGCTL                                              1D9H
.equ MSR_LER_FROM_LIP                                           1DDH
.equ MSR_LER_TO_LIP                                             1DEH
.equ MSR_PEBS_ENABLE                                            3F1H
.equ MSR_LASTBRANCH_0_FROM_IP                                   680H
.equ MSR_LASTBRANCH_1_FROM_IP                                   681H
.equ MSR_LASTBRANCH_2_FROM_IP                                   682H
.equ MSR_LASTBRANCH_3_FROM_IP                                   683H
.equ MSR_LASTBRANCH_4_FROM_IP                                   684H
.equ MSR_LASTBRANCH_5_FROM_IP                                   685H
.equ MSR_LASTBRANCH_6_FROM_IP                                   686H
.equ MSR_LASTBRANCH_7_FROM_IP                                   687H
.equ MSR_LASTBRANCH_8_FROM_IP                                   688H
.equ MSR_LASTBRANCH_9_FROM_IP                                   689H
.equ MSR_LASTBRANCH_10_FROM_IP                                  68AH
.equ MSR_LASTBRANCH_11_FROM_IP                                  68BH
.equ MSR_LASTBRANCH_12_FROM_IP                                  68CH
.equ MSR_LASTBRANCH_13_FROM_IP                                  68DH
.equ MSR_LASTBRANCH_14_FROM_IP                                  68EH
.equ MSR_LASTBRANCH_15_FROM_IP                                  68FH
.equ MSR_LASTBRANCH_0_TO_IP                                     6C0H
.equ MSR_LASTBRANCH_1_TO_IP                                     6C1H
.equ MSR_LASTBRANCH_2_TO_IP                                     6C2H
.equ MSR_LASTBRANCH_3_TO_IP                                     6C3H
.equ MSR_LASTBRANCH_4_TO_IP                                     6C4H
.equ MSR_LASTBRANCH_5_TO_IP                                     6C5H
.equ MSR_LASTBRANCH_6_TO_IP                                     6C6H
.equ MSR_LASTBRANCH_7_TO_IP                                     6C7H
.equ MSR_LASTBRANCH_8_TO_IP                                     6C8H
.equ MSR_LASTBRANCH_9_TO_IP                                     6C9H
.equ MSR_LASTBRANCH_10_TO_IP                                    6CAH
.equ MSR_LASTBRANCH_11_TO_IP                                    6CBH
.equ MSR_LASTBRANCH_12_TO_IP                                    6CCH
.equ MSR_LASTBRANCH_13_TO_IP                                    6CDH
.equ MSR_LASTBRANCH_14_TO_IP                                    6CEH
.equ MSR_LASTBRANCH_15_TO_IP                                    6CFH


# Nehalem 微架构的辅助 TSC 寄存器
.equ IA32_TSC_AUX                                                0C0000103H


.equ MSR_FSB_FREQ                                            0CDH
.equ IA32_MPERF                                              0E7H
.equ IA32_APERF                                              0E8H
.equ IA32_PLATFORM_ID                                        17H
.equ MSR_PLATFORM_ID                                         17H
.equ MSR_PERF_STATUS                                         198H


#
# vmx MSR registers
#
.equ IA32_VMX_BASIC                                          480H
.equ IA32_VMX_PINBASED_CTS                                   481H
.equ IA32_VMX_PROCBASED_CTLS                                 482H
.equ IA32_VMX_EXIT_CTLS                                      483H
.equ IA32_VMX_ENTRY_CTLS                                     484H
.equ IA32_VMX_MISC                                           485H
.equ IA32_VMX_CR0_FIXED0                                     486H
.equ IA32_VMX_CR0_FIXED1                                     487H
.equ IA32_VMX_CR4_FIXED0                                     488H
.equ IA32_VMX_CR4_FIXED1                                     489H
.equ IA32_VMX_VMCS_ENUM                                      48AH
.equ IA32_VMX_PROCBASED_CTLS2                                48BH




# ########## define some macro ##############


# ---------------------------------------------
# macro: FAST_A20_ENABLE
# description:
#                 set 0x92[1] to enable A20 line
#----------------------------------------------
.macro FAST_A20_ENABLE        0
        in al, SYSTEM_CONTROL_PORTA                                # port 0x92
        or al, 0x02                                                # set A20 bit
        out SYSTEM_CONTROL_PORTA, al
.endmacro


#------------------------------------------------
# macro: NMI_DISABLE
# description:
#                设置 NMI_EN 寄存器的 bit 7 为 1
#------------------------------------------------
.macro NMI_DISABLE 0
        in al, NMI_EN_PORT                                        # port 0x70
        or al, 0x80                                               # disable all NMI source
        out NMI_EN_PORT, al
.endmacro

#------------------------------------------------
# macro: NMI_ENABLE
# description:
#                set 0x70[7] to 0
#------------------------------------------------
.macro NMI_ENABLE 0
        in al, NMI_EN_PORT                                        # port 0x70
        and al, 0x7f                                                # enable NMI source
        out NMI_EN_PORT, al
.endmacro


#-----------------------------------------------
# macro: RESET_CPU
# description:
#                引发处理器产生 RESET# 信号
#-----------------------------------------------
.macro RESET_CPU 0
        mov dx, RESET_CONTROL_REGISTER                         # reset 控制寄存器在端口 0CF9H
        in al, dx
        or al, 3                                                # 执行硬件 RESET 动作
        out dx, al
.endmacro


#---------------------------------------------
# macro INIT_CPU
# description:
#                引发处理器产生 INIT# 信号
#----------------------------------------------
.macro INIT_CPU 0
        mov dx, FAST_A20_INIT_REGISTER                        # port 92
        in al, dx
        or al, 1                                              # INIT_NOW
        out dx, al
.endmacro


.macro DELAY 0
        mov ecx, 0xffff
%%L1:        dec ecx        
        jnz %%L1        
.endmacro


.macro DELAY1 0
        mov ecx, 0xffffffff
%%L1:        dec ecx        
        jnz %%L1        
.endmacro





