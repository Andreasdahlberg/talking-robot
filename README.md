# talking-robot
Firmware for a talking robot toy.

## Create audio-library
```bash
$ scripts/generator.py
```

## Compile
```bash
$ avr-gcc -mmcu=atmega328p robot.c audio_player.c audio_library.c -o robot.elf
$ avr-objcopy -j .text -j .data -O ihex robot.elf robot.hex
```

## Flash
```bash
$ avrdude -P /dev/ttyACM0 -c avrisp2 -p m328p -U flash:w:robot.hex
```

## Requirements
py-espeak-ng
