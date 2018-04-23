/* BSP specific wrapper for rtems_bsdnet_attach(). This wrapper
 * dispatches to the correct driver attach routine depending on
 * the board type, boot parameters, link status etc.
 *
 * Also, it performs board-specific setup of driver parameters
 * (such as ethernet address, base addresses and the like)
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

#include <unistd.h>

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/pci.h>

#include <bsp/early_enet_link_status.h>

#include <bsp/if_mve_pub.h>
#include <bsp/if_gfe_pub.h>
#include <bsp/if_em_pub.h>

#include <stdio.h>
#include <string.h>
#include <assert.h>

#define IS_6100()		(MVME6100 == BSP_getBoardType())
#define IS_5500()		(MVME5500 == BSP_getBoardType())

static int bsp_gfe_attach(struct rtems_bsdnet_ifconfig *, int);
static int mvme5500_em_attach (struct rtems_bsdnet_ifconfig *, int);
static int mvme5500_em_find_onboard_unit(void);

char BSP_auto_network_driver_name[20] = {0,};

static BSP_NetIFDescRec mvme6100_netifs[] = {
	{
		name:			"mve1",
		description:	"MV64360 built-in 10/100/1000 Ethernet 1",
		attach_fn:		rtems_mve_attach
	},
	{
		name:			"mve2",
		description:	"MV64360 built-in 10/100/1000 Ethernet 2",
		attach_fn:		rtems_mve_attach
	},
	{
		name:			0,
	}
};

static BSP_NetIFDescRec mvme5500_netifs[] = {
	{
		name:			"em1",
		description:	"Intel 82544 on-board 10/100/1000 Ethernet (port 1)",
		attach_fn:		mvme5500_em_attach,
	},
	{
		name:			"gfe1",
		description:	"GT64260 built-in 10/100 Ethernet (port 2)",
		attach_fn:		bsp_gfe_attach,
	},
	{
		name:			0,
	}
};


/* wrap the 'gfe' and 'em' attach functions.
 * RATIONALE: 'rtems_gfe_attach()' and 'rtems_em_attach()' are
 *            *chip* specific functions. However, they require
 *            some *board* specific parameter setup prior to
 *            being attached which is what these wrappers do...
 */

static unsigned em_info[];

static int scan4irqLine(int bus, int dev, int fn, void *uarg)
{
unsigned char  byte;
unsigned short word;
int            i;

	/* count the number of 82544 we find */
	pci_read_config_word(bus, dev, fn, PCI_VENDOR_ID, &word);
	if ( 0x8086 != word )
		return 0;

	pci_read_config_word(bus, dev, fn, PCI_DEVICE_ID, &word);
	for ( i = 0; em_info[i]; i++ ) {
		if ( em_info[i] == word )
			break;
	}

	if ( !em_info[i] )
		return 0;

	/* found a candidate; bump unit number */
	(*(int *)uarg)++;

	pci_read_config_byte(bus, dev, fn, PCI_INTERRUPT_LINE, &byte);

	/* On the mvme5500 the 82544 is hooked to GPP IRQ 20 */

	return ( BSP_IRQ_GPP_0 + 20 == byte ) ? 1 : 0;
}

/* Setup only once */
static void
onboard_em_setup_once(void)
{
static char done = 0;

	/* If scanning didn't do anything, passing 0 will setup all interfaces  */
	if (   !done
		&& rtems_em_pci_setup( mvme5500_em_find_onboard_unit() > 0 ) ) {
		done=1;
	}
}

static void
onboard_gfe_setup_once(void)
{
static char done = 0;

	/* must setup HW address -- note that the label on the
	 * board indicates that the gfe is the second interface
	 * but motLoad etc. interprets the order actually
	 * the other way round...
	 */
	if (   !done
		&& rtems_gfe_setup( 1, BSP_enetAddr0, BSP_MV64x60_BASE ) > 0 ) {
		done=1;
	}
}


/* Find the unit number of the on-board 82544 (even if there is another one
 * plugged into PMC...
 *
 * RETURNS: unit #  (>0) or zero if nothing at all was found. (New board rev.
 *          with the 82544 hooked to a different IRQ line, new PCI device ID,
 *          ...)
 */
static int
mvme5500_em_find_onboard_unit(void)
{
int unit = 0;
void *h;
	/* Determine the on-board 'em' adapter; we know it's interrupt line :-) */
	for ( h=0; (h=BSP_pciScan(h, scan4irqLine, &unit)); )
		/* nothing else to do */;
	return h ? unit : 0;
}

static int
mvme5500_em_attach(struct rtems_bsdnet_ifconfig *ifcfg, int attaching)
{
	if ( attaching ) {
		onboard_em_setup_once();

		/* Make sure there is no conflict in MAC address -- see below
		 * why we 'swap' the addresses. (We actually don't but swap the
		 * order of the interfaces so they match the label.)
		 */
		if ( !ifcfg->hardware_address )
			ifcfg->hardware_address = BSP_enetAddr1;
	}

	return rtems_em_attach(ifcfg, attaching);
}

static int
bsp_gfe_attach(struct rtems_bsdnet_ifconfig *ifcfg, int attaching)
{
	if ( attaching ) {
		onboard_gfe_setup_once();
	}
	return rtems_gfe_attach(ifcfg, attaching);
}

BSP_NetIFDesc
BSP_availableNetIFs(void)
{
	if ( IS_6100() )
		return mvme6100_netifs;
	if ( IS_5500() )
		return mvme5500_netifs;

	fprintf(stderr,"BSP_availableNetIFs() -- productIdent not set? unable to identify board type\n");

	return 0;
}

typedef int (*read_op_t)(int,unsigned);
typedef int (*write_op_t)(int,unsigned,unsigned);

struct poll_job {
	read_op_t	rdop;
	write_op_t	wrop;
	int			unit;
};

static int
check_phys(struct poll_job *job)
{
struct poll_job *j;
int             rval = -2;
	for ( j=job; j->rdop; j++ ) {
		unsigned w;
		w = j->rdop(j->unit, 1/*status*/);
		if ( 0x04 & w )	   /* active link */
			return j-job;
		if ( !(0x20 & w) ) /* aneg not done   */
			rval = -1;	
	}
	return rval;
}

/* check a number of phys
 * RETURNS: -1 if at least one is still busy
 *          -2 if all are terminated but no link is found
 *         >=0 index of first IF with a live link
 */
static int
poll_phys(struct poll_job *job)
{
int				rval;
struct poll_job *j   = job;
int             retry = 4;

	/* see if we already have a link */
	if ( (rval=check_phys(job)) < 0 ) {
		/* no - start negotiation */
		for ( j = job; j->rdop; j++ ) {
			j->wrop(j->unit, 0/* ctrl */, 0x1200 /* start aneg */);
		}

		do {
			sleep(1);
		} while ( -1 == (rval = check_phys(job)) && retry-- ); 
	}

	return rval;
}


/* note that detaching is not supported by the current version of the BSD stack */
int
BSP_auto_enet_attach(struct rtems_bsdnet_ifconfig *ifcfg, int attaching)
{
int             i = -1;
BSP_NetIFDesc   d = BSP_availableNetIFs();
struct poll_job	job[3];

	if ( !d )
		return -1;

	/* If they pass a name, find the attach fn */
	if ( ifcfg->name  && RTEMS_BSP_NETWORK_DRIVER_NAME != ifcfg->name && attaching ) {
		for (i = 0; d[i].name; i++ ) {
			if ( !strcmp(d[i].name, ifcfg->name) ) {
				ifcfg->attach = d[i].attach_fn;
				break;
			}
		}
		if ( !d[i].name ) {
			fprintf(stderr,"WARNING: have no '%s' interface - using default\n", ifcfg->name);
			ifcfg->name = 0;
		}
	}

	if ( !ifcfg->name || ifcfg->name == RTEMS_BSP_NETWORK_DRIVER_NAME ) {
		/* automatically choose and attach an interface */
		if ( RTEMS_BSP_NETWORK_DRIVER_NAME[0] ) {
			fprintf(stderr,
					"Configuration error: 'auto' network if already chosen (%s)\n",
					RTEMS_BSP_NETWORK_DRIVER_NAME);
			return -1;
		}
		if ( IS_6100() ) {
#define ops rtems_mve_early_link_check_ops
			assert(ops.num_slots >= 2);
			ops.init(0);
			ops.init(1);
			job[0].rdop = ops.read_phy;
			job[0].wrop = ops.write_phy;
			job[0].unit = 0;
			job[1].rdop = ops.read_phy;
			job[1].wrop = ops.write_phy;
			job[1].unit = 1;
#undef ops
		} else if ( IS_5500() ) {
#define opsgfe rtems_gfe_early_link_check_ops
#define opsem  rtems_em_early_link_check_ops
			assert(opsgfe.num_slots >= 1);
			onboard_gfe_setup_once();
			opsgfe.init(0);
			assert(opsem.num_slots >= 1);
			onboard_em_setup_once();
			opsem.init(0);
			job[0].rdop = opsem.read_phy;
			job[0].wrop = opsem.write_phy;
			job[0].unit = 0;
			job[1].rdop = opsgfe.read_phy;
			job[1].wrop = opsgfe.write_phy;
			job[1].unit = 0;
#undef opsgfe
#undef opsem
		}
		job[2].rdop = 0; /* tag end */
		i = poll_phys(job);
		if ( i >= 0 ) {
			printf("L");
		} else {
			i = 0;
			printf("No l");
		}
		printf("ink detected; attaching %s\n",d[i].name);

		/* set attach function and IF name */
		ifcfg->attach = d[i].attach_fn;
		ifcfg->name   = RTEMS_BSP_NETWORK_DRIVER_NAME;
		strcpy(RTEMS_BSP_NETWORK_DRIVER_NAME, d[i].name);
	}
	return ifcfg->attach(ifcfg, attaching);
}

/* from 'em' */

/* PCI Device IDs */
#define E1000_DEV_ID_82542               0x1000
#define E1000_DEV_ID_82543GC_FIBER       0x1001
#define E1000_DEV_ID_82543GC_COPPER      0x1004
#define E1000_DEV_ID_82544EI_COPPER      0x1008
#define E1000_DEV_ID_82544EI_FIBER       0x1009
#define E1000_DEV_ID_82544GC_COPPER      0x100C
#define E1000_DEV_ID_82544GC_LOM         0x100D
#define E1000_DEV_ID_82540EM             0x100E
#define E1000_DEV_ID_82541ER_LOM         0x1014
#define E1000_DEV_ID_82540EM_LOM         0x1015
#define E1000_DEV_ID_82540EP_LOM         0x1016
#define E1000_DEV_ID_82540EP             0x1017
#define E1000_DEV_ID_82540EP_LP          0x101E
#define E1000_DEV_ID_82545EM_COPPER      0x100F
#define E1000_DEV_ID_82545EM_FIBER       0x1011
#define E1000_DEV_ID_82545GM_COPPER      0x1026
#define E1000_DEV_ID_82545GM_FIBER       0x1027
#define E1000_DEV_ID_82545GM_SERDES      0x1028
#define E1000_DEV_ID_82546EB_COPPER      0x1010
#define E1000_DEV_ID_82546EB_FIBER       0x1012
#define E1000_DEV_ID_82546EB_QUAD_COPPER 0x101D
#define E1000_DEV_ID_82541EI             0x1013
#define E1000_DEV_ID_82541EI_MOBILE      0x1018
#define E1000_DEV_ID_82541ER             0x1078
#define E1000_DEV_ID_82547GI             0x1075
#define E1000_DEV_ID_82541GI             0x1076
#define E1000_DEV_ID_82541GI_MOBILE      0x1077
#define E1000_DEV_ID_82541GI_LF          0x107C
#define E1000_DEV_ID_82546GB_COPPER      0x1079
#define E1000_DEV_ID_82546GB_FIBER       0x107A
#define E1000_DEV_ID_82546GB_SERDES      0x107B
#define E1000_DEV_ID_82546GB_PCIE        0x108A
#define E1000_DEV_ID_82547EI             0x1019
#define E1000_DEV_ID_82547EI_MOBILE      0x101A
#define E1000_DEV_ID_82573E              0x108B
#define E1000_DEV_ID_82573E_IAMT         0x108C

#define E1000_DEV_ID_82546GB_QUAD_COPPER 0x1099

static unsigned em_info[] =
{
        /* Intel(R) PRO/1000 Network Connection */
        E1000_DEV_ID_82540EM,
        E1000_DEV_ID_82540EM_LOM,
        E1000_DEV_ID_82540EP,
        E1000_DEV_ID_82540EP_LOM,
        E1000_DEV_ID_82540EP_LP,

        E1000_DEV_ID_82541EI,
        E1000_DEV_ID_82541ER,
        E1000_DEV_ID_82541ER_LOM,
        E1000_DEV_ID_82541EI_MOBILE,
        E1000_DEV_ID_82541GI,
        E1000_DEV_ID_82541GI_LF,
        E1000_DEV_ID_82541GI_MOBILE,

        E1000_DEV_ID_82542,

        E1000_DEV_ID_82543GC_FIBER,
        E1000_DEV_ID_82543GC_COPPER,

        E1000_DEV_ID_82544EI_COPPER,
        E1000_DEV_ID_82544EI_FIBER,
        E1000_DEV_ID_82544GC_COPPER,
        E1000_DEV_ID_82544GC_LOM,

        E1000_DEV_ID_82545EM_COPPER,
        E1000_DEV_ID_82545EM_FIBER,
        E1000_DEV_ID_82545GM_COPPER,
        E1000_DEV_ID_82545GM_FIBER,
        E1000_DEV_ID_82545GM_SERDES,

        E1000_DEV_ID_82546EB_COPPER,
        E1000_DEV_ID_82546EB_FIBER,
        E1000_DEV_ID_82546EB_QUAD_COPPER,
        E1000_DEV_ID_82546GB_COPPER,
        E1000_DEV_ID_82546GB_FIBER,
        E1000_DEV_ID_82546GB_SERDES,
        E1000_DEV_ID_82546GB_PCIE,
        E1000_DEV_ID_82546GB_QUAD_COPPER,

        E1000_DEV_ID_82547EI,
        E1000_DEV_ID_82547EI_MOBILE,
        E1000_DEV_ID_82547GI,

        E1000_DEV_ID_82573E,
        E1000_DEV_ID_82573E_IAMT,

        /* required last entry */
        0,
};
