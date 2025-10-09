@echo Burn standard Uno 328P Bootloader V8.1 on a 328P by avrdude
rem unlock bootloader 0x3x No restrictions for SPM or LPM accessing the boot loader section. 0xDE is for 512 bytes bootsize.
rem lfuse 0xFF is low power crystal 8 to 16 Mhz, 0xF7 is full swing crystal. Both with slowly rising power.
avrdude -v -patmega328p -cstk500v1 -PCOM6 -b19200 -e -Ulock:w:0x3F:m -Uefuse:w:0xFD:m -Uhfuse:w:0xDE:m -Ulfuse:w:0xFF:m
rem lock bootloader against overwriting, but allow reading for do_spm() feature with: lock=0x2x. No access to bootloader with 0x0x.
avrdude -v -patmega328p -cstk500v1 -PCOM6 -b19200 -Uflash:w:optiboot_atmega328P_81.hex:a -Ulock:w:0x0F:m
pause
