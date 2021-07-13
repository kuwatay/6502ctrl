.SUFFIXES: .asm .p .hex .sr

all:	wozmon.hex wozmon-fe00.hex

.p.hex:
	p2hex -F Intel $*.p $*.hex

.p.sr:
	p2hex -F Moto $*.p $*.sr

.asm.p:	
	asl -L $*.asm

clean:
	rm -f *.p *.hex *.sr *.lst

#wozmon.hex : wozmon.asm
#	asl wozmon.asm -o wizmon.hex


