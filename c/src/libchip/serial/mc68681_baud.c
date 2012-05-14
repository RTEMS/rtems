/*
 *  MC68681 Default Baud Rate Table
 */

#include <rtems.h>
#include <libchip/serial.h>
#include <libchip/mc68681.h>

/* major index of 0 : ACR[7] = 0, X = 0 -- 68c681 only has these */
/* major index of 1 : ACR[7] = 1, X = 0 -- 68c681 only has these */
/* major index of 2 : ACR[7] = 0, X = 1 */
/* major index of 3 : ACR[7] = 1, X = 1 */

/* mc68681_baud_table_t mc68681_baud_rate_table[4] = { */
mc68681_baud_t mc68681_baud_rate_table[4][RTEMS_TERMIOS_NUMBER_BAUD_RATES] = {
  { /* ACR[7] = 0, X = 0 */
    MC68681_BAUD_NOT_VALID,    /* B0 */
    0x00,                      /* B50 */
    MC68681_BAUD_NOT_VALID,    /* B75 */
    0x01,                      /* B110 */
    0x02,                      /* B134 */
    MC68681_BAUD_NOT_VALID,    /* B150 */
    0x03,                      /* B200 */
    0x04,                      /* B300 */
    0x05,                      /* B600 */
    0x06,                      /* B1200 */
    MC68681_BAUD_NOT_VALID,    /* B1800 */
    0x08,                      /* B2400 */
    0x09,                      /* B4800 */
    0x0B,                      /* B9600 */
    MC68681_BAUD_NOT_VALID,    /* B19200 */
    0x0C,                      /* B38400 */
    MC68681_BAUD_NOT_VALID,    /* B57600 */
    MC68681_BAUD_NOT_VALID,    /* B115200 */
    MC68681_BAUD_NOT_VALID,    /* B230400 */
    MC68681_BAUD_NOT_VALID     /* B460800 */
  },
  { /* ACR[7] = 1, X = 0 */
    MC68681_BAUD_NOT_VALID,    /* B0 */
    MC68681_BAUD_NOT_VALID,    /* B50 */
    0x00,                      /* B75 */
    0x01,                      /* B110 */
    0x02,                      /* B134 */
    0x03,                      /* B150 */
    MC68681_BAUD_NOT_VALID,    /* B200 */
    0x04,                      /* B300 */
    0x05,                      /* B600 */
    0x06,                      /* B1200 */
    0x0A,                      /* B1800 */
    0x08,                      /* B2400 */
    0x09,                      /* B4800 */
    0x0B,                      /* B9600 */
    0x0C,                      /* B19200 */
    MC68681_BAUD_NOT_VALID,    /* B38400 */
    MC68681_BAUD_NOT_VALID,    /* B57600 */
    MC68681_BAUD_NOT_VALID,    /* B115200 */
    MC68681_BAUD_NOT_VALID,    /* B230400 */
    MC68681_BAUD_NOT_VALID     /* B460800 */
  },
  { /* ACR[7] = 0, X = 1 */
    MC68681_BAUD_NOT_VALID,    /* B0 */
    MC68681_BAUD_NOT_VALID,    /* B50 */
    0x00,                      /* B75 */
    0x01,                      /* B110 */
    0x02,                      /* B134 */
    0x03,                      /* B150 */
    MC68681_BAUD_NOT_VALID,    /* B200 */
    MC68681_BAUD_NOT_VALID,    /* B300 */
    MC68681_BAUD_NOT_VALID,    /* B600 */
    MC68681_BAUD_NOT_VALID,    /* B1200 */
    0x0A,                      /* B1800 */
    MC68681_BAUD_NOT_VALID,    /* B2400 */
    0x08,                      /* B4800 */
    0x0B,                      /* B9600 */
    0x0C,                      /* B19200 */
    MC68681_BAUD_NOT_VALID,    /* B38400 */
    0x07,                      /* B57600 */
    0x08,                      /* B115200 */
    MC68681_BAUD_NOT_VALID,    /* B230400 */
    MC68681_BAUD_NOT_VALID     /* B460800 */
  },
  { /* ACR[7] = 1, X = 1 */
    MC68681_BAUD_NOT_VALID,    /* B0 */
    0x00,                      /* B50 */
    MC68681_BAUD_NOT_VALID,    /* B75 */
    0x01,                      /* B110 */
    0x02,                      /* B134 */
    MC68681_BAUD_NOT_VALID,    /* B150 */
    0x03,                      /* B200 */
    MC68681_BAUD_NOT_VALID,    /* B300 */
    MC68681_BAUD_NOT_VALID,    /* B600 */
    MC68681_BAUD_NOT_VALID,    /* B1200 */
    MC68681_BAUD_NOT_VALID,    /* B1800 */
    MC68681_BAUD_NOT_VALID,    /* B2400 */
    0x09,                      /* B4800 */
    0x0B,                      /* B9600 */
    MC68681_BAUD_NOT_VALID,    /* B19200 */
    0x0C,                      /* B38400 */
    0x07,                      /* B57600 */
    0x08,                      /* B115200 */
    MC68681_BAUD_NOT_VALID,    /* B230400 */
    MC68681_BAUD_NOT_VALID     /* B460800 */
  },
};
