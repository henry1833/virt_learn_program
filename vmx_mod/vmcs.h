#ifndef __VMX_H__
#define __VMX_H__

#include<asm/svm.h>
#include<asm/desc.h>

#define GD_TSS          0x38
#define GD_TSS2         0x40
#define NR_GDT_ENTRIES  9

#define STA_A (_UL(1) << 0)        /* Accessed */
#define STA_W (_UL(1) << 1)        /* Writable (for data segments) */
#define STA_E (_UL(1) << 2)        /* Expand down (for data segments) */
#define STA_X (_UL(1) << 3)        /* 1 = Code segment (executable) */
#define STA_R (_UL(1) << 1)        /* Readable (for code segments) */
#define STA_C (_UL(1) << 2)        /* Conforming (for code segments) */

#define SEG_A (STA_A << 40)     /* Accessed */
#define SEG_W (STA_W << 40)     /* Writable (for data segments) */
#define SEG_E (STA_E << 40)     /* Expand down (for data segments) */
#define SEG_X (STA_X << 40)     /* 1 = Code segment (executable) */
#define SEG_R (STA_R << 40)     /* Readable (for code segments) */
#define SEG_C (STA_C << 40)     /* Conforming (for code segments) */

#define SEG_S (_UL(1) << 44)       /* 1 = non-system, 0 = system segment */

#define SEG_DPL(x) (((x) & _UL(3)) << 45) /* Descriptor privilege level */
#define SEG_P (_UL(1) << 47)       /* Present */
#define SEG_L (_UL(1) << 53)       /* Long mode */
#define SEG_D (_UL(1) << 54)       /* 1 = 32-bit in legacy, 0 in long mode */
#define SEG_G (_UL(1) << 55)       /* Granulatity: 1 = scale limit by 4K */


#define SEG_LIM(x) (((x) & 0xffff) | ((x) & _UL(0xf0000)) << 32)


#define SEG64(type, dpl)                                                \
  ((type) | SEG_S | SEG_P | SEG_G | SEG_L | SEG_A | SEG_DPL (dpl)       \
   | SEG_LIM (0xffffffff))
        



uint64_t gdt_template[NR_GDT_ENTRIES] = {
        0,
        0,
        SEG64(SEG_X | SEG_R, 0),
        SEG64(SEG_W, 0),
        0,
        SEG64(SEG_W, 3),
        SEG64(SEG_X | SEG_R, 3),
        0,
        0,
};


static const u32 svm_msr_index[] = {
#ifdef CONFIG_X86_64
        MSR_STAR, MSR_LSTAR, MSR_CSTAR, MSR_SYSCALL_MASK, MSR_KERNEL_GS_BASE,
        MSR_FS_BASE,
#endif
        MSR_IA32_SYSENTER_CS, MSR_IA32_SYSENTER_ESP, MSR_IA32_SYSENTER_EIP,
        MSR_TSC_AUX,
};

#define NR_SVM_MSR ARRAY_SIZE(svm_msr_index)


struct tptr {
        uint16_t        limit;
        uint64_t        base;
} __attribute__((packed));


enum x86_reg {
        VCPU_REGS_RAX = 0,
        VCPU_REGS_RCX = 1,
        VCPU_REGS_RDX = 2,
        VCPU_REGS_RBX = 3,
        VCPU_REGS_RSP = 4,
        VCPU_REGS_RBP = 5,
        VCPU_REGS_RSI = 6,
        VCPU_REGS_RDI = 7, 
#ifdef CONFIG_X86_64
        VCPU_REGS_R8 = 8,
        VCPU_REGS_R9 = 9,
        VCPU_REGS_R10 = 10,
        VCPU_REGS_R11 = 11,
        VCPU_REGS_R12 = 12,
        VCPU_REGS_R13 = 13,
        VCPU_REGS_R14 = 14,
        VCPU_REGS_R15 = 15,
#endif
        VCPU_REGS_RIP,
        NR_REGS
};


struct svm_vcpu{

    int asid;    
 
    u64 regs[NR_REGS];
    struct {
        u16 fs;
        u16 gs;
        u16 ldt;
        u64 gs_base;
    } host_state;
   
    u64 host_msrs[NR_SVM_MSR];
    u64 guest_msrs[NR_SVM_MSR];

    unsigned long cr2;
    unsigned long vmcb_pa;
    struct vmcb *vmcb;    
    
    u32 *msrpm;
    u32 *io_bitmap;
    
    struct  gdt_page *gdtr;

};

struct segment{
    __u64 base;
    __u32 limit;
    __u16 selector;
    __u8  type;
    __u8  present;
    __u8  dpl;
    __u8  db;
    __u8  s;
    __u8  l;
    __u8  g;
    __u8  avl;
    __u8  unusable;
    __u8  padding;
};

struct descriptor {
    __u64  base;
    __u16  limit;
    __u16  padding[3];
};

struct user_regs {
        __u64   rax;
        __u64   rbx;
        __u64   rcx;
        __u64   rdx;
        __u64   rsi;
        __u64   rdi;
        __u64   rsp;
        __u64   rbp;
        __u64   r8;
        __u64   r9;
        __u64   r10;
        __u64   r11;
        __u64   r12;
        __u64   r13;
        __u64   r14;
        __u64   r15;
        __u64   rip;
        __u64   rflags; 
} __attribute__((packed));

#define SVM_NR_INTERRUPTS 256

struct sys_regs {
        struct segment  cs;     
        struct segment  ds;
        struct segment  es;
        struct segment  fs;
        struct segment  gs;
        struct segment  ss;
        struct segment  tr;
        struct segment  ldt;
        struct descriptor       gdt;
        struct descriptor       idt;
        __u64   cr0;
        __u64   cr2; 
        __u64   cr3;
        __u64   cr4;
        __u64   cr8;
        __u64   efer;
        __u64   apic_base;
        __u64   interrupt_bitmap[(SVM_NR_INTERRUPTS + 63) / 64];
} __attribute__((packed));


struct svm_config {
      struct user_regs svm_user_regs;
      struct sys_regs  svm_sys_regs;
};


static inline void set_intercept(struct svm_vcpu *svm, int bit)
{
	struct vmcb *vmcb = svm->vmcb;
	
	vmcb->control.intercept |= (1ULL << bit);
}

static inline void clr_intercept(struct svm_vcpu *svm, int bit)
{
	struct vmcb *vmcb = svm->vmcb;
	
	vmcb->control.intercept &= ~(1ULL << bit);
}

u64 cr0(void)
{
    return (X86_CR0_PE | X86_CR0_PG | X86_CR0_ET | X86_CR0_NE);
}

u64 cr4(void)
{
    u64 cr4;

    cr4 = (X86_CR4_PAE | X86_CR4_PGE | X86_CR4_OSFXSR | X86_CR4_OSXMMEXCPT |X86_CR4_FSGSBASE |X86_CR4_OSXSAVE);

    return cr4;
}

u64 efer(void)
{ 
    return  EFER_NX | EFER_SVME | EFER_LME | EFER_LMA | EFER_SCE | EFER_FFXSR;
}

static inline u32 svm_segment_access_rights(struct segment *var)
{
	u32 ar;
	if(var->unusable || !var->present)
	    ar = 1 << 16;
	else {
	    ar = var->type & SVM_SELECTOR_TYPE_MASK;
	    ar |= (var->s & 1) << SVM_SELECTOR_S_SHIFT;
	    ar |= (var->s & 3) << SVM_SELECTOR_DPL_SHIFT;
            ar |= (var->present & 1) << SVM_SELECTOR_P_SHIFT;
	    ar |= (var->avl & 1) << SVM_SELECTOR_AVL_SHIFT;
	    ar |= (var->l & 1) << SVM_SELECTOR_L_SHIFT;
	    ar |= (var->db & 1) << SVM_SELECTOR_DB_SHIFT;
	    ar |= (var->g & 1) << SVM_SELECTOR_G_SHIFT;
	}

	return ar;
}

static inline void clgi(void)
{
        asm volatile (SVM_CLGI);
}

static inline void stgi(void)
{
        asm volatile (SVM_STGI);
}

static inline u16 svm_read_ldt(void)
{
    u16 ldt;
    asm("sldt %0" : "=g"(ldt));
    return ldt;
}

static inline void svm_load_ldt(u16 sel)
{
    asm("lldt %0" : : "rm"(sel));
}

static inline u64 read_host_rsp(void)
{
    u64 rsp;
    asm("mov %%rsp, %0":"=r" (rsp) : : "memory");
    return rsp;    
}

#endif
