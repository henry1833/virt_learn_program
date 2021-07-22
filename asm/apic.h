
#
# define APIC_BASE  linear address
#
.equ APIC_BASE					  ,           0x800000
									          
.equ APIC_ID					  ,           0x20
.equ APIC_VER					  ,           0x30
.equ TPR						  ,           0x80
.equ APR						  ,           0x90
.equ PPR						  ,           0xA0
.equ EOI						  ,           0xB0
.equ RRD						  ,           0xC0
.equ LDR						  ,           0xD0
.equ DFR						  ,           0xE0
.equ SVR						  ,           0xF0
.equ ISR0						  ,           0x100
.equ ISR1						  ,           0x110
.equ ISR2						  ,           0x120
.equ ISR3						  ,           0x130
.equ ISR4						  ,           0x140
.equ ISR5						  ,           0x150
.equ ISR6						  ,           0x160
.equ ISR7						  ,           0x170
.equ TMR0						  ,           0x180
.equ TMR1						  ,           0x190
.equ TMR2						  ,           0x1A0
.equ TMR3						  ,           0x1B0
.equ TMR4						  ,           0x1C0
.equ TMR5						  ,           0x1D0
.equ TMR6						  ,           0x1E0
.equ TMR7						  ,           0x1F0
.equ IRR0						  ,           0x200
.equ IRR1						  ,           0x210
.equ IRR2						  ,           0x220
.equ IRR3						  ,           0x230
.equ IRR4						  ,           0x240
.equ IRR5						  ,           0x250
.equ IRR6						  ,           0x260
.equ IRR7						  ,           0x270
.equ ESR						  ,           0x280
.equ ICR0						  ,           0x300
.equ ICR1						  ,           0x310
.equ LVT_CMCI					  ,           0x2F0
.equ LVT_TIMER					  ,           0x320
.equ LVT_THERMAL				  ,	          0x330
.equ LVT_PERFMON				  ,	          0x340
.equ LVT_LINT0					  ,           0x350
.equ LVT_LINT1					  ,           0x360
.equ LVT_ERROR					  ,           0x370
.equ TIMER_ICR					  ,           0x380
.equ TIMER_CCR					  ,           0x390
.equ TIMER_DCR					  ,           0x3E0



# define APIC timer value
.equ TIMER_ONE_SHOT				  ,           0x00000
.equ TIMER_PERIODIC				  ,           0x20000
.equ TIMER_TSC				      ,           0x40000

# define delivery mode value
.equ FIXED                        ,           0x0000
.equ LOWEST                       ,           0x0100
.equ SMI                          ,           0x0200
.equ NMI                          ,           0x0400
.equ EXTINT                       ,           0x0700
.equ INIT                         ,           0x0500
.equ FIXED_DELIVERY				  ,           0x0000
.equ LOWEST_DELIVERY              ,           0x0100
.equ SMI_DELIVERY                 ,           0x0200
.equ NMI_DELIVERY                 ,           0x0400
.equ EXTINT_DELIVERY              ,           0x0700
.equ INIT_DELIVERY                ,           0x0500


.equ MASKED					      ,           0x10000
.equ LOGICAL					  ,           0x0800
.equ PHYSICAL				      ,           0x0000
.equ EDGE					      ,           0x0000
.equ ASSERT                       ,           0x4000
.equ LEVEL					      ,           0x8000
.equ NOSHORTHAND				  ,           0x00000
.equ SELF					      ,           0x40000
.equ ALL_IN_SELF				  ,           0x80000
.equ ALL_EX_SELF				  ,           0x0C0000


#define IPI messages

.equ STARTUP_IPI                  ,           0x000C4600
.equ INIT_IPI                     ,           0x000C4500

.equ PHYSICAL_ID                  ,           (ASSERT | EDGE | PHYSICAL | NOSHORTHAND | FIXED)
.equ LOGICAL_ID                   ,           (ASSERT | EDGE | LOGICAL | NOSHORTHAND | FIXED)
