#ifndef PPC_MOTLOAD_VPD_H
#define PPC_MOTLOAD_VPD_H

/* MotLoad VPD format */

/* Till Straumann, 2005; see copyright notice at the end of this file */

#ifdef __cplusplus
  extern "C" {
#endif

/*
VPD = "MOTOROLA" , { field }

field = type_byte, length_byte, { data }
*/

/* Known fields so far */
typedef enum {
	ProductIdent	= 0x01, /* String */
	AssemblyNumber	= 0x02, /* String */
	SerialNumber	= 0x03,	/* String */
	CpuClockHz		= 0x05, /* binary (5bytes), 0x01 byte appended to unsigned int */
	BusClockHz		= 0x06, /* binary (5bytes), 0x01 byte appended to unsigned int */
	EthernetAddr	= 0x08, /* binary (7bytes), 0x00 byte appended, 2nd has 0x01 appended */
	CpuType			= 0x09, /* String */
	EEpromCrc		= 0x0a, /* binary (4bytes) */
	FlashConfig		= 0x0b, /* binary */
	L2CacheConfig	= 0x0e, /* binary */
	VPDRevision		= 0x0f, /* binary (4bytes) */
	L3CacheConfig	= 0x19, /* binary */
	End				= 0xff
} VpdKey;

typedef struct {
	VpdKey	key;		/* key for the data item to be read into 'buf' */
	char	instance;	/* instance # (starting with 0) - some keys are present more than one time */
	void	*buf;		/* pointer to area where the data item is to be stored */
	int		buflen;		/* available space in the buffer */
	char	found;		/* set by BSP_vpdRetrieveFields() to the original length as found in the PROM */
} VpdBufRec, *VpdBuf;


#define VPD_END	{ key:End, }


/* Scan the VPD EEPROM for a number of fields
 *
 * Pass an array of VpdBufRec items. The routine
 * fills the 'buf'fers for all keys that are found
 * and sets the 'found' field to the original length
 * of the data (i.e., as found in the PROM) so that
 * the routine could be called again with a larger
 * buffer.
 *
 * NOTE: - the array must be terminated by a VPD_END record!
 *       - no CRC check is performed.
 *       - INTERRUPT MANAGEMENT MUST BE FUNCTIONAL
 *
 * RETURNS: 0 on success, -1 if any read errors were
 *          encountered or if the "MOTOROLA" header
 *			was not found.
 */
int
BSP_vpdRetrieveFields(VpdBuf data);

/* Example:
 *	Read 2nd ethernet address:
 *
 *		char enet_addr_2[6];
 *
 *		VpdBufRec enetVpd [] = {
 *			{ key: EthernetAddr, instance: 1, buf: enet_addr_2, buflen: 2},
 *			VPD_END
 *		};
 *
 *		if ( BSP_vpdRetrieveFields(enetVpd) ) {
 *			error("ethernet address couldn't be read\n");
 *		} else if ( enetVpd[0].found < 6 ) {
 *			error("2nd ethernet address not found in VPD\n");
 *		} else {
 *			use_it(enet_addr_2);
 *		}
 */


/* Simple wrapper if only one field is needed
 *
 * RETURNS: original length if key is found, -1 on error or if key is not found
 */
int
BSP_vpdRetrieveKey(VpdKey k, void *buf, int buflen, int instance);

#ifdef __cplusplus
  }
#endif

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

#endif
