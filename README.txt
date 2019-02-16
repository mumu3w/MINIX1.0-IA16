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
  

![image](https://github.com/mumu3w/MINIX1.0-IA16/blob/master/01.png)
