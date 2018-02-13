# dmfmvc - Deflemask Master Volume Changer

```
Usage: dmfmvc [options] <infile>
	-o <outfile>  Specify the output file. (defualt none)
	-a <amount>   Specify the volume change. (Can only be positive) 	(defualt 0)
	-n            Subtract by the amount instead of add.
```

## Build instructions:

1. Obtain libdmf.a 
2. Set the compiler in the makefile to a C compiler, (only tested with gcc and x86_64-w64-mingw32-gcc).
3. Run make
4. dmfmvc *should* build, if it doesn't work, and you've tried everything, contact me on discord (Minerscale#9319)
