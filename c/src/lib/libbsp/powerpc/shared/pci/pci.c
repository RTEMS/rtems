/*
 * pci.c :  this file contains basic PCI Io functions.
 *
 *  Copyright (C) 1999 valette@crf.canon.fr
 *
 *  This code is heavily inspired by the public specification of STREAM V2
 *  that can be found at :
 *
 *      <http://www.chorus.com/Documentation/index.html> by following
 *  the STREAM API Specification Document link.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Till Straumann, <strauman@slac.stanford.edu>, 1/2002
 *   - separated bridge detection code out of this file
 */

#include <rtems.h>
#include <bsp.h>

#include <libcpu/io.h>
#include <bsp/pci.h>
#include <rtems/bspIo.h>

#undef SHOW_PCI_SETTING

/* allow for overriding these definitions */
#ifndef PCI_CONFIG_ADDR
#define PCI_CONFIG_ADDR      0xcf8
#endif
#ifndef PCI_CONFIG_DATA
#define PCI_CONFIG_DATA      0xcfc
#endif

/* define a shortcut */
#define pci  BSP_pci_configuration

#ifndef  PCI_CONFIG_ADDR_VAL
#define  PCI_CONFIG_ADDR_VAL(bus, slot, funcion, offset) \
     (0x80<<24|((bus)<<16)|(PCI_DEVFN((slot),(function))<<8)|(((offset)&~3)))
#endif

#ifndef  PCI_CONFIG_WR_ADDR
#define  PCI_CONFIG_WR_ADDR( addr, val ) out_le32((unsigned int*)(addr), (val))
#endif

#define PCI_CONFIG_SET_ADDR(addr, bus, slot,function,offset) \
  PCI_CONFIG_WR_ADDR((addr), PCI_CONFIG_ADDR_VAL((bus), (slot), (function), (offset)))


extern void detect_host_bridge(void);

/*
 * Bit encode for PCI_CONFIG_HEADER_TYPE register
 */
unsigned char ucMaxPCIBus;

static int
indirect_pci_read_config_byte(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint8_t       *val
) {
  PCI_CONFIG_SET_ADDR(pci.pci_config_addr, bus, slot, function, offset);
  *val = in_8(pci.pci_config_data + (offset&3));
  return PCIBIOS_SUCCESSFUL;
}

static int
indirect_pci_read_config_word(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint16_t      *val
) {
  *val = 0xffff;
  if (offset&1)
    return PCIBIOS_BAD_REGISTER_NUMBER;

  PCI_CONFIG_SET_ADDR(pci.pci_config_addr, bus, slot, function, offset);
  *val = in_le16((volatile unsigned short *)(pci.pci_config_data + (offset&3)));
  return PCIBIOS_SUCCESSFUL;
}

static int
indirect_pci_read_config_dword(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint32_t *val
) {
  *val = 0xffffffff;
  if (offset&3)
    return PCIBIOS_BAD_REGISTER_NUMBER;

  PCI_CONFIG_SET_ADDR(pci.pci_config_addr, bus, slot, function, offset);
  *val = in_le32((volatile unsigned int *)pci.pci_config_data);
  return PCIBIOS_SUCCESSFUL;
}

static int
indirect_pci_write_config_byte(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint8_t       val
) {
  PCI_CONFIG_SET_ADDR(pci.pci_config_addr, bus, slot, function, offset);
  out_8(pci.pci_config_data + (offset&3), val);
  return PCIBIOS_SUCCESSFUL;
}

static int
indirect_pci_write_config_word(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint16_t      val
) {
  if (offset&1)
    return PCIBIOS_BAD_REGISTER_NUMBER;

  PCI_CONFIG_SET_ADDR(pci.pci_config_addr, bus, slot, function, offset);
  out_le16((volatile unsigned short *)(pci.pci_config_data + (offset&3)), val);
  return PCIBIOS_SUCCESSFUL;
}

static int
indirect_pci_write_config_dword(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint32_t      val
) {
  if (offset&3)
    return PCIBIOS_BAD_REGISTER_NUMBER;
  PCI_CONFIG_SET_ADDR(pci.pci_config_addr, bus, slot, function, offset);
  out_le32((volatile unsigned int *)pci.pci_config_data, val);
  return PCIBIOS_SUCCESSFUL;
}

const pci_config_access_functions pci_indirect_functions = {
  indirect_pci_read_config_byte,
  indirect_pci_read_config_word,
  indirect_pci_read_config_dword,
  indirect_pci_write_config_byte,
  indirect_pci_write_config_word,
  indirect_pci_write_config_dword
};

rtems_pci_config_t BSP_pci_configuration = {
  (volatile unsigned char*)PCI_CONFIG_ADDR,
  (volatile unsigned char*)PCI_CONFIG_DATA,
  &pci_indirect_functions
};

static int
direct_pci_read_config_byte(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint8_t      *val
) {
  if (bus != 0 || (1<<slot & 0xff8007fe)) {
    *val=0xff;
     return PCIBIOS_DEVICE_NOT_FOUND;
  }
  *val=in_8(pci.pci_config_data + ((1<<slot)&~1)
   + (function<<8) + offset);
  return PCIBIOS_SUCCESSFUL;
}

static int
direct_pci_read_config_word(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint16_t     *val
) {
  *val = 0xffff;
  if (offset&1)
    return PCIBIOS_BAD_REGISTER_NUMBER;
  if (bus != 0 || (1<<slot & 0xff8007fe))
     return PCIBIOS_DEVICE_NOT_FOUND;

  *val=in_le16((volatile unsigned short *)
      (pci.pci_config_data + ((1<<slot)&~1)
       + (function<<8) + offset));
  return PCIBIOS_SUCCESSFUL;
}

static int
direct_pci_read_config_dword(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint32_t     *val
) {
  *val = 0xffffffff;
  if (offset&3)
    return PCIBIOS_BAD_REGISTER_NUMBER;
  if (bus != 0 || (1<<slot & 0xff8007fe))
     return PCIBIOS_DEVICE_NOT_FOUND;

  *val=in_le32((volatile unsigned int *)
      (pci.pci_config_data + ((1<<slot)&~1)
       + (function<<8) + offset));
  return PCIBIOS_SUCCESSFUL;
}

static int
direct_pci_write_config_byte(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint8_t       val
) {
  if (bus != 0 || (1<<slot & 0xff8007fe))
     return PCIBIOS_DEVICE_NOT_FOUND;

  out_8(pci.pci_config_data + ((1<<slot)&~1)
     + (function<<8) + offset,
     val);
  return PCIBIOS_SUCCESSFUL;
}

static int
direct_pci_write_config_word(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint16_t      val
) {
  if (offset&1)
    return PCIBIOS_BAD_REGISTER_NUMBER;
  if (bus != 0 || (1<<slot & 0xff8007fe))
     return PCIBIOS_DEVICE_NOT_FOUND;

  out_le16((volatile unsigned short *)
     (pci.pci_config_data + ((1<<slot)&~1)
   + (function<<8) + offset),
     val);
  return PCIBIOS_SUCCESSFUL;
}

static int
direct_pci_write_config_dword(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint32_t      val
) {
  if (offset&3)
    return PCIBIOS_BAD_REGISTER_NUMBER;
  if (bus != 0 || (1<<slot & 0xff8007fe))
     return PCIBIOS_DEVICE_NOT_FOUND;

  out_le32((volatile unsigned int *)
     (pci.pci_config_data + ((1<<slot)&~1)
   + (function<<8) + offset),
     val);
  return PCIBIOS_SUCCESSFUL;
}

const pci_config_access_functions pci_direct_functions = {
  direct_pci_read_config_byte,
  direct_pci_read_config_word,
  direct_pci_read_config_dword,
  direct_pci_write_config_byte,
  direct_pci_write_config_word,
  direct_pci_write_config_dword
};

#define PRINT_MSG() \
  printk("pci : Device %d:0x%02x:%d routed to interrupt_line %d\n", \
    pbus, pslot, pfun, int_name )

/*
** Validate a test interrupt name and print a warning if its not one of
** the names defined in the routing record.
*/
static int test_intname(
  const struct _int_map *row,
  int pbus,
  int pslot,
  int pfun,
  int int_pin,
  int int_name
) {
  int j, k;
  int _nopin= -1, _noname= -1;

  for (j=0; row->pin_route[j].pin > -1; j++) {
    if ( row->pin_route[j].pin == int_pin ) {
   _nopin = 0;

   for (k=0; k<4 && row->pin_route[j].int_name[k] > -1; k++ ) {
     if ( row->pin_route[j].int_name[k] == int_name ) {
       _noname=0; break;
     }
   }
   break;
    }
  }

   if( _nopin  )
   {
      printk("pci : Device %d:0x%02x:%d supplied a bogus interrupt_pin %d\n", pbus, pslot, pfun, int_pin );
      return -1;
   }
   else
   {
      if( _noname ) {
		unsigned char v = row->pin_route[j].int_name[0];
		printk("pci : Device %d:0x%02x:%d supplied a suspicious interrupt_line %d, ", pbus, pslot, pfun, int_name );
		if ( (row->opts & PCI_FIXUP_OPT_OVERRIDE_NAME) && 255 != (v = row->pin_route[j].int_name[0]) ) {
			printk("OVERRIDING with %d from fixup table\n", v);
            pci_write_config_byte(pbus,pslot,pfun,PCI_INTERRUPT_LINE,v);
		} else {
         	printk("using it anyway\n");
		}
	  }
   }
   return 0;
}

struct pcibridge
{
  int bus;
  int slot;
};

static int FindPCIbridge( int mybus, struct pcibridge *pb )
{
  int          pbus, pslot;
  uint8_t      bussec, buspri;
  uint16_t     devid, vendorid, dclass;

  for(pbus=0; pbus< pci_bus_count(); pbus++) {
    for(pslot=0; pslot< PCI_MAX_DEVICES; pslot++) {
      pci_read_config_word(pbus, pslot, 0, PCI_DEVICE_ID, &devid);
      if ( devid == 0xffff ) continue;

      pci_read_config_word(pbus, pslot, 0, PCI_DEVICE_ID, &vendorid);
      if ( vendorid == 0xffff ) continue;

      pci_read_config_word(pbus, pslot, 0, PCI_CLASS_DEVICE, &dclass);

      if ( dclass == PCI_CLASS_BRIDGE_PCI ) {
        pci_read_config_byte(pbus, pslot, 0, PCI_PRIMARY_BUS,    &buspri);
        pci_read_config_byte(pbus, pslot, 0, PCI_SECONDARY_BUS,  &bussec);

#ifdef SHOW_PCI_SETTING
         printk("pci : Found bridge at %d:0x%02x, mybus %d, pribus %d, secbus %d ",
                 pbus, pslot, mybus, buspri, bussec );
#endif
         if ( bussec == mybus ) {
#ifdef SHOW_PCI_SETTING
           printk("match\n");
#endif
           /* found our nearest bridge going towards the root */
           pb->bus = pbus;
           pb->slot = pslot;

           return 0;
        }
#ifdef SHOW_PCI_SETTING
         printk("no match\n");
#endif
      }

     }
   }
   return -1;
}

void FixupPCI( const struct _int_map *bspmap, int (*swizzler)(int,int) )
{
  unsigned char        cvalue;
  uint16_t             devid;
  int                  ismatch, i, j, pbus, pslot, pfun, int_pin, int_name, nfuns;

  /*
   * If the device has a non-zero INTERRUPT_PIN, assign a bsp-specific
   * INTERRUPT_NAME if one isn't already in place.  Then, drivers can
   * trivially use INTERRUPT_NAME to hook up with devices.
   */

  for (pbus=0; pbus< pci_bus_count(); pbus++) {
	for (pslot=0; pslot< PCI_MAX_DEVICES; pslot++) {
	  pci_read_config_word(pbus, pslot, 0, PCI_DEVICE_ID, &devid);
	  if ( devid == 0xffff ) continue;

	  /* got a device */
	  pci_read_config_byte(pbus, pslot, 0, PCI_HEADER_TYPE, &cvalue);
	  nfuns = cvalue & PCI_HEADER_TYPE_MULTI_FUNCTION ? PCI_MAX_FUNCTIONS : 1;

	  for (pfun=0; pfun< nfuns; pfun++) {
		pci_read_config_word(pbus, pslot, pfun, PCI_DEVICE_ID, &devid);
		if( devid == 0xffff ) continue;

		pci_read_config_byte( pbus, pslot, pfun, PCI_INTERRUPT_PIN, &cvalue);
		int_pin = cvalue;

		pci_read_config_byte( pbus, pslot, pfun, PCI_INTERRUPT_LINE, &cvalue);
		int_name = cvalue;

		/* printk("pci : device %d:0x%02x:%i devid %04x, intpin %d, intline  %d\n",
		   pbus, pslot, pfun, devid, int_pin, int_name ); */

#ifdef SHOW_PCI_SETTING
		{
		  unsigned short cmd,stat;
		  unsigned char  lat, seclat, csize;

		  pci_read_config_word(pbus,pslot,pfun,PCI_COMMAND, &cmd );
		  pci_read_config_word(pbus,pslot,pfun,PCI_STATUS, &stat );
		  pci_read_config_byte(pbus,pslot,pfun,PCI_LATENCY_TIMER, &lat );
		  pci_read_config_byte(pbus,pslot,pfun,PCI_SEC_LATENCY_TIMER, &seclat );
		  pci_read_config_byte(pbus,pslot,pfun,PCI_CACHE_LINE_SIZE, &csize );


		  printk("pci : device %d:0x%02x:%d  cmd %04X, stat %04X, latency %d, "
			  " sec_latency %d, clsize %d\n", pbus, pslot, pfun, cmd, stat,
			  lat, seclat, csize);
		}
#endif

		if ( int_pin > 0 ) {
		  ismatch = 0;

		  /*
		   * first run thru the bspmap table and see if we have an
		   * explicit configuration
		   */
		  for (i=0; bspmap[i].bus > -1; i++) {
			if ( bspmap[i].bus == pbus && bspmap[i].slot == pslot ) {
			  ismatch = -1;
			  /* we have a record in the table that gives specific
			   * pins and interrupts for devices in this slot */
			  if ( int_name == 255 ) {
				/* find the vector associated with whatever pin the
				 * device gives us
				 */
				for ( int_name=-1, j=0; bspmap[i].pin_route[j].pin > -1; j++ ) {
				  if ( bspmap[i].pin_route[j].pin == int_pin ) {
					int_name = bspmap[i].pin_route[j].int_name[0];
					break;
				  }
				}
				if ( int_name == -1 ) {
				  printk("pci : Unable to resolve device %d:0x%02x:%d w/ swizzled int "
					  "pin %i to an interrupt_line.\n", pbus, pslot, pfun, int_pin );
				} else {
				  PRINT_MSG();
				  pci_write_config_byte( pbus,pslot,pfun,
					  PCI_INTERRUPT_LINE,(cvalue= int_name, cvalue));
				}
			  } else {
				test_intname( &bspmap[i],pbus,pslot,pfun,int_pin,int_name);
			  }
			  break;
			}
		  }

		  if ( !ismatch ) {
			/*
			 * no match, which means we're on a bus someplace.  Work
			 * backwards from it to one of our defined busses,
			 * swizzling thru each bridge on the way.
			 */

			/* keep pbus, pslot pointed to the device being
			 * configured while we track down the bridges using
			 * tbus,tslot.  We keep searching the routing table because
			 * we may end up finding our bridge in it
			 */

			int tbus= pbus, tslot= pslot;

			for (;;) {
			  for (i=0; bspmap[i].bus > -1; i++) {
				if ( bspmap[i].bus == tbus &&
					(bspmap[i].slot == tslot || bspmap[i].slot == -1) ) {
				  ismatch = -1;
				  /* found a record for this bus, so swizzle the
				   * int_pin which we then use to find the
				   * interrupt_name.
				   */

				  if ( int_name == 255 ) {
					/*
					 * FIXME.  I can't believe this little hack
					 * is right.  It does not yield an error in
					 * convienently simple situations.
					 */
					if ( tbus ) int_pin = (*swizzler)(tslot,int_pin);

					/*
					 * int_pin points to the interrupt channel
					 * this card ends up delivering interrupts
					 * on.  Find the int_name servicing it.
					 */
					for (int_name=-1, j=0; bspmap[i].pin_route[j].pin > -1; j++){
					  if ( bspmap[i].pin_route[j].pin == int_pin ) {
						int_name = bspmap[i].pin_route[j].int_name[0];
						break;
					  }
					}

					if ( int_name == -1 ) {
					  printk("pci : Unable to resolve device %d:0x%02x:%d w/ swizzled "
						  "int pin %i to an interrupt_line.\n",
						  pbus, pslot, pfun, int_pin );
					} else {
					  PRINT_MSG();
					  pci_write_config_byte(pbus,pslot,pfun,
						  PCI_INTERRUPT_LINE,(cvalue=int_name, cvalue));
					}
				  } else {
					test_intname(&bspmap[i],pbus,pslot,pfun,int_pin,int_name);
				  }
				  goto donesearch;
				}
			  }

			  if ( !ismatch ) {
				struct pcibridge   pb;

				/*
				 * Haven't found our bus in the int map, so work
				 * upwards thru the bridges till we find it.
				 */

				if ( FindPCIbridge( tbus, &pb )== 0 ) {
				  int_pin = (*swizzler)(tslot,int_pin);

				  /* our next bridge up is on pb.bus, pb.slot- now
				   * instead of pointing to the device we're
				   * trying to configure, we move from bridge to
				   * bridge.
				   */

				  tbus = pb.bus;
				  tslot = pb.slot;
				} else {
				  printk("pci : No bridge from bus %i towards root found\n",
					  tbus );
				  goto donesearch;
				}
			  }
			}
		  }
donesearch:

		  if ( !ismatch && int_pin != 0 && int_name == 255 ) {
			printk("pci : Unable to match device %d:0x%02x:%d with an int "
				"routing table entry\n", pbus, pslot, pfun  );
		  }
		}
	  }
	}
  }
}

/*
 * This routine determines the maximum bus number in the system
 */
int pci_initialize(void)
{
  unsigned char ucSlotNumber, ucFnNumber, ucNumFuncs;
  unsigned char ucHeader;
  unsigned char ucMaxSubordinate;
  uint32_t ulClass;
  uint32_t ulDeviceID;

  detect_host_bridge();

  /*
   * Scan PCI bus 0 looking for PCI-PCI bridges
   */
  for (ucSlotNumber=0;ucSlotNumber<PCI_MAX_DEVICES;ucSlotNumber++) {
    pci_read_config_dword(0, ucSlotNumber, 0, PCI_VENDOR_ID, &ulDeviceID);
    if (ulDeviceID==PCI_INVALID_VENDORDEVICEID) {
      /* This slot is empty */
      continue;
    }
    pci_read_config_byte(0, ucSlotNumber, 0, PCI_HEADER_TYPE, &ucHeader);
    if (ucHeader&PCI_HEADER_TYPE_MULTI_FUNCTION)  {
      ucNumFuncs=PCI_MAX_FUNCTIONS;
    } else {
      ucNumFuncs=1;
    }
    for (ucFnNumber=0;ucFnNumber<ucNumFuncs;ucFnNumber++) {
      pci_read_config_dword(0, ucSlotNumber, ucFnNumber,
                            PCI_VENDOR_ID, &ulDeviceID);
      if (ulDeviceID==PCI_INVALID_VENDORDEVICEID) {
        /* This slot/function is empty */
        continue;
      }

      /* This slot/function has a device fitted. */
      pci_read_config_dword(0, ucSlotNumber, ucFnNumber,
                            PCI_CLASS_REVISION, &ulClass);
      ulClass >>= 16;
      if (ulClass == PCI_CLASS_BRIDGE_PCI) {
        /* We have found a PCI-PCI bridge */
        pci_read_config_byte(0, ucSlotNumber, ucFnNumber,
                     PCI_SUBORDINATE_BUS, &ucMaxSubordinate);
       if (ucMaxSubordinate>ucMaxPCIBus) {
         ucMaxPCIBus=ucMaxSubordinate;
       }
     }
   }
 }
 return PCIB_ERR_SUCCESS;
}

/*
 * Return the number of PCI busses in the system
 */
unsigned char pci_bus_count(void)
{
  return (ucMaxPCIBus+1);
}
