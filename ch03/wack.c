#include <stdint.h>
char shellcode[] = "\xbb\x00\x00\x00\x00"           
                   "\xb8\x01\x00\x00\x00"                  
                   "\xcd\x80";                  

int main()
{

  uint64_t *ret;
  ret = (uint64_t *)&ret + 2;
  (*ret) = (uint64_t)shellcode;
}
