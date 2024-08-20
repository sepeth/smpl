# Getting this error on arm64 when linking libmlreloc.so
#
#     gcc -g -c ml_main.c -o ml_mainreloc.o
#     gcc -shared -o libmlreloc.so ml_mainreloc.o
#     ld: ml_mainreloc.o: relocation R_AARCH64_ADR_PREL_PG_HI21 against symbol `myglob' which may bind externally can not be used when making a shared object; recompile with -fPIC
#     ml_mainreloc.o: in function `ml_func':
#     /home/sepeth/smpl/elf/reloc/ml_main.c:5:(.text+0xc): dangerous relocation: unsupported relocation
#     ld: ml_mainreloc.o: relocation R_AARCH64_ADR_PREL_PG_HI21 against symbol `myglob' which may bind externally can not be used when making a shared object; recompile with -fPIC
#     /home/sepeth/smpl/elf/reloc/ml_main.c:5:(.text+0x20): dangerous relocation: unsupported relocation
#     ld: ml_mainreloc.o: relocation R_AARCH64_ADR_PREL_PG_HI21 against symbol `myglob' which may bind externally can not be used when making a shared object; recompile with -fPIC
#     /home/sepeth/smpl/elf/reloc/ml_main.c:6:(.text+0x2c): dangerous relocation: unsupported relocation
#     collect2: error: ld returned 1 exit status
#     make: *** [Makefile:3: libmlreloc.so] Error 1
#
# And, same result on x86_64 and/or clang
#
#     gcc -shared -o libmlreloc.so ml_mainreloc.o
#     ld: ml_mainreloc.o: warning: relocation against `myglob' in read-only section `.text'
#     ld: ml_mainreloc.o: relocation R_X86_64_PC32 against symbol `myglob' can not be used when making a shared object; recompile with -fPIC
#     ld: final link failed: bad value
#     collect2: error: ld returned 1 exit status
#
#     clang -shared -o libmlreloc.so ml_mainreloc.o
#     ld: ml_mainreloc.o: warning: relocation against `myglob' in read-only section `.text'
#     ld: ml_mainreloc.o: relocation R_X86_64_PC32 against symbol `myglob' can not be used when making a shared object; recompile with -fPIC
#     ld: final link failed: bad value
#     clang: error: linker command failed with exit code 1 (use -v to see invocation)


libmlreloc.so:
	$(CC) -g -c ml_main.c -o ml_mainreloc.o
	$(CC) -shared -o libmlreloc.so ml_mainreloc.o
