/*****************************************************************************/
/*
  M68302 Scc Polled Uart Support

 */
/*****************************************************************************/

#if !defined(_M68302SCC_H_)
#define _M68302SCC_H_

#if __cplusplus
extern "C"
{
#endif

#define SCC_4800    (0)
#define SCC_9600    (1)
#define SCC_19200   (2)
#define SCC_38400   (3)
#define SCC_57600   (4)
#define SCC_115700  (5)

void scc_initialise(int channel, int baud_rate, int lf_translate);
unsigned char scc_status(int channel, const unsigned char status);
unsigned char scc_in(int channel);
void scc_out(int channel, const unsigned char character);

#if __cplusplus
}
#endif

#endif
