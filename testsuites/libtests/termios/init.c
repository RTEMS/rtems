/*
 * RTEMS configuration/initialization
 *
 * This program may be distributed and used for any purpose.
 * I ask only that you:
 *  1. Leave this author information intact.
 *  2. Document any changes you make.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 * Additions:
 * Charles-Antoine Gauthier
 * Software Engineering Group
 * Institute for Information Technology
 * National Research Council of Canada
 * charles.gauthier@nrc.ca
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>

#include <rtems/shell.h>

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <string.h>

#include <tmacros.h>

const char rtems_test_name[] = "TERMIOS";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void print_32bits(unsigned long bits, unsigned char size, char * names[]);
void print_c_iflag(struct termios * tp);
void print_c_oflag(struct termios * tp);
void print_c_lflag(struct termios * tp);
void print_c_cflag(struct termios * tp);
void print_c_cc(struct termios * tp);
void print_baud(const char* name, speed_t spd);
void print_termios(struct termios *tp);
unsigned long get_baud_rate(void);
unsigned long get_parity(void);
unsigned long get_stop_bits(void);
unsigned long get_data_bits(void);
void change_line_settings(struct termios *tp);
void canonical_input(struct termios *tp);
void do_raw_input(int vmin, int vtime);
void usage(void);

#if !defined(fileno)
int fileno( FILE *stream); /* beyond ANSI */
#endif

/* Some of the termios dumping code depends on bit positions! */

void print_32bits( unsigned long bits, unsigned char size, char * names[] )
{
  unsigned char i;

  for( i = 0; i < size; i++ ) {
    if( (bits >> i) & 0x1 )
      printf( "%s ", names[i] );
  }
}


void print_c_iflag( struct termios * tp )
{
  char * c_iflag_bits [] = {
    "IGNBRK",   /* 0000001 */
    "BRKINT",   /* 0000002 */
    "IGNPAR",   /* 0000004 */
    "PARMRK",   /* 0000010 */
    "INPCK",    /* 0000020 */
    "ISTRIP",   /* 0000040 */
    "INLCR",    /* 0000100 */
    "IGNCR",    /* 0000200 */
    "ICRNL",    /* 0000400 */
    "IUCLC",    /* 0001000 */
    "IXON",     /* 0002000 */
    "IXANY",    /* 0004000 */
    "IXOFF",    /* 0010000 */
    "IMAXBEL",  /* 0020000 */
    "unknown",  /* 0040000 */
    "unknown",  /* 0100000 */
    "unknown",  /* 0200000 */
    "unknown",  /* 0400000 */
    "unknown",  /* 1000000 */
    "unknown",  /* 2000000 */
    "unknown"   /* 4000000 */
  };

  printf( "c_iflag = 0x%08x\n\t", tp->c_iflag );
  print_32bits( tp->c_iflag, sizeof( c_iflag_bits )/sizeof( char * ), c_iflag_bits );
  printf( "\n" );
}


void print_c_oflag( struct termios * tp )
{
  printf( "c_oflag = 0x%08x\n\t", tp->c_oflag );

  if( tp->c_oflag & OPOST )
    printf( "OPOST " );

  if( tp->c_oflag & OLCUC )
    printf( "OLCUC " );

  if( tp->c_oflag & ONLCR )
    printf( "ONLCR " );

  if( tp->c_oflag & ONOEOT )
    printf( "ONOEOT " );

  if( tp->c_oflag & OCRNL )
    printf( "OCRNL " );

  if( tp->c_oflag & ONOCR )
    printf( "ONOCR " );

  if( tp->c_oflag & ONLRET )
    printf( "ONLRET " );

  if( tp->c_oflag & OFILL )
    printf( "OFILL " );

  if( tp->c_oflag & OFDEL )
    printf( "OFDEL " );

  switch( tp->c_oflag & NLDLY ) {
    case NL0:
      printf( "NL0 " );
      break;

    case NL1:
      printf( "NL1 " );
      break;
  }

  switch( tp->c_oflag & CRDLY ) {
    case CR0:
      printf( "CR0 " );
      break;

    case CR1:
      printf( "CR1 " );
      break;

    case CR2:
      printf( "CR2 " );
      break;

    case CR3:
      printf( "CR3 " );
      break;
  }

  switch( tp->c_oflag & TABDLY ) {
    case TAB0:
      printf( "TAB0 " );
      break;

    case TAB1:
      printf( "TAB1 " );
      break;

    case TAB2:
      printf( "TAB2 " );
      break;

    case TAB3:
      printf( "TAB3 " );
      break;
  }

  switch( tp->c_oflag & BSDLY ) {
    case BS0:
      printf( "BS0 " );
      break;

    case BS1:
      printf( "BS1 " );
      break;
  }

  switch( tp->c_oflag & VTDLY ) {
    case VT0:
      printf( "VT0 " );
      break;

    case VT1:
      printf( "VT1 " );
      break;
  }

  switch( tp->c_oflag & FFDLY ) {
    case FF0:
      printf( "FF0" );
      break;

    case FF1:
      printf( "FF1" );
      break;
  }
  printf( "\n" );
}


void print_c_lflag( struct termios * tp )
{
  char * c_lflag_bits [] = {
    "ECHOKE",      /* 0x00000001 */
    "ECHOE",       /* 0x00000002 */
    "ECHOK",       /* 0x00000004 */
    "ECHO",        /* 0x00000008 */
    "ECHONL",      /* 0x00000010 */
    "ECHOPRT",     /* 0x00000020 */
    "ECHOCTL",     /* 0x00000040 */
    "ISIG",        /* 0x00000080 */
    "ICANON",      /* 0x00000100 */
    "ALTWERASE",   /* 0x00000200 */
    "IEXTEN",      /* 0x00000400 */
    "EXTPROC",     /* 0x00000800 */
    "XCASE",       /* 0x00001000 */
    "unknown",     /* 0x00002000 */
    "unknown",     /* 0x00004000 */
    "unknown",     /* 0x00008000 */
    "unknown",     /* 0x00010000 */
    "unknown",     /* 0x00020000 */
    "unknown",     /* 0x00040000 */
    "unknown",     /* 0x00080000 */
    "unknown",     /* 0x00100000 */
    "unknown",     /* 0x00200000 */
    "TOSTOP",      /* 0x00400000 */
    "FLUSHO",      /* 0x00800000 */
    "unknown",     /* 0x01000000 */
    "NOKERNINFO",  /* 0x02000000 */
    "unknown",     /* 0x04000000 */
    "unknown",     /* 0x08000000 */
    "unknown",     /* 0x10000000 */
    "PENDIN",      /* 0x20000000 */
    "unknown",     /* 0x40000000 */
    "NOFLSH",      /* 0x80000000 */
  };

  printf( "c_lflag = 0x%08x\n\t", tp->c_lflag );
  print_32bits( tp->c_lflag, sizeof( c_lflag_bits )/sizeof( char * ), c_lflag_bits );
  printf( "\n" );
}


void print_c_cflag( struct termios * tp )
{
  printf( "c_cflag = 0x%08x\n", tp->c_cflag );

  switch( tp->c_cflag & CSIZE ) {
    case CS5:
      printf( "\tCSIZE =\tCS5\n" );
      break;

    case CS6:
      printf( "\tCSIZE =\tCS6\n" );
      break;

    case CS7:
      printf( "\tCSIZE =\tCS7\n" );
      break;

    case CS8:
      printf( "\tCSIZE =\tCS8\n" );
      break;
  }

  if( tp->c_cflag & CIGNORE )
    printf( "\tCIGNORE set: iqnore c_cflags enabled\n" );
  else
    printf( "\tCIGNORE clear: iqnore c_cflags disabled\n" );

  if( tp->c_cflag & CSTOPB )
    printf( "\tCSTOPB set: send 2 stop bits\n" );
  else
    printf( "\tCSTOPB clear: send 1 stop bit\n" );

  if( tp->c_cflag & PARENB )
    printf( "\tPARENB set: parity enabled\n" );
  else
    printf( "\tPARENB clear: parity disabled\n" );

  if( tp->c_cflag & PARODD )
    printf( "\tPARODD set: parity odd\n" );
  else
    printf( "\tPARODD clear: parity even\n" );

  if( tp->c_cflag & CREAD )
    printf( "\tCREAD set: receiver enabled\n" );
  else
    printf( "\tCREAD clear: treceiver disabled\n" );

  if( tp->c_cflag & HUPCL )
    printf( "\tHUPCL set: enabled\n" );
  else
    printf( "\tHUPCL clear: disabled\n" );

  if( tp->c_cflag & CLOCAL )
    printf( "\tCLOCAL set: ignore modem lines\n" );
  else
    printf( "\tCLOCAL clear: don't ignore modem lines\n" );

  if( tp->c_cflag & CCTS_OFLOW )
    printf( "\tCCTS_OFLOW: hardware CTS output flow control enabled\n" );
  else
    printf( "\tCCTS_OFLOW: hardware CTS output flow control disabled\n" );

  if( tp->c_cflag & CRTS_IFLOW )
    printf( "\tCRTS_IFLOW: hardware RTS input flow control enabled\n" );
  else
    printf( "\tCRTS_IFLOW: hardware RTS input flow control disabled\n" );

  if( tp->c_cflag & CRTSCTS )
    printf( "\tCRTSCTS: harware flow control enabled?\n" );
  else
    printf( "\tCRTSCTS: hardware flow control disabled?\n" );

  if( tp->c_cflag & CDSR_OFLOW )
    printf( "\tCDSR_OFLOW: hardware DSR output flow control enabled\n" );
  else
    printf( "\tCDSR_OFLOW: hardware DSR output flow control disabled\n" );

  if( tp->c_cflag & CDTR_IFLOW )
    printf( "\tCDTR_IFLOW: hardware DTR input flow control enabled\n" );
  else
    printf( "\tCDTR_IFLOW: hardware DTR input flow control disabled\n" );

  if( tp->c_cflag & CCAR_OFLOW )
    printf( "\tCCAR_OFLOW: hardware CD output flow control enabled\n" );
  else
    printf( "\tCCAR_OFLOW: hardware CD output flow control disabled\n" );
}


void print_c_cc( struct termios * tp )
{
  size_t i;
  char * cc_index_names [ /* NCCS */ ] = {
    "[VEOF]    ",   /* 0 */
    "[VEOL]    ",   /* 1 */
    "[VEOL2]   ",   /* 2 */
    "[VERASE]  ",   /* 3 */
    "[VWERASE] ",   /* 4 */
    "[VKILL]   ",   /* 5 */
    "[VREPRINT]",   /* 6 */
    "[VERASE2] ",   /* 7 */
    "[VINTR]   ",   /* 8 */
    "[VQUIT]   ",   /* 9 */
    "[VSUSP]   ",   /* 10 */
    "[VDSUSP]  ",   /* 11 */
    "[VSTART]  ",   /* 12 */
    "[VSTOP]   ",   /* 13 */
    "[VLNEXT]  ",   /* 14 */
    "[VDISCARD]",   /* 15 */
    "[VMIN]    ",   /* 16 */
    "[VTIME]   ",   /* 17 */
    "[VSTATUS] ",   /* 18 */
    "unknown   ",   /* 19 */
  };

  for( i = 0; i < sizeof(cc_index_names)/sizeof(char*) ; i++ ) {
    printf( "c_cc%s = 0x%08x\n", cc_index_names[i], tp->c_cc[i] );
  }
}


void print_baud( const char* name, speed_t spd )
{
  switch( spd ) {
  case B0:
    printf( "%s = B0\n", name );
    break;

  case B50:
    printf( "%s = B50\n", name );
    break;

  case B75:
    printf( "%s = B75\n", name );
    break;

  case B110:
    printf( "%s = B110\n", name );
    break;

  case B134:
    printf( "%s = B134\n", name );
    break;

  case B150:
    printf( "%s = B150\n", name );
    break;

  case B200:
    printf( "%s = B200\n", name );
    break;

  case B300:
    printf( "%s = B300\n", name );
    break;

  case B600:
    printf( "%s = B600\n", name );
    break;

  case B1200:
    printf( "%s = B1200\n", name );
    break;

  case B1800:
    printf( "%s = B1800\n", name );
    break;

  case B2400:
    printf( "%s = B2400\n", name );
    break;

  case B4800:
    printf( "%s = B4800\n", name );
    break;

  case B9600:
    printf( "%s = B9600\n", name );
    break;

  case B19200:
    printf( "%s = B19200\n", name );
    break;

  case B38400:
    printf( "%s = B38400\n", name );
    break;

  case B7200:
    printf( "%s = B7200\n", name );
    break;

  case B14400:
    printf( "%s = B14400\n", name );
    break;

  case B28800:
    printf( "%s = B28800\n", name );
    break;

  case B57600:
    printf( "%s = B57600\n", name );
    break;

  case B76800:
    printf( "%s = B76800\n", name );
    break;

  case B115200:
    printf( "%s = B115200\n", name );
    break;

  case B230400:
    printf( "%s = B230400\n", name );
    break;

  case B460800:
    printf( "%s = B460800\n", name );
    break;

  case B921600:
    printf( "%s = B921600\n", name );
    break;

  default:
    printf( "%s = unknown (0x%08x)\n", name, (unsigned int)spd );
    break;
  }
}


void print_termios( struct termios *tp )
{
  printf( "\nLooking at the current termios settings:\n\n" );
  print_c_iflag( tp );
  print_c_oflag( tp );
  print_c_cflag( tp );
  print_c_lflag( tp );
  print_c_cc( tp );
  print_baud( "c_ispeed", tp->c_ispeed );
  print_baud( "c_ospeed", tp->c_ospeed );
  printf( "\n" );
}


unsigned long get_baud_rate(void)
{
  unsigned long baud_rate;

  while( 1 ) {
    printf( "Enter the numerical value for the new baud rate.\n" );
    printf( "Choices are: 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800\n" );
    printf( "2400, 4800, 9600, 19200, 38400, 7200, 14400, 28800, 57600, 76800\n" );
    printf( "115200, 230400, 460800, 921600\n" );
    printf( "\nYour choice: " );
    scanf( "%lu", &baud_rate );
    printf( "\n" );
    switch( baud_rate ) {
      case 50:     return B50;
      case 75:     return B75;
      case 110:    return B110;
      case 134:    return B134;
      case 150:    return B150;
      case 200:    return B200;
      case 300:    return B300;
      case 600:    return B600;
      case 1200:   return B1200;
      case 1800:   return B1800;
      case 2400:   return B2400;
      case 4800:   return B4800;
      case 9600:   return B9600;
      case 19200:  return B19200;
      case 38400:  return B38400;
      case 7200:   return B7200;
      case 14400:  return B14400;
      case 28800:  return B28800;
      case 57600:  return B57600;
      case 76800:  return B76800;
      case 115200: return B115200;
      case 230400: return B230400;
      case 460800: return B460800;
      case 921600: return B921600;

      default:
        printf( "%lu is not a valid choice. Try again.\n\n", baud_rate );
        break;
    }
  }
}


unsigned long get_parity(void)
{
  int parity;

  while( 1 ) {
    printf( "Enter the numerical value for the new parity\n" );
    printf( "Choices are: 0 for no parity, 1 for even parity, 2 for odd parity\n" );
    printf( "\nYour choice: " );
    scanf( "%d", &parity );
    printf( "\n" );
    switch( parity ) {
      case 0:
        return 0;

      case 1:
        return PARENB;

      case 2:
        return PARENB | PARODD;

      default:
        printf( "%d is not a valid choice. Try again.\n\n", parity );
        break;
    }
  }
}


unsigned long get_stop_bits(void)
{
  int stop_bits;

  while( 1 ) {
    printf( "Enter the numerical value for the new number of stop bits\n" );
    printf( "Choices are: 1 or 2\n" );
    printf( "\nYour choice: " );
    scanf( "%d", &stop_bits );
    printf( "\n" );
    switch( stop_bits ) {
      case 1:
        return 0;

      case 2:
        return CSTOPB;

      default:
        printf( "%d is not a valid choice. Try again.\n\n", stop_bits );
        break;
    }
  }
}


unsigned long get_data_bits(void)
{
  int data_bits;

  while( 1 ) {
    printf( "Enter the numerical value for the new number of data bits\n" );
    printf( "Choices are: 5, 6, 7 or 8\n" );
    printf( "\nYour choice: " );
    scanf( "%d", &data_bits );
    printf( "\n" );
    switch( data_bits ) {
      case 5:
        return CS5;

      case 6:
        return CS6;

      case 7:
        return CS7;

      case 8:
        return CS8;

      default:
        printf( "%d is not a valid choice. Try again.\n\n", data_bits );
        break;
    }
  }
}


void change_line_settings( struct termios *tp )
{
  unsigned long baud_rate, parity, stop_bits, data_bits, sleep_time;

  printf( "\nSetting line characteristics\n\n" );

  baud_rate = get_baud_rate();
  parity = get_parity();
  stop_bits = get_stop_bits();
  data_bits = get_data_bits();

  printf( "NOTE: You will not see output until you switch your terminal settings!\n" );
  printf( "WARNING: If you do not switch your terminal settings, your terminal may hang.\n" );
  printf( "Enter the number of seconds the test will wait for you to switch your terminal\n" );
  printf( "settings before it continues\n" );
  printf( "Sleep time (in seconds): " );
  scanf( "%lu", &sleep_time );
  printf( "\n" );
  printf( "Setting line to new termios settings in %lu seconds.\n", sleep_time );

  sleep( sleep_time );

  tp->c_cflag = CLOCAL | CREAD | parity | stop_bits | data_bits;
  tp->c_ispeed = baud_rate;
  tp->c_ospeed = baud_rate;
  if( tcsetattr( fileno( stdin ), TCSADRAIN, tp ) < 0 ) {
    perror( "change_line_settings(): tcsetattr() failed" );
    rtems_test_exit( 1 );
  }
  printf( "Line settings set.\n" );
}


void canonical_input( struct termios *tp )
{
  char c;
  bool first_time = true;

  printf( "\nTesting canonical input\n\n" );

  printf( "Setting line to canonical input mode.\n" );
  tp->c_lflag = ISIG | ICANON | ECHO | ECHONL | ECHOK | ECHOE | ECHOPRT | ECHOCTL | IEXTEN;
  tp->c_iflag = BRKINT | ICRNL | IXON | IMAXBEL;
  if( tcsetattr( fileno( stdin ), TCSADRAIN, tp ) < 0 ) {
    perror( "canonical_input(): tcsetattr() failed" );
    rtems_test_exit( 1 );
  }

  while ( ( c = getchar () ) != '\n');
  printf( "Testing getchar(). Type some text followed by carriage return\n" );
  printf( "Each character you entered will be echoed back to you\n\n" );
  while ( ( c = getchar () ) != '\n') {
    if( first_time ) {
      printf( "\nYou typed:\n");
      first_time = false;
    }
    printf( "%c", c );
  }
  printf( "\n\nCanonical input test done.\n" );
}


/*
 * Test raw (ICANON=0) input
 */
void do_raw_input( int vmin, int vtime )
{
  int i;
  struct termios old, new;
  rtems_interval ticksPerSecond, then, now;
  unsigned int msec;
  unsigned long count;
  int nread;
  unsigned char cbuf[100];

  printf( "Raw input test with VMIN=%d  VTIME=%d\n", vmin, vtime );

  ticksPerSecond = rtems_clock_get_ticks_per_second();
  if ( tcgetattr( fileno ( stdin ), &old ) < 0 ) {
    perror( "do_raw_input(): tcgetattr() failed" );
    return;
  }

  new = old;
  new.c_lflag &= ~( ICANON | ECHO | ECHONL | ECHOK | ECHOE | ECHOPRT | ECHOCTL );
  new.c_cc[VMIN] = vmin;
  new.c_cc[VTIME] = vtime;
  if( tcsetattr( fileno( stdin ), TCSADRAIN, &new ) < 0 ) {
    perror ("do_raw_input(): tcsetattr() failed" );
    return;
  }

  do {
    then = rtems_clock_get_ticks_since_boot();
    count = 0;
    for(;;) {
      nread = read( fileno( stdin ), cbuf, sizeof cbuf );
      if( nread < 0 ) {
        perror( "do_raw_input(): read() failed" );
        goto out;
      }
      count++;
      if( nread != 0 )
        break;
    }
    now = rtems_clock_get_ticks_since_boot();
    msec = (now - then) * 1000 / ticksPerSecond;
    printf( "Count:%-10lu  Interval:%3u.%3.3d  Char:",
          count, msec / 1000, msec % 1000 );

    for( i = 0 ; i < nread ; i++ )
      printf (" 0x%2.2x", cbuf[i]);
    printf ("\n");

  } while( cbuf[0] != 'q' );

out:
  if( tcsetattr( fileno( stdin ), TCSADRAIN, &old) < 0 )
    perror("do_raw_input(): tcsetattr() failed: %s\n" );

  TEST_BEGIN();
}


static void raw_input( struct termios *tp )
{
  printf( "\nTesting raw input input\n\n" );
  printf( "Hit 'q' to terminate the test\n" );

  do_raw_input( 0, 0 );
  do_raw_input( 0, 20 );
  do_raw_input( 5, 0 );
  do_raw_input( 5, 20 );

  printf( "\nRaw input test done.\n" );
}


void usage( void )
{
  printf( "\nYou have the following choices:\n" );
  printf( "  1 - Reset the struct termios\n" );
  printf( "  2 - Look at the current termios setting\n" );
  printf( "  3 - Change the line characteristics\n" );
  printf( "  4 - Test canonical input\n" );
  printf( "  5 - Test raw input\n" );
  printf( "  9 - Exit\n" );
  printf( "Enter your choice (1 to 5 or 9, followed by a carriage return): " );
}

static void notification( int fd, int seconds_remaining, void *arg )
{
  printf(
    "Press any key to check the termios input capabilities (%is remaining)\n",
    seconds_remaining
  );
}

/*
 * RTEMS Startup Task
 */
rtems_task
Init (rtems_task_argument ignored)
{
  rtems_status_code status;
  char c ;
  struct termios orig_termios, test_termios;

  TEST_BEGIN();

  status = rtems_shell_wait_for_input(
    STDIN_FILENO,
    20,
    notification,
    NULL
  );
  if (status == RTEMS_SUCCESSFUL) {
    if( tcgetattr( fileno( stdin ), &orig_termios ) < 0 ) {
      perror( "tcgetattr() failed" );
      rtems_test_exit( 0 );
    }

    test_termios = orig_termios;

    usage();
    for(;;) {
      switch( c = getchar() ) {
        case '1':
          printf( "\nResetting the line to the original termios setting\n\n" );
          test_termios = orig_termios;
          if( tcsetattr( fileno( stdin ), TCSADRAIN, &test_termios ) < 0 ) {
            perror( "tcsetattr() failed" );
            rtems_test_exit( 1 );
          }
          usage();
          break;

        case '2':
          print_termios( &test_termios );
          usage();
          break;

        case '3':
          change_line_settings( &test_termios );
          usage();
          break;

        case '4':
          canonical_input( &test_termios );
          usage();
          break;

        case '5':
          raw_input( &test_termios );
          usage();
          break;

        case '9':
          rtems_test_exit( 1 );

        case '\n':
          break;

        default:
          printf( "\n%c is not a valid choice. Try again\n\n", c );
          usage();
          break;
      }
    }
  } else {
    TEST_END();

    rtems_test_exit( 0 );
  }
}

/* application configuration */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS       1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MICROSECONDS_PER_TICK 1000

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of configuration */

