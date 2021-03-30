
#
# 定义 opcode
#
.equ REX_W         ,         DB 0x48
.equ O16           ,         DB 0x66
.equ O32           ,         DB 0x66
.equ O64           ,         DB 0x48
.equ A16           ,         DB 0x67
.equ A32           ,         DB 0x67

.equ iret64        ,         iretq
.equ retf64        ,         DW 0x0CB48
.equ sysexit64     ,         DB 48h, 0Fh, 35h
.equ sysret64      ,         DB 48h, 0Fh, 07h


# 定义 lea rax, [rip] 指令的 encode
# 因为: nasm 语言层上不支持 [rip] 寻址
.equ GET_RIP                 DB 48h, 8Dh, 05h, 00h, 00h, 00h, 00h



# CR0 寄存器控制位
.equ PE_BIT       ,          0
.equ MP_BIT       ,          1
.equ EM_BIT       ,          2
.equ TS_BIT       ,          3
.equ ET_BIT       ,          4
.equ NE_BIT       ,          5
.equ WP_BIT       ,          16
.equ AM_BIT       ,          18
.equ NW_BIT       ,          29
.equ CD_BIT       ,          30
.equ PG_BIT       ,          31

#CR4 寄存器控制位
.equ WME_BIT                 0
.equ PVI_BIT                 1
.equ TSD_BIT                 2
.equ DE_BIT                  3
.equ PSE_BIT                 4
.equ PAE_BIT                 5
.equ MCE_BIT                 6
.equ PGE_BIT                 7
.equ PCE_BIT                 8
.equ OSFXSR_BIT              9
.equ OSXMMEXCPT_BIT          10
.equ VMXE_BIT                13
.equ SMXE_BIT                14
.equ PCIDE_BIT               17
.equ OSXSAVE_BIT             18
.equ SMEP_BIT                20

# SMM 模式的基地址
.equ SMBASE                  30000H


.equ IA32_APIC_BASE                  1BH
.equ IA32_FEATURE_CONTROL            3AH
.equ IA32_SMM_MONITOR_CTL            9BH
.equ IA32_MISC_ENABLE                1A0H
.equ IA32_CPU_DCA_CAP                1F9H
.equ IA32_MONITOR_FILTER_LINE_SIZE   06H
.equ IA32_TIME_STAMP_COUNTER         10H
.equ IA32_TSC_COUNTER                10H


#***** 定义 MTRR 寄存器的地址 ******

#下面是 Fixed-rang MTRR 寄存器的定义　
.equ IA32_MTRR_FIX64K_00000                                       250H
.equ IA32_MTRR_FIX16K_80000                                       258H
.equ IA32_MTRR_FIX16K_A0000                                       259H
.equ IA32_MTRR_FIX4K_C0000                                        268H
.equ IA32_MTRR_FIX4K_C8000                                        269H
.equ IA32_MTRR_FIX4K_D0000                                        26AH
.equ IA32_MTRR_FIX4K_D8000                                        26BH
.equ IA32_MTRR_FIX4K_E0000                                        26CH
.equ IA32_MTRR_FIX4K_E8000                                        26DH
.equ IA32_MTRR_FIX4K_F0000                                        26EH
.equ IA32_MTRR_FIX4K_F8000                                        26FH

# 下面是 MTRR 的功能寄存器定义
.equ IA32_MTRRCAP                                                 0FEH
.equ IA32_MTRR_DEF_TYPE                                           2FFH

# 下面定义 MTRR 的 PHYSBASE/PHYSMASK 寄存器
.equ IA32_MTRR_PHYSBASE0                                        200H
.equ IA32_MTRR_PHYSMASK0                                        201H
.equ IA32_MTRR_PHYSBASE1                                        202H
.equ IA32_MTRR_PHYSMASK1                                        203H
.equ IA32_MTRR_PHYSBASE2                                        204H
.equ IA32_MTRR_PHYSMASK2                                        205H
.equ IA32_MTRR_PHYSBASE3                                        206H
.equ IA32_MTRR_PHYSMASK3                                        207H
.equ IA32_MTRR_PHYSBASE4                                        208H
.equ IA32_MTRR_PHYSMASK4                                        209H
.equ IA32_MTRR_PHYSBASE5                                        20AH
.equ IA32_MTRR_PHYSMASK5                                        20BH
.equ IA32_MTRR_PHYSBASE6                                        20CH
.equ IA32_MTRR_PHYSMASK6                                        20DH
.equ IA32_MTRR_PHYSBASE7                                        20EH
.equ IA32_MTRR_PHYSMASK7                                        20FH
.equ IA32_MTRR_PHYSBASE8                                        210H
.equ IA32_MTRR_PHYSMASK8                                        211H
.equ IA32_MTRR_PHYSBASE9                                        212H
.equ IA32_MTRR_PHYSMASK9                                        213H

# 下面是 SMRR 寄存器
.equ IA32_SMRR_PHYSBASE                                         1F2H
.equ IA32_SMRR_PHYSMASK                                         1F3H

# 下面是 PAT 寄存器
.equ IA32_PAT                                                   277H


# 下面是对特殊指令提供支持的 MSR 寄存器定义
.equ IA32_SYSENTER_CS                                           174H
.equ IA32_SYSENTER_ESP                                          175H
.equ IA32_SYSENTER_EIP                                          176H


# 下面是 x64 体系的支持
.equ IA32_EFER                                                  0C0000080H
.equ IA32_STAR                                                  0C0000081H
.equ IA32_LSTAR                                                 0C0000082H
.equ IA32_FMASK                                                 0C0000084H
.equ IA32_FS_BASE                                               0C0000100H
.equ IA32_GS_BASE                                               0C0000101H
.equ IA32_KERNEL_GS_BASE                                        0C0000102H



# ##### 为 AMD 机器定义 #######
.equ MSR_SMI_ON_IO_TRAP0                                        0C0010050H
.equ MSR_SMI_ON_IO_TRAP1                                        0C0010051H
.equ MSR_SMI_ON_IO_TRAP2                                        0C0010052H
.equ MSR_SMI_ON_IO_TRAP3                                        0C0010053H
.equ MSR_SMI_ON_IO_TRAP_CTL_STS                                 0C0010054H


# 与 dubug/performance monitoring 相关的 MSRs

.equ IA32_PEBS_ENABLE                                           3F1H
# general-purpose counter 寄存器
.equ IA32_PERFEVTSEL0                                           186H
.equ IA32_PERFEVTSEL1                                           187H
.equ IA32_PERFEVTSEL2                                           188H
.equ IA32_PERFEVTSEL3                                           189H
.equ IA32_PMC0                                                  0C1H
.equ IA32_PMC1                                                  0C2H
.equ IA32_PMC2                                                  0C3H
.equ IA32_PMC3                                                  0C4H

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
# 下面是关于 VMX 的 MSR 
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




# ########## 关于 CPU 的一些宏定义 ##############


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





