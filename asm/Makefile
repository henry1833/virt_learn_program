demo.img: learn.elf
	objcopy -O binary $< $@
	
learn.elf: learn.o	
	ld -T layout.lds $< -o $@
	
#	objcopy --remove-section .start_blank $<
#	ld --oformat binary -N -e _start  -Ttext 0x7c00 $< -o $@

#%.o: %.s
learn.o: learn.S
	as  $< -o $@ 

clean:
	rm *.o learn.elf demo.img