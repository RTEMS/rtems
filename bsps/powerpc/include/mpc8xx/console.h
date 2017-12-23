#ifndef _MPC8XX_CONSOLE_H
#define _MPC8XX_CONSOLE_H

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


#ifdef mpc860
#define NUM_PORTS 6                /* number of serial ports for mpc860 */
#else
#define NUM_PORTS 4                /* number of serial ports for mpc821 */
#endif

#define SMC1_MINOR    0
#define SMC2_MINOR    1
#define SCC1_MINOR    2
#define SCC2_MINOR    3
#define SCC3_MINOR    4
#define SCC4_MINOR    5


#endif
