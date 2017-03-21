/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"
#define TTYDEFCHARS
#include <termios.h>
#include <rtems/libcsupport.h>
#include <rtems/malloc.h>
#include <rtems/termiostypes.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/stat.h>

const char rtems_test_name[] = "TERMIOS 1";

/* rtems_termios_baud_t is a typedefs to int32_t */
#define PRIdrtems_termios_baud_t PRId32

/*
 *  Termios Test Driver
 */
#include "termios_testdriver.h"

static const rtems_driver_address_table test_driver =
  TERMIOS_TEST_DRIVER_TABLE_ENTRY;

/*
 *  Baud Rate Constant Mapping Entry
 */
typedef struct {
  tcflag_t constant;
  rtems_termios_baud_t baud;
} termios_baud_test_r;

#define INVALID_CONSTANT ((tcflag_t) -2)

#define INVALID_BAUD ((rtems_termios_baud_t) -2)
/*
 *  Baud Rate Constant Mapping Table
 */
static const termios_baud_test_r baud_table[] = {
  { B0,           0 },
  { B50,         50 },
  { B75,         75 },
  { B110,       110 },
  { B134,       134 },
  { B150,       150 },
  { B200,       200 },
  { B300,       300 },
  { B600,       600 },
  { B1200,     1200 },
  { B1800,     1800 },
  { B2400,     2400 },
  { B4800,     4800 },
  { B9600,     9600 },
  { B19200,   19200 },
  { B38400,   38400 },
  { B7200,     7200 },
  { B14400,   14400 },
  { B28800,   28800 },
  { B57600,   57600 },
  { B76800,   76800 },
  { B115200, 115200 },
  { B230400, 230400 },
  { B460800, 460800 },
  { B921600, 921600 },
  { INVALID_CONSTANT, INVALID_BAUD }
};

/*
 *  Character Size Constant Mapping Entry
 */
typedef struct {
  tcflag_t constant;
  int bits;
} termios_character_size_test_r;

/*
 *  Character Size Constant Mapping Table
 */
static const termios_character_size_test_r char_size_table[] = {
  { CS5,      5 },
  { CS6,      6 },
  { CS7,      7 },
  { CS8,      8 },
  { INVALID_CONSTANT, -1 }
};

/*
 *  Parity Constant Mapping Entry
 */
typedef struct {
  tcflag_t constant;
  const char *parity;
} termios_parity_test_r;

/*
 *  Parity Constant Mapping Table
 */
static const termios_parity_test_r parity_table[] = {
  { 0,                "none" },
  { PARENB,           "even" },
  { PARENB | PARODD,  "odd" },
  { INVALID_CONSTANT, NULL }
};

/*
 *  Stop Bit Constant Mapping Entry
 */
typedef struct {
  tcflag_t constant;
  int stop;
} termios_stop_bits_test_r;

/*
 *  Stop Bit Constant Mapping Table
 */
static const termios_stop_bits_test_r stop_bits_table[] = {
  { 0,       1 },
  { CSTOPB,  2 },
  { INVALID_CONSTANT, -1 }
};

/*
 *  Test converting baud rate into an index
 */
static void test_termios_baud2index(void)
{
  int i;
  int index;

  puts( "Test termios_baud2index..." );
  puts( "termios_baud_to_index(-2) - NOT OK" );
  i = rtems_termios_baud_to_index( INVALID_CONSTANT );
  rtems_test_assert( i == -1 );

  for (i=0 ; baud_table[i].constant != INVALID_CONSTANT ; i++ ) {
    printf(
      "termios_baud_to_index(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    index = rtems_termios_baud_to_index( baud_table[i].constant );
    if ( index != i ) {
      printf( "ERROR - returned %d should be %d\n", index, i );
      rtems_test_exit(0);
    }
  }
}

/*
 *  Test converting termios baud constant to baud number
 */
static void test_termios_baud2number(void)
{
  int i;
  rtems_termios_baud_t number;

  puts(
    "\n"
    "Test termios_baud2number..."
  );
  puts( "termios_baud_to_number(-2) - NOT OK" );
  number = rtems_termios_baud_to_number( INVALID_CONSTANT );
  rtems_test_assert( number == 0 );

  for (i=0 ; baud_table[i].constant != INVALID_CONSTANT ; i++ ) {
    printf(
      "termios_baud_to_number(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    number = rtems_termios_baud_to_number( baud_table[i].constant );
    if ( number != baud_table[i].baud ) {
      printf(
        "ERROR - returned %" PRIdrtems_termios_baud_t
        " should be %" PRIdrtems_termios_baud_t "\n",
        number,
        baud_table[i].baud
      );
      rtems_test_exit(0);
    }
  }
}

/*
 *  Test converting baud number to termios baud constant
 */
static void test_termios_number_to_baud(void)
{
  int i;
  tcflag_t termios_baud;

  puts(
    "\n"
    "Test termios_number_to_baud..."
  );
  puts( "termios_number_to_baud(-2) - NOT OK" );
  termios_baud = rtems_termios_number_to_baud( INVALID_BAUD );
  rtems_test_assert( termios_baud == 0 );

  for (i=0 ; baud_table[i].constant != INVALID_CONSTANT ; i++ ) {
    printf(
      "termios_number_to_baud(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    termios_baud = rtems_termios_number_to_baud( baud_table[i].baud );
    if ( termios_baud != baud_table[i].constant ) {
      printf(
        "ERROR - returned %d should be %d\n",
        termios_baud,
        baud_table[i].constant
      );
      rtems_test_exit(0);
    }
  }
}

/*
 *  Test all the baud rate options
 */
static void test_termios_set_baud(
  int test
)
{
  int             sc;
  int             i;
  struct termios  attr;

  puts( "Test termios setting device baud rate..." );
  for (i=0 ; baud_table[i].constant != INVALID_CONSTANT ; i++ ) {
    sc = tcgetattr( test, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }

    attr.c_ispeed = baud_table[i].constant;
    attr.c_ospeed = baud_table[i].constant;

    printf(
      "tcsetattr(TCSANOW, B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    sc = tcsetattr( test, TCSANOW, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }

    printf(
      "tcsetattr(TCSADRAIN, B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    sc = tcsetattr( test, TCSADRAIN, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }

    printf(
      "tcsetattr(TCSAFLUSH, B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    sc = tcsetattr( test, TCSAFLUSH, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }
  }
}

/*
 *  Test all the character size options
 */
static void test_termios_set_charsize(
  int test
)
{
  int             sc;
  int             i;
  struct termios  attr;

  puts(
    "\n"
    "Test termios setting device character size ..."
  );
  for (i=0 ; char_size_table[i].constant != INVALID_CONSTANT ; i++ ) {
    tcflag_t csize = CSIZE;

    sc = tcgetattr( test, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }

    attr.c_cflag &= ~csize;
    attr.c_cflag |= char_size_table[i].constant;

    printf( "tcsetattr(TCSANOW, CS%d) - OK\n", char_size_table[i].bits );
    sc = tcsetattr( test, TCSANOW, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }

    printf( "tcsetattr(TCSADRAIN, CS%d) - OK\n", char_size_table[i].bits );
    sc = tcsetattr( test, TCSADRAIN, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }

    printf( "tcsetattr(TCSAFLUSH, CS%d) - OK\n", char_size_table[i].bits );
    sc = tcsetattr( test, TCSAFLUSH, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }

    printf( "tcsetattr(TCSASOFT, CS%d) - OK\n", char_size_table[i].bits );
    sc = tcsetattr( test, TCSASOFT, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }
  }
}

/*
 *  Test all the parity options
 */
static void test_termios_set_parity(
  int test
)
{
  int             sc;
  int             i;
  struct termios  attr;

  puts(
    "\n"
    "Test termios setting device parity ..."
  );
  for (i=0 ; parity_table[i].constant != INVALID_CONSTANT ; i++ ) {
    tcflag_t par = PARENB | PARODD;

    sc = tcgetattr( test, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }

    attr.c_cflag &= ~par;
    attr.c_cflag |= parity_table[i].constant;

    printf( "tcsetattr(TCSANOW, %s) - OK\n", parity_table[i].parity );
    sc = tcsetattr( test, TCSANOW, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }

    printf( "tcsetattr(TCSADRAIN, %s) - OK\n", parity_table[i].parity );
    sc = tcsetattr( test, TCSADRAIN, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }

    printf( "tcsetattr(TCSAFLUSH, %s) - OK\n", parity_table[i].parity );
    sc = tcsetattr( test, TCSAFLUSH, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }

    printf( "tcsetattr(TCSASOFT, %s) - OK\n", parity_table[i].parity );
    sc = tcsetattr( test, TCSASOFT, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }
  }
}

/*
 *  Test all the stop bit options
 */
static void test_termios_set_stop_bits(
  int test
)
{
  int             sc;
  int             i;
  struct termios  attr;

  puts(
    "\n"
    "Test termios setting device character size ..."
  );
  for (i=0 ; stop_bits_table[i].constant != INVALID_CONSTANT ; i++ ) {
    tcflag_t cstopb = CSTOPB;

    sc = tcgetattr( test, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }

    attr.c_cflag &= ~cstopb;
    attr.c_cflag |= stop_bits_table[i].constant;

    printf( "tcsetattr(TCSANOW, %d bit%s) - OK\n",
      stop_bits_table[i].stop,
      ((stop_bits_table[i].stop == 1) ? "" : "s")
    );
    sc = tcsetattr( test, TCSANOW, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }

    printf( "tcsetattr(TCSADRAIN, %d bits) - OK\n", stop_bits_table[i].stop );
    sc = tcsetattr( test, TCSADRAIN, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }

    printf( "tcsetattr(TCSAFLUSH, %d bits) - OK\n", stop_bits_table[i].stop );
    sc = tcsetattr( test, TCSAFLUSH, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }

    printf( "tcsetattr(TCSASOFT, %d bits) - OK\n", stop_bits_table[i].stop );
    sc = tcsetattr( test, TCSASOFT, &attr );
    if ( sc != 0 ) {
      printf( "ERROR - return %d\n", sc );
      rtems_test_exit(0);
    }
  }
}

static void test_termios_cfoutspeed(void)
{
  int i;
  int sc;
  speed_t speed;
  struct termios term;
  speed_t bad;

  bad = B921600 << 1;
  memset( &term, '\0', sizeof(term) );
  puts( "cfsetospeed(BAD BAUD) - EINVAL" );
  sc = cfsetospeed( &term, bad );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  for (i=0 ; baud_table[i].constant != INVALID_CONSTANT ; i++ ) {
    memset( &term, '\0', sizeof(term) );
    printf(
      "cfsetospeed(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    sc = cfsetospeed( &term, baud_table[i].constant );
    rtems_test_assert( !sc );
    printf(
      "cfgetospeed(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    speed = cfgetospeed( &term );
    rtems_test_assert( speed == baud_table[i].constant );
  }
}

static void test_termios_cfinspeed(void)
{
  int             i;
  int             sc;
  speed_t         speed;
  struct termios  term;
  speed_t         bad;

  bad = B921600 << 1;
  memset( &term, '\0', sizeof(term) );
  puts( "cfsetispeed(BAD BAUD) - EINVAL" );
  sc = cfsetispeed( &term, bad );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  for (i=0 ; baud_table[i].constant != INVALID_CONSTANT ; i++ ) {
    memset( &term, '\0', sizeof(term) );
    printf(
      "cfsetispeed(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    sc = cfsetispeed( &term, baud_table[i].constant );
    rtems_test_assert( !sc );

    printf(
      "cfgetispeed(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    speed = cfgetispeed( &term );
    rtems_test_assert( speed == baud_table[i].constant );
  }
}

static void test_termios_cfsetspeed(void)
{
  int             i;
  int             status;
  speed_t         speed;
  struct termios  term;
  speed_t         bad;

  bad = B921600 << 1;
  memset( &term, '\0', sizeof(term) );
  puts( "cfsetspeed(BAD BAUD) - EINVAL" );
  status = cfsetspeed( &term, bad );
  rtems_test_assert( status == -1 );
  rtems_test_assert( errno == EINVAL );

  for (i=0 ; baud_table[i].constant != INVALID_CONSTANT ; i++ ) {
    memset( &term, '\0', sizeof(term) );
    printf(
      "cfsetspeed(B%" PRIdrtems_termios_baud_t ") - OK\n",
      baud_table[i].baud
    );
    status = cfsetspeed( &term, baud_table[i].constant );
    rtems_test_assert( !status );

    printf(
      "cfgetspeed(B%" PRIdrtems_termios_baud_t ") - checking both inspeed and outspeed - OK\n",
      baud_table[i].baud
    );
    speed = cfgetispeed( &term );
    rtems_test_assert( speed == baud_table[i].constant );

    speed = cfgetospeed( &term );
    rtems_test_assert( speed == baud_table[i].constant );
  }
}

static void test_termios_cfmakeraw(void)
{
  struct termios  term;

  memset( &term, '\0', sizeof(term) );
  cfmakeraw( &term );
  puts( "cfmakeraw - OK" );

  /* Check that all of the flags were set correctly */
  rtems_test_assert( ~(term.c_iflag & (IMAXBEL|IXOFF|INPCK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON|IGNPAR)) );

  rtems_test_assert( term.c_iflag & (IGNBRK) );

  rtems_test_assert( ~(term.c_oflag & OPOST) );

  rtems_test_assert( ~(term.c_lflag & (ECHO|ECHOE|ECHOK|ECHONL|ICANON|ISIG|IEXTEN|NOFLSH|TOSTOP|PENDIN)) );

  rtems_test_assert( ~(term.c_cflag & (CSIZE|PARENB)) );

  rtems_test_assert( term.c_cflag & (CS8|CREAD) );

  rtems_test_assert( term.c_cc[VMIN] == 1 );

  rtems_test_assert( term.c_cc[VTIME] == 0 );
}

static void test_termios_cfmakesane(void)
{
  struct termios  term;

  memset( &term, '\0', sizeof(term) );
  cfmakesane( &term );
  puts( "cfmakesane - OK" );

  /* Check that all of the flags were set correctly */
  rtems_test_assert( term.c_iflag == TTYDEF_IFLAG );

  rtems_test_assert( term.c_oflag == TTYDEF_OFLAG );

  rtems_test_assert( term.c_lflag == TTYDEF_LFLAG );

  rtems_test_assert( term.c_cflag == TTYDEF_CFLAG );

  rtems_test_assert( term.c_ispeed == TTYDEF_SPEED );

  rtems_test_assert( term.c_ospeed == TTYDEF_SPEED );

  rtems_test_assert( memcmp(&term.c_cc, ttydefchars, sizeof(term.c_cc)) == 0 );
}

typedef struct {
  rtems_termios_device_context base;
  bool done;
} device_context;

static rtems_status_code test_early_device_install(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *arg
)
{
  static const rtems_termios_device_handler handler;
  static const char dev[] = "/foobar";

  rtems_resource_snapshot snapshot;
  rtems_status_code sc;
  int fd;
  int rv;

  rtems_resource_snapshot_take( &snapshot );

  sc = rtems_termios_device_install( &dev[0], &handler, NULL, NULL );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  errno = 0;
  fd = open( &dev[0], O_RDWR );
  rtems_test_assert( fd == -1 );
  rtems_test_assert( errno == ENXIO );

  rv = unlink( &dev[0] );
  rtems_test_assert( rv == 0 );

  rtems_test_assert( rtems_resource_snapshot_check( &snapshot ) );

  return RTEMS_SUCCESSFUL;
}

static void test_device_install_remove(void)
{
  static const rtems_termios_device_handler handler;
  static const char dev[] = "/foobar";

  rtems_resource_snapshot snapshot;
  rtems_status_code sc;
  void *greedy;
  int rv;

  rtems_resource_snapshot_take( &snapshot );

  greedy = rtems_heap_greedy_allocate( NULL, 0 );

  sc = rtems_termios_device_install( "/", &handler, NULL, NULL );
  rtems_test_assert( sc == RTEMS_NO_MEMORY );

  rtems_heap_greedy_free( greedy );

  rtems_test_assert( rtems_resource_snapshot_check( &snapshot ) );

  sc = rtems_termios_device_install( "/", &handler, NULL, NULL );
  rtems_test_assert( sc == RTEMS_UNSATISFIED );

  rtems_test_assert( rtems_resource_snapshot_check( &snapshot ) );

  sc = rtems_termios_device_install( &dev[0], &handler, NULL, NULL );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  rv = unlink( &dev[0] );
  rtems_test_assert( rv == 0 );

  rtems_test_assert( rtems_resource_snapshot_check( &snapshot ) );
}

static bool first_open_error(
  rtems_termios_tty *tty,
  rtems_termios_device_context *base,
  struct termios *term,
  rtems_libio_open_close_args_t *args
)
{
  device_context *ctx = (device_context *) base;

  (void) tty;
  (void) term;
  (void) args;

  ctx->done = true;

  return false;
}

static void test_first_open_error(void)
{
  static const rtems_termios_device_handler handler = {
    .first_open = first_open_error
  };
  static const char dev[] = "/foobar";

  rtems_resource_snapshot snapshot;
  rtems_status_code sc;
  int fd;
  int rv;
  device_context ctx = {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER( "abc" ),
    .done = false
  };

  rtems_resource_snapshot_take( &snapshot );

  sc = rtems_termios_device_install( &dev[0], &handler, NULL, &ctx.base );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  rtems_test_assert( !ctx.done );
  errno = 0;
  fd = open( &dev[0], O_RDWR );
  rtems_test_assert( fd == -1 );
  rtems_test_assert( errno == ENOMEM );
  rtems_test_assert( ctx.done );

  rv = unlink( &dev[0] );
  rtems_test_assert( rv == 0 );

  rtems_test_assert( rtems_resource_snapshot_check( &snapshot ) );
}

static bool set_attributes_error(
  rtems_termios_device_context *base,
  const struct termios *term
)
{
  device_context *ctx = (device_context *) base;

  (void) term;

  ctx->done = true;

  return false;
}

static void test_set_attributes_error(void)
{
  static const rtems_termios_device_handler handler = {
    .set_attributes = set_attributes_error
  };
  static const char dev[] = "/foobar";

  rtems_resource_snapshot snapshot;
  rtems_status_code sc;
  struct termios term;
  device_context ctx = {
    .base = RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER( "abc" ),
    .done = false
  };
  int fd;
  int rv;

  rtems_resource_snapshot_take( &snapshot );

  sc = rtems_termios_device_install( &dev[0], &handler, NULL, &ctx.base );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  fd = open( &dev[0], O_RDWR );
  rtems_test_assert( fd >= 0 );

  rtems_test_assert( !ctx.done );
  errno = 0;
  rv = ioctl( fd, TIOCSETA, &term );
  rtems_test_assert( rv == -1 );
  rtems_test_assert( errno == EIO );
  rtems_test_assert( ctx.done );

  rv = close( fd );
  rtems_test_assert( rv == 0 );

  rv = unlink( &dev[0] );
  rtems_test_assert( rv == 0 );

  rtems_test_assert( rtems_resource_snapshot_check( &snapshot ) );
}

static void test_set_best_baud(void)
{
  static const struct {
    uint32_t baud;
    speed_t speed;
  } baud_to_speed_table[] = {
    { 0,          B0 },
    { 25,         B0 },
    { 26,         B50 },
    { 50,         B50 },
    { 62,         B50 },
    { 63,         B75 },
    { 75,         B75 },
    { 110,        B110 },
    { 134,        B134 },
    { 150,        B150 },
    { 200,        B200 },
    { 300,        B300 },
    { 600,        B600 },
    { 1200,       B1200 },
    { 1800,       B1800 },
    { 2400,       B2400 },
    { 4800,       B4800 },
    { 9600,       B9600 },
    { 19200,      B19200 },
    { 38400,      B38400 },
    { 57600,      B57600 },
    { 115200,     B115200 },
    { 230400,     B230400 },
    { 460800,     B460800 },
    { 0xffffffff, B460800 }
  };

  size_t n = RTEMS_ARRAY_SIZE(baud_to_speed_table);
  size_t i;

  for ( i = 0; i < n; ++i ) {
    struct termios term;

    memset( &term, 0xff, sizeof( term ) );
    rtems_termios_set_best_baud( &term, baud_to_speed_table[ i ].baud );

    rtems_test_assert( term.c_ispeed == baud_to_speed_table[ i ].speed );
    rtems_test_assert( term.c_ospeed == baud_to_speed_table[ i ].speed );
  }
}

static rtems_task Init(
  rtems_task_argument ignored
)
{
  int                       rc;
  rtems_status_code         sc;
  rtems_device_major_number registered;
  int                       test;
  struct termios            t;
  int index = 0;

  TEST_BEGIN();

  test_termios_baud2index();
  test_termios_baud2number();
  test_termios_number_to_baud();

  sc = rtems_termios_bufsize( 256, 128, 64 );
  directive_failed( sc, "rtems_termios_bufsize" );

  /*
   * Register a driver
   */
  puts(
    "\n"
    "Init - rtems_io_register_driver - Termios Test Driver - OK"
  );
  sc = rtems_io_register_driver( 0, &test_driver, &registered );
  printf( "Init - Major slot returned = %d\n", (int) registered );
  directive_failed( sc, "rtems_io_register_driver" );

  /*
   * Test baud rate
   */
  puts( "Init - open - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK" );
  test = open( TERMIOS_TEST_DRIVER_DEVICE_NAME, O_RDWR );
  if ( test == -1 ) {
    printf( "ERROR - baud opening test device (%d)\n", test );
    rtems_test_exit(0);
  }

  /*
   * tcsetattr - ERROR invalid operation
   */
  puts( "tcsetattr - invalid operation - EINVAL" );
  rc = tcsetattr( test, INT_MAX, &t );
  rtems_test_assert( rc == -1 );
  rtems_test_assert( errno == EINVAL );

  test_termios_cfmakeraw();
  test_termios_cfmakesane();

  /*
   * tcsetattr - TCSADRAIN
   */
  puts( "\ntcsetattr - drain - OK" );
  rc = tcsetattr( test, TCSADRAIN, &t );
  rtems_test_assert( rc == 0 );

  test_termios_set_baud(test);

  puts( "Init - close - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK" );
  rc = close( test );
  if ( rc != 0 ) {
    printf( "ERROR - baud close test device (%d) %s\n", test, strerror(errno) );
    rtems_test_exit(0);
  }

  /*
   * Test character size
   */
  puts(
    "\n"
    "Init - open - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK"
  );
  test = open( TERMIOS_TEST_DRIVER_DEVICE_NAME, O_RDWR );
  if ( test == -1 ) {
    printf( "ERROR - size open test device (%d) %s\n", test, strerror(errno) );
    rtems_test_exit(0);
  }

  test_termios_set_charsize(test);

  puts( "Init - close - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK" );
  rc = close( test );
  if ( rc != 0 ) {
    printf( "ERROR - size close test device (%d) %s\n", test, strerror(errno) );
    rtems_test_exit(0);
  }

  /*
   * Test parity
   */
  puts(
    "\n"
    "Init - open - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK"
  );
  test = open( TERMIOS_TEST_DRIVER_DEVICE_NAME, O_RDWR );
  if ( test == -1 ) {
    printf( "ERROR - parity open test device (%d) %s\n",
      test, strerror(errno) );
    rtems_test_exit(0);
  }

  test_termios_set_parity(test);

  puts( "Init - close - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK" );
  rc = close( test );
  if ( rc != 0 ) {
    printf( "ERROR - parity close test device (%d) %s\n",
      test, strerror(errno) );
    rtems_test_exit(0);
  }

  /*
   * Test stop bits
   */
  puts(
    "\n"
    "Init - open - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK"
  );
  test = open( TERMIOS_TEST_DRIVER_DEVICE_NAME, O_RDWR );
  if ( test == -1 ) {
    printf( "ERROR - stop bits open test device (%d) %s\n",
      test, strerror(errno) );
    rtems_test_exit(0);
  }

  test_termios_set_stop_bits(test);

  test_termios_cfoutspeed();

  test_termios_cfinspeed();

  test_termios_cfsetspeed();

  puts( "Init - close - " TERMIOS_TEST_DRIVER_DEVICE_NAME " - OK" );
  rc = close( test );
  if ( rc != 0 ) {
    printf( "ERROR - stop bits close test device (%d) %s\n",
      test, strerror(errno) );
    rtems_test_exit(0);
  }


  puts( "Multiple open of the device" );
  for( ; index < 26; ++index ) {
    test = open( TERMIOS_TEST_DRIVER_DEVICE_NAME, O_RDWR );
    rtems_test_assert( test != -1 );
    rc = close( test );
    rtems_test_assert( rc == 0 );
  }
  puts( "" );

  test_device_install_remove();
  test_first_open_error();
  test_set_attributes_error();
  test_set_best_baud();

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_PREREQUISITE_DRIVERS \
  { .initialization_entry = test_early_device_install }

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/* include an extra slot for registering the termios one dynamically */
#define CONFIGURE_MAXIMUM_DRIVERS 4

/* one for the console and one for the test port */
#define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 3

/* we need to be able to open the test device */
#define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 4

#define CONFIGURE_MAXIMUM_TASKS         1
#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
