
#include <stdio.h>
#include <stdlib.h>

/*
 * This function will patch binary file
 */


static char buf[512];

static void
usage(void)
{
  printf("usage: binpatch [-h] <ofile> <ifile> <reloc> <off> <byte0> "
         "[<byte1> [<byte2> [<byte3>]]]\n");
  printf("this function patches binary file at specified offset with\n");
  printf("up to 4 bytes provided on command line \n");
  printf("-h       - prints this message\n\n");
  printf("<ofile>  - output file\n");
  printf("<ifile>  - input ifile\n");
  printf("<reloc>  - relocation address of image\n");
  printf("<off>    - offset of patch, offset in file is at off - reloc\n");
  printf("<byte0>  - byte 0 of patch\n");
  printf("<byte1>  - byte 1 of patch\n");
  printf("<byte2>  - byte 1 of patch\n");
  printf("<byte3>  - byte 1 of patch\n");

  return;
}

int
main(int argc, char **argv)
{
  int   c;
  FILE  *ofp, *ifp;
  char  patch[4], *end;
  int   patchLen, tmp, i, off, cnt, patched, len, reloc;


  /* parse command line options */
  while ((c = getopt(argc, argv, "h")) >= 0)
    {
      switch (c)
	{
	case 'h':
	  usage();
	  return 0;
	default:
	  usage();
	  return 1;
	}
    }

  if(argc < 6)
    {
      usage();
      return 1;
    }

  /* Let us get offset in file */
  reloc = strtol(argv[3], &end, 0);
  if(end == argv[3] || off < 0)
    {
      fprintf(stderr, "bad reloc value %s\n", argv[3]);
      return 1;
    }

  off = strtol(argv[4], &end, 0);
  if(end == argv[4] || off < 0 || off < reloc)
    {
      fprintf(stderr, "bad offset value %s\n", argv[4]);
      return 1;
    }

  off -= reloc;

  /* Let us get patch  */
  patchLen = argc - 5;

  for(i=0; i<patchLen; i++)
    {
      tmp = strtol(argv[5+i], &end, 0);

      if(end == argv[4+i] || tmp < 0 || tmp > 0xff)
	{
	  fprintf(stderr, "bad byte value %s\n", argv[5+i]);
	  return 1;
	}
      patch[i] = tmp;
    }

  ifp = fopen(argv[2], "r");
  if(ifp == NULL)
    {
      fprintf(stderr, "unable to open file %s\n", argv[2]);
      return 1;
    }

  ofp = fopen(argv[1], "w");
  if(ofp == NULL)
    {
      fprintf(stderr, "unable to open file %s\n", argv[1]);
      return 1;
    }

  cnt     = 0;
  patched = 0;
  for(;;)
    {
      len = fread(buf, 1, sizeof(buf), ifp);

      if(len == 0)
	{
	  break;
	}

      if(cnt <= off && (cnt + len) > off)
	{
	  /* Perform patch */
	  for(i=0; i<patchLen && (off+i)<(cnt+len); i++)
	    {
	      buf[off-cnt+i] = patch[i];
	    }
	  patched = 1;
	}
      else if(cnt > off && cnt < (off + patchLen))
	{
	  /* Perform patch */
	  for(i=cnt-off; i<patchLen; i++)
	    {
	      buf[off-cnt+i] = patch[i];
	    }
	  patched = 1;
	}

      fwrite(buf, 1, len, ofp);

      cnt += len;
    }

  fclose(ifp);
  fclose(ofp);

  if(!patched)
    {
      fprintf(stderr, "warning: offset is beyond input file length\n");
      fprintf(stderr, "         no patch is performed\n");
    }

  return 0;
}
