/*****************************************************************************/
/*
  M68302 SCC Polled Driver

 */
/*****************************************************************************/

#include <bsp.h>
#include <rtems/m68k/m68302.h>
#include <m68302scc.h>

#define M68302_SCC_COUNT (3)

static volatile m302_SCC_t *scc[M68302_SCC_COUNT] = { 0, 0, 0 };
static volatile m302_SCC_Registers_t *scc_reg[M68302_SCC_COUNT] = { 0, 0, 0 };
static int scc_translate[M68302_SCC_COUNT] = { 0, 0, 0 };

static const uint16_t         baud_clocks[] =
{
  (SYSTEM_CLOCK / (  4800 * 16)),
  (SYSTEM_CLOCK / (  9600 * 16)),
  (SYSTEM_CLOCK / ( 19200 * 16)),
  (SYSTEM_CLOCK / ( 38400 * 16)),
  (SYSTEM_CLOCK / ( 57600 * 16)),
  (SYSTEM_CLOCK / (115700 * 16))
};

void scc_initialise(int channel, int baud, int translate)
{
  uint16_t         scon;

  if (channel < M68302_SCC_COUNT)
  {
    scc[channel] = &m302.scc1 + channel;
    scc_reg[channel] = &m302.reg.scc[channel];
    scc_translate[channel] = translate;

    scon  = (baud_clocks[baud] & 0xF800) == 0 ? 0 : 1;
    scon |= (((baud_clocks[baud] / (1 + scon * 3)) - 1) << 1) & 0x0FFE;

    scc_reg[channel]->scon = scon;
    scc_reg[channel]->scm = 0x0171;

    scc[channel]->bd.tx[0].status = 0x2000;
    scc[channel]->bd.tx[0].length = 0;
    scc[channel]->bd.tx[0].buffer =
      (uint8_t*) &(scc[channel]->bd.tx[1].buffer);

    scc[channel]->bd.rx[0].status = 0x2000;
    scc[channel]->bd.rx[0].length = 0;
    scc[channel]->bd.rx[0].buffer =
      (uint8_t*) &(scc[channel]->bd.rx[1].buffer);

    scc[channel]->parm.rfcr = 0x50;
    scc[channel]->parm.tfcr = 0x50;

    scc[channel]->parm.mrblr = 0x0001;
    scc[channel]->prot.uart.max_idl = 0x0004;
    scc[channel]->prot.uart.brkcr = 1;
    scc[channel]->prot.uart.parec = 0;
    scc[channel]->prot.uart.frmec = 0;
    scc[channel]->prot.uart.nosec = 0;
    scc[channel]->prot.uart.brkec = 0;
    scc[channel]->prot.uart.uaddr1 = 0;
    scc[channel]->prot.uart.uaddr2 = 0;
    scc[channel]->prot.uart.character[0] = 0x0003;
    scc[channel]->prot.uart.character[1] = 0x8000;

    scc_reg[channel]->scce = 0xFF;
    scc_reg[channel]->sccm = 0x15;

    scc_reg[channel]->scm = 0x17d;
  }
}

unsigned char scc_status(int channel, unsigned char status)
{
  uint16_t         rx_status;

  m302.reg.wcn = 0;

  if ((channel < M68302_SCC_COUNT) && scc[channel])
  {
    rx_status = scc[channel]->bd.rx[0].status;

    if ((rx_status & 0x8000) == 0)
    {
      if (rx_status & 0x003B)
      {
        return 2;
      }
      if (status == 0)
      {
        return 1;
      }
    }
  }

  return 0;
}

unsigned char scc_in(int channel)
{
  m302.reg.wcn = 0;

  if ((channel < M68302_SCC_COUNT) && scc[channel])
  {
    if ((scc[channel]->bd.rx[0].status & 0x8000) == 0)
    {
      unsigned char c;

      c = *(scc[channel]->bd.rx[0].buffer);

      scc[channel]->bd.rx[0].status = 0xa000;

      return c;
    }
  }

  return 0;
}

void scc_out(int channel, unsigned char character)
{
  if ((channel < M68302_SCC_COUNT) && scc[channel])
  {
    do
    {
      m302.reg.wcn = 0;
    }
    while (scc[channel]->bd.tx[0].status & 0x8000);

    *(scc[channel]->bd.tx[0].buffer) = character;

    scc[channel]->bd.tx[0].length = 1;
    scc[channel]->bd.tx[0].status = 0xa000;

    if (scc_translate[channel])
    {
      if (character == '\n')
      {
        scc_out(channel, '\r');
      }
    }
  }
}
