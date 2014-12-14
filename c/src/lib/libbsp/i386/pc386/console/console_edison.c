
#include <bsp.h>
#include <libchip/serial.h>

/* XXX hack until real support is available, code copied from libchip */
#define NS16550_RECEIVE_BUFFER   0
#define NS16550_TRANSMIT_BUFFER  0
#define NS16550_INTERRUPT_ENABLE 1
#define NS16550_INTERRUPT_ID     2
#define NS16550_FIFO_CONTROL     2
#define NS16550_LINE_CONTROL     3
#define NS16550_MODEM_CONTROL    4
#define NS16550_LINE_STATUS      5
#define NS16550_MODEM_STATUS     6
#define NS16550_SCRATCH_PAD      7
#define NS16550_FRACTIONAL_DIVIDER 10

/* status bits we use in line status */

#define SP_LSR_TX     0x40
#define SP_LSR_THOLD  0x20
#define SP_LSR_RDY    0x01
static volatile uint8_t *edison_com = (volatile uint8_t *)0xff010180;

void edison_write_polled(int minor, char cChar);
int edison_inbyte_nonblocking_polled(int minor);

static int edison_open(int major, int minor, void *arg)
{
  return 0;
}

static int edison_close(int major, int minor, void *arg)
{
  return 0;
}

int edison_inbyte_nonblocking_polled(int minor)
{
  if ( edison_com[NS16550_LINE_STATUS] & 0x01 )
    return (int) edison_com[NS16550_RECEIVE_BUFFER];
  return -1;
}

static ssize_t edison_write_support_polled(int minor, const char *buf, size_t len)
{
  ssize_t i;

  for ( i=0 ; i<len ; i++ ) {
#if 0
    if ( (edison_com[NS16550_LINE_STATUS] & SP_LSR_TX) == 0 )
      break;
#else
    while ( (edison_com[NS16550_LINE_STATUS] & SP_LSR_TX) == 0x00 )
      /* wait until ready */;
#endif
    edison_com[NS16550_TRANSMIT_BUFFER] = (uint8_t) buf[i];
  }
  return i;
}

static void edison_init(int minor)
{
}

void edison_write_polled(int minor, char cChar)
{
  while ( (edison_com[NS16550_LINE_STATUS] & SP_LSR_TX) == 0x00 )
    /* wait until ready */;
  edison_com[NS16550_TRANSMIT_BUFFER] = (uint8_t) cChar;
}

static bool edison_probe(int minor)
{
  return true;
}

static int edison_set_attributes(int minor, const struct termios *t)
{
  return 0;
}

const console_fns edison_fns =
{
  edison_probe,                       /* deviceProbe */
  edison_open,                        /* deviceFirstOpen */
  edison_close,                       /* deviceLastClose */
  edison_inbyte_nonblocking_polled,   /* deviceRead */
  edison_write_support_polled,        /* deviceWrite */
  edison_init,                        /* deviceInitialize */
  edison_write_polled,                /* deviceWritePolled */
  edison_set_attributes,              /* deviceSetAttributes */
  FALSE,                              /* deviceOutputUsesInterrupts */
};
