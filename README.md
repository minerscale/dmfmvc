# dmfmvc - Deflemask Master Volume Changer

```
Usage: dmfmvc [options] <infile>
	-o <outfile>  Specify the output file. (default none)
	-a <amount>   Specify the volume change. (Can only be positive) 	(default 0)
	-n            Subtract by the amount instead of add.
```

## Build instructions:

1. Set the compiler in the makefile to a C compiler, (only tested with gcc and x86_64-w64-mingw32-gcc).
2. Run make clean
3. Run make
4. dmfmvc *should* build and the output should be in the out directory, if it doesn't work, and you've tried everything, contact me on discord (Minerscale#9319)
5. If you're on windows, rename the output file to dmfmvc.exe, stupid windows :/

## Bugs

If you find any bugs please make an issue. That would help a lot. :D
