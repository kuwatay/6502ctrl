targer: osi_bas.bin
	@echo done

osi_bas.o: osi_bas.s
	ca65 -I ~/PROJECT/VintageMicro/cc65/asminc osi_bas.s -o osi_bas.o -l osi_bas.lst

osi_bas.bin: osi_bas.o
	ld65 -C osi_bas.cfg osi_bas.o -o osi_bas.bin

clean:
	-rm osi_bas.o osi_bas.bin osi_bas.lst
