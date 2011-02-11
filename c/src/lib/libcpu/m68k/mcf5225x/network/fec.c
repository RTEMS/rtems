#include <stdint.h>
#include <stddef.h>

#include <fec.h>
#include <lwip/pbuf.h>
#include <netif/etharp.h>

#include <rtems.h>
#include <bsp.h>

#if LWIP_NETIF_STATUS_CALLBACK
static void mcf5225xif_status(struct netif*);
#endif


#if LWIP_NETIF_LINK_CALLBACK
static void mcf5225xif_link(struct netif*);
#endif

static u8_t __attribute__((aligned (16))) rx_buf[MAX_FRAME_LEN];
static u8_t __attribute__((aligned (16))) tx_buf[MAX_FRAME_LEN];

struct __attribute__((aligned(16))) rx_desc {
  u16_t ctl;
  u16_t len;
  u8_t* ptr;
} rx_bd = { MCF_FEC_RXBD_E | MCF_FEC_RXBD_W ,0,rx_buf};

struct tx_desc {
	u16_t ctl;
	u16_t len;
	u8_t* buf;
} tx_bd[2] ={{MCF_FEC_TXBD_R | MCF_FEC_TXBD_L | MCF_FEC_TXBD_TC ,0,tx_buf},{MCF_FEC_TXBD_L | MCF_FEC_TXBD_TC |  MCF_FEC_TXBD_W ,0,tx_buf}}; 

static rtems_id net_task_id;

rtems_isr rx_frame_handler(rtems_vector_number vector)
{
	MCF_INTC0_IMRL |= MCF_INTC_IMRL_MASK27; /* disable FEC RX_INTF interrupt */
	MCF_FEC_EIR |= MCF_FEC_EIR_RXF; /* clear pending interrupt event */
	
	rtems_event_send(net_task_id,RTEMS_EVENT_0);
	
	MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASK27; /* enable FEC RX_INTF interrupt */
}

void handle_rx_frame(struct netif* netif)
{
	/* Scan the Rx buffer rescriptor ring */
	if (rx_bd.ctl & (MCF_FEC_RXBD_RO1 | MCF_FEC_RXBD_E | MCF_FEC_RXBD_TR)) rx_bd.ctl = MCF_FEC_RXBD_E | MCF_FEC_RXBD_W; /* 1: receive process pending, pbuf still allocated, 2: Buffer empty, quit, 3: frame too long */ 
	else if (rx_bd.ctl & MCF_FEC_RXBD_L) { /* Last buffer in frame, finalize */
		if (rx_bd.ctl & (MCF_FEC_RXBD_LG | MCF_FEC_RXBD_NO | MCF_FEC_RXBD_CR | MCF_FEC_RXBD_OV))  rx_bd.ctl = MCF_FEC_RXBD_E | MCF_FEC_RXBD_W; /* Reception error */
		else { //pass_frame_to_upper_layer(&rx_bd);
			register struct pbuf* lwip_buf=pbuf_alloc(PBUF_RAW,rx_bd.len,PBUF_RAM);
	
			if (lwip_buf) {
				memcpy(lwip_buf->payload,rx_bd.ptr,lwip_buf->tot_len);
				switch (htons(((struct eth_hdr *)lwip_buf->payload)->type)) {
				/* IP or ARP packet? */
					case ETHTYPE_IP:
						/* full packet send to tcpip_thread to process */
						/* skip Ethernet header */
						if (netif->input(lwip_buf, netif) != ERR_OK) pbuf_free(lwip_buf);
						break;
					case ETHTYPE_ARP:
						/* pass p to ARP module */
						etharp_arp_input(netif, (struct eth_addr *)&netif->hwaddr, lwip_buf);
						break;
					default:
						pbuf_free(lwip_buf);
						break;
				}
				lwip_buf = NULL;
				rx_bd.ctl = MCF_FEC_RXBD_E | MCF_FEC_RXBD_W;
			}
		}
	}
	MCF_FEC_RDAR=0;
}

/**
 * This function does the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 */
static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  u8_t *ptr;
	static u8_t txbd_index;
	
  /* Suppress 'argument not used' warning */
  netif = netif;

  if (p->tot_len > MAX_FRAME_LEN)
    /* Frame too long, drop it */
    return ERR_MEM;
  
	tx_bd[txbd_index].len = p->tot_len; // < 64 ? p->tot_len+(64-p->tot_len) : p->tot_len;
  
  /* Walk the pbuf chain, fill the Tx packet buffer */
  q = p;
  ptr=tx_bd[txbd_index].buf;
  while (q != NULL) {
    memcpy(ptr, q->payload, q->len);
    ptr += q->len;
    q = q->next;
  }
	tx_bd[txbd_index].ctl |= MCF_FEC_TXBD_R;
  
  MCF_FEC_TDAR = 0; /* Start transmission */
  
  while (MCF_FEC_TDAR != 0); /* wait for transmission complete */
  
  MCF_FEC_EIR |= MCF_FEC_EIR_TXF | MCF_FEC_EIR_TXB;
  
  /* JUST debugging stuff */
   //~ unsigned int my_q_len=0;
	//~ while (my_q_len<tx_bd[txbd_index].len)
		//~ printf("%02x ",tx_bd[txbd_index].buf[my_q_len++]);

	//~ printf("buf.len: %d, buf.ctl: 0x%x, tx_bd.buf: 0x%x\n",tx_bd[txbd_index].len,tx_bd[txbd_index].ctl,tx_bd[txbd_index].buf);

  txbd_index=!txbd_index;
	
  return ERR_OK;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It does the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the interface is initialized
 *         We always return ERR_OK
 */
err_t
mcf5225xif_init(struct netif *netif)
{
	rtems_isr_entry old_isr_handler;
	struct if_config* if_config=netif->state;
	
	net_task_id=if_config->net_task;
	
	/* We directly use etharp_output() here to save a function call.
    * You can instead declare your own function an call etharp_output()
    * from it if you have to do some checks before sending (e.g. if link
    * is available...) */
	netif->output = etharp_output;
	netif->linkoutput = low_level_output;
	#if LWIP_NETIF_STATUS_CALLBACK
	netif->status_callback = mcf5225xif_status;
	#endif
	#if LWIP_NETIF_LINK_CALLBACK
	netif->link_callback = mcf5225xif_link;
	#endif
	
	netif->name[0]=if_config->name[0];
	netif->name[1]=if_config->name[1];
	netif->hwaddr_len = if_config->hwaddr_len;
	memcpy(netif->hwaddr,if_config->hwaddr,ETHARP_HWADDR_LEN);	/* set the mac address configured by the application */
	netif->mtu = if_config->mtu ; /* maximum transfer unit, configured by application */
	netif->flags = if_config->flags;	/* device capabilities, configured by application */

	
  MCF_FEC_ECR |= MCF_FEC_ECR_RESET;
  
  while (MCF_FEC_ECR&MCF_FEC_ECR_RESET) __asm__ ("nop");
  
  if (if_config->phy_init) if_config->phy_init();  /* call application specific optional extern phy initialization function */
  
  MCF_FEC_EIMR = 0;
  MCF_FEC_EIR= 0xFFFFFFFF;
  
  //~ Set MAC hardware address:
   MCF_FEC_PALR = (u32_t)( (netif->hwaddr[0] << 24)
                      | (netif->hwaddr[1] << 16)
                      | (netif->hwaddr[2] << 8 )
                      | (netif->hwaddr[3] << 0 ) );
    MCF_FEC_PAUR = (u32_t)( (netif->hwaddr[4] << 24)
                      | (netif->hwaddr[5] << 16) );
 
  /* Do whatever else is needed to initialize interface. */  
  MCF_FEC_OPD |= MCF_FEC_OPD_PAUSE_DUR(2); /* pause duration: send 2 pause frames */
  MCF_FEC_IAUR = 0;
  MCF_FEC_IALR = 0;
  MCF_FEC_GAUR = 0;
  MCF_FEC_GALR = 0;  
  MCF_FEC_EMRBR = ((MAX_FRAME_LEN+15)&~15); //<<4;  //RX_RING_SIZE*PBUF_POOL_BUFSIZE; //1536;
  MCF_FEC_ERDSR = (uint32_t)&rx_bd;
  MCF_FEC_ETDSR = (uint32_t)&tx_bd;
  MCF_FEC_RCR = (MAX_FRAME_LEN << 16) | MCF_FEC_RCR_MII_MODE;
  MCF_FEC_FRSR = 0x48<<2; /* avoid address clashing of receive and transmit data in FEC fifo */
	
	MCF_FEC_TCR = MCF_FEC_TCR_FDEN | MCF_FEC_TCR_HBC;
	
	MCF_FEC_MIBC = MCF_FEC_MIBC_MIB_DISABLE;
	/* TODO: clear MIB RAM??? */
	MCF_FEC_MIBC =~MCF_FEC_MIBC_MIB_DISABLE; /* enable MIBC */
  
	MCF_FEC_EIMR = 0;
	
	rtems_interrupt_catch(rx_frame_handler,91,&old_isr_handler); /* register ISR for RX_INTF interrupt*/
	MCF_INTC0_ICR27=0x10; /* set interrupt level */
	MCF_FEC_EIR= 0xFFFFFFFF; /* clear all pending interrupts */
	MCF_FEC_EIMR |= MCF_FEC_EIR_RXF; /* enable RX_INTF interrupt */
	MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASK27; /* enable FEC RX_INTF interrupt */
	
	/* enable FEC */
	MCF_FEC_ECR |= MCF_FEC_ECR_ETHER_EN;
	/* Start reception, if it's not started already */
	MCF_FEC_RDAR = 0;
	
	return ERR_OK;
}

void smi_init(u32_t clk_speed)
{
	MCF_FEC_MSCR = MSCR_MII_SPEED(clk_speed);
}

void smi_write(u8_t phy_addr,u8_t reg_addr,u16_t data)
{
	MCF_FEC_MMFR = MCF_FEC_MMFR_ST(0x1) | MCF_FEC_MMFR_OP_WRITE | (MCF_FEC_MMFR_PA(phy_addr)) | MCF_FEC_MMFR_RA(reg_addr) | MCF_FEC_MMFR_TA_10 | data;
	smi_init(bsp_get_CPU_clock_speed()); /* enable MII clock speed after MMFR is written */
	while ((MCF_FEC_EIR & MCF_FEC_EIR_MII) == 0) { __asm__ ("nop"); }
	smi_init(0); /* MII frame sent, disable clock until next operation */
	MCF_FEC_EIR |= MCF_FEC_EIR_MII;
}

u16_t smi_read(u8_t phy_addr,u8_t reg_addr)
{
	MCF_FEC_MMFR = MCF_FEC_MMFR_ST(0x1) | MCF_FEC_MMFR_OP_READ | (MCF_FEC_MMFR_PA(phy_addr)) | MCF_FEC_MMFR_RA(reg_addr) | MCF_FEC_MMFR_TA_10;
	smi_init(bsp_get_CPU_clock_speed()); /* enable MII clock speed after MMFR is written */
	while ((MCF_FEC_EIR & MCF_FEC_EIR_MII) == 0) { __asm__ ("nop"); }
	smi_init(0); /* MII frame sent, disable clock until next operation */
	MCF_FEC_EIR |= MCF_FEC_EIR_MII;
	
	return MCF_FEC_MMFR&0xFFFF;
}

#if LWIP_NETIF_STATUS_CALLBACK
static void
mcf5225xif_status(struct netif* netif)
{
	
  return;
}
#endif

#if LWIP_NETIF_LINK_CALLBACK
static void
mcf5225xif_link(struct netif* net_if)
{
	u16_t phy_status=smi_read(1,1); /* get phy status */	
	
	printf("received status: 0x%x\n",phy_status);
	
	if (!(phy_status&0x4))
		phy_status=smi_read(1,1);
	
	printf("received status: 0x%x\n",phy_status);
	
	if (phy_status&0x4)
		MCF_GPIO_PORTTC |= MCF_GPIO_PORTTC_PORTTC1; 
	else
		MCF_GPIO_PORTTC &=~MCF_GPIO_PORTTC_PORTTC1; 
}
#endif
