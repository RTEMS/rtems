
/*
 * Simplyfied version of original bin2boot
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <string.h>

static unsigned char buf[512];

static void usage(void)
{
  printf("usage: bin2boot [-h][-v] <outFile> <headerAddr> \n");
  printf("<imFile1> <imAddr1> <imSize1> [<imFile2> <imAddr2> <imSize2>]\n");
  printf("this function makes image bootable by netboot\n");
  printf("from one or two binary images\n");
  printf("-h         - prints this message\n");
  printf("-v         - verbose output\n");
  printf("outFile    - output file\n");
  printf("headerAddr - address to place header in memory\n");
  printf("             it should be below or equal 0x97e00\n");
  printf("imFile1    - first image\n");
  printf("imAddr1    - its start address, image has to be placed whole\n");
  printf("             below 0x98000 and should not overlap with header\n");
  printf("imSize1    - actual size of compressed image, 0 for uncompressed\n");
  printf("imFile2    - second image\n");
  printf("imAddr2    - its start address\n");
  printf("imSize2    - actual size of compressed image, 0 for uncompressed\n");

  return;
}

int main(int argc, char* argv[])
{
  int      c, verbose;
  extern   int optind;
  FILE     *ofp, *ifp;
  uintptr_t headerAddr, addr1, addr2;
  int      size1, size2, len1, len2, len, imageCnt, cnt;
  char     *ofile, *ifile, *end;

  len2 = 0;   /* avoid warning */
  size2 = 0;  /* avoid warning */
  addr2 = 0;  /* avoid warning */
  verbose = 0;

  /* parse command line options */
  while ((c = getopt(argc, argv, "hv")) >= 0)
    {
      switch (c)
	{
	case 'v':
	  verbose = 1;
	  break;
	case 'h':
	  usage();
	  return 0;
	default:
	  usage();
	  return 1;
	}
    }

  if((argc - optind) != 8 && (argc - optind) != 5)
    {
      usage();
      return 1;
    }

  ofile = argv[optind];
  ofp   = fopen(ofile, "wb");
  if(ofp == NULL)
    {
      fprintf(stderr, "unable to open file %s\n", ofile);
      return 1;
    }

  /*
   * Layout is very simple first 512 is header shared by all
   * images, then images at 512 bytes border
   */

  /* Fill buffer with 0's */
  memset(buf, 0, sizeof(buf));

  fwrite(buf, 1, sizeof(buf), ofp);

  optind++;
  headerAddr = strtoul(argv[optind], &end, 0);
  if(end == argv[optind])
    {
      fprintf(stderr, "bad headerAddr %s\n", argv[optind]);
      fclose(ofp);
      return 1;
    }

  if(headerAddr > 0x97e00)
    {
      fprintf(stderr, "headerAddr is too high 0x%08lx\n", headerAddr);
      fclose(ofp);
      return 1;
    }

  /* Copy the first image */
  optind++;
  ifile = argv[optind];
  ifp   = fopen(ifile,"rb");
  if(ifp == NULL)
    {
      fprintf(stderr, "unable to open output file %s\n", ifile);
      fclose(ofp);
      return 1;
    }

  optind++;
  addr1 = strtoul(argv[optind], &end, 0);
  if(end == argv[optind])
    {
      fprintf(stderr, "bad image address %s\n", argv[optind]);
      fclose(ofp);
      return 1;
    }

  optind++;
  size1 = strtoul(argv[optind], &end, 0);
  if(end == argv[optind])
    {
      fprintf(stderr, "bad image size %s\n", argv[optind]);
      fclose(ofp);
      return 1;
    }

  /* Copy first image out and remember its length */
  cnt  = 0;
  for(;;)
    {
      len = fread(buf, 1, sizeof(buf), ifp);

      if(len != 0)
	{
	  fwrite(buf, 1, len, ofp);
	  cnt += sizeof(buf);

          if(len != sizeof(buf))
	    {
	      memset(buf, 0, sizeof(buf) - len);
	      fwrite(buf, 1, sizeof(buf) - len, ofp);
	      break;
	    }

	}
      else
	{
	  break;
	}
    }

  fclose(ifp);

  len1 = cnt;

  if(size1 == 0)
    {
      size1 = cnt;
    }
  else
    {
      memset(buf, 0, sizeof(buf));

      while(cnt < size1)
	{
	  fwrite(buf, 1, sizeof(buf), ofp);
	  cnt += sizeof(buf);
	}

      size1 = cnt;
    }

  /* Let us check agains overlapping */
  if(!(addr1 >= (headerAddr + sizeof(buf)) || (headerAddr >= addr1+size1)))
    {
      /* Areas overlapped */
      printf("area overlapping: \n");
      printf("header address      0x%08lx, its memory size 0x%08zx\n",
	     headerAddr, sizeof(buf));
      printf("first image address 0x%08lx, its memory size 0x%08x\n",
	     addr1, size1);

      fclose(ofp);
      return 1;
    }

  if((addr1 + size1) > 0x98000)
    {
      fprintf(stderr, "imAddr1 is too high 0x%08lx\n", addr1);
      fclose(ofp);
      return 1;
    }

  if(optind == (argc - 1))
    {
      imageCnt = 1;
      goto writeHeader;
    }

  imageCnt = 2;

  /* Copy Second Image */
  optind++;
  ifile = argv[optind];
  ifp   = fopen(ifile,"rb");
  if(ifp == NULL)
    {
      fprintf(stderr, "unable to open output file %s\n", ifile);
      fclose(ofp);
      return 1;
    }

  optind++;
  addr2 = strtoul(argv[optind], &end, 0);
  if(end == argv[optind])
    {
      fprintf(stderr, "bad image address %s\n", argv[optind]);
      fclose(ofp);
      return 1;
    }

  optind++;
  size2 = strtoul(argv[optind], &end, 0);
  if(end == argv[optind])
    {
      fprintf(stderr, "bad image size %s\n", argv[optind]);
      fclose(ofp);
      return 1;
    }

  /* Copy second image out and remember its length */
  cnt  = 0;
  for(;;)
    {
      len = fread(buf, 1, sizeof(buf), ifp);

      if(len != 0)
	{
	  fwrite(buf, len, 1, ofp);
	  cnt  += sizeof(buf);

	  if(len != sizeof(buf))
	    {
	      memset(buf, 0, sizeof(buf) - len);
	      fwrite(buf, 1, sizeof(buf) - len, ofp);
	      break;
	    }
	}
      else
	{
	  break;
	}
    }

  fclose(ifp);

  len2 = cnt;

  if(size2 == 0)
    {
      size2 = cnt;
    }
  else
    {
      memset(buf, 0, sizeof(buf));

      while(cnt < size2)
	{
	  fwrite(buf, 1, sizeof(buf), ofp);
	  cnt += sizeof(buf);
	}

      size2 = cnt;
    }

  /* Let us check against overlapping */
  if(!((addr2 >= (addr1 + size1) && addr2 >= (headerAddr + sizeof(buf))) ||
       (addr2 < addr1 && addr2 < headerAddr) ||
       (addr1 > headerAddr && addr2 > (headerAddr + sizeof(buf)) &&
	(addr2 + size2) <= addr1) ||
       (addr1 < headerAddr && addr2 > (addr1 + size1) &&
	(addr2 + size2) <= headerAddr)))

    {
      /* Areas overlapped */
      printf("area overlapping: \n");
      printf("header address       0x%08" PRIxPTR ", its memory size 0x%08zx\n",
	     headerAddr, sizeof(buf));
      printf("first  image address 0x%08" PRIxPTR ", its memory size 0x%08x\n",
	     addr1, size1);
      printf("second image address 0x%08" PRIxPTR ", its memory size 0x%08x\n",
	     addr2, size2);

      fclose(ofp);
      return 1;
    }

writeHeader:

  /* We know everything so it is time to write buffer */
  memset(buf, 0, 0x30);

  buf[0x0]  = 0x36;
  buf[0x1]  = 0x13;
  buf[0x2]  = 0x03;
  buf[0x3]  = 0x1b;

  buf[0x4]  = 4;

  /* Header address in ds:bx format */
  buf[0x8]  = headerAddr & 0xf;
  buf[0x9]  = 0;
  buf[0xa]  = (headerAddr >> 4) & 0xff;
  buf[0xb]  = (headerAddr >> 12) & 0xff;

  /*
   * Execute address in cs:ip format, which addr1
   */
  buf[0xc] = addr1 & 0xf;
  buf[0xd] = 0;
  buf[0xe] = (addr1 >> 4) & 0xff;
  buf[0xf] = (addr1 >> 12) & 0xff;

  /* Flags, tags and lengths */
  buf[0x10] = 4;

  if(imageCnt == 1)
    {
      buf[0x13] = 4;
    }

  /* Load address */
  buf[0x14] = addr1 & 0xff;
  buf[0x15] = (addr1 >> 8) & 0xff;
  buf[0x16] = (addr1 >> 16) & 0xff;
  buf[0x17] = (addr1 >> 24) & 0xff;

  /* Image Length */
  buf[0x18] = len1 & 0xff;
  buf[0x19] = (len1 >> 8) & 0xff;
  buf[0x1a] = (len1 >> 16) & 0xff;
  buf[0x1b] = (len1 >> 24) & 0xff;

  /* Memory Size */
  buf[0x1c] = size1 & 0xff;
  buf[0x1d] = (size1 >> 8) & 0xff;
  buf[0x1e] = (size1 >> 16) & 0xff;
  buf[0x1f] = (size1 >> 24) & 0xff;

  if(imageCnt != 1)
    {

      /* Flags, tags and lengths */
      buf[0x20] = 4;

      buf[0x23] = 4;

      /* Load address */
      buf[0x24] = addr2 & 0xff;
      buf[0x25] = (addr2 >> 8) & 0xff;
      buf[0x26] = (addr2 >> 16) & 0xff;
      buf[0x27] = (addr2 >> 24) & 0xff;

      /* Image Length */
      buf[0x28] = len2 & 0xff;
      buf[0x29] = (len2 >> 8) & 0xff;
      buf[0x2a] = (len2 >> 16) & 0xff;
      buf[0x2b] = (len2 >> 24) & 0xff;

      /* Memory Size */
      buf[0x2c] = size2 & 0xff;
      buf[0x2d] = (size2 >> 8) & 0xff;
      buf[0x2e] = (size2 >> 16) & 0xff;
      buf[0x2f] = (size2 >> 24) & 0xff;
    }

  rewind(ofp);

  fwrite(buf, 1, 0x30, ofp);

  fclose(ofp);

  if(verbose)
    {
      printf("header address       0x%08" PRIxPTR ", its memory size 0x%08zx\n",
	     headerAddr, sizeof(buf));
      printf("first  image address 0x%08" PRIxPTR ", its memory size 0x%08x\n",
	     addr1, size1);
      printf("second image address 0x%08" PRIxPTR ", its memory size 0x%08x\n",
	     addr2, size2);
    }

  return 0;
}
