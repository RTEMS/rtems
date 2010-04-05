#ifndef FEC_H
#define FEC_H

#include <arch/sys_arch.h>
#include <lwip/netif.h>
#include <stdbool.h>

struct if_config {
	u8_t flags;
	u8_t hwaddr_len;
	u16_t mtu;
	u8_t hwaddr[NETIF_MAX_HWADDR_LEN];
	sys_thread_t net_task;
	void(*phy_init)(void);
	u8_t name[2];
};

extern err_t mcf5225xif_init(struct netif *);
extern void handle_rx_frame(struct netif*);

extern void smi_init(u32_t);
extern void smi_write(u8_t,u8_t,u16_t);
extern u16_t smi_read(u8_t,u8_t);

#define MAX_FRAME_LEN		1518
#define MTU_SIZE  (MAX_FRAME_LEN-18)
#define MSCR_MII_SPEED(clk)  ((clk/5000000+1)<<1)
#define PHY_ADDR			1
#define PHY_REG_ID1		2	
#define PHY_REG_ID2		3	

#endif /* FEC_H */
