/*-------------------------------------------------------------------------+
| bin2boot.c v1.0 - PC386 BSP - 1998/04/09
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
 
#include "Image.h"
#include "Header.h"
#include "bytetype.h"

#include <stdio.h>
#include <stdlib.h>

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/

#define MAX_IMAGES 15

/*-------------------------------------------------------------------------+
| Global Variables
+--------------------------------------------------------------------------*/

// Help message for users
const char UsageMsg[] = "\
Usage: bin2boot <outFile> <locAddr> <inFile1> <startAddr1> <memSize1> \\\n\
               [<infile2> <startAddr2> <memSize2>][...][-v][-h][-?]\n\
\n\
<outFile>    : output file name (mandatory)\n\
<locAddr>    : location address in memory of image header (mandatory)\n\
<inFileX>    : name of Xth input file\n\
<startAddrX> : start address of Xth image\n\
<memSizeX>   : actual size (for compressed images), use 0 if uncompressed\n\
-v           : verbose output\n\
-h, -?       : this help message\n\
\n\
At least one set of <inFile> <startAddr> and <memSize> is mandatory!\n\
<locAddr>, <startAddrX> and <memSizeX> can be in Decimal, Hexadecimal or Octal.\n\
The maximum number of input files is 15.\n";

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

int main(int argc, char* argv[])
{
  Image    img[MAX_IMAGES]; /* array to store up to MAX_IMAGE images */
  Header   hdr;             /* boot image file header                */
  FILE*    outFile;         /* boot image file stream                */
  char*    outFileName;     /* name of boot image file               */

  int  argPos = 1, numImages, i;
  int  verboseFlag = 0, helpFlag = 0; /* flags for command line options */

  /*-------------------------------------------------------------------------+
  | Parse command line arguments and options
  +-------------------------------------------------------------------------*/
  
  {
    char opt;

    /* parse command line options */
    while ((opt = getopt(argc, argv, "vh?")) >= 0)
    {
      argPos++;
      switch (opt)
      {
        case 'v' : verboseFlag = 1; break;
        case 'h' : case '?' : helpFlag = 1; break;
      }
    }
  }

  if (helpFlag)
  {
    fprintf(stderr, "%s\n", UsageMsg);
    if (argc == 2)
      exit(0);
  }

  numImages = (argc - argPos - 2) / 3;
 
  if (numImages < 1)
  {
    fprintf(stderr,
           "ERROR!!! Not enough command line arguments.\n\n%s\n",
           UsageMsg);
    exit(1);
  }
  if (numImages > 15)
  {
    fprintf(stderr, "ERROR!!! Too many input files.\n\n%s\n", UsageMsg);
    exit(1);
  }

  newHeader(&hdr); /* initialize hdr */

  argPos = 1;

  while (argv[argPos][0] == '-') argPos++; /* discard options */
  if(!(outFile = fopen((outFileName = argv[argPos++]), "w")))
  {
    fprintf(stderr,
            "ERROR!!! While opening file '%s' for output.\n",
            outFileName);
    exit(1);
  }
  reserveSpaceHeader(outFile);

  while (argv[argPos][0] == '-') argPos++; /* discard options */
  setLocAddrHeader(&hdr, getNumArg(argv[argPos++]));

  /*-------------------------------------------------------------------------+
  | Parse command line arguments concerning images
  +-------------------------------------------------------------------------*/

  for(i = 0; i < numImages; i++)
  {
    newImage(&img[i]);
    while (argv[argPos][0] == '-') argPos++; /* discard options */
    setImageFile(&img[i], argv[argPos++]);
    while (argv[argPos][0] == '-') argPos++; /* discard options */
    setLoadAddrImage(&img[i], getNumArg(argv[argPos++]));
    while (argv[argPos][0] == '-') argPos++; /* discard options */
    setMemSizeImage(&img[i], getNumArg(argv[argPos++]));
  }

  setLstImgFlagImage(&img[numImages - 1]);        /* set flag on last image */
  setExecAddrHeader(&hdr, getLoadAddrImage(&img[0]));
                /* boot file execution address is the same as first image's */
  if (verboseFlag)
  {
    fprintf(stderr, "BootImage file '%s' info:\n\n", outFileName);
    fprintHeader(stderr, &hdr);
    fprintf(stderr, "\n");
  }

  /*-------------------------------------------------------------------------+
  | Dump images and header to file
  +-------------------------------------------------------------------------*/

  for(i = 0; i < numImages; i++)
  {
    addImageToHeader(&hdr, &img[i]);
    dumpImageToFileAndPad(outFile, &img[i]);

    if (verboseFlag)
      fprintImage(stderr, &img[i]);

    /* kill image. we don't need anymore and should free its resources */
    killImage(&img[i]);
  }

  dumpHeaderToFile(outFile, &hdr);

  fclose(outFile);

  exit(0);
} /* main */

