Minix 1.0 (GCC-IA16 BINUTILS-IA16 NASM)  
  

login: root  
passwd /* No password */  


Toolchain:  
    https://github.com/tkchia  
    https://github.com/crtc-demos  
  
    
bug:  2019-02-16
    ./lib/memcpy.s   
    ./lib/catchsig.s "call %bx" 
    ./test/test5  ./test/test8  /* The ramdisk is too small */  
    ./test/test11    
    
  
combined I & D space:    
1. total memory allocated to program (OK text+data+bss+malloc+stack)  
-----------------------------------------------------------------------   
PROC   -----TEXT-----  -----DATA-----  ----STACK-----  BASE SIZE  
MM        0  7FB  21C     0  A17  12D   12D  B44    0   32K  13K  
FS        0  B44  438     0  F7C  66B   66B 15E7    0   45K  43K  
INIT      0 15E7   64     0 164B   1E    1E 1669    0   88K   2K  
   3      0 45BD    0     0 45BD  672   76A 4D27    8  279K  30K  1.  
   4      0 444F    0     0 444F   2E   164 45B3    A  273K   6K  

proc  -pid- -pc-  -sp-  flag  user  -sys-  base limit recv   command  
PRINTR    0 54B1 1D50    8      0       0    2K 646K  ANY  
TTY       0  36C 1DB8    0      0       0    2K 646K  
WINCHE    0 54B1 1F7A    8      0       0    2K 646K  ANY  
FLOPPY    0 54B1 2086    8      0       0    2K 646K  ANY  
RAMDSK    0 54B1 2180    8      0       0    2K 646K  ANY  
CLOCK     0 54B1 227E    0      0       0    2K 646K  
SYS       0 54B1 2374    8      0       0    2K 646K  ANY  
HARDWR    0  143 24AC    0      0  107063    2K 646K  
MM        0 2067 129C    8      0       0   32K  45K  ANY  
FS        0 42AE 5E94    8      0       0   45K  88K  ANY  
INIT      0  63A  15A    8      1       2   88K  90K  MM  
   3      7 54E3 7652    8      2       5  279K 309K  FS    /bin/sh  
   4      6  232 1652    8      0       2  273K 279K  MM    /bin/update  
  
  
separate I & D:   
1. total memory allocated to program (? data+bss+malloc+stack)    
-----------------------------------------------------------------------   
PROC   -----TEXT-----  -----DATA-----  ----STACK-----  BASE SIZE  
MM        0  7FB  21C     0  A17  12D   12D  B44    0   32K  13K  
FS        0  B44  438     0  F7C  66B   66B 15E7    0   45K  43K  
INIT      0 15E7   64     0 164B   1E    1E 1669    0   88K   2K  
   3      0 45BE  561     0 4B1F  111   209 4D28    8  279K  30K  1.    
   4      0 444F   29     0 4478    6   13C 45B4    A  273K   6K  

proc  -pid- -pc-  -sp-  flag  user  -sys-  base limit recv   command  
PRINTR    0 54B1 1D50    8      0       0    2K 646K  ANY  
TTY       0  36C 1DD8    0      0       0    2K 646K  
WINCHE    0 54B1 1F7A    8      0       0    2K 646K  ANY  
FLOPPY    0 54B1 2086    8      0       0    2K 646K  ANY  
RAMDSK    0 54B1 2180    8      0       0    2K 646K  ANY  
CLOCK     0 54B1 227E    0      0       0    2K 646K  
SYS       0 54B1 2374    8      0       0    2K 646K  ANY  
HARDWR    0  143 24AC    0      0    8978    2K 646K  
MM        0 2067 129C    8      0       0   32K  45K  ANY  
FS        0 42AE 5E94    8      0       0   45K  88K  ANY  
INIT      0  63A  15A    8      0       2   88K  90K  MM  
   3      7 54E3 2042    8      0       7  279K 309K  FS    /bin/sh  
   4      6  232 13D2    8      1       1  273K 279K  MM    /bin/update  
  
  