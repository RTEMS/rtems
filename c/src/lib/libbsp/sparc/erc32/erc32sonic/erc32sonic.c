/*
 *  THARSYS VME SPARC RT board SONIC Configuration Information
 *
 *  References:
 *
 *  1) SVME/DMV-171 Single Board Computer Documentation Package, #805905,
 *     DY 4 Systems Inc., Kanata, Ontario, September, 1996.
 */

#include <bsp.h>
#include <libchip/sonic.h>
#if (SONIC_DEBUG & SONIC_DEBUG_PRINT_REGISTERS)
#include <stdio.h>
#endif

void erc32_sonic_write_register(
  void       *base,
  uint32_t    regno,
  uint32_t    value
)
{
  volatile uint32_t   *p = base;

#if (SONIC_DEBUG & SONIC_DEBUG_PRINT_REGISTERS)
  printf( "%p Write 0x%04x to %s (0x%02x)\n",
      &p[regno], value, SONIC_Reg_name[regno], regno );
  fflush( stdout );
#endif
  p[regno] = 0x0ffff & value;
}

uint32_t   erc32_sonic_read_register(
  void       *base,
  uint32_t    regno
)
{
  volatile uint32_t   *p = base;
  uint32_t             value;

  value = p[regno];
#if (SONIC_DEBUG & SONIC_DEBUG_PRINT_REGISTERS)
  printf( "%p Read 0x%04x from %s (0x%02x)\n",
      &p[regno], value, SONIC_Reg_name[regno], regno );
  fflush( stdout );
#endif
  return 0x0ffff & value;
}

/*
 * Default sizes of transmit and receive descriptor areas
 */
#define RDA_COUNT     20 /* 20 */
#define TDA_COUNT     20 /* 10 */

/*
 * Default device configuration register values
 * Conservative, generic values.
 * DCR:
 *      No extended bus mode
 *      Unlatched bus retry
 *      Programmable outputs unused
 *      Asynchronous bus mode
 *      User definable pins unused
 *      No wait states (access time controlled by DTACK*)
 *      32-bit DMA
 *      Empty/Fill DMA mode
 *      Maximum Transmit/Receive FIFO
 * DC2:
 *      Extended programmable outputs unused
 *      Normal HOLD request
 *      Packet compress output unused
 *      No reject on CAM match
 */
#define SONIC_DCR  ( DCR_DW32 | DCR_RFT24 | DCR_TFT28)
#define SONIC_DC2 (0)

/*
 * Default location of device registers
 */
#define SONIC_BASE_ADDRESS 0x10000100

/*
 * Default interrupt vector
 */
#define SONIC_VECTOR 0x1E

sonic_configuration_t erc32_sonic_configuration = {
  SONIC_BASE_ADDRESS,        /* base address */
  SONIC_VECTOR,              /* vector number */
  SONIC_DCR,                 /* DCR register value */
  SONIC_DC2,                 /* DC2 register value */
  TDA_COUNT,                 /* number of transmit descriptors */
  RDA_COUNT,                 /* number of receive descriptors */
  erc32_sonic_write_register,
  erc32_sonic_read_register
};

int rtems_erc32_sonic_driver_attach(struct rtems_bsdnet_ifconfig *config)
{

  ERC32_MEC.IO_Configuration |= (0x15 << (((SONIC_BASE_ADDRESS >> 24) & 0x3) * 8));
  ERC32_MEC.Control &= ~0x60001; /* Disable DMA time-out, parity & power-down */
  ERC32_MEC.Control |= 0x10000; 		/* Enable DMA */
  ERC32_MEC.Interrupt_Mask &= ~(1 << (SONIC_VECTOR - 0x10));
  return(rtems_sonic_driver_attach( config, &erc32_sonic_configuration ));

}
