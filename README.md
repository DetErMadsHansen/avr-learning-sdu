# Compile via makefile

Compile:
```bash
make
```

Flash:
```bash
make flash
```


# Compile and flash commands:
Compile:
```bash
avr-gcc -mmcu=atmega32a -Os -DF_CPU=1000000UL file-name.c -o file-name.elf
avr-objcopy -O ihex -R .eeprom file-name.elf file-name.hex
```

Flash:
```bash
avrdude -c usbasp -p m32 -U flash:w:file-name.hex:i
```
