/*
 * GRSPW ROUTER APB-Register Driver.
 * 
 * COPYRIGHT (c) 2010-2017.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __GRSPW_ROUTER_H__
#define __GRSPW_ROUTER_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum number of ROUTER devices supported by driver */
#define ROUTER_MAX 2

#define ROUTER_ERR_OK 0
#define ROUTER_ERR_EINVAL -1
#define ROUTER_ERR_ERROR -2
#define ROUTER_ERR_TOOMANY -3
#define ROUTER_ERR_IMPLEMENTED -4

/* Hardware Information */
struct router_hw_info {
	uint8_t nports_spw;
	uint8_t nports_amba;
	uint8_t nports_fifo;
	int8_t srouting;
	int8_t pnp_enable;
	int8_t timers_avail;
	int8_t pnp_avail;
	uint8_t ver_major;
	uint8_t ver_minor;
	uint8_t ver_patch;
	uint8_t iid;

	/* Router capabilities */
	uint8_t amba_port_fifo_size;
	uint8_t spw_port_fifo_size;
	uint8_t rmap_maxdlen;
	int8_t aux_async;
	int8_t aux_dist_int_support;
	int8_t dual_port_support;
	int8_t dist_int_support;
	int8_t spwd_support;
	uint8_t pktcnt_support;
	uint8_t charcnt_support;
};

#define ROUTER_FLG_CFG		0x01
#define ROUTER_FLG_IID		0x02
#define ROUTER_FLG_IDIV		0x04
#define ROUTER_FLG_TPRES	0x08
#define ROUTER_FLG_TRLD		0x10
#define ROUTER_FLG_ALL		0x1f	/* All Above Flags */

struct router_config {
	uint32_t flags; /* Determine what configuration should be updated */

	/* Router Configuration Register */
	uint32_t config;

	/* Set Instance ID */
	uint8_t iid;

	/* SpaceWire Link Initialization Clock Divisor */
	uint8_t idiv;

	/* Timer Prescaler */
	uint32_t timer_prescaler;
};

/* Routing table address control */
struct router_route_acontrol {
	uint32_t control[31];
	uint32_t control_logical[224];
};

/* Routing table port mapping */
struct router_route_portmap {
	uint32_t pmap[31]; /* Port Setup for ports 1-31 */
	uint32_t pmap_logical[224]; /* Port setup for locgical addresses 32-255 */
};

/* Routing table */
#define ROUTER_ROUTE_FLG_MAP 0x01
#define ROUTER_ROUTE_FLG_CTRL 0x02
#define ROUTER_ROUTE_FLG_ALL 0x3	/* All Above Flags */
struct router_routing_table {
	uint32_t flags; /* Determine what configuration should be updated */

	struct router_route_acontrol acontrol;
	struct router_route_portmap portmap;
};

/* Set/Get Port Control/Status */
#define ROUTER_PORT_FLG_SET_CTRL	0x01
#define ROUTER_PORT_FLG_GET_CTRL	0x02
#define ROUTER_PORT_FLG_SET_STS	0x04
#define ROUTER_PORT_FLG_GET_STS	0x08
#define ROUTER_PORT_FLG_SET_CTRL2	0x10
#define ROUTER_PORT_FLG_GET_CTRL2	0x20
#define ROUTER_PORT_FLG_SET_TIMER	0x40
#define ROUTER_PORT_FLG_GET_TIMER	0x80
#define ROUTER_PORT_FLG_SET_PKTLEN	0x100
#define ROUTER_PORT_FLG_GET_PKTLEN	0x200
struct router_port {
	uint32_t flag;
	/* Port control */
	uint32_t ctrl;
	/* Port status */
	uint32_t sts;
	/* Port control 2 */
	uint32_t ctrl2;
	/* Timer  Reload */
	uint32_t timer_reload;
	/* Maximum packet length */
	uint32_t packet_length;
};

/* Register GRSPW Router driver to Driver Manager */
void router_register_drv(void);

extern void *router_open(unsigned int dev_no);
extern int router_close(void *d);
extern int router_print(void *d);
extern int router_hwinfo_get(void *d, struct router_hw_info *hwinfo);

/* Router general config */
extern int router_config_set(void *d, struct router_config *cfg);
extern int router_config_get(void *d, struct router_config *cfg);

/* Routing table config */
extern int router_routing_table_set(void *d, 
		struct router_routing_table *cfg);
extern int router_routing_table_get(void *d, 
		struct router_routing_table *cfg);

/* Router Set/Get Port configuration */
extern int router_port_ioc(void *d, int port, struct router_port *cfg);

/* Read Port Control register */
extern int router_port_ctrl_get(void *d, int port, uint32_t *ctrl);
/* Read Port Control2 register */
extern int router_port_ctrl2_get(void *d, int port, uint32_t *ctrl2);
/* Write Port Control Register */
extern int router_port_ctrl_set(void *d, int port, uint32_t ctrl);
/* Write Port Control2 Register */
extern int router_port_ctrl2_set(void *d, int port, uint32_t ctrl2);
/* Set Timer Reload Value for a specific port */
extern int router_port_treload_set(void *d, int port, uint32_t reload);
/* Get Timer Reload Value for a specific port */
extern int router_port_treload_get(void *d, int port, uint32_t *reload);
/* Get Maximum packet length for a specific port */
extern int router_port_maxplen_get(void *d, int port, uint32_t *length);
/* Set Maximum packet length for a specific port */
extern int router_port_maxplen_set(void *d, int port, uint32_t length);

/* Check Port Status register and clear errors if there are */
extern int router_port_status(void *d, int port, uint32_t *sts);

#define ROUTER_LINK_STATUS_ERROR_RESET 0
#define ROUTER_LINK_STATUS_ERROR_WAIT 1
#define ROUTER_LINK_STATUS_READY 2
#define ROUTER_LINK_STATUS_STARTED 3
#define ROUTER_LINK_STATUS_CONNECTING 4
#define ROUTER_LINK_STATUS_RUN_STATE 5
/* Get Link status */
extern int router_port_link_status(void *d, int port);
/* Operate a Link */
extern int router_port_enable(void *d, int port);
extern int router_port_disable(void *d, int port);
extern int router_port_link_stop(void *d, int port);
extern int router_port_link_start(void *d, int port);
extern int router_port_link_receive_spill(void *d, int port);
extern int router_port_link_transmit_reset(void *d, int port);

/* Get port credit counter register */
extern int router_port_cred_get(void *d, int port, uint32_t *cred);

/*
 * ROUTER RTACTRL register fields
 */
#define RTACTRL_SR (0x1 << RTACTRL_SR_BIT)
#define RTACTRL_EN (0x1 << RTACTRL_EN_BIT)
#define RTACTRL_PR (0x1 << RTACTRL_PR_BIT)
#define RTACTRL_HD (0x1 << RTACTRL_HD_BIT)

#define RTACTRL_SR_BIT 3
#define RTACTRL_EN_BIT 2
#define RTACTRL_PR_BIT 1
#define RTACTRL_HD_BIT 0

/* Individual route modification */
#define ROUTER_ROUTE_PACKETDISTRIBUTION_ENABLE (0x1 << 16)
#define ROUTER_ROUTE_PACKETDISTRIBUTION_DISABLE (0x0 << 16)
#define ROUTER_ROUTE_SPILLIFNOTREADY_ENABLE RTACTRL_SR
#define ROUTER_ROUTE_SPILLIFNOTREADY_DISABLE 0
#define ROUTER_ROUTE_ENABLE RTACTRL_EN
#define ROUTER_ROUTE_DISABLE 0
#define ROUTER_ROUTE_PRIORITY_HIGH RTACTRL_PR
#define ROUTER_ROUTE_PRIORITY_LOW 0
#define ROUTER_ROUTE_HEADERDELETION_ENABLE RTACTRL_HD
#define ROUTER_ROUTE_HEADERDELETION_DISABLE 0
struct router_route {
	uint8_t from_address;
	uint8_t to_port[32];
	int count;
	int options;
};
extern int router_route_set(void *d, struct router_route *route);
extern int router_route_get(void *d, struct router_route *route);

/* Router configuration port write enable */
extern int router_write_enable(void *d);
extern int router_write_disable(void *d);

/* Router reset */
extern int router_reset(void *d);

/* Set Instance ID */
extern int router_instance_set(void *d, uint8_t iid);
/* Get Instance ID */
extern int router_instance_get(void *d, uint8_t *iid);

/* Set SpaceWire Link Initialization Clock Divisor */
extern int router_idiv_set(void *d, uint8_t idiv);
/* Get SpaceWire Link Initialization Clock Divisor */
extern int router_idiv_get(void *d, uint8_t *idiv);

/* Set Timer Prescaler */
extern int router_tpresc_set(void *d, uint32_t prescaler);
/* Get Timer Prescaler */
extern int router_tpresc_get(void *d, uint32_t *prescaler);

/* Set/get Router configuration */
extern int router_cfgsts_set(void *d, uint32_t cfgsts);
extern int router_cfgsts_get(void *d, uint32_t *cfgsts);

/* Router timecode */
extern int router_tc_enable(void *d);
extern int router_tc_disable(void *d);
extern int router_tc_reset(void *d);
extern int router_tc_get(void *d);

/* Router Interrupts */
/*
 * ROUTER IMASK register fields
 */
#define IMASK_PE (0x1 << IMASK_PE_BIT)
#define IMASK_SR (0x1 << IMASK_SR_BIT)
#define IMASK_RS (0x1 << IMASK_RS_BIT)
#define IMASK_TT (0x1 << IMASK_TT_BIT)
#define IMASK_PL (0x1 << IMASK_PL_BIT)
#define IMASK_TS (0x1 << IMASK_TS_BIT)
#define IMASK_AC (0x1 << IMASK_AC_BIT)
#define IMASK_RE (0x1 << IMASK_RE_BIT)
#define IMASK_IA (0x1 << IMASK_IA_BIT)
#define IMASK_LE (0x1 << IMASK_LE_BIT)
#define IMASK_ME (0x1 << IMASK_ME_BIT)
#define IMASK_ALL ( IMASK_PE | IMASK_SR | IMASK_RS | IMASK_TT \
		IMASK_PL | IMASK_TS | IMASK_AC | IMASK_RE | IMASK_IA \
		IMASK_LE | IMASK_ME)

#define IMASK_PE_BIT 10
#define IMASK_SR_BIT 9
#define IMASK_RS_BIT 8
#define IMASK_TT_BIT 7
#define IMASK_PL_BIT 6
#define IMASK_TS_BIT 5
#define IMASK_AC_BIT 4
#define IMASK_RE_BIT 3
#define IMASK_IA_BIT 2
#define IMASK_LE_BIT 1
#define IMASK_ME_BIT 0

#define ROUTER_INTERRUPT_ALL IMASK_ALL
#define ROUTER_INTERRUPT_SPWPNP_ERROR IMASK_PE
#define ROUTER_INTERRUPT_SPILLED IMASK_SR
#define ROUTER_INTERRUPT_RUNSTATE IMASK_RS
#define ROUTER_INTERRUPT_TC_TRUNCATION IMASK_TT
#define ROUTER_INTERRUPT_PACKET_TRUNCATION IMASK_PL
#define ROUTER_INTERRUPT_TIMEOUT IMASK_TS
#define ROUTER_INTERRUPT_CFGPORT IMASK_AC
#define ROUTER_INTERRUPT_RMAP_ERROR IMASK_RE
#define ROUTER_INTERRUPT_INVALID_ADDRESS IMASK_IA
#define ROUTER_INTERRUPT_LINK_ERROR IMASK_LE
#define ROUTER_INTERRUPT_MEMORY_ERROR IMASK_ME
extern int router_port_interrupt_unmask(void *d, int port);
extern int router_port_interrupt_mask(void *d, int port);
extern int router_interrupt_unmask(void *d, int options);
extern int router_interrupt_mask(void *d, int options);

/* Router Interrupt code generation */
/*
 * ROUTER ICODEGEN register fields
 */
#define ICODEGEN_UA (0x1 << ICODEGEN_UA_BIT)
#define ICODEGEN_AH (0x1 << ICODEGEN_AH_BIT)
#define ICODEGEN_IT (0x1 << ICODEGEN_IT_BIT)
#define ICODEGEN_TE (0x1 << ICODEGEN_TE_BIT)
#define ICODEGEN_EN (0x1 << ICODEGEN_EN_BIT)
#define ICODEGEN_IN (0x1f << ICODEGEN_IN_BIT)

#define ICODEGEN_UA_BIT 20
#define ICODEGEN_AH_BIT 19
#define ICODEGEN_IT_BIT 18
#define ICODEGEN_TE_BIT 17
#define ICODEGEN_EN_BIT 16
#define ICODEGEN_IN_BIT 0

#define ROUTER_ICODEGEN_ITYPE_EDGE ICODEGEN_IT
#define ROUTER_ICODEGEN_ITYPE_LEVEL 0
#define ROUTER_ICODEGEN_AUTOUNACK_ENABLE ICODEGEN_UA
#define ROUTER_ICODEGEN_AUTOUNACK_DISABLE 0
#define ROUTER_ICODEGEN_AUTOACK_ENABLE ICODEGEN_AH
#define ROUTER_ICODEGEN_AUTOACK_DISABLE 0
extern int router_icodegen_enable(void *d, uint8_t intn, uint32_t aitimer, 
		int options);
extern int router_icodegen_disable(void *d);

/* Router interrupt change timers */
extern int router_isrctimer_set(void *d, uint32_t reloadvalue);
extern int router_isrctimer_get(void *d, uint32_t *reloadvalue);

/* Router interrupt timers */
extern int router_isrtimer_set(void *d, uint32_t reloadvalue);
extern int router_isrtimer_get(void *d, uint32_t *reloadvalue);

#ifdef __cplusplus
}
#endif

#endif
