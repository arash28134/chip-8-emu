# CHIP-8 Emulator
CHIP-8 is an interpretted programming language developed by Joseph Weisbecker in the mid 70s and was initally used on the COSMAC VIP and Telmac 1800 8-bit microcomputers to make game programming easier. CHIP-8 programs are run using a CHIP-8 virtual machine.

This is a fairly complete implementation of a CHIP-8 virtual machine however currently the sound seems to behave weird in some ROMs and the timing is wrong. (Frequency parameter basically does nothing)
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
$ ./chip8 <ROM> <Video Scale> <Frequency>
```
Public domain ROMs/Test ROMs I found and used for developing the emulator are included in `roms/` and `test_roms/`.
## References
Some helpful resources I used:

- [CHIP-8 Wikipedia page](https://en.wikipedia.org/wiki/CHIP-8)
- [Guide to making a CHIP-8 emulator](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/) by Tobias V. Langhoff
