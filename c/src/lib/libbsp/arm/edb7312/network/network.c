#include <rtems.h>
#include <sys/mbuf.h>
#include <bsp/irq.h>
#include <libchip/cs8900.h>

#define CS8900_BASE 0x20000300
unsigned int bsp_cs8900_io_base = 0;
unsigned int bsp_cs8900_memory_base = 0;
cs8900_device *g_cs;
void cs8900_isr(rtems_irq_hdl_param unused);
rtems_irq_connect_data cs8900_isr_data = {BSP_EINT3,
					  cs8900_isr,
					  NULL,
					  NULL,
					  NULL,
					  NULL};

char g_enetbuf[1520];

void cs8900_isr(rtems_irq_hdl_param unused)
{
    cs8900_interrupt(BSP_EINT3, g_cs);
}

/* cs8900_io_set_reg - set one of the I/O addressed registers */
void cs8900_io_set_reg (cs8900_device *cs, unsigned short reg, unsigned short data)
{
    /* works the same for all values of dev */
/*
    printf("cs8900_io_set_reg: reg: %#6x, val %#6x\n",
           CS8900_BASE + reg,
           data);
*/
   *(unsigned short *)(CS8900_BASE + reg) = data;
}

/* cs8900_io_get_reg - reads one of the I/O addressed registers */
unsigned short cs8900_io_get_reg (cs8900_device *cs, unsigned short reg)
{
    unsigned short val;
    /* works the same for all values of dev */
    val = *(unsigned short *)(CS8900_BASE + reg);
/*
    printf("cs8900_io_get_reg: reg: %#6x, val %#6x\n", reg, val);
*/
    return val;
}

/* cs8900_mem_set_reg - sets one of the registers mapped through
 *                      PacketPage
 */
void cs8900_mem_set_reg (cs8900_device *cs, unsigned long reg, unsigned short data)
{
    /* works the same for all values of dev */
    cs8900_io_set_reg(cs, CS8900_IO_PACKET_PAGE_PTR, reg);
    cs8900_io_set_reg(cs, CS8900_IO_PP_DATA_PORT0, data);
}

/* cs8900_mem_get_reg - reads one of the registers mapped through
 *                      PacketPage
 */
unsigned short cs8900_mem_get_reg (cs8900_device *cs, unsigned long reg)
{
    /* works the same for all values of dev */
    cs8900_io_set_reg(cs, CS8900_IO_PACKET_PAGE_PTR, reg);
    return cs8900_io_get_reg(cs, CS8900_IO_PP_DATA_PORT0);
}

void cs8900_attach_interrupt (cs8900_device *cs)
{
    g_cs = cs;
    BSP_install_rtems_irq_handler(&cs8900_isr_data);
}

void cs8900_detach_interrupt (cs8900_device *cs)
{
    BSP_remove_rtems_irq_handler(&cs8900_isr_data);
}

unsigned short cs8900_get_data_block (cs8900_device *cs, unsigned char *data)
{
    int len;
    int i;

    len = cs8900_mem_get_reg(cs, CS8900_PP_RxLength);

    for (i = 0; i < ((len + 1) / 2); i++) {
        ((short *)data)[i] = cs8900_io_get_reg(cs,
                                               CS8900_IO_RX_TX_DATA_PORT0);
    }
    return len;
}

void cs8900_tx_load (cs8900_device *cs, struct mbuf *m)
{
    int len;
    unsigned short *data;
    int i;

    len = 0;

    do {
        memcpy(&g_enetbuf[len], mtod(m, const void *), m->m_len);
        len += m->m_len;
        m = m->m_next;
    } while (m != 0);

    data = (unsigned short *) &g_enetbuf[0];
    for (i = 0; i < ((len + 1) / 2); i++) {
        cs8900_io_set_reg(cs,
                          CS8900_IO_RX_TX_DATA_PORT0,
                          data[i]);
    }
}
