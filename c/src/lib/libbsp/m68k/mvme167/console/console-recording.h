/*
 *  Copyright (c) 2000, National Research Council of Canada
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/* CD2401 CONSOLE DRIVER DEBUG INFO RECORDING */

#ifdef CD2401_RECORD_DEBUG_INFO

/* Control individual recording here. That way, we don't clutter console.c */
#define CD2401_RECORD_WRITE
#define CD2401_RECORD_TX_ISR
#define CD2401_RECORD_RX_ISR
#define CD2401_RECORD_RE_ISR
#define CD2401_RECORD_MODEM_ISR
#define CD2401_RECORD_SET_ATTRIBUTE
#define CD2401_RECORD_FIRST_OPEN
#define CD2401_RECORD_LAST_CLOSE
#define CD2401_RECORD_START_REMOTE_TX
#define CD2401_RECORD_STOP_REMOTE_TX
#define CD2401_RECORD_DRAIN_OUTPUT
#define CD2401_RECORD_DELAY

/* Call the data recording functions */
#ifdef CD2401_RECORD_WRITE
#define CD2401_RECORD_WRITE_INFO( args )              cd2401_record_write_info args
#else
#define CD2401_RECORD_WRITE_INFO( args )
#endif

#ifdef CD2401_RECORD_TX_ISR
#define CD2401_RECORD_TX_ISR_INFO( args )             cd2401_record_tx_isr_info args
#define CD2401_RECORD_TX_ISR_SPURIOUS_INFO( args )    cd2401_record_tx_isr_spurious_info args
#define CD2401_RECORD_TX_ISR_BUSERR_INFO( args )      cd2401_record_tx_isr_buserr_info args
#else
#define CD2401_RECORD_TX_ISR_INFO( args )
#define CD2401_RECORD_TX_ISR_SPURIOUS_INFO( args )
#define CD2401_RECORD_TX_ISR_BUSERR_INFO( args )
#endif

#ifdef CD2401_RECORD_RX_ISR
#define CD2401_RECORD_RX_ISR_INFO( args )             cd2401_record_rx_isr_info args
#define CD2401_RECORD_RX_ISR_SPURIOUS_INFO( args )    cd2401_record_rx_isr_spurious_info args
#else
#define CD2401_RECORD_RX_ISR_INFO( args )
#define CD2401_RECORD_RX_ISR_SPURIOUS_INFO( args )
#endif

#ifdef CD2401_RECORD_RE_ISR
#define CD2401_RECORD_RE_ISR_SPURIOUS_INFO( args )    cd2401_record_re_isr_spurious_info args
#else
#define CD2401_RECORD_RE_ISR_SPURIOUS_INFO( args )
#endif

#ifdef CD2401_RECORD_MODEM_ISR
#define CD2401_RECORD_MODEM_ISR_SPURIOUS_INFO( args ) cd2401_record_modem_isr_spurious_info args
#else
#define CD2401_RECORD_MODEM_ISR_SPURIOUS_INFO( args )
#endif

#ifdef CD2401_RECORD_SET_ATTRIBUTES
#define CD2401_RECORD_SET_ATTRIBUTES_INFO( args )     cd2401_record_set_attributes_info args
#else
#define CD2401_RECORD_SET_ATTRIBUTES_INFO( args )
#endif

#ifdef CD2401_RECORD_FIRST_OPEN
#define CD2401_RECORD_FIRST_OPEN_INFO( args )         cd2401_record_first_open_info args
#else
#define CD2401_RECORD_FIRST_OPEN_INFO( args )
#endif

#ifdef CD2401_RECORD_LAST_CLOSE
#define CD2401_RECORD_LAST_CLOSE_INFO( args )         cd2401_record_last_close_info args
#else
#define CD2401_RECORD_LAST_CLOSE_INFO( args )
#endif

#ifdef CD2401_RECORD_START_REMOTE_TX
#define CD2401_RECORD_START_REMOTE_TX_INFO( args )    cd2401_record_start_remote_tx_info args
#else
#define CD2401_RECORD_START_REMOTE_TX_INFO( args )
#endif

#ifdef CD2401_RECORD_STOP_REMOTE_TX
#define CD2401_RECORD_STOP_REMOTE_TX_INFO( args )     cd2401_record_stop_remote_tx_info args
#else
#define CD2401_RECORD_STOP_REMOTE_TX_INFO( args )
#endif

#ifdef CD2401_RECORD_DRAIN_OUTPUT
#define CD2401_RECORD_DRAIN_OUTPUT_INFO( args )       cd2401_record_drain_output_info args
#else
#define CD2401_RECORD_DRAIN_OUTPUT_INFO( args )
#endif

#ifdef CD2401_RECORD_DELAY
#define CD2401_RECORD_DELAY_INFO( args )              cd2401_record_delay_info args
#else
#define CD2401_RECORD_DELAY_INFO( args )
#endif

/* Define the data and the recording functions */
#define CD2401_DEBUG_BUFFER_SIZE     256
#define CD2401_DEBUG_CHAR_BUFSIZE     64
#define CD2401_WRITE_INFO              1
#define CD2401_TX_ISR_INFO             2
#define CD2401_TX_ISR_SPURIOUS_INFO    3
#define CD2401_TX_ISR_BUSERR_INFO      4
#define CD2401_RX_ISR_INFO             5
#define CD2401_RX_ISR_SPURIOUS_INFO    6
#define CD2401_RE_ISR_SPURIOUS_INFO    7
#define CD2401_MODEM_ISR_SPURIOUS_INFO 8
#define CD2401_FIRST_OPEN_INFO         9
#define CD2401_LAST_CLOSE_INFO        10
#define CD2401_START_REMOTE_TX_INFO   11
#define CD2401_STOP_REMOTE_TX_INFO    12
#define CD2401_SET_ATTRIBUTE_INFO     13
#define CD2401_DRAIN_OUTPUT_INFO      14
#define CD2401_DELAY_INFO             15

struct cd2401_debug_info {
  short discriminant;
  short record_size;
  union {
    struct cd2401_write_info {
      int length;
      char buffer[CD2401_DEBUG_CHAR_BUFSIZE];
      char dmabuf;
    } write_info;
    struct cd2401_tx_isr_info {
      unsigned char channel;
      unsigned char status;
      unsigned char initial_ier;
      unsigned char final_ier;
      uint8_t         txEmpty;
    } tx_isr_info;
    struct cd2401_tx_isr_spurious_info {
      unsigned char channel;
      unsigned char status;
      unsigned char initial_ier;
      unsigned char final_ier;
      unsigned long spurdev;
      unsigned long spurcount;
    } tx_isr_spurious_info;
    struct cd2401_tx_isr_buserr_info {
      unsigned char channel;
      unsigned char status;
      unsigned char initial_ier;
      unsigned char buserr;
      unsigned long type;
      unsigned long addr;
    } tx_isr_buserr_info;
    struct cd2401_rx_isr_info {
      unsigned char channel;
      int length;
      char buffer[CD2401_DEBUG_CHAR_BUFSIZE];
    } rx_isr_info;
    struct cd2401_rx_isr_spurious_info {
      unsigned char channel;
      unsigned char status;
      unsigned long spurdev;
      unsigned long spurcount;
    } rx_isr_spurious_info;
    struct cd2401_re_isr_spurious_info {
      unsigned char channel;
      unsigned long spurdev;
      unsigned long spurcount;
    } re_isr_spurious_info;
    struct cd2401_modem_isr_spurious_info {
      unsigned char channel;
      unsigned long spurdev;
      unsigned long spurcount;
    } modem_isr_spurious_info;
    struct cd2401_first_open_info {
      unsigned char channel;
      uint8_t         init_count;
    } first_open_info;
    struct cd2401_last_close_info {
      unsigned char channel;
      uint8_t         init_count;
    } last_close_info;
    struct cd2401_start_remote_tx_info {
      unsigned char channel;
    } start_remote_tx_info;
    struct cd2401_stop_remote_tx_info {
      unsigned char channel;
    } stop_remote_tx_info;
    struct cd2401_set_attribute_info {
      int minor;
      uint8_t         need_reinit;
      uint8_t         txEmpty;
      uint8_t         csize;
      uint8_t         cstopb;
      uint8_t         parodd;
      uint8_t         parenb;
      uint8_t         ignpar;
      uint8_t         inpck;
      uint8_t         hw_flow_ctl;
      uint8_t         sw_flow_ctl;
      uint8_t         extra_flow_ctl;
      uint8_t         icrnl;
      uint8_t         igncr;
      uint8_t         inlcr;
      uint8_t         brkint;
      uint8_t         ignbrk;
      uint8_t         parmrk;
      uint8_t         istrip;
      uint16_t         tx_period;
      uint16_t         rx_period;
      uint32_t         out_baud;
      uint32_t         in_baud;
    } set_attribute_info;
    struct cd2401_drain_output_info {
      uint8_t         txEmpty;
      uint8_t         own_buf_A;
      uint8_t         own_buf_B;
    } drain_output_info;
    struct cd2401_delay_info {
      rtems_interval start;
      rtems_interval end;
      rtems_interval current;
      unsigned long loop_count;
    } delay_info;
  } u;
};

struct cd2401_debug_info cd2401_debug_buffer[CD2401_DEBUG_BUFFER_SIZE];
int cd2401_debug_index = 0;

#include <string.h>

int cd2401_get_record_size(
  int size
)
{
  /* Not the best way to do this */
  return size + 4;
}

void cd2401_record_write_info(
  int len,
  const char * buf,
  char dmabuf
)
{
  int max_length;

  max_length = (len < CD2401_DEBUG_CHAR_BUFSIZE ) ? len : CD2401_DEBUG_CHAR_BUFSIZE;

  memset( &(cd2401_debug_buffer[cd2401_debug_index]), '\0', sizeof( struct cd2401_debug_info )  );
  cd2401_debug_buffer[cd2401_debug_index].discriminant = CD2401_WRITE_INFO;
  cd2401_debug_buffer[cd2401_debug_index].record_size =
      cd2401_get_record_size( sizeof( struct cd2401_write_info ) );
  cd2401_debug_buffer[cd2401_debug_index].u.write_info.length = len;
  memcpy ( &(cd2401_debug_buffer[cd2401_debug_index].u.write_info.buffer), buf, max_length );
  cd2401_debug_buffer[cd2401_debug_index].u.write_info.dmabuf = dmabuf;

  cd2401_debug_index = (cd2401_debug_index + 1 ) % CD2401_DEBUG_BUFFER_SIZE;
}

void cd2401_record_tx_isr_info(
  unsigned char ch,
  unsigned char status,
  unsigned char initial_ier,
  unsigned char final_ier,
  uint8_t         txEmpty
)
{
  memset( &(cd2401_debug_buffer[cd2401_debug_index]), '\0', sizeof( struct cd2401_debug_info )  );
  cd2401_debug_buffer[cd2401_debug_index].discriminant = CD2401_TX_ISR_INFO;
  cd2401_debug_buffer[cd2401_debug_index].record_size =
      cd2401_get_record_size( sizeof( struct cd2401_tx_isr_info ) );
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_info.channel = ch;
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_info.status = status;
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_info.initial_ier = initial_ier;
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_info.final_ier = final_ier;
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_info.txEmpty = txEmpty;

  cd2401_debug_index = (cd2401_debug_index + 1 ) % CD2401_DEBUG_BUFFER_SIZE;
}

void cd2401_record_tx_isr_spurious_info(
  unsigned char ch,
  unsigned char status,
  unsigned char initial_ier,
  unsigned char final_ier,
  unsigned char spur_dev,
  unsigned char spur_cnt
)
{
  memset( &(cd2401_debug_buffer[cd2401_debug_index]), '\0', sizeof( struct cd2401_debug_info )  );
  cd2401_debug_buffer[cd2401_debug_index].discriminant = CD2401_TX_ISR_SPURIOUS_INFO;
  cd2401_debug_buffer[cd2401_debug_index].record_size =
      cd2401_get_record_size( sizeof( struct cd2401_tx_isr_spurious_info ) );
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_spurious_info.channel = ch;
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_spurious_info.status = status;
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_spurious_info.initial_ier = initial_ier;
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_spurious_info.final_ier = final_ier;
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_spurious_info.spurdev = spur_dev;
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_spurious_info.spurcount = spur_cnt;

  cd2401_debug_index = (cd2401_debug_index + 1 ) % CD2401_DEBUG_BUFFER_SIZE;
}

void cd2401_record_tx_isr_buserr_info(
  unsigned char ch,
  unsigned char status,
  unsigned char initial_ier,
  unsigned char buserr,
  unsigned long buserr_type,
  unsigned long buserr_addr
)
{
  memset( &(cd2401_debug_buffer[cd2401_debug_index]), '\0', sizeof( struct cd2401_debug_info )  );
  cd2401_debug_buffer[cd2401_debug_index].discriminant = CD2401_TX_ISR_BUSERR_INFO;
  cd2401_debug_buffer[cd2401_debug_index].record_size =
    cd2401_get_record_size( sizeof( struct cd2401_tx_isr_buserr_info ) );
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_buserr_info.channel = ch;
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_buserr_info.status = status;
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_buserr_info.initial_ier = initial_ier;
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_buserr_info.buserr = buserr;
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_buserr_info.type = buserr_type;
  cd2401_debug_buffer[cd2401_debug_index].u.tx_isr_buserr_info.addr = buserr_addr;

  cd2401_debug_index = (cd2401_debug_index + 1 ) % CD2401_DEBUG_BUFFER_SIZE;
}

void cd2401_record_rx_isr_info(
  unsigned char ch,
  unsigned char total,
  char * buffer
)
{
  int max_length;

  max_length = (total < CD2401_DEBUG_CHAR_BUFSIZE ) ? total : CD2401_DEBUG_CHAR_BUFSIZE;

  memset( &(cd2401_debug_buffer[cd2401_debug_index]), '\0', sizeof( struct cd2401_debug_info )  );
  cd2401_debug_buffer[cd2401_debug_index].discriminant = CD2401_RX_ISR_INFO;
  cd2401_debug_buffer[cd2401_debug_index].record_size =
      cd2401_get_record_size( sizeof( struct cd2401_rx_isr_info ) );
  cd2401_debug_buffer[cd2401_debug_index].u.rx_isr_info.length = max_length;
 memcpy ( &(cd2401_debug_buffer[cd2401_debug_index].u.rx_isr_info.buffer), buffer, max_length );

  cd2401_debug_index = (cd2401_debug_index + 1 ) % CD2401_DEBUG_BUFFER_SIZE;
}

void cd2401_record_rx_isr_spurious_info(
  unsigned char ch,
  unsigned char status,
  uint32_t         spur_dev,
  uint32_t         spur_cnt
)
{
  memset( &(cd2401_debug_buffer[cd2401_debug_index]), '\0', sizeof( struct cd2401_debug_info )  );
  cd2401_debug_buffer[cd2401_debug_index].discriminant = CD2401_RX_ISR_SPURIOUS_INFO;
  cd2401_debug_buffer[cd2401_debug_index].record_size =
      cd2401_get_record_size( sizeof( struct cd2401_rx_isr_spurious_info ) );
  cd2401_debug_buffer[cd2401_debug_index].u.rx_isr_spurious_info.channel = ch;
  cd2401_debug_buffer[cd2401_debug_index].u.rx_isr_spurious_info.status = status;
  cd2401_debug_buffer[cd2401_debug_index].u.rx_isr_spurious_info.spurdev = spur_dev;
  cd2401_debug_buffer[cd2401_debug_index].u.rx_isr_spurious_info.spurcount = spur_cnt;

  cd2401_debug_index = (cd2401_debug_index + 1 ) % CD2401_DEBUG_BUFFER_SIZE;
}

void cd2401_record_re_isr_spurious_info(
  unsigned char ch,
  uint32_t         spur_dev,
  uint32_t         spur_cnt
)
{
  memset( &(cd2401_debug_buffer[cd2401_debug_index]), '\0', sizeof( struct cd2401_debug_info )  );
  cd2401_debug_buffer[cd2401_debug_index].discriminant = CD2401_RE_ISR_SPURIOUS_INFO;
  cd2401_debug_buffer[cd2401_debug_index].record_size =
      cd2401_get_record_size( sizeof( struct cd2401_re_isr_spurious_info ) );
  cd2401_debug_buffer[cd2401_debug_index].u.re_isr_spurious_info.channel = ch;
  cd2401_debug_buffer[cd2401_debug_index].u.re_isr_spurious_info.spurdev = spur_dev;
  cd2401_debug_buffer[cd2401_debug_index].u.re_isr_spurious_info.spurcount = spur_cnt;

  cd2401_debug_index = (cd2401_debug_index + 1 ) % CD2401_DEBUG_BUFFER_SIZE;
}

void cd2401_record_modem_isr_spurious_info(
  unsigned char ch,
  uint32_t         spur_dev,
  uint32_t         spur_cnt
)
{
  memset( &(cd2401_debug_buffer[cd2401_debug_index]), '\0', sizeof( struct cd2401_debug_info )  );
  cd2401_debug_buffer[cd2401_debug_index].discriminant = CD2401_MODEM_ISR_SPURIOUS_INFO;
  cd2401_debug_buffer[cd2401_debug_index].record_size =
      cd2401_get_record_size( sizeof( struct cd2401_modem_isr_spurious_info ) );
  cd2401_debug_buffer[cd2401_debug_index].u.modem_isr_spurious_info.channel = ch;
  cd2401_debug_buffer[cd2401_debug_index].u.modem_isr_spurious_info.spurdev = spur_dev;
  cd2401_debug_buffer[cd2401_debug_index].u.modem_isr_spurious_info.spurcount = spur_cnt;

  cd2401_debug_index = (cd2401_debug_index + 1 ) % CD2401_DEBUG_BUFFER_SIZE;
}

void cd2401_record_first_open_info(
  unsigned char ch,
  uint8_t         init_count
)
{
  memset( &(cd2401_debug_buffer[cd2401_debug_index]), '\0', sizeof( struct cd2401_debug_info )  );
  cd2401_debug_buffer[cd2401_debug_index].discriminant = CD2401_FIRST_OPEN_INFO;
  cd2401_debug_buffer[cd2401_debug_index].record_size =
      cd2401_get_record_size( sizeof( struct cd2401_first_open_info ) );
  cd2401_debug_buffer[cd2401_debug_index].u.first_open_info.channel = ch;
  cd2401_debug_buffer[cd2401_debug_index].u.first_open_info.init_count = init_count;

  cd2401_debug_index = (cd2401_debug_index + 1 ) % CD2401_DEBUG_BUFFER_SIZE;
}

void cd2401_record_last_close_info(
  unsigned char ch,
  uint8_t         init_count
)
{
  memset( &(cd2401_debug_buffer[cd2401_debug_index]), '\0', sizeof( struct cd2401_debug_info )  );
  cd2401_debug_buffer[cd2401_debug_index].discriminant = CD2401_LAST_CLOSE_INFO;
  cd2401_debug_buffer[cd2401_debug_index].record_size =
      cd2401_get_record_size( sizeof( struct cd2401_last_close_info ) );
  cd2401_debug_buffer[cd2401_debug_index].u.last_close_info.channel = ch;
  cd2401_debug_buffer[cd2401_debug_index].u.last_close_info.init_count = init_count;

  cd2401_debug_index = (cd2401_debug_index + 1 ) % CD2401_DEBUG_BUFFER_SIZE;
}

void cd2401_record_start_remote_tx_info(
  unsigned char ch
)
{
  memset( &(cd2401_debug_buffer[cd2401_debug_index]), '\0', sizeof( struct cd2401_debug_info )  );
  cd2401_debug_buffer[cd2401_debug_index].discriminant = CD2401_START_REMOTE_TX_INFO;
  cd2401_debug_buffer[cd2401_debug_index].record_size =
      cd2401_get_record_size( sizeof( struct cd2401_start_remote_tx_info ) );
  cd2401_debug_buffer[cd2401_debug_index].u.start_remote_tx_info.channel = ch;

  cd2401_debug_index = (cd2401_debug_index + 1 ) % CD2401_DEBUG_BUFFER_SIZE;
}

void cd2401_record_stop_remote_tx_info(
  unsigned char ch
)
{
  memset( &(cd2401_debug_buffer[cd2401_debug_index]), '\0', sizeof( struct cd2401_debug_info )  );
  cd2401_debug_buffer[cd2401_debug_index].discriminant = CD2401_STOP_REMOTE_TX_INFO;
  cd2401_debug_buffer[cd2401_debug_index].record_size =
      cd2401_get_record_size( sizeof( struct cd2401_stop_remote_tx_info ) );
  cd2401_debug_buffer[cd2401_debug_index].u.stop_remote_tx_info.channel = ch;

  cd2401_debug_index = (cd2401_debug_index + 1 ) % CD2401_DEBUG_BUFFER_SIZE;
}

void cd2401_record_set_attributes_info(
  int minor,
  uint8_t         need_reinit,
  uint8_t         csize,
  uint8_t         cstopb,
  uint8_t         parodd,
  uint8_t         parenb,
  uint8_t         ignpar,
  uint8_t         inpck,
  uint8_t         hw_flow_ctl,
  uint8_t         sw_flow_ctl,
  uint8_t         extra_flow_ctl,
  uint8_t         icrnl,
  uint8_t         igncr,
  uint8_t         inlcr,
  uint8_t         brkint,
  uint8_t         ignbrk,
  uint8_t         parmrk,
  uint8_t         istrip,
  uint16_t         tx_period,
  uint16_t         rx_period,
  uint32_t         out_baud,
  uint32_t         in_baud
)
{
  memset( &(cd2401_debug_buffer[cd2401_debug_index]), '\0', sizeof( struct cd2401_debug_info )  );
  cd2401_debug_buffer[cd2401_debug_index].discriminant = CD2401_SET_ATTRIBUTE_INFO;
  cd2401_debug_buffer[cd2401_debug_index].record_size =
  cd2401_get_record_size( sizeof( struct cd2401_set_attribute_info ) );
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.minor = minor;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.need_reinit = need_reinit;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.txEmpty = CD2401_Channel_Info[minor].txEmpty;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.csize = csize;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.cstopb = cstopb;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.parodd = parodd;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.parenb = parenb;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.ignpar = ignpar;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.inpck = inpck;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.hw_flow_ctl = hw_flow_ctl;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.sw_flow_ctl = sw_flow_ctl;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.extra_flow_ctl = extra_flow_ctl;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.icrnl = icrnl;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.igncr = igncr;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.inlcr = inlcr;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.brkint = brkint;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.ignbrk = ignbrk;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.parmrk = parmrk;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.istrip = istrip;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.tx_period = tx_period;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.rx_period = rx_period;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.out_baud = out_baud;
  cd2401_debug_buffer[cd2401_debug_index].u.set_attribute_info.in_baud = in_baud;

  cd2401_debug_index = (cd2401_debug_index + 1 ) % CD2401_DEBUG_BUFFER_SIZE;
}

void cd2401_record_drain_output_info(
  uint8_t         txEmpty,
  uint8_t         own_buf_A,
  uint8_t         own_buf_B
)
{
  memset( &(cd2401_debug_buffer[cd2401_debug_index]), '\0', sizeof( struct cd2401_debug_info )  );
  cd2401_debug_buffer[cd2401_debug_index].discriminant = CD2401_DRAIN_OUTPUT_INFO;
  cd2401_debug_buffer[cd2401_debug_index].record_size =
      cd2401_get_record_size( sizeof( struct cd2401_drain_output_info ) );
  cd2401_debug_buffer[cd2401_debug_index].u.drain_output_info.txEmpty = txEmpty;
  cd2401_debug_buffer[cd2401_debug_index].u.drain_output_info.own_buf_A = own_buf_A;
  cd2401_debug_buffer[cd2401_debug_index].u.drain_output_info.own_buf_B = own_buf_B;

  cd2401_debug_index = (cd2401_debug_index + 1 ) % CD2401_DEBUG_BUFFER_SIZE;
}

void cd2401_record_delay_info(
  rtems_interval start,
  rtems_interval end,
  rtems_interval current,
  unsigned long loop_count
)
{
  memset( &(cd2401_debug_buffer[cd2401_debug_index]), '\0', sizeof( struct cd2401_debug_info )  );
  cd2401_debug_buffer[cd2401_debug_index].discriminant = CD2401_DELAY_INFO;
  cd2401_debug_buffer[cd2401_debug_index].record_size =
      cd2401_get_record_size( sizeof( struct cd2401_delay_info ) );
  cd2401_debug_buffer[cd2401_debug_index].u.delay_info.start = start;
  cd2401_debug_buffer[cd2401_debug_index].u.delay_info.end = end;
  cd2401_debug_buffer[cd2401_debug_index].u.delay_info.current = current;
  cd2401_debug_buffer[cd2401_debug_index].u.delay_info.loop_count = loop_count;

  cd2401_debug_index = (cd2401_debug_index + 1 ) % CD2401_DEBUG_BUFFER_SIZE;
}

#else

/* Do not call the data recording functions */
#define CD2401_RECORD_WRITE_INFO( args )
#define CD2401_RECORD_TX_ISR_INFO( args )
#define CD2401_RECORD_TX_ISR_SPURIOUS_INFO( args )
#define CD2401_RECORD_TX_ISR_BUSERR_INFO( args )
#define CD2401_RECORD_RX_ISR_INFO( args )
#define CD2401_RECORD_RX_ISR_SPURIOUS_INFO( args )
#define CD2401_RECORD_RE_ISR_SPURIOUS_INFO( args )
#define CD2401_RECORD_MODEM_ISR_SPURIOUS_INFO( args )
#define CD2401_RECORD_FIRST_OPEN_INFO( args )
#define CD2401_RECORD_LAST_CLOSE_INFO( args )
#define CD2401_RECORD_START_REMOTE_TX_INFO( args )
#define CD2401_RECORD_STOP_REMOTE_TX_INFO( args )
#define CD2401_RECORD_SET_ATTRIBUTES_INFO( args )
#define CD2401_RECORD_DRAIN_OUTPUT_INFO( args )
#define CD2401_RECORD_DELAY_INFO( args )

#endif
