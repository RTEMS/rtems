#include "if_xxx_rtems.c"
#include <bsp/early_enet_link_status.h>
#include <bsp/if_em_pub.h>

/* Provide a routine to check link status early,
 * i.e., before the network is really running.
 * In case someone wants to decide whether to use/configure
 * this interface at all :-)
 *
 * NOTE: this routine tries to enable autonegotiation!
 *
 * unit: unit number starting with 1 (usual BSDNET convention)
 *
 * RETURNS: Phy status register contents (1<<2 means link up).
 *          or -1 on error.
 */

/* 
 * Authorship
 * ----------
 * This software ('beatnik' RTEMS BSP for MVME6100 and MVME5500) was
 *     created by Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 * 
 * Acknowledgement of sponsorship
 * ------------------------------
 * The 'beatnik' BSP was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 * 
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 * 
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 * 
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.  
 * 
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 * 
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */ 

static int
em_early_init(int idx)
{
	if ( idx < 0 || idx >= NETDRIVER_SLOTS )
		return -1;
	return em_hw_early_init(&the_em_devs[idx]);
}

static int
em_early_read_phy(int idx, unsigned reg)
{
unsigned short data;
	if ( idx < 0 || idx >= NETDRIVER_SLOTS )
		return -1;
	/* Bizarre - I always have to read PHY_STATUS twice until a good link
	 * status is read
	 */	
	if ( em_read_phy_reg(&the_em_devs[idx].d_softc.hw, reg, &data) )
		return -1;
	if ( PHY_STATUS == reg ) {
		/* read again */
		if ( em_read_phy_reg(&the_em_devs[idx].d_softc.hw, PHY_STATUS, &data) )
			return -1;
	}
	return data;
}

static int
em_early_write_phy(int idx, unsigned reg, unsigned val)
{
	if ( idx < 0 || idx >= NETDRIVER_SLOTS )
		return -1;
	return em_write_phy_reg(&the_em_devs[idx].d_softc.hw, reg, val);
}

rtems_bsdnet_early_link_check_ops
rtems_em_early_link_check_ops = {
	init:		em_early_init,
	read_phy:	em_early_read_phy,
	write_phy:	em_early_write_phy,
	name:		NETDRIVER,
	num_slots:	NETDRIVER_SLOTS
};
