#ifndef __VMX_INSN_H
#define __VMX_INSN_H

#include<asm/vmx.h>

#define asm_volatile_goto(x...) do { asm goto(x); asm (""); } while (0)

static __always_inline unsigned long __vmcs_readl(unsigned long field)
{
	unsigned long value;
	    
    asm volatile("vmread %1, %0\n\t"
                :"=a"(value) : "d"(field): "cc");
	
	return value;
}

static __always_inline u16 vmcs_read16(unsigned long field)
{
	return __vmcs_readl(field);
}

static __always_inline u32 vmcs_read32(unsigned long field)
{
	return __vmcs_readl(field);
}

static __always_inline u64 vmcs_read64(unsigned long field)
{
#ifdef CONFIG_X86_64
    return __vmcs_readl(field);
#else
	return __vmcs_readl(field) | ((u64)__vmcs_readl(field+1)<<32);
#endif
}


static __always_inline unsigned long vmcs_readl(unsigned long field)
{
	return __vmcs_readl(field);
}


static __always_inline unsigned long __vmcs_writel(unsigned long field,unsigned long value)
{
	u8 error;
	
    asm volatile("vmwrite %1,%2\n\t"
				 "setna %0\n\t"
	             : "=q"(error) : "a"(value), "d"(field): "cc");
	
	if(unlikely(error))
        vmwrite_error(field,value);
}

static __always_inline void vmcs_write16(unsigned long field, u16 value)
{
	__vmcs_writel(field,value);
}

static __always_inline void vmcs_write32(unsigned long field, u16 value)
{
	__vmcs_writel(field,value);
}
	
static __always_inline void vmcs_write64(unsigned long field, u16 value)
{
	__vmcs_writel(field,value);
}
	
static __always_inline void vmcs_writel(unsigned long field, u16 value)
{
	__vmcs_writel(field,value);
}

static inline void vmcs_clear(struct vmcs *vmcs)
{
	u64 phys_addr = __pa(vmcs);
	u8 error;
	
	asm volatile("vmclear %1\n\t"
				 "setna %0  \n\t"
	            : "=qm"(error): "m"(phys_addr): "cc");
	if(error)
        printk(KERN_ERR"vmx: vmclear %p/%llx failed\n",
                   vmcs,phys_addr);    
}
	
static inline void vmcs_load(struct vmcs *vmcs)
{
	u64 phys_addr = __pa(vmcs);
	u8 error;
	
	asm volatile("vmptrld %1 \n\t"
				 "setna %0 \n\t"
	             : "=qm"(error): "m"(phys_addr): "cc");
	
	if(error)
        printk(KERN_ERR"vmx: vmptrld %p/%llx failed\n",
                   vmcs,phys_addr);
}

static inline void __invvpid(unsigned long ext, u16 vpid, gva_t gva)
{
	struct {
		u64 vpid:16;
		u64 rsvd:48;
		u64 gva;
	} operand = {vpid, 0, gva};
	
	asm_volatile_goto("invvpid %1,%0 \n\t"
		              ".byte  0x2e \n\t"
					  "jna %l[error] \n\t"
	                 : :"r"(ext), "m"(operand) : "cc": error);
    return;
error:
    printk(KERN_ERR"vmx: invvpid failed: ext=0x%lx vpid=%llx gpa=0x%llx\n",
                   ext, vpid, gva);	

}

static inline void __invept(unsigned long ext, u64 eptp, gpa_t gpa)	
{
    struct {
	    u64 eptp,gpa;
	} operand ={eptp,pga};
	
	asm_volatile_goto("invept %1, %0 \n\t"
					  ".byte 0x2e \n\t"
					  "jna %l[error] \n\t"
	                 : :"r"(ext), "m"(operand) : "cc": error);
	
	return;
error:
    printk(KERN_ERR"vmx: invept failed: ext=0x%lx eptp=%llx gpa=0x%llx\n",
                  ext, eptp, gva);
}

static inline void vpid_sync_vcpu_single(int vpid)
{
	if(vpid == 0)
		return;
	
	__invvpid(VMX_VPID_EXTENT_SINGLE_CONTEXT, vpid, 0);
}

static inline void vpid_sync_vcpu_global(void)
{
	__invvpid(VMX_VPID_EXTENT_ALL_CONTEXT,0,0);
}

static inline void ept_sync_global(void)
{
	__invept(VMX_EPT_EXTENT_ GLOBAL, 0, 0);
}


#endif
