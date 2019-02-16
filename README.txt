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
  
  