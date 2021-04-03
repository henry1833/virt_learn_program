	.code32

clear_4k_page:
clear_4K_page:
        pxor %xmm1, %xmm1
        mov $4096, %eax
clear_4K_page_loop:        
        movdqu %xmm1, (%esi + %eax - 16)
		movdqu %xmm1, (%esi + %eax - 32)
		movdqu %xmm1, (%esi + %eax - 48)
		movdqu %xmm1, (%esi + %eax - 64)
		movdqu %xmm1, (%esi + %eax - 80)
		movdqu %xmm1, (%esi + %eax - 96)
		movdqu %xmm1, (%esi + %eax - 112)
		movdqu %xmm1, (%esi + %eax - 128)
        sub $128,%eax
        jnz clear_4K_page_loop
        ret


#PML4T              PDPT             PDET               PT  
#0x200000           0x201000         0x203000           0x205000  
#0x200000+0x1ff*8   0x202000
#0x200000+0x0ff*8   0x300000 
#
#                   0x201000+3*8     0x210000        
#                                    0x210000+0x1ff*8   0x600000
#                                    
#
#

bsp_init_page:
init_page:
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
 
    movd 0x201000|RW|US|P, (0x200000)                 # PML4T[0] level 2 0x201000

    movq $0x202000|RW|P, (0x200000 + 0x1FF* 8)        

    movq $0x300000|RW|US|P, (0x200000 + 0x0ff *8)

    movq $0x203000|RW|US|P, (0x201000)
	
	movq $0x201000|RW|US|P, (0x201000 + 3*8)
	
	movq $0x204000|RW|US|P, (0x202000 + 0x1e0*8)
	
init_page_done:
    ret
	