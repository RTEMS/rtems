/*-------------------------------------------------------------------------+
| bin2boot.c v1.1 - PC386 BSP - 1997/08/18
+--------------------------------------------------------------------------+
| This file contains the i386 binary to boot image filter.
+--------------------------------------------------------------------------+
| (C) Copyright 1997 -
| - NavIST Group - Real-Time Distributed Systems and Industrial Automation
|
| http://pandora.ist.utl.pt
|
| Instituto Superior Tecnico * Lisboa * PORTUGAL
+--------------------------------------------------------------------------+
| Disclaimer:
|
| This file is provided "AS IS" without warranty of any kind, either
| expressed or implied.
+--------------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bytetype.h"
#include "bootimg.h"

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/
#define SEG_MASK            0xffff0000
                                /* Mask for segment part of seg:off address. */
#define OFF_MASK            0x0000ffff
                                 /* Mask for offset part of seg:off address. */
#define DEFAULT_START_ADDR  0x10200     /* Default start address for binary. */
#define BOOTIMG_HDR_SIZE    0x0200      /* Size of output file header.       */
#define BUFFER_BLOCK_SIZE   0x1000      /* Size of transfer buffer size.     */
#define LAST_BLOCK_SIZE     0x0200
                 /* The output file must have a size multiple of this value. */


/*-------------------------------------------------------------------------+
| Macros
+--------------------------------------------------------------------------*/
#define getSeg(x) (Word)((x) >> 16)
                    /* Return seg part (Word) of a seg:off address (DWord). */ 

#define getOff(x) (Word)((x) & OFF_MASK)
                    /* Return off part (Word) of a seg:off address (DWord). */ 

#define getSegOff(x) ((((x) & SEG_MASK) << 12) + ((x) & OFF_MASK))
                           /* Converts a flat address to a seg:off address. */ 

/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/
const char UsageMsg[] = "\
Usage: bin2boot [<infile> [<outfile>]] [-s <start_address>] [-v]\n\
\n\
    <infile>     : input file name\n\
    <outfile>    : output file name\n\
    -s <outfile> : start address of binary image\n\
    -m <size>    : actual size (for compressed images)\n\
    -v           : verbose output\n";                     /* Usage message. */


/*-------------------------------------------------------------------------+
| External Prototypes (for use with getopt)
+--------------------------------------------------------------------------*/
extern char *optarg;

int getopt(int, char *const[], const char *);


/*-------------------------------------------------------------------------+
| Auxiliary Functions
+--------------------------------------------------------------------------*/
static DWord
getNumArg(char *arg)
{
  char *dummy;

  if (arg[0] == '0')
    if ((arg[1] == 'x') || (arg[1] == 'X'))  /* Hexadecimal */
      return (DWord)strtol(arg, &dummy, 16); 
    else  /* Octal */
      return (DWord)strtol(arg, &dummy, 8); 
    else  /* Decimal */
      return (DWord)strtol(arg, &dummy, 10); 
} /* getNumArg */


/*-------------------------------------------------------------------------+
| Main
+--------------------------------------------------------------------------*/
void main(int argc, char *argv[])
{
  FileHeader  bootimgFileHdr;             /* Output file header.             */
  LoadingInfo imageInfo;                  /* Section header.                 */
  Byte        auxBuf[BUFFER_BLOCK_SIZE];  /* */
  DWord       nRead;                      /* Number of bytes read.           */
  Word        padSize;                    /* Size of padding at end of file. */

  char *progName = argv[0];               /* Program name for errors.        */  
  FILE *fpIn     = stdin;
  FILE *fpOut    = stdout;
  DWord binStart = DEFAULT_START_ADDR;    /* Start address of image.         */
  DWord memLen   = 0;                  /* Real length for compressed images. */

  char currArg;
  int  argCount;
  int  flag;          /* general purpose flag    */
  int  verbose = 0;   /* flag for verbose output */

  while ((currArg = getopt(argc, argv, "hm:s:v")) >= 0)  /* parse command line */
    switch (currArg)
    {
      case 'h' :
        fprintf(stderr, UsageMsg);
        exit(0);
        break;
      case 'm' :
        memLen = getNumArg(optarg); 
        break;
      case 's' :
        binStart = getNumArg(optarg); 
        break;
      case 'v' :
        verbose = 1;
        break;
      default :
        fprintf(stderr, UsageMsg);
        exit(1);
        break;
    } /* switch */

  flag = 0;

  for (argCount = 1; argCount < argc; argCount++)
    if (argv[argCount][0] == '-')
    {
      if (argv[argCount][1] == 's')
        argCount++;
    }
    else if (flag)  /* we already have the input file => output file */
    {
      if ((fpOut = fopen(argv[argCount], "w")) == NULL)
      {
        fprintf(stderr, "%s: can't open %s\n", progName, argv[argCount]);
        exit(1);
      }
    }
    else /* input file */
    {
      if ((fpIn = fopen(argv[argCount], "r")) == NULL)
      {
        fprintf(stderr, "%s: can't open %s\n", progName, argv[argCount]);
        exit(1);
      }
      flag = 1;
    }

  /*** begin: Conversion to Bootimg */

  /*** File Header */

  if (verbose)
    fprintf(stderr, "\nBoot Image File Header:\n\n");

  bootimgFileHdr.magicNum = BOOT_IMAGE_MAGIC;  /* 4 bytes - magic number     */
  bootimgFileHdr.flagsLen = NON_VENDOR_LEN;    /* 4 bytes - flags and length */
  bootimgFileHdr.locAddr = getSegOff(binStart - BOOTIMG_HDR_SIZE);
                               /* 4 bytes - location address in ds:bx format */
  bootimgFileHdr.execAddr = getSegOff(binStart);
                                /* 4 bytes - execute address in cs:ip format */

  if (verbose)
  {
    fprintf(stderr, ">> location address in ds:bx format: %04x:%04x\n",
              getSeg(bootimgFileHdr.locAddr), getOff(bootimgFileHdr.locAddr));
    fprintf(stderr, ">>  execute address in cs:ip format: %04x:%04x\n",
              getSeg(bootimgFileHdr.execAddr), getOff(bootimgFileHdr.execAddr));
  }

  /*** Write File Header to output file */

  fwrite((void *)(& bootimgFileHdr), sizeof(FileHeader), 1, fpOut);

  /*** Sections */

  if (verbose)
    fprintf(stderr, "\nCode Section:\n\n");

  imageInfo.flagsTagsLens = 0x04000004;
                       /* flags, vendor's tags, vendor and non-vendor lengths */
  imageInfo.loadAddr      = binStart;                        /* load address  */

  rewind(fpIn);
  fseek(fpIn, 0, SEEK_END);
  nRead = ftell(fpIn);
  rewind(fpIn);
  padSize = LAST_BLOCK_SIZE - (nRead % LAST_BLOCK_SIZE);
  imageInfo.imageLength  = nRead + padSize;                  /* image length  */
  imageInfo.memoryLength = (memLen != 0) ? memLen : imageInfo.imageLength;
                                                             /* memory length */

  fwrite((void *)(&imageInfo), sizeof(LoadingInfo), 1, fpOut); 

  if (verbose)
  {
    fprintf(stderr, ">>  load address: 0x%08lx\n", imageInfo.loadAddr);
    fprintf(stderr, ">>  image length: 0x%08lx\n", imageInfo.imageLength);
    fprintf(stderr, ">> memory length: 0x%08lx\n\n", imageInfo.memoryLength);
  }

  nRead = BOOTIMG_HDR_SIZE - sizeof(FileHeader) - sizeof(LoadingInfo);
  memset((void *)auxBuf, 0x00, nRead);
  fwrite((void *)auxBuf, 1, nRead, fpOut);

  nRead = fread((void *)auxBuf, 1, BUFFER_BLOCK_SIZE, fpIn);

  while (!feof(fpIn))
  {
    fwrite((void *)auxBuf, BUFFER_BLOCK_SIZE, 1, fpOut); 
    nRead = fread((void *)auxBuf, 1, BUFFER_BLOCK_SIZE, fpIn);
  }

  fwrite((void *)auxBuf, 1, nRead, fpOut);

  memset((void *)auxBuf, 0x00, padSize);
  fwrite((void *)auxBuf, 1, padSize, fpOut);

  fclose(fpOut);
  fclose(fpIn); 

  /*** end: Conversion to Bootimg */
  
  exit(0);

} /* main */
