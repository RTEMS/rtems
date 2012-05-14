/*
 * unhex
 *      convert a hex file to binary equivalent.  If more than one file name
 *      is given, then the output will be logically concatenated together.
 *      stdin and stdout are defaults. Verbose will enable checksum output.
 *
 *  Supported input formats are Intel hex, Motorola S records, and TI 'B'
 *  records.
 *
 * Intel hex input format is
 *      Byte
 *       1          Colon :
 *       2..3       Record length, eg: "20"
 *       4..7       load address nibbles
 *       8..9       record type: "00" (data) or "02" base addr
 *       10..x      data bytes in ascii-hex
 *       x+1..x+2   cksum (2's compl of (len+addr+data))
 *       x+3        \n -- newline
 */

char *USAGE = "\
usage:    unhex [-va] [ -o file ] [ file [file ... ] ]\n\
                -v          -- verbose\n\
                -a base     -- 1st byte of output corresponds to this address\n\
                -l          -- linear, just writes data out\n\
                -o file     -- output file; must not be input file\n\
                -F k_bits   -- \"holes\" in input will be filled with 0xFF's\n\
                                  up to \"k_bits\" * 1024 bits\n\
";

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include "config.h"

#ifndef VMS
#ifndef HAVE_STRERROR
extern int sys_nerr;
extern char *sys_errlist[];

#define strerror( _err ) \
  ((_err) < sys_nerr) ? sys_errlist [(_err)] : "unknown error"

#else   /* HAVE_STRERROR */
char *strerror ();
#endif
#else   /* VMS */
char *strerror (int,...);
#endif


#define OK      0
#define FAILURE (-1)
#define Failed(x)       ((x) == FAILURE)
#define TRUE    1
#define FALSE   0
typedef char bool;
#define STREQ(a,b)      (strcmp(a,b) == 0)

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

/*
 * Pick out designated bytes
 */

#define B0(x)       ((x) & 0xff)
#define B1(x)       B0((x) >> 8)
#define B2(x)       B0((x) >> 16)
#define B3(x)       B0((x) >> 24)

typedef struct buffer_rec {
    u32 dl_destaddr;
    u32 dl_jumpaddr;
    int dl_count;
    u8  dl_buf[512];
} buffer_rec;

/*
 * vars controlled by command line options
 */

bool verbose = FALSE;                   /* be verbose */
bool linear = FALSE;                    /* just write out linear data */
char *outfilename = "-";                /* default output is stdout */
u32 base = 0L;                         /* base address */
u32 FFfill = 0L;                       /* how far to fill w 0xFF's */

extern char *optarg;                    /* getopt(3) control vars */
extern int optind;

char *progname;                         /* for error() */

void error(int errn, ...);
#define ERR_ERRNO  (1<<((sizeof(int) * 8) - 2)) /* hi bit; use 'errno' */
#define ERR_FATAL  (ERR_ERRNO / 2)              /* error is fatal; no return */
#define ERR_ABORT  (ERR_ERRNO / 4)              /* error is fatal; abort */
#define ERR_MASK   (ERR_ERRNO | ERR_FATAL | ERR_ABORT) /* all */

#ifdef HAVE_STRTOUL
#define stol(p) strtoul(p, (char **) NULL, 0)
#else
#define stol(p) strtol(p, (char **) NULL, 0)
#endif

int   unhex(FILE *ifp, char *inm, FILE *ofp, char *onm);
int   convert_Intel_records(FILE *ifp, char *inm, FILE *ofp, char *onm);
int   convert_S_records(FILE *ifp, char *inm, FILE *ofp, char *onm);
int   convert_TI_records(FILE *ifp, char *inm, FILE *ofp, char *onm);
void  write_record(buffer_rec *tb, FILE *fp);
int   getnibble(char **p);
int   getbyte(char **p);
long  getNbytes(char **p, int n);
void  badformat(char *s, char *fname, char *msg);

#define get1bytes(p)    ((int) getbyte(p))
#define get2bytes(p)    ((int) getNbytes(p, 2))
#define get3bytes(p)    getNbytes(p, 3)
#define get4bytes(p)    getNbytes(p, 4)

char *BADADDR = "Invalid record address";
char *BADLEN  = "Invalid record length";
char *BADBASE = "Bad base or starting address";
char *BADFMT =  "Unrecognized record type";
char *BADDATA = "Invalid data byte";
char *BADCSUM = "Invalid checksum";
char *MISCSUM = "Checksum mismatch";
char *BADTYPE = "Unrecognized record type";
char *MISTYPE = "Incompatible record types";

int main(
  int argc,
  char **argv
)
{
    register int c;
    bool showusage = FALSE;                     /* usage error? */
    int rc = 0;
    FILE *outfp, *infp;

    /*
     * figure out invocation leaf-name
     */

    if ((progname = strrchr(argv[0], '/')) == (char *) NULL)
        progname = argv[0];
    else
        progname++;

    argv[0] = progname;                         /* for getopt err reporting */

    /*
     *  Check options and arguments.
     */

    progname = argv[0];
    while ((c = getopt(argc, argv, "F:a:o:vl")) != EOF)
        switch (c)
        {
            case 'a':                           /* base address */
                base = stol(optarg);
                break;

            case 'l':                           /* linear output */
                linear = TRUE;
                break;

            case 'v':                           /* toggle verbose */
                verbose = ! verbose;
                break;

            case 'o':                           /* output file */
                outfilename = optarg;
                break;

            case 'F':                           /* 0xFF fill amount (bytes) */
                FFfill = stol(optarg) * 1024L / 8L;
                break;

            case '?':
                showusage = TRUE;
        }

    if (showusage)
    {
        (void) fprintf(stderr, "%s", USAGE);
        exit(1);
    }

    if (linear && (base != 0))
    {
        error(0, "-l and -a may not be specified in combination");
        exit(1);
    }

    if (STREQ(outfilename, "-"))
    {
        outfp = stdout;
        outfilename = "stdout";
    }
    else
        if ((outfp = fopen(outfilename, "w")) == (FILE *) NULL)
        {
            error(-1, "couldn't open '%s' for output", outfilename);
            exit(1);
        }

    /*
     * Now process the input files (or stdin, if none specified)
     */

    if (argv[optind] == (char *) NULL)          /* just stdin */
        exit(unhex(stdin, "stdin", outfp, outfilename));
    else
        for (; (optarg = argv[optind]); optind++)
        {
            if (STREQ(optarg, "-"))
                rc += unhex(stdin, "stdin", outfp, outfilename);
            else
            {
                if ((infp = fopen(optarg, "r")) == (FILE *) NULL)
                {
                    error(-1, "couldn't open '%s' for input", optarg);
                    exit(1);
                }
                rc += unhex(infp, optarg, outfp, outfilename);
            }
        }

    return(rc);
}

u16 filesum;

int
unhex(FILE *ifp,
      char *inm,
      FILE *ofp,
      char *onm)
{
    int c;

    filesum = 0;

    /*
     * Make sure holes will be filled with 0xFF's if requested.  We
     *  do this the easy way by just filling the file with FF's before
     *  getting started.  To do it more optimally would be quite a bit
     *  more difficult since the user can skip around as much as he/she
     *  likes in the input hex file addressing.
     *
     *  We'll clean this up later (after this program has run) with
     *  'stripffs'
     */

    if (FFfill)
    {
        (void) fseek(ofp, 0, 0);
        for (c = FFfill; c > 0; c--)
            (void) fputc(0xFF, ofp);
    }

    /*
     * Read the first char from file and determine record types
     */

    if ((c = getc(ifp)) != EOF)
    {
        ungetc(c, ifp);
        switch(c)
        {
            case 'S':
                convert_S_records(ifp, inm, ofp, onm);
                break;

            case ':':
                convert_Intel_records(ifp, inm, ofp, onm);
                break;

            case '9':
            case 'B':
                convert_TI_records(ifp, inm, ofp, onm);
                break;

            default:
            {
                char tmp[2];
                tmp[0] = c; tmp[1] = 0;
                badformat(tmp, inm, BADFMT);
            }
        }
    }

    if (verbose)
        fprintf(stderr, "'%s' checksum is 0x%04x\n", inm, filesum);

    return 0;
}

int
convert_Intel_records(
    FILE *ifp,
    char *inm,
    FILE *ofp,
    char *onm)
{
    char buff[512];
    char *p;
    u8 cksum;
    int incksum;
    int c;
    int rectype;                    /* record type */
    int len;                        /* data length of current line */
    u32 addr;
    u32 base_address = 0;
    bool endrecord = FALSE;
    buffer_rec tb;

    while ( ! endrecord && (fgets(buff, sizeof(buff), ifp)))
    {
        p = &buff[0];

        if (p[strlen(p)-1] == '\n')                 /* get rid of newline */
            p[strlen(p)-1] = '\0';

        if (p[strlen(p)-1] == '\r')                 /* get rid of any CR */
            p[strlen(p)-1] = '\0';

        tb.dl_count = 0;

        if (*p != ':')
            badformat(p, inm, BADFMT);
        p++;

        if ((len = getbyte(&p)) == -1)      /* record len */
            badformat(buff, inm, BADLEN);

        if ((addr = get2bytes(&p)) == -1L)          /* record addr */
            badformat(buff, inm, BADADDR);

        rectype = getbyte(&p);

        cksum = len + B0(addr) + B1(addr) + rectype;

        switch (rectype)
        {
            case 0x00:                  /* normal data record */
                tb.dl_destaddr = base_address + addr;
                while (len--)
                {
                    if ((c = getbyte(&p)) == -1)
                        badformat(buff, inm, BADDATA);
                    cksum += c;
                    filesum += c;
                    tb.dl_buf[tb.dl_count++] = c;
                }
                break;

            case 0x01:                  /* execution start address */
                base_address = addr;
                endrecord = TRUE;
                break;

            case 0x02:                  /* new base */
                if ((base_address = get2bytes(&p)) == -1L)
                    badformat(buff, inm, BADBASE);
                cksum += B0(base_address) + B1(base_address);
                base_address <<= 4;
                break;

            case 0x03:                  /* seg/off execution start address */
            {
                u32 seg, off;

                seg = get2bytes(&p);
                off = get2bytes(&p);
                if ((seg == -1L) || (off == -1L))
                    badformat(buff, inm, BADADDR);

                cksum += B0(seg) + B1(seg) + B0(off) + B1(off);

                tb.dl_jumpaddr = (seg << 4) + off;
                break;
            }

            default:
                error(0, "unknown Intel-hex record type: 0x%02x", rectype);
                badformat(buff, inm, BADTYPE);
        }

        /*
         * Verify checksums are correct in file.
         */

        cksum = (-cksum) & 0xff;
        if ((incksum = getbyte(&p)) == -1)
            badformat(buff, inm, BADCSUM);
        if (((u8) incksum) != cksum)
            badformat(buff, inm, MISCSUM);

        if (tb.dl_count)
            write_record(&tb, ofp);
    }
    return 0;
}

int
convert_S_records(
    FILE *ifp,
    char *inm,
    FILE *ofp,
    char *onm)
{
    char buff[512];
    char *p;
    u8 cksum;
    int incksum;
    int c;
    int len;                        /* data length of current line */
    int rectype;                    /* record type */
    u32 addr;
    bool endrecord = FALSE;
    buffer_rec tb;

    while ( ! endrecord && (fgets(buff, sizeof(buff), ifp)))
    {
        p = &buff[0];

        if (p[strlen(p)-1] == '\n')                 /* get rid of newline */
            p[strlen(p)-1] = '\0';

        if (p[strlen(p)-1] == '\r')                 /* get rid of any CR */
            p[strlen(p)-1] = '\0';

        tb.dl_count = 0;

        if (*p != 'S')
            badformat(p, inm, BADFMT);
        p++;

        if ((rectype = getnibble(&p)) == -1)        /* record type */
            badformat(buff, inm, BADTYPE);

        if ((len = getbyte(&p)) == -1)              /* record len */
            badformat(buff, inm, BADLEN);
        cksum = len;

        switch (rectype)
        {
            case 0x00:                  /* comment field, ignored */
                goto write_it;

            case 0x01:                          /* data record, 16 bit addr */
                if ((addr = get2bytes(&p)) == -1L)
                    badformat(buff, inm, BADADDR);
                len -= 3;
                goto doit;

            case 0x02:                          /* ... 24 bit addr */
                if ((addr = get3bytes(&p)) == -1L)
                    badformat(buff, inm, BADADDR);
                len -= 4;
                goto doit;

            case 0x03:                          /* ... 32 bit addr */
                if ((addr = get4bytes(&p)) == -1L)
                    badformat(buff, inm, BADADDR);
                len -= 5;
    doit:
                cksum += B0(addr) + B1(addr) + B2(addr) + B3(addr);

                tb.dl_destaddr = addr;
                while (len--)
                {
                    if ((c = getbyte(&p)) == -1)
                        badformat(buff, inm, BADDATA);
                    cksum += c;
                    filesum += c;
                    tb.dl_buf[tb.dl_count++] = c;
                }
                break;

            case 0x07:                  /* 32 bit end record */
                if ((addr = get4bytes(&p)) == -1L)
                    badformat(buff, inm, BADADDR);
                goto end_rec;

            case 0x08:                  /* 24 bit end record */
                if ((addr = get3bytes(&p)) == -1L)
                    badformat(buff, inm, BADADDR);
                goto end_rec;

            case 0x09:                  /* 16 bit end record */
                if ((addr = get2bytes(&p)) == -1L)
                    badformat(buff, inm, BADADDR);

end_rec:
                cksum += B0(addr) + B1(addr) + B2(addr) + B3(addr);
                tb.dl_jumpaddr = addr;
                break;

            default:
                error(0, "unknown Motorola-S record type: 0x%02x", rectype);
                badformat(buff, inm, BADTYPE);
                break;
        }

        /*
         * Verify checksums are correct in file.
         */

        cksum = (~cksum) & 0xff;
        if ((incksum = getbyte(&p)) == -1)
            badformat(buff, inm, BADCSUM);
        if (((u8) incksum) != cksum)
            badformat(buff, inm, MISCSUM);

write_it:
        if (tb.dl_count)
            write_record(&tb, ofp);
    }
    return 0;
}

int
convert_TI_records(
    FILE *ifp,
    char *inm,
    FILE *ofp,
    char *onm)
{
    char buff[512];
    char *p;
    int c;
    bool endrecord = FALSE;
    bool eol;
    buffer_rec tb;

    while ( ! endrecord && (fgets(buff, sizeof(buff), ifp)))
    {
        if (p[strlen(p)-1] == '\n')                 /* get rid of newline */
            p[strlen(p)-1] = '\0';

        if (p[strlen(p)-1] == '\r')                 /* get rid of any CR */
            p[strlen(p)-1] = '\0';

        tb.dl_count = 0;

        p = &buff[0];
        eol = FALSE;
        while ( ! eol && ! endrecord)
        {
            switch (*p++)
            {
                case '9':
                    if (tb.dl_count)
                        write_record(&tb, ofp);
                    tb.dl_destaddr = get2bytes(&p);
                    break;

                case 'B':
                    c = getbyte(&p);
                    filesum += c;
                    tb.dl_buf[tb.dl_count++] = c;
                    c = getbyte(&p);
                    filesum += c;
                    tb.dl_buf[tb.dl_count++] = c;
                    break;

                case 'F':
                    eol = TRUE;
                    break;

                case ':':
                    endrecord = TRUE;
                    break;

                default:
                    badformat(p, inm, BADFMT);
            }
        }
        if (tb.dl_count)
            write_record(&tb, ofp);
    }
    return 0;
}

void
write_record(buffer_rec *tb,
             FILE *fp)
{
    if ( ! linear)
    {
        if (tb->dl_destaddr < base)
            error(ERR_FATAL, "record at address 0x%x precedes base of 0x%x",
                     tb->dl_destaddr, base);
        (void) fseek(fp, tb->dl_destaddr - base, 0);
    }

    (void) fwrite(tb->dl_buf, tb->dl_count, 1, fp);
    tb->dl_destaddr += tb->dl_count;
    tb->dl_count = 0;
}

int
getnibble(char **p)
{
    register int val;

    **p = toupper(**p);
    switch (**p)
    {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            val = **p - '0';
            break;

        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            val = 10 + (**p - 'A');
            break;

        default:
            return(-1);
    }
    *p += 1;

    return(val & 0x0f);
}

int
getbyte(char **p)
{
    int n0, n1;

    if ((n0 = getnibble(p)) == -1)
        return(-1);
    if ((n1 = getnibble(p)) == -1)
        return(-1);

    return(((n0 << 4) + n1) & 0xff);
}

long
getNbytes(char **p,
          int n)
{
    int t;
    u32 val = 0;

    while (n--)
    {
        if ((t = getbyte(p)) == -1)
            return(-1L);
        val <<= 8;
        val += t;
    }

    return(val);
}

void
badformat(char *s,
          char *fname,
          char *msg)
{
    if (s[strlen(s)-1] == '\n')             /* get rid of newline */
        s[strlen(s)-1] = '\0';
    error(0, "line '%s'::\n\tfrom file '%s'; %s", s, fname, msg);
    exit(1);
}

/*
 * error(errn, arglist)
 *      report an error to stderr using printf(3) conventions.
 *      Any output is preceded by '<progname>: '
 *
 * Uses ERR_EXIT  bit to request exit(errn)
 *      ERR_ABORT to request abort()
 *      ERR_ERRNO to indicate use of errno instead of argument.
 *
 * If resulting 'errn' is non-zero, it is assumed to be an 'errno' and its
 *      associated error message is appended to the output.
 */

/*VARARGS*/

void
error(int error_flag, ...)
{
    va_list arglist;
    register char *format;
    int local_errno;

    extern int errno;

    (void) fflush(stdout);          /* in case stdout/stderr same */

    local_errno = error_flag & ~ERR_MASK;
    if (error_flag & ERR_ERRNO)     /* use errno? */
        local_errno = errno;

    va_start(arglist, error_flag);
    format = va_arg(arglist, char *);
    (void) fprintf(stderr, "%s: ", progname);
    (void) vfprintf(stderr, format, arglist);
    va_end(arglist);

    if (local_errno)
      (void) fprintf(stderr, " (%s)\n", strerror(local_errno));
    else
      (void) fprintf(stderr, "\n");

    (void) fflush(stderr);

    if (error_flag & (ERR_FATAL | ERR_ABORT))
    {
        if (error_flag & ERR_FATAL)
        {
            error(0, "fatal error, exiting");
            exit(local_errno ? local_errno : 1);
        }
        else
        {
            error(0, "fatal error, aborting");
            abort();
        }
    }
}
