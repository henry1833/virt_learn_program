#ifndef __VMX_INSN_H
#define __VMX_INSN_H

static __always_inline u16 vmcs_read16(unsigned long field)
{
}

static __always_inline u32 vmcs_read32(unsigned long field)
{
}

static __always_inline u64 vmcs_read64(unsigned long field)
{
}

static __always_inline unsigned long vmcs_readl(unsigned long field)
{
}

static __always_inline void vmcs_write16(unsigned long field, u16 value)
{
}

static __always_inline void vmcs_write32(unsigned long field, u16 value)
{
}
	
static __always_inline void vmcs_write64(unsigned long field, u16 value)
{
}
	
static __always_inline void vmcs_writel(unsigned long field, u16 value)
{
}
	
#endif