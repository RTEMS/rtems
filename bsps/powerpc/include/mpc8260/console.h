#ifndef _MPC8260_CONSOLE_H
#define _MPC8260_CONSOLE_H

#include <rtems/libio.h>

void m8xx_uart_initialize(void);
void m8xx_uart_interrupts_initialize(void);
void m8xx_uart_scc_initialize (int minor);
void m8xx_uart_smc_initialize (int minor);

/* Termios callbacks */
int m8xx_uart_pollRead(int minor);
ssize_t m8xx_uart_pollWrite(int minor, const char* buf, size_t len);
ssize_t m8xx_uart_write(int minor, const char *buf, size_t len);
int m8xx_uart_setAttributes(int, const struct termios* t);


#if 0
int m8260_smc_set_attributes(int, const struct termios*);
int m8260_scc_set_attributes(int, const struct termios*);
void m8260_scc_initialize(int);
void m8260_smc_initialize(int);
int m8260_char_poll_read(int);
int m8260_char_poll_write(int, const char*, int);
rtems_isr m8260_scc1_console_interrupt_handler(rtems_vector_number);
rtems_isr m8260_scc2_console_interrupt_handler(rtems_vector_number);
rtems_isr m8260_scc3_console_interrupt_handler(rtems_vector_number);
rtems_isr m8260_scc4_console_interrupt_handler(rtems_vector_number);
rtems_isr m8260_smc1_console_interrupt_handler(rtems_vector_number);
rtems_isr m8260_smc2_console_interrupt_handler(rtems_vector_number);
int m8260_buf_poll_read(int, char**);
int m8260_buf_poll_write(int, char*, int);
void m8260_console_initialize(void);
rtems_device_driver m8260_console_read(rtems_device_major_number,
				      rtems_device_minor_number,
				      void*);
rtems_device_driver m8260_console_write(rtems_device_major_number,
				       rtems_device_minor_number,
				       void*);


typedef struct Buf_t_ {
  struct Buf_t_ *next;
  volatile char *buf;
  volatile int   len;
  int   pos;
} Buf_t;
#endif

#define NUM_PORTS 6

#define SMC1_MINOR    0
#define SMC2_MINOR    1
#define SCC1_MINOR    2
#define SCC2_MINOR    3
#define SCC3_MINOR    4
#define SCC4_MINOR    5


#endif
