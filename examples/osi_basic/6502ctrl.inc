; 6502ctrl macro for ca65

DMA_HALT = 0
DMA_SIO = 1

DMA_RESET = 1	
DMA_READ = 2
DMA_WRITE = 3	
DMA_STATUS = 4
DMA_READ_BLOCK = 5

;; CALL DMA
.macro svc arg
       .byte $2c  ; BIT XX (skip 2 bytes)
       .word arg
       wai
.endmacro

;; allocate DMAEQ for DMASIO
.macro dmasio arg
       .byte DMA_SIO
       .byte arg
       .word $ffff
       .byte 0
.endmacro
