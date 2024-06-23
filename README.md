# CHIP-8 Emulator
CHIP-8 is an interpretted programming language developed by Joseph Weisbecker in the mid 70s and was initally used on the COSMAC VIP and Telmac 1800 8-bit microcomputers to make game programming easier. CHIP-8 programs are run using a CHIP-8 virtual machine.

This is a fairly complete implementation of a CHIP-8 virtual machine however currently the sound seems to behave weird in some ROMs. 
```
$ ./chip8 roms/PONG2 20 500
```
![image](https://github.com/arash28134/chip-8-emu/assets/74412308/7032cb17-6a43-4193-9c6b-902f64a888d5)
```
$ ./chip8 roms/INVADERS 20 500
```
![image](https://github.com/arash28134/chip-8-emu/assets/74412308/3156f001-9103-4e86-8204-73fb3573bed6)

## Compiling and running
Requires SDL2:
```
$ sudo apt-get install libsdl2-dev
```
Compile:
```
$ make
```
Run:
```
$ ./chip8 <rom file> <video scale> <clock rate>
```
Public domain ROMs/Test ROMs I found and used for developing the emulator are included in `roms/` and `test_roms/`.
## References
Some helpful resources I used:

- [CHIP-8 Wikipedia page](https://en.wikipedia.org/wiki/CHIP-8)
- [Guide to making a CHIP-8 emulator](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) by Tobias V. Langhoff
- [BUILDING A CHIP-8 EMULATOR](https://austinmorlan.com/posts/chip8_emulator/) by Austin Morlan
