/*
 *
 *        Copyright (c) 1994 by EISCAT Scientific Association.
 *                        All Rights Reserved.
 *                             M.Savitski
 *
 *     S-record code - courtesy of Kym Newbery
 *     8918927y@lux.levels.unisa.edu.au
 *
 *     Loading S-records into the VMEbus memory.
 *
 *  Loads an executable in s-record format into the MVME dual-ported
 *  memory and directs the MVME CPU to start execution.
 *  VMEbus access is done via the FORCE CPU-2CE vmeplus driver in
 *  read/write mode for loading and in mmap mode for accessing MVME registers.
 *  See mvme162.h for #define's dependent on the MVME162 setup.
 *
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/vme.h>
#include <sys/mman.h>

#include "../include/bsp.h"

#define FALSE 0
#define TRUE 1

#define DATA19 0
#define DATA28 1
#define DATA37 3
#define HEADER 4
#define TERMINATOR 5
#define NONE 6

unsigned int ahdtoi(unsigned char digit);
int issrec(char *str);
int validrec(char *str);
void hdr2str(char *sstr, char *pstr);
unsigned long getaddr(char *str);
unsigned int datasize(char *str);
void usage (void);
int MVMEControl(u_long entry, int reset, int go);

unsigned int ahdtoi(unsigned char digit)
/*   converts a hexadecimal char to an integer
 *
 *   entry        : digit = character to convert
 *                        : 0..15 = result
 *                        : -1    = char is not a digit
 */
{
/* check digit */
  if (!isxdigit(digit))
    return(-1);

  switch (toupper(digit)) {
    case 'A' : return(0xA);
    case 'B' : return(0xB);
    case 'C' : return(0xC);
    case 'D' : return(0xD);
    case 'E' : return(0xE);
    case 'F' : return(0xF);
    default  : return(digit - 0x30);
  }
}

int issrec(char *str)
/*  attempts to identify the type of Srecord string passed
 *
 *   entry   : str =  pointer to null terminated string
 *   returns : 0,1,2,3,5,7,8,9 for S0..S9 except S6 & S4
 *           : -1 = invalid header or header not found
 *                   : -2 = invalid header number
 */
{
/* Check first character for S */
  if ((isupper(str[0]) && (str[0] == 'S')) || (islower(str[0]) && (str[0] == 's')))
  {
  /* check for valid header number */
    switch (str[1]) {
      case '0' : return 0;    /* header record */
      case '1' : return 1;    /* data record, 2byte addr */
      case '2' : return 2;    /*  "     "   , 3byte addr */
      case '3' : return 3;    /*  "     "   , 4byte addr */
      case '5' : return 5;    /* number of S1,S2,S3 blocks */
      case '7' : return 7;    /* S3 terminator */
      case '8' : return 8;    /* S2 terminator */
      case '9' : return 9;    /* S1 terminator */
      default  : return -2;   /* all others are invalid */
    }
  }
  return(-1);
}

int validrec(char *str)
/*  Tests for a valid srecord. tests checksum & for nondigit characters
 *  doesn't rely on any other srecord routines.
 *
 *    entry   : str   =  pointer to null terminated string
 *    returns : -1        =  srecord contains invalid characters
 *                        : -2    =  srecord checksum is invalid
 *                    : -3    =  srecord record length is invalid
 *            : 0     =  srecord is valid
 */
{
  int         cn = 1, rlen=0;
  int         mchksum=0, rchksum=0;

/* first check if there are any non-digit characters except S */
  while (str[cn]!=0)
    if (!isxdigit(str[cn++]))
      return(-1);

/* test number of data bytes */
  rlen = ahdtoi(str[2])* 0x10 + ahdtoi(str[3]);
  if (((strlen(str)-4)/2U) != rlen) return(-3);

/* get checksum from string */
  rchksum = ahdtoi(str[rlen*2+2])*0x10 + ahdtoi(str[rlen*2+3]); /* string chksum */

/* now calculate my own checksum */
  for (cn=2; cn <= rlen*2; )
    mchksum += ahdtoi(str[cn++])*0x10 + ahdtoi(str[cn++]);
  mchksum = ~mchksum & 0xFF;
  if (mchksum != rchksum) return(-2); /* return -2 in not equal */

/* return OK if we didn't fail any of these tests */
  return(0);
}

void hdr2str(char *sstr, char *pstr)
/*  converts header record (S0) string into a plain string
 *
 *  entry        : sstr = pointer to S0 string record
 *  exit         : pstr = pointer to string long enough to hold string
 *                                (caller must allocate enough space for string)
 */
{
  int     rlen, cn, pn=0;

  rlen = ahdtoi(sstr[2])*0x10 + ahdtoi(sstr[3]);
  for (cn=8; cn <= rlen*2; )
    pstr[pn++] = ahdtoi(sstr[cn++])*0x10 + ahdtoi(sstr[cn++]);
  pstr[pn]=0;
}

unsigned long getaddr(char *str)
/*  returns the address of the srecord in str. assumes record is valid.
 *
 *  entry        : str = pointer to srecord string
 *  exit         : address of data, word or long.
 */
{
  unsigned long addr=0;

  switch (issrec(str)) {
    case 0 :
    case 1 :
    case 5 :
    case 9 :  addr = ahdtoi(str[4])*0x1000 + ahdtoi(str[5])*0x100
              + ahdtoi(str[6])*0x10 + ahdtoi(str[7]);
              return(addr);
    case 2 :
    case 8 :  addr = ahdtoi(str[4])*0x100000 + ahdtoi(str[5])*0x10000
              + ahdtoi(str[6])*0x1000 + ahdtoi(str[7])*0x100
              + ahdtoi(str[8])*0x10 + ahdtoi(str[9]);
              return(addr);
    case 3 :
    case 7 :  addr = ahdtoi(str[4])*0x10000000 + ahdtoi(str[5])*0x1000000
              + ahdtoi(str[6])*0x100000 + ahdtoi(str[7])*0x10000
              + ahdtoi(str[8])*0x1000 + ahdtoi(str[9])*0x100
              + ahdtoi(str[10])*0x10 + ahdtoi(str[11]);
              return(addr);
    default : return(-1);
  }
}

unsigned int datasize(char *str)
/*
 *  returns the number of data bytes in the srecord. assumes record is valid.
 *
 *  entry        : str = pointer to srecord string
 *  exit         : number of bytes of data in the data field.
 */
{
  unsigned int size=0;

  switch (issrec(str)) {
    case 0  :
    case 1  :
    case 5  :
    case 7  :
    case 8  :
    case 9  : size = ahdtoi(str[2])*0x10 + ahdtoi(str[3]);
              return(size-3);
    case 2  : size = ahdtoi(str[2])*0x10 + ahdtoi(str[3]);
              return(size-4);
    case 3  : size = ahdtoi(str[2])*0x10 + ahdtoi(str[3]);
              return(size-5);
    default : return(-1);
  }
}

void usage (void)
/*
 *  prints correct usage on stdout
 */
{
   printf("\nUSAGE :  sload [-v][-g][-r] [file]\n");
   printf("  file is an s-record file\n");
   printf("  -v for verbose summary of s-records loaded\n");
   printf("  -g to start execution\n");
   printf("  -r to reset MVME162\n\n");
}

int MVMEControl(u_long entry, int reset, int go)
/*  Controls MVME-162 from other VME master:
 *  if entry != 0, loads it as start address
 *  if go != 0, starts program execution from entry
 *  if reset != 0, resets mvme162's local bus
 *  Depends upon #define'ed GROUP_BASE_ADDRESS and BOARD_BASE_ADDRESS
 *  which in turn are set by the 162-BUG's ENV command.
 */
{
  int         vme;
  char        vmedev[32] = "/dev/vme16d32";   /* d32 is important !!!  */
  u_long      pagesize;
  struct gcsr *gcsr_map;

  pagesize = sysconf(_SC_PAGESIZE);   /* mmap likes to be page-aligned */

  if ((vme = open(vmedev, O_RDWR)) == -1) {
    perror("open");
    fprintf(stderr, "Cannot open vme as %s to access GCSR\n", vmedev);
    return 1;
  }

/* "MAP_SHARED" is important here */
  gcsr_map = (struct gcsr *) mmap(0, 0x1000, PROT_WRITE|PROT_READ, MAP_SHARED,
                          vme, (u_long)gcsr_vme / pagesize * pagesize);
  if (gcsr_map == (struct gcsr *) - 1) {
    perror("mmap");
    fprintf(stderr, "Cannot mmap() to remote bus address 0x%08X\n",
                 (u_long)gcsr_vme / pagesize * pagesize);
    return 1;
  }

/*
 * use GCSR to start execution in MVME162
 * adjust pointer to compensate for page alignement
 */
  gcsr_map = (struct gcsr *)((u_long)gcsr_map + (u_long)gcsr_vme % pagesize);

  if (reset) {                /* reset the local bus... */
    gcsr_map->board_scr |= 0x80;
  }
  if (entry) {                /* ...load start address... */
    gcsr_map->gpr[0] = entry >> 16U;
    gcsr_map->gpr[1] = entry & 0x0000FFFF;
  }
  if (go) {                   /* ... and kick it in the ass! */
    gcsr_map->lmsig = 0x1;
  }
}

/*=================================================================== */
main(int argc, char *argv[])
{
  char        inpstr[256];
  u_char      image[256];
  char        hdrstr[64];
  int         i, j, k, result, size, line=0, lastrec=0;
  long        addr, tsize=0, naddr=0, blksize=0, blknum=1;
  FILE        *in;
  char        infile[256] = "";
  char        vmedev[32] = "/dev/vme32d32";   /* Assume "/dev/vme32d32"  */
  int         vme, verbose = 0, go = 0, reset = 0, havefile = 0;

/*  Parse the command line */

  --argc;

  while (argv++, argc--) {
    if (**argv != '-') {
      strcpy(infile, *argv);
      havefile = 1;
    } else if (!strcmp(*argv, "-v")) {
      verbose = 1;
    } else if (!strcmp(*argv, "-g")) {
      go = 1;
    } else if (!strcmp(*argv, "-r")) {
      reset = 1;
/*    } else if (!strcmp(*argv, "-vme32")) { */
/*      strcpy(vmedev, "/dev/vme32d32"); */
/*    } else if (!strcmp(*argv, "-vme24")) { */
/*      strcpy(vmedev, "/dev/vme24d32"); */
/*    } else if (!strcmp(*argv, "-vme16")) { */
/*      strcpy(vmedev, "/dev/vme16d32"); */
    } else if (!strcmp(*argv, "-")) {
        usage();
        exit(0);
    } else {
        usage();
        exit(0);
    }
  }

  if (!havefile) {
    if (!reset && !go) {
        usage();
    }
    else {
      MVMEControl(0, reset, go);
    }
    exit(0);
  }

  if ((in = fopen(infile, "r")) == NULL) {
    perror("open");
    fprintf(stderr, "Cannot open input file %s\n", infile);
    exit(1);
  }

  if ((vme = open(vmedev, O_RDWR)) == -1) {
    fprintf(stderr, "Cannot open vme as %s\n", vmedev);
  }

  while (fscanf(in, "%s", &inpstr) != EOF) {
    line++;
    if (validrec(inpstr) == 0) {
      switch (issrec(inpstr)) {
      case 0 :
        hdr2str(inpstr, hdrstr);
        if (verbose) printf("HEADER string = `%s'\n", hdrstr);
        lastrec=HEADER;
        break;
      case 1 :
        addr = getaddr(inpstr);
        size = datasize(inpstr);
        if (blksize == 0) {
          blksize+=size;
          naddr=addr+size;
          if (verbose) printf("DATA\tS19\t$%04lX", addr);
          lastrec=DATA19;
        }
        else if ((blksize!=0) && (addr==naddr)) {
          blksize+=size;
          naddr=addr+size;
        }
        else {
          if (verbose) printf("\t$%04lX\t%lu", naddr-1, blksize);
          if (verbose) printf("\t%d\n", blknum);
          blknum+=1;
          naddr=addr+size;
          blksize=size;
          if (verbose) printf("DATA\tS19\t$%04lX", addr);
          lastrec=DATA19;
        }
        tsize += size;
        if (vme == -1) break;
        for (i = 0, j = 8, k = size; k-- > 0; i += 1, j += 2) {
          image[i] = ahdtoi(inpstr[j])*0x10 + ahdtoi(inpstr[j+1]);
        }
        if (lseek(vme, addr, SEEK_SET) == -1) {
          fprintf(stderr, "lseek() to vme address %08X failed\n", addr);
        }
        else {
          if (write(vme, (u_char *)image, size) != size) {
            fprintf(stderr, "Write to vme address %08X failed\n", addr);
          }
        }
        break;
      case 2 :
        addr = getaddr(inpstr);
        size = datasize(inpstr);
        if (blksize == 0) {
          blksize+=size;
          naddr=addr+size;
          if (verbose) printf("DATA\tS28\t$%06lX",addr);
          lastrec=DATA28;
        }
        else if ((blksize!=0) && (addr==naddr)) {
          blksize+=size;
          naddr=addr+size;
        }
        else {
          if (verbose) printf("\t$%06lX\t%lu",naddr-1,blksize);
          if (verbose) printf("\t%d\n",blknum);
          blknum+=1;
          naddr=addr+size;
          blksize=size;
          if (verbose) printf("DATA\tS28\t$%06lX",addr);
          lastrec=DATA28;
        }
        tsize += size;
        if (vme == -1) break;
        for (i = 0, j = 10, k = size; k-- > 0; i += 1, j += 2) {
          image[i] = ahdtoi(inpstr[j])*0x10 + ahdtoi(inpstr[j+1]);
        }
        if (lseek(vme, addr, SEEK_SET) == -1) {
          fprintf(stderr, "lseek() to vme address %08X failed\n", addr);
        }
        else {
          if (write(vme, (u_char *)image, size) != size) {
            fprintf(stderr, "Write to vme address %08X failed\n", addr);
          }
        }
        break;
      case 3 :
        addr = getaddr(inpstr);
        size = datasize(inpstr);
        if (blksize == 0) {
          blksize+=size;
          naddr=addr+size;
          if (verbose) printf("DATA\tS37\t$%08lX",addr);
          lastrec=DATA37;
        }
        else if ((blksize!=0) && (addr==naddr)) {
          blksize+=size;
          naddr=addr+size;
        }
        else {
          if (verbose) printf("\t$%08lX\t%lu",naddr-1,blksize);
          if (verbose) printf("\t%d\n",blknum);
          blknum+=1;
          naddr=addr+size;
          blksize=size;
          if (verbose) printf("DATA\tS37\t$%08lX",addr);
          lastrec=DATA37;
        }
        tsize += size;
        if (vme == -1) break;
        for (i = 0, j = 12, k = size; k-- > 0; i += 1, j += 2) {
          image[i] = ahdtoi(inpstr[j])*0x10 + ahdtoi(inpstr[j+1]);
        }
        if (lseek(vme, addr, SEEK_SET) == -1) {
          fprintf(stderr, "lseek() to vme address %08X failed\n", addr);
        }
        else {
          if (write(vme, (u_char *)image, size) != size) {
            fprintf(stderr, "Write to vme address %08X failed\n", addr);
          }
        }
        break;
      case 7 :
        if (lastrec==DATA19){if (verbose) printf("\t$%04lX\t%lu",naddr-1,blksize);}
        if (lastrec==DATA28){if (verbose) printf("\t$%06lX\t%lu",naddr-1,blksize);}
        if (lastrec==DATA37){if (verbose) printf("\t$%08lX\t%lu",naddr-1,blksize);}
        if (verbose) printf("\t%d\n",blknum);
        addr = getaddr(inpstr);
        if (verbose) printf("TERM\tS37");
        printf("\nExecution address = $%08lX\n", addr);
        lastrec=TERMINATOR;
        break;
      case 8 :
        if (lastrec==DATA19){if (verbose) printf("\t$%04lX\t%lu",naddr-1,blksize);}
        if (lastrec==DATA28){if (verbose) printf("\t$%06lX\t%lu",naddr-1,blksize);}
        if (lastrec==DATA37){if (verbose) printf("\t$%08lX\t%lu",naddr-1,blksize);}
        if (verbose) printf("\t%d\n",blknum);
        addr = getaddr(inpstr);
        if (verbose) printf("TERM\tS28");
        printf("\nExecution address = $%06lX\n", addr);
        lastrec=TERMINATOR;
        break;
      case 9 :
        if (lastrec==DATA19){if (verbose) printf("\t$%04lX\t%lu",naddr-1,blksize);}
        if (lastrec==DATA28){if (verbose) printf("\t$%06lX\t%lu",naddr-1,blksize);}
        if (lastrec==DATA37){if (verbose) printf("\t$%08lX\t%lu",naddr-1,blksize);}
        if (verbose) printf("\t%d\n",blknum);
        addr = getaddr(inpstr);
        if (verbose) printf("TERM\tS19");
        printf("\nExecution address = $%04lX\n", addr);
        lastrec=TERMINATOR;
        break;
      }
    }
    else {
      printf("\nError on line %d. ",line);
      switch (validrec(inpstr)) {
      case -1 : {printf("SRecord contains invalid characters.\n"); break; }
      case -2 : {printf("SRecord checksum is invalid.\n"); break;}
      case -3 : {printf("SRecord length is invalid.\n"); break;}
      }
      exit(1);
    }
  }

  if ((lastrec==DATA19) || (lastrec==DATA28) || (lastrec==DATA37)) {
    if (lastrec==DATA19){if (verbose) printf("\t$%04lX\t%lu",naddr-1,blksize);}
    if (lastrec==DATA28){if (verbose) printf("\t$%06lX\t%lu",naddr-1,blksize);}
    if (lastrec==DATA37){if (verbose) printf("\t$%08lX\t%lu",naddr-1,blksize);}
    if (verbose) printf("\t%d\n",blknum);
    printf("ERROR: terminator record not found.\n");
  }
  else {
    for (i = 0x000FFFF; i-- > 0;) ;           /* mystique delay... */
    MVMEControl(addr, reset, go);
  }
  if (verbose) printf("total data size = %lu bytes\n", tsize);
}
