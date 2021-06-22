	.code32

       .align 16
pae_enable:
    mov $0x1,%eax
	cpuid
	bt  $0x6,%edx
	jnc pae_enable_done
	mov %cr4,%eax
	bts $PAE_BIT, %eax
	mov %eax, %cr4
pae_enable_done:
    ret


       .align 16
clear_4K_page:
        pxor %xmm1, %xmm1
        mov $4096, %eax
clear_4K_page_loop:        
        movdqu %xmm1, -16(%esi,%eax)
		movdqu %xmm1, -32(%esi,%eax)
		movdqu %xmm1, -48(%esi,%eax)
		movdqu %xmm1, -64(%esi,%eax)
		movdqu %xmm1, -80(%esi,%eax)
		movdqu %xmm1, -96(%esi,%eax)
		movdqu %xmm1, -112(%esi,%eax)
		movdqu %xmm1, -128(%esi,%eax)
        sub $128,%eax
        jnz clear_4K_page_loop
        ret
#------------------------------------
#input parameters:
#       %esi:address, %edi: pages number
#
clear_4k_pages:
    test %edi,%edi
	jz clear_4k_pages_done
clear_4k_pages_loop:
    call clear_4K_page
	add $0x1000,%esi
	dec %edi
	jnz clear_4k_pages_loop
clear_4k_pages_done:
    ret

       
bsp_init_page:
init_paging:
	mov $IA32_APIC_BASE,%ecx
	rdmsr
	bt $8,%eax
	jnc init_page_done
	
	mov $0x200000,%esi
	mov $15, %edi
	call clear_4k_pages
	mov $0x201000,%esi
	mov $3, %edi
	call clear_4k_pages
 	mov $0x300000,%esi
	mov $5, %edi
	call clear_4k_pages
 
    movl $(0x201000|RW|US|P), (0x200000)                 # PML4T[0]
    movl $0x0, 0x200004

    movl $(0x202000|RW|P), (0x200000 + 0x1FF* 8)         # PML4 entry for kernel stack,
	movl $0x0, (0x200000+0x1FF*8+4)

    movl $(0x300000|RW|US|P),(0x200000 + 0xff*8)         # PML4 entry for processor stack 
	movl $0,(0x200000 +0xff *8 +4)

###########	
	movl $(0x203000|RW|US|P),(0x201000)
	movl $0x0,(0x201004)

    movl $(0x205000|RW|US|P),(0x203000)
	movl $0,(0x203004)
	
	movl $(0x200000|PS|RW|P),(0x203000 + 1*8)
	movl $0,(0x203000 + 1*8 + 4)

    movl $(0x207000|RW|P),(0x203000 + 2*8)
    movl $0,(0x203000 + 2*8 +4)

    movl $(0xfee00000| PCD |PWT| PS| RW | P),(0x203000 + 4*8)
    movl $0,(0x203000 + 4*8 + 4)
########


    movl $(0x204000|RW|US|P),(0x202000 + 0x1e0*8)
	movl $0,(0x202000 + 0x1e0*8 +4)

    movl $(0x208000|RW|P),(0x204000 + 0x80*8)
	movl $0,(0x204000 + 0x80*8 + 4)

    movl $(0x209000|RW|P),(0x202000 + 0x1ff*8)
	movl $0x0,(0x202000 + 0x1ff*8 +4)

    movl $(0x301000|RW|US|P),(0x300000 + 0x1fc*8)
	movl $0,(0x300000 + 0x1fc*8 +4)
	movl $(0x302000|RW|US|P),(0x300000 + 0x1ff*8) #processor stack PDPT table
	movl $0,(0x300000 + 0x1ff*8 +4)

     
    movl $(0x800000|PS|RW|US|PS),(0x301000)
	movl $0, (0x301004)
	
    movl $(0x303000|RW|US|P),(0x302000 + 0x1ff*8) #processor stack PDET table
	movl $0x0,(0x302000 + 0x1ff*8 + 4)
  
#Mapping 2M large page,it is used for kernel stack IDT stack,sysenter stack,syscall stack  
	movl $(0xa00000|PS|RW|P),(0x209000 + 0x1ff*8)
	movl $0,(0x209000 + 0x1ff*8+4)
	
#mapping code address,virutual address equl physical address
    movl $(0x0|RW|US),(0x205000 + 8)
    movl $0,(0x205000+4)

    movl $(0x09000|RW|US|P),(0x205000 + 8*0x9)
    movl $0,(0x205000+ 8*0x9 + 4)
    movl $(0x0a000|RW|US|P),(0x205000 + 8*0xa)
    movl $0,(0x205000+ 8*0xa + 4)
    movl $(0x0b000|RW|US|P),(0x205000 + 8*0xb)
    movl $0,(0x205000+ 8*0xb + 4)
    movl $(0x0c000|RW|US|P),(0x205000 + 8*0xc)
    movl $0,(0x205000+ 8*0xc + 4)
    movl $(0x0d000|RW|US|P),(0x205000 + 8*0xd)
    movl $0,(0x205000+ 8*0xd + 4)
    movl $(0x0e000|RW|US|P),(0x205000 + 8*0xe)
    movl $0,(0x205000+ 8*0xe + 4)
    movl $(0x0f000|RW|US|P),(0x205000 + 8*0xf)
    movl $0,(0x205000+ 8*0xf + 4)
	movl $(0x10000|RW|US|P),(0x205000 + 8*0x10)
    movl $0,(0x205000+ 8*0x10 + 4)
	movl $(0x11000|RW|US|P),(0x205000 + 8*0x11)
    movl $0,(0x205000+ 8*0x11 + 4)
    movl $(0x12000|RW|US|P),(0x205000 + 8*0x12)
    movl $0,(0x205000+ 8*0x12 + 4)
    movl $(0x13000|RW|US|P),(0x205000 + 8*0x13)
    movl $0,(0x205000+ 8*0x13 + 4)
	movl $(0x14000|RW|US|P),(0x205000 + 8*0x14)
    movl $0,(0x205000+ 8*0x14 + 4)
	movl $(0x15000|RW|US|P),(0x205000 + 8*0x15)
    movl $0,(0x205000+ 8*0x15 + 4)
    movl $(0x16000|RW|US|P),(0x205000 + 8*0x16)
    movl $0,(0x205000+ 8*0x16 + 4)
    movl $(0x17000|RW|US|P),(0x205000 + 8*0x17)
    movl $0,(0x205000+ 8*0x17 + 4)	
	movl $(0x18000|RW|US|P),(0x205000 + 8*0x18)
    movl $0,(0x205000+ 8*0x18 + 4)
    movl $(0x19000|RW|US|P),(0x205000 + 8*0x19)
    movl $0,(0x205000+ 8*0x19 + 4)
    movl $(0x20000|RW|US|P),(0x205000 + 8*0x20)
    movl $0,(0x205000+ 8*0x20 + 4)	

#   video memory virtual address 0xb8000 - 0xb9fff mapping 
#   physical address  0xb8000 - 0xb9fff
#
    movl $(0xb8000|RW|US|P),(0x205000 + 0xb8*8)
	movl $0, (0x205000 + 0xb8*8 +4)
    movl $(0xb9000|RW|US|P),(0x205000 + 0xb9*8)	
	movl $0, (0x205000 + 0xb9*8 +4)
#
#
	movl $(0x412000|RW|P),(0x208000)
    movl $0, (0x208004)
	
	movl $(0x413000|RW|P),(0x205000 + 8*1)
	movl $0, (0x208000 + 1*8 + 4)
	
#   
#	Four processor user stack PTE
	movl $(0x414000|RW|US|P),(0x303000)        #processor 1 stack 
	movl $0, (0x303000+4)
	movl $(0x415000|RW|US|P),(0x303000 + 1*8)  #processor 2 stack	
	movl $0, (0x303000 +1*8 +4)
	movl $(0x416000|RW|US|P),(0x303000 + 2*8)  #processor 3 stack
	movl $0, (0x303000 + 2*8 +4)
    movl $(0x417000|RW|US|P),(0x303000 + 3*8)  #processor 4 stack
	movl $0, (0x303000 + 3*8 +4)
	
init_page_done:
    ret
	