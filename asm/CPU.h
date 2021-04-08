
#
# define opcode
#
.equ REX_W         ,          0x48
.equ O16           ,          0x66
.equ O32           ,          0x66
.equ O64           ,          0x48
.equ A16           ,          0x67
.equ A32           ,          0x67

.equ iret64        ,           iretq
.equ retf64        ,          0x0CB48
#.equ sysexit64     ,          .byte 0x48, 0x0F, 0x35
#.equ sysret64      ,          .byte 0x48h, 0x0Fh, 0x07


# define lea rax, [rip] instruction encode
#.equ GET_RIP       ,          .byte 48h, 8Dh, 05h, 00h, 00h, 00h, 00h



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

# Fixed-rang MTRR ¡¡
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
# general-purpose counter ¼Ä´æÆ÷
.equ IA32_PERFEVTSEL0               ,                            0x186
.equ IA32_PERFEVTSEL1               ,                            0x187
.equ IA32_PERFEVTSEL2               ,                            0x188
.equ IA32_PERFEVTSEL3               ,                            0x189
.equ IA32_PMC0                      ,                            0x0C1
.equ IA32_PMC1                      ,                            0x0C2
.equ IA32_PMC2                      ,                            0x0C3
.equ IA32_PMC3                      ,                            0x0C4

# fixed-counter register            
.equ IA32_FIXED_CTR0                ,                            0x309
.equ MSR_PERF_FIXED_CTR0            ,                            0x309
.equ IA32_FIXED_CTR1                ,                            0x30A
.equ MSR_PERF_FIXED_CTR1            ,                            0x30A
.equ IA32_FIXED_CTR2                ,                            0x30B
.equ MSR_PERF_FIXED_CTR2            ,                            0x30B

.equ IA32_FIXED_CTR_CTRL            ,                            0x38D
.equ MSR_PERF_FIXED_CTR_CTRL        ,                            0x38D

.equ IA32_PERF_STATUS               ,                            0x198
.equ IA32_PERF_CTL                  ,                            0x199

.equ IA32_PERF_CAPABILITIES         ,                            0x345
.equ IA32_PERF_GLOBAL_STATUS        ,                            0x38E
.equ MSR_PERF_GLOBAL_STATUS         ,                            0x38E
.equ MSR_PERF_GLOBAL_CTRL           ,                            0x38F
.equ IA32_PERF_GLOBAL_CTRL          ,                            0x38F
.equ IA32_PERF_GLOBAL_OVF_CTRL      ,                            0x390
.equ MSR_PERF_GLOBAL_OVF_CTRL       ,                            0x390

.equ MSR_PEBS_LD_LAT                ,                            0x3F6

.equ IA32_DS_AREA                   ,                            0x600
.equ IA32_TSC_DEADLINE              ,                            0x6E0



# Nehalem  TSC registers
.equ IA32_TSC_AUX                   ,                             0x0C0000103


.equ MSR_FSB_FREQ                   ,                         0x0CD
.equ IA32_MPERF                     ,                         0x0E7
.equ IA32_APERF                     ,                         0x0E8
.equ IA32_PLATFORM_ID               ,                         0x17
.equ MSR_PLATFORM_ID                ,                         0x17
.equ MSR_PERF_STATUS                ,                         0x198


#
# vmx MSR registers
#
.equ IA32_VMX_BASIC                  ,                        0x480
.equ IA32_VMX_PINBASED_CTS           ,                        0x481
.equ IA32_VMX_PROCBASED_CTLS         ,                        0x482
.equ IA32_VMX_EXIT_CTLS              ,                        0x483
.equ IA32_VMX_ENTRY_CTLS             ,                        0x484
.equ IA32_VMX_MISC                   ,                        0x485
.equ IA32_VMX_CR0_FIXED0             ,                        0x486
.equ IA32_VMX_CR0_FIXED1             ,                        0x487
.equ IA32_VMX_CR4_FIXED0             ,                        0x488
.equ IA32_VMX_CR4_FIXED1             ,                        0x489
.equ IA32_VMX_VMCS_ENUM              ,                        0x48A
.equ IA32_VMX_PROCBASED_CTLS2        ,                        0x48B




# ########## define some macro ##############


# ---------------------------------------------
# macro: FAST_A20_ENABLE
# description:
#                 set 0x92[1] to enable A20 line
#----------------------------------------------
.macro FAST_A20_ENABLE      
        in $SYSTEM_CONTROL_PORTA,%al                                # port 0x92
        or $0x02,%al                                                # set A20 bit
        out %al, $SYSTEM_CONTROL_PORTA
.endm


#------------------------------------------------
# macro: NMI_DISABLE
# description:
#                set NMI_EN register bit 7 as 1
#------------------------------------------------
.macro NMI_DISABLE
        in $NMI_EN_PORT,%al                                        # port 0x70
        or $0x80,%al                                               # disable all NMI source
        out %al,$NMI_EN_PORT
.endm

#------------------------------------------------
# macro: NMI_ENABLE
# description:
#                set 0x70[7] to 0
#------------------------------------------------
.macro NMI_ENABLE 
        in $NMI_EN_PORT,%al                                        # port 0x70
        and $0x7f,%al                                                # enable NMI source
        out %al,$NMI_EN_PORT
.endm


#-----------------------------------------------
# macro: RESET_CPU
# description:
#                Generate RESET# signal
#-----------------------------------------------
.macro RESET_CPU
        mov $RESET_CONTROL_REGISTER,%dx                         # reset control register port 0CF9H
        in %dx,%al
        or $0x3,%al                                                #
        out %al,%dx
.endm


#---------------------------------------------
# macro INIT_CPU
# description:
#                Generate INIT# signal
#----------------------------------------------
.macro INIT_CPU
        mov FAST_A20_INIT_REGISTER,%dx                        # port 92
        in %dx, %al
        or $0x1,%al                                              # INIT_NOW
        out %al,%dx
.endm


.macro DELAY
        mov $0xffff,%ecx
%%L1:   dec %ecx        
        jnz %%L1        
.endm


.macro DELAY1
        mov $0xffffffff, %ecx
%%L1:   dec %ecx        
        jnz %%L1        
.endm





