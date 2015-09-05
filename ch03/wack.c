#include <stdint.h>
#if ELF32 
char shellcode[] = "\xbb\x00\x00\x00\x00"           
                   "\xb8\x01\x00\x00\x00"                  
                   "\xcd\x80";                  
#endif
#if LONG_ZERO_STRINGS
char shellcode[] = "\x48\xbb\x00\x00\x00\x00\x00\x00\x00\x00"           
                   "\x48\xb8\x01\x00\x00\x00\x00\x00\x00\x00"
                   "\xcd\x80";                  
#else
char shellcode[] = "\x48\x31\xdb"
								   "\xb0\x01"
									 "\xcd\x80"
									 "\x48\x31\xdb";
#endif

int main()
{

  uint64_t *ret;
  ret = (uint64_t *)&ret + 2;
  (*ret) = (uint64_t)shellcode;
}
