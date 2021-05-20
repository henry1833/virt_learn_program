#include<linux/init.h>
#include<linux/module.h>
#include<linux/rbtree.h>
#include<linux/mm.h>
#include<linux/fs.h>
#include<linux/kernel.h>
#include<linux/list.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/mman.h>
#include<linux/cpu.h>
#include<asm/desc.h>
#include<asm/vmx.h>
#include<asm/special_insns.h>
#include<asm/msr-index.h>

#include "vmcs.h"

static DEFINE_PER_CPU(struct vmcs *,vmxarea);


static noinline void vmwrite_error(unsigned long field, unsigned long value)
{
        printk(KERN_ERR"vmwrite error: reg %lx value %lx (err %d)",
                   field, value, vmcs_read32(VM_INSTRUCTION_ERROR));
        dump_stack(); 
}



static void vmx_dump_sel(char *name,uint32_t sel)
{
	printk("%s sel=0x%04x, attr=0x%05x, limit=0x%08x, base=0x%016lx\n",
	       name,vmcs_read16(sel),
	       vmcs_read32(sel + GUEST_ES_AR_BYTES - GUEST_ES_SELECTOR),
	       vmcs_read32(sel + GUEST_ES_LIMIT - GUEST_ES_SELECTOR),
	       vmcs_readl(sel + GUEST_ES_BASE - GUEST_ES_SELECTOR));
}

static void vmx_dump_dtsel(char *name,uint32_t limit)
{
	printk("%s                           limit=0x%08x, base=0x%016lx\n",
	      name,vmcs_read32(limit),
	      vmcs_readl(limit + GUEST_GDTR_BASE - GUEST_GDTR_LIMIT));
}

/**
 *  * svm_dump_cpu - prints the CPU state
 *   * @vcpu: VCPU to print
 */
static void vmx_dump_cpu(struct vmx_vcpu *vcpu)
{
        u32 vmentry_ctl, vmexit_ctl;
        u32 cpu_based_exec_ctrl, pin_based_exec_ctrl, secondary_exec_control;
        unsigned long cr4;
        u64 efer;

        if (!dump_invalid_vmcs) {
                pr_warn_ratelimited("set kvm_intel.dump_invalid_vmcs=1 to dump internal KVM state.\n");
                return;
        }

        vmentry_ctl = vmcs_read32(VM_ENTRY_CONTROLS);
        vmexit_ctl = vmcs_read32(VM_EXIT_CONTROLS);
        cpu_based_exec_ctrl = vmcs_read32(CPU_BASED_VM_EXEC_CONTROL);
        pin_based_exec_ctrl = vmcs_read32(PIN_BASED_VM_EXEC_CONTROL);
        cr4 = vmcs_readl(GUEST_CR4);
        efer = vmcs_read64(GUEST_IA32_EFER);
        secondary_exec_control = 0;
        if (cpu_has_secondary_exec_ctrls())
                secondary_exec_control = vmcs_read32(SECONDARY_VM_EXEC_CONTROL);

        printk("*** Guest State ***\n");
        printk("CR0: actual=0x%016lx, shadow=0x%016lx, gh_mask=%016lx\n",
               vmcs_readl(GUEST_CR0), vmcs_readl(CR0_READ_SHADOW),
               vmcs_readl(CR0_GUEST_HOST_MASK));
        printk("CR4: actual=0x%016lx, shadow=0x%016lx, gh_mask=%016lx\n",
               cr4, vmcs_readl(CR4_READ_SHADOW), vmcs_readl(CR4_GUEST_HOST_MASK));
        printk("CR3 = 0x%016lx\n", vmcs_readl(GUEST_CR3));
        if ((secondary_exec_control & SECONDARY_EXEC_ENABLE_EPT) &&
            (cr4 & X86_CR4_PAE) && !(efer & EFER_LMA))
        {
                printk("PDPTR0 = 0x%016llx  PDPTR1 = 0x%016llx\n",
                       vmcs_read64(GUEST_PDPTR0), vmcs_read64(GUEST_PDPTR1));
                printk("PDPTR2 = 0x%016llx  PDPTR3 = 0x%016llx\n",
                       vmcs_read64(GUEST_PDPTR2), vmcs_read64(GUEST_PDPTR3));
        }
        printk("RSP = 0x%016lx  RIP = 0x%016lx\n",
               vmcs_readl(GUEST_RSP), vmcs_readl(GUEST_RIP));
        printk("RFLAGS=0x%08lx         DR7 = 0x%016lx\n",
               vmcs_readl(GUEST_RFLAGS), vmcs_readl(GUEST_DR7));
        printk("Sysenter RSP=%016lx CS:RIP=%04x:%016lx\n",
               vmcs_readl(GUEST_SYSENTER_ESP),
               vmcs_read32(GUEST_SYSENTER_CS), vmcs_readl(GUEST_SYSENTER_EIP));
        vmx_dump_sel("CS:  ", GUEST_CS_SELECTOR);
        vmx_dump_sel("DS:  ", GUEST_DS_SELECTOR);
        vmx_dump_sel("SS:  ", GUEST_SS_SELECTOR);
        vmx_dump_sel("ES:  ", GUEST_ES_SELECTOR);
        vmx_dump_sel("FS:  ", GUEST_FS_SELECTOR);
        vmx_dump_sel("GS:  ", GUEST_GS_SELECTOR);
        vmx_dump_dtsel("GDTR:", GUEST_GDTR_LIMIT);
        vmx_dump_sel("LDTR:", GUEST_LDTR_SELECTOR);
        vmx_dump_dtsel("IDTR:", GUEST_IDTR_LIMIT);
        vmx_dump_sel("TR:  ", GUEST_TR_SELECTOR);
        if ((vmexit_ctl & (VM_EXIT_SAVE_IA32_PAT | VM_EXIT_SAVE_IA32_EFER)) ||
            (vmentry_ctl & (VM_ENTRY_LOAD_IA32_PAT | VM_ENTRY_LOAD_IA32_EFER)))
                printk("EFER =     0x%016llx  PAT = 0x%016llx\n",
                       efer, vmcs_read64(GUEST_IA32_PAT));
        printk("DebugCtl = 0x%016llx  DebugExceptions = 0x%016lx\n",
               vmcs_read64(GUEST_IA32_DEBUGCTL),
               vmcs_readl(GUEST_PENDING_DBG_EXCEPTIONS));
        if (cpu_has_load_perf_global_ctrl() &&
            vmentry_ctl & VM_ENTRY_LOAD_IA32_PERF_GLOBAL_CTRL)
                printk("PerfGlobCtl = 0x%016llx\n",
                       vmcs_read64(GUEST_IA32_PERF_GLOBAL_CTRL));
        if (vmentry_ctl & VM_ENTRY_LOAD_BNDCFGS)
                printk("BndCfgS = 0x%016llx\n", vmcs_read64(GUEST_BNDCFGS));
        printk("Interruptibility = %08x  ActivityState = %08x\n",
               vmcs_read32(GUEST_INTERRUPTIBILITY_INFO),
               vmcs_read32(GUEST_ACTIVITY_STATE));
        if (secondary_exec_control & SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY)
                printk("InterruptStatus = %04x\n",
                       vmcs_read16(GUEST_INTR_STATUS));

        printk("*** Host State ***\n");
        printk("RIP = 0x%016lx  RSP = 0x%016lx\n",
               vmcs_readl(HOST_RIP), vmcs_readl(HOST_RSP));
        printk("CS=%04x SS=%04x DS=%04x ES=%04x FS=%04x GS=%04x TR=%04x\n",
               vmcs_read16(HOST_CS_SELECTOR), vmcs_read16(HOST_SS_SELECTOR),
               vmcs_read16(HOST_DS_SELECTOR), vmcs_read16(HOST_ES_SELECTOR),
               vmcs_read16(HOST_FS_SELECTOR), vmcs_read16(HOST_GS_SELECTOR),
               vmcs_read16(HOST_TR_SELECTOR));
        printk("FSBase=%016lx GSBase=%016lx TRBase=%016lx\n",
               vmcs_readl(HOST_FS_BASE), vmcs_readl(HOST_GS_BASE),
               vmcs_readl(HOST_TR_BASE));
        printk("GDTBase=%016lx IDTBase=%016lx\n",
               vmcs_readl(HOST_GDTR_BASE), vmcs_readl(HOST_IDTR_BASE));
        printk("CR0=%016lx CR3=%016lx CR4=%016lx\n",
               vmcs_readl(HOST_CR0), vmcs_readl(HOST_CR3),
               vmcs_readl(HOST_CR4));
        printk("Sysenter RSP=%016lx CS:RIP=%04x:%016lx\n",
               vmcs_readl(HOST_IA32_SYSENTER_ESP),
               vmcs_read32(HOST_IA32_SYSENTER_CS),
               vmcs_readl(HOST_IA32_SYSENTER_EIP));
        if (vmexit_ctl & (VM_EXIT_LOAD_IA32_PAT | VM_EXIT_LOAD_IA32_EFER))
                printk("EFER = 0x%016llx  PAT = 0x%016llx\n",
                       vmcs_read64(HOST_IA32_EFER),
                       vmcs_read64(HOST_IA32_PAT));
        if (cpu_has_load_perf_global_ctrl() &&
            vmexit_ctl & VM_EXIT_LOAD_IA32_PERF_GLOBAL_CTRL)
                printk("PerfGlobCtl = 0x%016llx\n",
                       vmcs_read64(HOST_IA32_PERF_GLOBAL_CTRL));

        printk("*** Control State ***\n");
        printk("PinBased=%08x CPUBased=%08x SecondaryExec=%08x\n",
               pin_based_exec_ctrl, cpu_based_exec_ctrl, secondary_exec_control);
        printk("EntryControls=%08x ExitControls=%08x\n", vmentry_ctl, vmexit_ctl);
        printk("ExceptionBitmap=%08x PFECmask=%08x PFECmatch=%08x\n",
               vmcs_read32(EXCEPTION_BITMAP),
               vmcs_read32(PAGE_FAULT_ERROR_CODE_MASK),
               vmcs_read32(PAGE_FAULT_ERROR_CODE_MATCH));
        printk("VMEntry: intr_info=%08x errcode=%08x ilen=%08x\n",
               vmcs_read32(VM_ENTRY_INTR_INFO_FIELD),
               vmcs_read32(VM_ENTRY_EXCEPTION_ERROR_CODE),
               vmcs_read32(VM_ENTRY_INSTRUCTION_LEN));
        printk("VMExit: intr_info=%08x errcode=%08x ilen=%08x\n",
               vmcs_read32(VM_EXIT_INTR_INFO),
               vmcs_read32(VM_EXIT_INTR_ERROR_CODE),
               vmcs_read32(VM_EXIT_INSTRUCTION_LEN));
        printk("        reason=%08x qualification=%016lx\n",
               vmcs_read32(VM_EXIT_REASON), vmcs_readl(EXIT_QUALIFICATION));
        printk("IDTVectoring: info=%08x errcode=%08x\n",
               vmcs_read32(IDT_VECTORING_INFO_FIELD),
               vmcs_read32(IDT_VECTORING_ERROR_CODE));
        printk("TSC Offset = 0x%016llx\n", vmcs_read64(TSC_OFFSET));
        if (secondary_exec_control & SECONDARY_EXEC_TSC_SCALING)
                printk("TSC Multiplier = 0x%016llx\n",
                       vmcs_read64(TSC_MULTIPLIER));
        if (cpu_based_exec_ctrl & CPU_BASED_TPR_SHADOW) {
                if (secondary_exec_control & SECONDARY_EXEC_VIRTUAL_INTR_DELIVERY) {
                        u16 status = vmcs_read16(GUEST_INTR_STATUS);
                        printk("SVI|RVI = %02x|%02x ", status >> 8, status & 0xff);
                }
                pr_cont("TPR Threshold = 0x%02x\n", vmcs_read32(TPR_THRESHOLD));
                if (secondary_exec_control & SECONDARY_EXEC_VIRTUALIZE_APIC_ACCESSES)
                        printk("APIC-access addr = 0x%016llx ", vmcs_read64(APIC_ACCESS_ADDR));
                pr_cont("virt-APIC addr = 0x%016llx\n", vmcs_read64(VIRTUAL_APIC_PAGE_ADDR));
        }
        if (pin_based_exec_ctrl & PIN_BASED_POSTED_INTR)
                printk("PostedIntrVec = 0x%02x\n", vmcs_read16(POSTED_INTR_NV));
        if ((secondary_exec_control & SECONDARY_EXEC_ENABLE_EPT))
                printk("EPT pointer = 0x%016llx\n", vmcs_read64(EPT_POINTER));
        if (secondary_exec_control & SECONDARY_EXEC_PAUSE_LOOP_EXITING)
                printk("PLE Gap=%08x Window=%08x\n",
                       vmcs_read32(PLE_GAP), vmcs_read32(PLE_WINDOW));
        if (secondary_exec_control & SECONDARY_EXEC_ENABLE_VPID)
                printk("Virtual processor ID = 0x%04x\n",
                       vmcs_read16(VIRTUAL_PROCESSOR_ID));

}


static void svm_init_vmcb(struct svm_vcpu *svm)
{
    struct vmcb *vmcb = svm->vmcb;

    vmcb->control.asid = svm->asid;
    vmcb->control.int_ctl = V_INTR_MASKING_MASK;
    vmcb->control.nested_ctl = 0;
    vmcb->control.nested_cr3 = 0;

    vmcb->control.iopm_base_pa =   virt_to_phys(svm->io_bitmap); 
    vmcb->control.msrpm_base_pa =  virt_to_phys(svm->msrpm);        

    vmcb->control.virt_ext = 0;
    vmcb->save.dbgctl = 0;    

    set_intercept(svm, INTERCEPT_INTR);
    set_intercept(svm, INTERCEPT_NMI);
    set_intercept(svm, INTERCEPT_SMI);
    set_intercept(svm, INTERCEPT_SELECTIVE_CR0);
    set_intercept(svm, INTERCEPT_RDPMC);
//    set_intercept(svm, INTERCEPT_CPUID);
    set_intercept(svm, INTERCEPT_INVD);
    set_intercept(svm, INTERCEPT_HLT);
    set_intercept(svm, INTERCEPT_INVLPG);
    set_intercept(svm, INTERCEPT_INVLPGA);
    set_intercept(svm, INTERCEPT_IOIO_PROT);
    set_intercept(svm, INTERCEPT_MSR_PROT);
    set_intercept(svm, INTERCEPT_TASK_SWITCH);
    set_intercept(svm, INTERCEPT_SHUTDOWN);
    set_intercept(svm, INTERCEPT_VMRUN);
    set_intercept(svm, INTERCEPT_VMMCALL);
    set_intercept(svm, INTERCEPT_VMSAVE);
    set_intercept(svm, INTERCEPT_STGI);
    set_intercept(svm, INTERCEPT_CLGI);
    set_intercept(svm, INTERCEPT_SKINIT);
    set_intercept(svm, INTERCEPT_WBINVD);
    set_intercept(svm, INTERCEPT_MONITOR);
    set_intercept(svm, INTERCEPT_MWAIT);
    set_intercept(svm, INTERCEPT_XSETBV);

}


static void reload_tss(void)
{
        struct desc_struct *gdt;
        struct ldttss_desc *tss_desc;

        gdt = get_current_gdt_rw();
        tss_desc = (struct ldttss_desc *)(gdt + GDT_ENTRY_TSS);
        tss_desc->type = 9; /* available 32/64-bit TSS */

        load_TR_desc();
}

#ifdef CONFIG_X86_64
#define R "r"
#define Q "q"
#else
#define R "e"
#define Q "l"
#endif

#define regs_offset(TYPE, MEMBER)  ((size_t)(((TYPE*)0)+MEMBER))


static int   __vmx_run_vcpu(struct vmx_vcpu *vmx, u64 *regs,bool launched)
{
	    int fail=0;
        asm volatile("\n\t"
           "push %[vmx] \n\t"
           "push %[regs] \n\t"
           "push %[regs] \n\t"
           "cmp %%"R"sp, %c[host_rsp](%0) \n\t"
           "je 1f\n\t"
           "mov %%"R"sp, %c[host_rsp](%0) \n\t"
           "vmwrite %%rsp,%%rdx \n\t"
           "1: \n\t"
           "mov (%%"R"sp), %%"R"ax \n\t"
           "cmp $0, %[launched] \n\t"
           "mov %c[cx](%%"R"ax), %%"R"cx\n\t"
           "mov %c[dx](%%"R"ax), %%"R"dx \n\t"
           "mov %c[bx](%%"R"ax), %%"R"bx \n\t"
           "mov %c[si](%%"R"ax), %%"R"si \n\t"
           "mov %c[di](%%"R"ax), %%"R"di \n\t"
           "mov %c[bp](%%"R"ax), %%"R"bp \n\t"
#ifdef CONFIG_X86_64
           "mov %c[r8](%%"R"ax), %%r8 \n\t"
           "mov %c[r9](%%"R"ax), %%r9 \n\t"
           "mov %c[r10](%%"R"ax), %%r10 \n\t"
           "mov %c[r11](%%"R"ax), %%r11 \n\t"
           "mov %c[r12](%%"R"ax), %%r12 \n\t"
           "mov %c[r13](%%"R"ax), %%r13 \n\t"
           "mov %c[r14](%%"R"ax), %%r14 \n\t"
           "mov %c[r15](%%"R"ax), %%r15 \n\t"
#endif
           "mov %c[ax](%%"R"ax), %%"R"ax \n\t"

           "jne .Llaunched \n\t"
           "vmlaunch \n\t"
           "jmp .Lkvm_vmx_return \n\t"
           ".Llaunched: vmresume \n\t"
           ".Lkvm_vmx_return: "

           "mov %%"R"ax, %c[wordsize](%%"R"sp)\n\t"
           "pop %%"R"ax \n\t"
           "pop %c[ax](%%"R"ax) \n\t"
           "mov %%"R"cx, %c[cx](%%"R"ax) \n\t"
           "mov %%"R"dx, %c[dx](%%"R"ax) \n\t"
           "mov %%"R"bx, %c[bx](%%"R"ax) \n\t"
           "mov %%"R"si, %c[si](%%"R"ax) \n\t"
           "mov %%"R"di, %c[di](%%"R"ax) \n\t"
           "mov %%"R"bp, %c[bp](%%"R"ax) \n\t"
#ifdef CONFIG_X86_64
           "mov %%r8, %c[r8](%%"R"ax) \n\t"
           "mov %%r9, %c[r9](%%"R"ax) \n\t"
           "mov %%r10, %c[r10](%%"R"ax) \n\t"
           "mov %%r11, %c[r11](%%"R"ax) \n\t"
           "mov %%r12, %c[r12](%%"R"ax) \n\t"
           "mov %%r13, %c[r13](%%"R"ax) \n\t"
           "mov %%r14, %c[r14](%%"R"ax) \n\t"
           "mov %%r15, %c[r15](%%"R"ax) \n\t"
#endif
           "setbe %[fail] \n\t"
           "pop %[vmx] \n\t"
    ::[vmx]"r"(vmx),[regs]"r"(regs),
        [launched]"r"(launched), "d"((unsigned long)HOST_RSP),
        [host_rsp]"i"(offsetof(struct vmx_vcpu,host_rsp)),
        [ax]"i"(regs_offset(u64, VCPU_REGS_RAX)),
        [bx]"i"(regs_offset(u64, VCPU_REGS_RBX)),
        [cx]"i"(regs_offset(u64, VCPU_REGS_RCX)),
        [dx]"i"(regs_offset(u64, VCPU_REGS_RDX)),
        [si]"i"(regs_offset(u64, VCPU_REGS_RSI)),
        [di]"i"(regs_offset(u64, VCPU_REGS_RDI)),
        [bp]"i"(regs_offset(u64, VCPU_REGS_RBP)),
#ifdef CONFIG_X86_64
        [r8]"i"(regs_offset(u64, VCPU_REGS_R8)),
        [r9]"i"(regs_offset(u64, VCPU_REGS_R9)),
        [r10]"i"(regs_offset(u64, VCPU_REGS_R10)),
        [r11]"i"(regs_offset(u64, VCPU_REGS_R11)),
        [r12]"i"(regs_offset(u64, VCPU_REGS_R12)),
        [r13]"i"(regs_offset(u64, VCPU_REGS_R13)),
        [r14]"i"(regs_offset(u64, VCPU_REGS_R14)),
        [r15]"i"(regs_offset(u64, VCPU_REGS_R15)),
#endif
        [fail]"m"(fail),
        [wordsize]"i"(sizeof(ulong))
      :"cc");
      return fail;
}
	
int vmx_run(struct vmx_vcpu *vmx)
{
    
     vmx->fail = __vmx_run_vcpu(vmx,vmx->regs,vmx->launched);

    return ;   
}



int svm_launch(struct svm_vcpu *svm)
{
    int reason;
    int done = 0;

    svm_load_vcpu(svm); 
   
    while(1)
    {
        reason = vmx_run(svm);        
         
        local_irq_enable();
         
        switch (reason){
        case SVM_EXIT_VMMCALL:
		    printk("exit vm for vmmcall\n");
		    break;
        case SVM_EXIT_CPUID:
		    printk("exit vm for cpuid\n");
            break;
	case SVM_EXIT_MSR:
		    printk("exit vm for msr\n");
		    break;
        case SVM_EXIT_EXCP_BASE ... (SVM_EXIT_EXCP_BASE + 19):
		    printk("exit vm for excp\n");
            break;
        default:
		    printk("exit reason: %x\n",reason);
                    svm_dump_cpu(svm);
		    done = 1;
            break;
	}
//	    if(done)
         break;
    }

    svm_put_vcpu(svm);
    return 0;
}

static const u32 msrpm_ranges[] = {0, 0xc0000000,0xc0010000};

#define NUM_MSR_MAPS ARRAY_SIZE(msrpm_ranges)
#define MSRS_RANGE_SIZE 2048
#define MSRS_IN_RANGE (MSRS_RANGE_SIZE * 8 / 2)

static u32 svm_msrpm_offset(u32 msr)
{
        u32 offset;
        int i;

        for (i = 0; i < NUM_MSR_MAPS; i++) {
                if (msr < msrpm_ranges[i] ||
                                msr >= msrpm_ranges[i] + MSRS_IN_RANGE)
                        continue;

                offset  = (msr - msrpm_ranges[i]) / 4; /* 4 msrs per u8 */
                offset += (i * MSRS_RANGE_SIZE);       /* add range offset */

                /* Now we have the u8 offset - but need the u32 offset */
                return offset / 4;
        }

        /* MSR not in any range */
        return MSR_INVALID;
}


static void set_msr_interception(u32 *msrpm,unsigned msr,int read,int write)
{
    u8 bit_read,bit_write;
    unsigned long tmp;
    u32 offset;
 
    offset  = svm_msrpm_offset(msr);
    bit_read = 2 * (msr & 0x0f);
    bit_write = 2 * (msr &0x0f) + 1;
    tmp  = msrpm[offset];
  

    BUG_ON(offset == MSR_INVALID);

    read  ? clear_bit(bit_read,  &tmp) : set_bit(bit_read,  &tmp);
    write ? clear_bit(bit_write, &tmp) : set_bit(bit_write, &tmp);

    msrpm[offset] = tmp;
}

/*
 *  * vmsave_load_state show the state of cpu before and after vmsave or load
 */
void vmsave_load_state(struct svm_vcpu *svm)
{
        struct vmcb *vmcb = svm->vmcb;        

        u64 fs_base, gs_base, knl_gs_base, star, lstar, cstar, sfmask;
        u16 fs_sel, gs_sel, ldt_sel;

        rdmsrl(MSR_GS_BASE, gs_base);
        rdmsrl(MSR_FS_BASE, fs_base);
        rdmsrl(MSR_KERNEL_GS_BASE, knl_gs_base);
        rdmsrl(MSR_STAR, star);
        rdmsrl(MSR_LSTAR, lstar);
        rdmsrl(MSR_CSTAR, cstar);
        rdmsrl(MSR_SYSCALL_MASK, sfmask);

        savesegment(gs, gs_sel);
        savesegment(fs, fs_sel);
        ldt_sel = svm_read_ldt();

        printk("gs_sel 0x%08x, gs_base 0x%016llx, fs_sel 0x%08x, fs_base 0x%016llx,\n"
                        "knl_gs_base 0x%016llx, ldt_sel 0x%08x, star 0x%016llx, lstar 0x%016llx,\n "
                        "cstar 0x%016llx, sfmask 0x%016llx\n", gs_sel, gs_base, fs_sel,
                        fs_base, knl_gs_base, ldt_sel, star, lstar, cstar, sfmask);
 
        vmcb->save.kernel_gs_base = knl_gs_base;
        vmcb->save.star = star;
        vmcb->save.lstar = lstar;
        vmcb->save.cstar = cstar;
        vmcb->save.sfmask = sfmask;
        

        vmcb->save.gs.selector = gs_sel;
        vmcb->save.gs.base = gs_base;       
  
        vmcb->save.fs.selector = fs_sel;
        vmcb->save.fs.base = fs_base;

              
}

/*
static void setup_gdt(struct svm_vcpu *svm)
{
        memcpy(svm->gdtr->gdt, gdt_template, sizeof(uint64_t) * NR_GDT_ENTRIES);

        svm->gdtr->gdt[GD_TSS >> 3] = (SEG_TSSA | SEG_P | SEG_A |
                                    SEG_BASELO(&pe:rcpu->tss) |
                                    SEG_LIM(sizeof(struct Tss) - 1));
        svm->gdtr->gdt[GD_TSS2 >> 3] = SEG_BASEHI(&percpu->tss);
}
*/

static int svm_dt_set(struct svm_vcpu *svm)
{
    struct tptr _idtr, _gdtr;
    struct desc_struct *gdt;

    asm volatile (
    "sgdt %0\n"
    "sidt %1\n"
    : : "m" (_gdtr), "m" (_idtr) : "rax");

    svm->vmcb->save.gdtr.base = (u64) get_cpu_gdt_rw(cpu);
    svm->vmcb->save.gdtr.limit = _gdtr.limit;

    svm->vmcb->save.idtr.base = _idtr.base;
    svm->vmcb->save.idtr.limit = _idtr.limit;

//    conf->svm_sys_regs.gdt.base = _gdtr.base;
//    conf->svm_sys_regs.gdt.limit = _gdtr.limit;
//    setup_gdt(svm);

//    conf->svm_sys_regs.idt.base  = _idtr.base;
//    conf->svm_sys_regs.idt.limit   = _idtr.limit;

    return 0;
}


static int setup_guest_regs(struct svm_vcpu *svm, struct svm_config *conf)
{
    
    struct vmcb *vmcb = svm->vmcb;    
    
    svm->asid = 1;
// common registers
// 
    svm->regs[VCPU_REGS_RAX] = conf->svm_user_regs.rax;
    svm->regs[VCPU_REGS_RBX] = conf->svm_user_regs.rbx;
    svm->regs[VCPU_REGS_RCX] = conf->svm_user_regs.rcx;
    svm->regs[VCPU_REGS_RDX] = conf->svm_user_regs.rdx;
    svm->regs[VCPU_REGS_RSI] = conf->svm_user_regs.rsi;
    svm->regs[VCPU_REGS_RDI] = conf->svm_user_regs.rdi;
    svm->regs[VCPU_REGS_RDI] = conf->svm_user_regs.rdi;
    svm->regs[VCPU_REGS_RBP] = conf->svm_user_regs.rbp;
    svm->regs[VCPU_REGS_R8]  = conf->svm_user_regs.r8;
    svm->regs[VCPU_REGS_R9]  = conf->svm_user_regs.r9;
    svm->regs[VCPU_REGS_R10] = conf->svm_user_regs.r10;
    svm->regs[VCPU_REGS_R11] = conf->svm_user_regs.r11;
    svm->regs[VCPU_REGS_R12] = conf->svm_user_regs.r12;
    svm->regs[VCPU_REGS_R13] = conf->svm_user_regs.r13;
    svm->regs[VCPU_REGS_R14] = conf->svm_user_regs.r14;
    svm->regs[VCPU_REGS_R15] = conf->svm_user_regs.r15;

    vmcb->save.rip = conf->svm_user_regs.rip;
    vmcb->save.rsp = conf->svm_user_regs.rsp;
    vmcb->save.rflags = conf->svm_user_regs.rflags;

    svm->regs[VCPU_REGS_RIP] = conf->svm_user_regs.rip;
    svm->regs[VCPU_REGS_RSP] = conf->svm_user_regs.rsp;


//  set sys register
    
    vmcb->save.cr0 = conf->svm_sys_regs.cr0;
    vmcb->save.cr3 = conf->svm_sys_regs.cr3;
    vmcb->save.cr4 = conf->svm_sys_regs.cr4;
    vmcb->save.efer = conf->svm_sys_regs.efer;

    vmcb->save.dr7 = 0;
    vmcb->save.dr6 = 0;
    vmcb->save.cpl = 0;

    vmcb->save.cs.base = conf->svm_sys_regs.cs.base;
    vmcb->save.cs.limit = conf->svm_sys_regs.cs.limit;
    vmcb->save.cs.selector = conf->svm_sys_regs.cs.selector;
    vmcb->save.cs.attrib = svm_segment_access_rights(&conf->svm_sys_regs.cs);
//    vmcb->save.cs.attrib = 0x29b;

    vmcb->save.ds.base = conf->svm_sys_regs.ds.base;
    vmcb->save.ds.limit = conf->svm_sys_regs.ds.limit;
    vmcb->save.ds.selector = conf->svm_sys_regs.ds.selector;
    vmcb->save.ds.attrib = svm_segment_access_rights(&conf->svm_sys_regs.ds);

    vmcb->save.es.base = conf->svm_sys_regs.es.base;
    vmcb->save.es.limit = conf->svm_sys_regs.es.limit;
    vmcb->save.es.selector = conf->svm_sys_regs.es.selector;
    vmcb->save.es.attrib = svm_segment_access_rights(&conf->svm_sys_regs.es);

    vmcb->save.gs.base = conf->svm_sys_regs.gs.base;
    vmcb->save.gs.limit = conf->svm_sys_regs.gs.limit;
    vmcb->save.gs.selector = conf->svm_sys_regs.gs.selector;
    vmcb->save.gs.attrib = svm_segment_access_rights(&conf->svm_sys_regs.gs);

    vmcb->save.fs.base = conf->svm_sys_regs.fs.base;
    vmcb->save.fs.limit = conf->svm_sys_regs.fs.limit;
    vmcb->save.fs.selector = conf->svm_sys_regs.fs.selector;
    vmcb->save.fs.attrib = svm_segment_access_rights(&conf->svm_sys_regs.fs);

    vmcb->save.ss.base = conf->svm_sys_regs.ss.base;
    vmcb->save.ss.limit = conf->svm_sys_regs.ss.limit;
    vmcb->save.ss.selector = conf->svm_sys_regs.ss.selector;
    vmcb->save.ss.attrib = svm_segment_access_rights(&conf->svm_sys_regs.ss);

    vmcb->save.gdtr.base = conf->svm_sys_regs.gdt.base;
    vmcb->save.gdtr.limit = conf->svm_sys_regs.gdt.limit;
    vmcb->save.idtr.base = conf->svm_sys_regs.idt.base;
    vmcb->save.idtr.limit = conf->svm_sys_regs.idt.limit;

    vmcb->save.tr.base = conf->svm_sys_regs.tr.base;
    vmcb->save.tr.attrib = svm_segment_access_rights(&conf->svm_sys_regs.tr);
    vmcb->save.tr.limit = conf->svm_sys_regs.tr.limit;

    vmsave_load_state(svm);
    svm_dt_set(svm);

    return 0;
}


int svm_config_init(struct svm_config *conf)
{	

//   set guest system register
     conf->svm_sys_regs.cr3 = __native_read_cr3();

     conf->svm_sys_regs.cr0 = cr0();
     conf->svm_sys_regs.cr4 = cr4();
     conf->svm_sys_regs.efer = efer();

     conf->svm_sys_regs.cs.base = 0x0;
     conf->svm_sys_regs.ds.base = 0x0;
     conf->svm_sys_regs.es.base = 0x0;
     conf->svm_sys_regs.ss.base = 0x0;
     conf->svm_sys_regs.fs.base = 0x0;


     conf->svm_sys_regs.cs.type = 0xb;
     conf->svm_sys_regs.cs.s = 0x1;
     conf->svm_sys_regs.cs.dpl = 0x0;
     conf->svm_sys_regs.cs.present = 0x1;
     conf->svm_sys_regs.cs.avl = 0x0;
     conf->svm_sys_regs.cs.l = 0x1;
     conf->svm_sys_regs.cs.db = 0x0;
     conf->svm_sys_regs.cs.g = 0x1;

     conf->svm_sys_regs.ds.type = 0x3;
     conf->svm_sys_regs.ds.s = 0x1;
     conf->svm_sys_regs.ds.dpl = 0x0;
     conf->svm_sys_regs.ds.present = 0x1;
     conf->svm_sys_regs.ds.avl = 0x0;
     conf->svm_sys_regs.ds.l = 0x1;
     conf->svm_sys_regs.ds.db = 0x0;
     conf->svm_sys_regs.ds.g = 0x1;

     conf->svm_sys_regs.es.type = 0x3;
     conf->svm_sys_regs.es.s = 0x1;
     conf->svm_sys_regs.es.dpl = 0x0;
     conf->svm_sys_regs.es.present = 0x1;
     conf->svm_sys_regs.es.avl = 0x0;
     conf->svm_sys_regs.es.l = 0x1;
     conf->svm_sys_regs.es.db = 0x0;
     conf->svm_sys_regs.es.g = 0x1;

     conf->svm_sys_regs.fs.type = 0x3;
     conf->svm_sys_regs.fs.s = 0x1;
     conf->svm_sys_regs.fs.dpl = 0x0;
     conf->svm_sys_regs.fs.present = 0x1;
     conf->svm_sys_regs.fs.avl = 0x0;
     conf->svm_sys_regs.fs.l = 0x1;
     conf->svm_sys_regs.fs.db = 0x0;
     conf->svm_sys_regs.fs.g = 0x1;

     conf->svm_sys_regs.gs.type = 0x3;
     conf->svm_sys_regs.gs.s = 0x1;
     conf->svm_sys_regs.gs.dpl= 0x0;
     conf->svm_sys_regs.gs.present = 0x1;
     conf->svm_sys_regs.gs.avl = 0x0;
     conf->svm_sys_regs.gs.l = 0x1;
     conf->svm_sys_regs.gs.db = 0x0;
     conf->svm_sys_regs.gs.g = 0x1;

     conf->svm_sys_regs.ss.type = 0x3;
     conf->svm_sys_regs.ss.s = 0x1;
     conf->svm_sys_regs.ss.dpl = 0x0;
     conf->svm_sys_regs.ss.present = 0x1;
     conf->svm_sys_regs.ss.avl = 0x0;
     conf->svm_sys_regs.ss.l = 0x1;
     conf->svm_sys_regs.ss.db = 0x0;
     conf->svm_sys_regs.ss.g = 0x1;
     
     conf->svm_sys_regs.cs.limit = 0xffffffff;
     conf->svm_sys_regs.ds.limit = 0xffffffff;
     conf->svm_sys_regs.es.limit = 0xffffffff;
     conf->svm_sys_regs.fs.limit = 0xffffffff;
     conf->svm_sys_regs.gs.limit = 0xffffffff;
     conf->svm_sys_regs.ss.limit = 0xffffffff;

     conf->svm_sys_regs.cs.selector = __KERNEL_CS;
     conf->svm_sys_regs.ds.selector = __KERNEL_DS;
     conf->svm_sys_regs.es.selector = __KERNEL_DS;
     conf->svm_sys_regs.fs.selector = 0x0;
     conf->svm_sys_regs.gs.selector = 0x0;
     conf->svm_sys_regs.ss.selector = __KERNEL_DS;
     conf->svm_sys_regs.tr.selector = 0x0;

     conf->svm_sys_regs.ldt.selector = 0x0;
     conf->svm_sys_regs.ldt.base = 0x0;
     conf->svm_sys_regs.ldt.limit = 0x0;

     conf->svm_sys_regs.ldt.type = 0x2;
     conf->svm_sys_regs.ldt.s = 0x0;
     conf->svm_sys_regs.ldt.dpl = 0x0;
     conf->svm_sys_regs.ldt.present = 0x1;
     conf->svm_sys_regs.ldt.avl = 0x0;
     conf->svm_sys_regs.ldt.l = 0x0;
     conf->svm_sys_regs.ldt.db = 0x0;
     conf->svm_sys_regs.ldt.g = 0x0;

     conf->svm_sys_regs.tr.base = 0x0;
     conf->svm_sys_regs.tr.limit = 0xff;

     conf->svm_sys_regs.tr.type = 0xb;
     conf->svm_sys_regs.tr.s = 0x0;
     conf->svm_sys_regs.tr.dpl = 0x0;
     conf->svm_sys_regs.tr.present = 0x1;
     conf->svm_sys_regs.tr.avl = 0x0;
     conf->svm_sys_regs.tr.l = 0x0;
     conf->svm_sys_regs.tr.db = 0x0;
     conf->svm_sys_regs.tr.g = 0x0;

 //    svm_dt_set(conf); 
    
     return 0;
}

cpumask_t cpu_vmx_enabled;

int vmx_hardware_enable(void)
{
    int cpu = raw_smp_processor_id();
    u64 phys_addr = __pa(per_cpu(vmxarea,cpu));
	
	if(cr4_read_shadow() & X86_CR4_VMXE)
		return -EBUSY;
	
	
	
	cpumask_set_cpu(smp_processor_id(), &cpu_vmx_enable);
	return 0;
}

int vmx_hardware_disable(void)
{
    int cpu;
    return 0;

}




unsigned long addr = 0;

static int __init vmx_init(void)
{

   memset(&cpu_vmx_enabled,0,sizeof(cpumask_t));
	
//enable hardware support vmx; 
   on_each_cpu(vmx_hardware_enable,null,1);


     
    return 0;

out:
    svm_hardware_disable();
    return -1;
}

static void __exit vmx_exit(void)
{
    printk("module exit\n");
	
    on_each_cpu(vmx_hardware_disable);

}

module_init(vmx_init);
module_exit(vmx_exit);

MODULE_LICENSE("GPL");
