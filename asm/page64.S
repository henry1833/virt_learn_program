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
	mov IA32_APIC_BASE,%ecx
	rdmsr
	bt %eax,8
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

    movl $(0x202000|RW|P), (0x200000 + 0x1FF* 8)
	movl $0x0, (0x200000 + 0x1FF* 8 +4)
	
init_page_done:
    ret
	