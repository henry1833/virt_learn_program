
#
#
# support_apic: check it if support APIC on chip
support_apic:
    mov $1,%rax
	cpuid
	bt $9, %rdx
	setc %al
	movzx %al, %rax
	ret
	
#
#support_x2apic: check it if support x2apic
support_x2apic:
    mov $1, %rax
	cpuid
	bt $21, %ecx
	setc %al
	movzx %al, %rax
	ret


