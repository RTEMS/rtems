/**
 *  @file
 *
 *  This file was based on the powerpc.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <bsp.h>

#include <bsp/pci.h>
#include <bsp/irq.h>
#include <rtems/bspIo.h>
#include <rtems/endian.h>

/*
 * DEFINES
 */

#undef SHOW_PCI_SETTING

// #define DEBUG_PCI 1

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
     (0x80000000|((bus)<<16)|(PCI_DEVFN((slot),(function))<<8)|(((offset)&~3)))
#endif

#ifdef DEBUG_PCI
  #define JPRINTK(fmt, ...) printk("%s: " fmt, __FUNCTION__, ##__VA_ARGS__)
#else
  #define JPRINTK(fmt, ...)
#endif

#ifndef  PCI_CONFIG_WR_ADDR
#define  PCI_CONFIG_WR_ADDR( addr, val ) out_le32((uint32_t)(addr), (val))
#endif

/* Bit encode for PCI_CONFIG_HEADER_TYPE register */
#define PCI_CONFIG_SET_ADDR(addr, bus, slot,function,offset) \
  PCI_CONFIG_WR_ADDR( \
    (addr), \
    PCI_CONFIG_ADDR_VAL((bus), (slot), (function), (offset))\
  )

#define PRINT_MSG() \
  printk("pci : Device %d:0x%02x:%d routed to interrupt_line %d\n", \
    pbus, pslot, pfun, int_name )

/*
 * STRUCTURES
 */

/*
 * PROTOTYPES
 */
void print_bars(
  unsigned char slot,
  unsigned char func
);
int direct_pci_read_config_byte(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint8_t      *val
);
int direct_pci_read_config_word(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint16_t     *val
);
int direct_pci_read_config_dword(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint32_t     *val
);
int direct_pci_write_config_byte(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint8_t       val
);
int direct_pci_write_config_word(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint16_t      val
);
int direct_pci_write_config_dword(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint32_t      val
);
int test_intname(
  const struct _int_map *row,
  int pbus,
  int pslot,
  int pfun,
  int int_pin,
  int int_name
);
void pci_memory_enable(
  unsigned char bus,
  unsigned char slot,
  unsigned char function
);
void pci_io_enable(
  unsigned char bus,
  unsigned char slot,
  unsigned char function
);
void pci_busmaster_enable(
  unsigned char bus,
  unsigned char slot,
  unsigned char function
);

/*
 * GLOBALS
 */
unsigned char ucMaxPCIBus;
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

const pci_config_access_functions pci_direct_functions = {
  direct_pci_read_config_byte,
  direct_pci_read_config_word,
  direct_pci_read_config_dword,
  direct_pci_write_config_byte,
  direct_pci_write_config_word,
  direct_pci_write_config_dword
};

/*
 * PCI specific accesses.  Note these are made on 32 bit
 * boundries.
 */
void pci_out_32(uint32_t base, uint32_t addr, uint32_t val)
{
  volatile uint32_t *ptr;

  ptr = (volatile uint32_t *) (base + addr);
  *ptr = val;

  JPRINTK( "%p data: 0x%x\n", ptr, val);
}

void pci_out_le32(uint32_t base, uint32_t addr, uint32_t val)
{
  volatile uint32_t *ptr;
  uint32_t           data = 0;

  ptr = (volatile uint32_t *) (base + addr);
  rtems_uint32_to_little_endian( val, (uint8_t *) &data);
  *ptr = data;

  JPRINTK( "%p data: 0x%x\n", ptr, data);
}

uint8_t pci_in_8( uint32_t base, uint32_t addr ) {
  volatile uint32_t *ptr;
  uint8_t            val;
  uint32_t           data;

  data = addr/4;
  ptr = (volatile uint32_t *) (base + (data*4));
  data = *ptr;

  switch ( addr%4 ) {
    case 0: val = (data & 0x000000ff) >>  0; break;
    case 1: val = (data & 0x0000ff00) >>  8; break;
    case 2: val = (data & 0x00ff0000) >> 16; break;
    case 3: val = (data & 0xff000000) >> 24; break;
   }

   JPRINTK( "0x%x data: 0x%x raw: 0x%x\n", ptr, val, data);

  return val;
}

int16_t pci_in_le16( uint32_t base, uint32_t addr ) {
  volatile uint32_t *ptr;
  uint16_t           val;
  uint16_t           rval;
  uint32_t           data;

  data = addr/4;
  ptr = (volatile uint32_t *) (base + (data*4));
  data = *ptr;
  if ( addr%4 == 0 )
    val = data & 0xffff;
  else
    val = (data>>16) & 0xffff;

  rval = rtems_uint16_from_little_endian( (uint8_t *) &val);
  JPRINTK( "0x%x data: 0x%x raw: 0x%x\n", ptr, rval, data);
  return rval;
}

int16_t pci_in_16( uint32_t base, uint32_t addr ) {
  volatile uint32_t *ptr;
  uint16_t           val;
  uint32_t           data;

  data = addr/4;
  ptr = (volatile uint32_t *) (base + (data*4));
  data = *ptr;
  if ( addr%4 == 0 )
    val = data & 0xffff;
  else
    val = (data>>16) & 0xffff;

  JPRINTK( "0x%x data: 0x%x raw: 0x%x\n", ptr, val, data);
  return val;
}

uint32_t pci_in_32( uint32_t base, uint32_t addr ) {
  volatile uint32_t *ptr;
  uint32_t           val;

  ptr = (volatile uint32_t *) (base + addr);
  val = *ptr;

  JPRINTK( "0x%x data: 0x%x raw: 0x%x\n", ptr, val, val);
  return val;
}
uint32_t pci_in_le32( uint32_t base, uint32_t addr ) {
  volatile uint32_t *ptr;
  uint32_t           val;
  uint32_t           rval;

  ptr = (volatile uint32_t *) (base + addr);
  val = *ptr;
  rval = rtems_uint32_from_little_endian( (uint8_t *) &val);

  JPRINTK( "0x%x data: 0x%x raw: 0x%x\n", ptr, rval, val);
  return rval;
}

void pci_out_8( uint32_t base, uint32_t addr, uint8_t val ) {
  volatile uint32_t *ptr;

  ptr = (volatile uint32_t *) (base + addr);
  JPRINTK("Address: %p\n", ptr);
  *ptr = val;
  JPRINTK( "%p data: 0x%x\n", ptr, val);
}

void pci_out_le16( uint32_t base, uint32_t addr, uint16_t val ) {
  volatile uint32_t *ptr;
  uint32_t           out_data;
  uint32_t           data;

  ptr = (volatile uint32_t *) (base + (addr & ~0x3));
  data = *ptr;
  if ( addr%4 == 0 )
    out_data = (data & 0xffff0000) | val;
  else
    out_data = ((val << 16)&0xffff0000) | (data & 0xffff);
  rtems_uint32_to_little_endian( out_data, (uint8_t *) &data);
  *ptr = data;

  JPRINTK( "0x%x data: 0x%x\n", ptr, data);
}

void pci_out_16( uint32_t base, uint32_t addr, uint16_t val ) {
  volatile uint32_t *ptr;
  uint32_t           out_data;
  uint32_t           data;

  ptr = (volatile uint32_t *) (base + (addr & ~0x3));
  data = *ptr;
  if ( addr%4 == 0 )
    out_data = (data & 0xffff0000) | val;
  else
    out_data = ((val << 16)&0xffff0000) | (data & 0xffff);
  *ptr = out_data;

  JPRINTK( "0x%x data: 0x%x\n", ptr, out_data);
}

/*
 * INDIRECT PCI CONFIGURATION ACCESSES
 */
int indirect_pci_read_config_byte(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint8_t       *val
) {

  JPRINTK("==>\n");
  PCI_CONFIG_SET_ADDR(pci.pci_config_addr, bus, slot, function, offset);
  *val = in_8((uint32_t) (pci.pci_config_data +  (offset&3)) );
  JPRINTK("\n\n");

  return PCIBIOS_SUCCESSFUL;
}

int indirect_pci_read_config_word(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint16_t      *val
) {

  JPRINTK("==>\n");

  *val = 0xffff;
  if (offset&1)
    return PCIBIOS_BAD_REGISTER_NUMBER;

  PCI_CONFIG_SET_ADDR(pci.pci_config_addr, bus, slot, function, offset);
  *val = in_16((uint32_t)(pci.pci_config_data + (offset&3)));

  JPRINTK("\n\n");

  return PCIBIOS_SUCCESSFUL;
}

int indirect_pci_read_config_dword(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint32_t *val
) {
  uint32_t v;
  JPRINTK("==>\n");

  *val = 0xffffffff;
  if (offset&3)
    return PCIBIOS_BAD_REGISTER_NUMBER;
  PCI_CONFIG_SET_ADDR(pci.pci_config_addr, bus, slot, function, offset);
  v = in_32( (uint32_t) pci.pci_config_data );
  *val = v;
   if ( offset == 0x0b )
     JPRINTK( "%x:%x %x ==> 0x%08x, 0x%08x\n", bus, slot, function, v, *val );

  JPRINTK("\n\n");

  return PCIBIOS_SUCCESSFUL;
}

int indirect_pci_write_config_byte(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint8_t       val
) {

  JPRINTK("==>\n");

  PCI_CONFIG_SET_ADDR(pci.pci_config_addr, bus, slot, function, offset);
  out_8( (uint32_t) (pci.pci_config_data + (offset&3)), val);

  JPRINTK("\n\n");

  return PCIBIOS_SUCCESSFUL;
}

int indirect_pci_write_config_word(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint16_t      val
) {

  JPRINTK("==>\n");

  if (offset&1)
    return PCIBIOS_BAD_REGISTER_NUMBER;

  PCI_CONFIG_SET_ADDR(pci.pci_config_addr, bus, slot, function, offset);
  out_16((uint32_t)(pci.pci_config_data + (offset&3)), val);

  JPRINTK("\n\n");

  return PCIBIOS_SUCCESSFUL;
}

int indirect_pci_write_config_dword(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint32_t      val
) {

  if (offset&3)
    return PCIBIOS_BAD_REGISTER_NUMBER;

  JPRINTK("==>\n");

  /*
   * The Base Address Registers get accessed big endian while the
   * other registers are little endian.
   */
  PCI_CONFIG_SET_ADDR(pci.pci_config_addr, bus, slot, function, offset);
  if ( bus == 0 && slot == 0x0b &&
       (offset >= PCI_BASE_ADDRESS_0 && offset <= PCI_BASE_ADDRESS_5) ) {
    out_32((uint32_t)pci.pci_config_data, val);
  } else {
    out_le32((uint32_t)pci.pci_config_data, val);
  }

  JPRINTK("\n\n");

  return PCIBIOS_SUCCESSFUL;
}

/*
 * DIRECT CONFIGUREATION ACCESSES.
 */
int direct_pci_read_config_byte(
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

  JPRINTK("==>\n");

  *val=in_8((uint32_t) (pci.pci_config_data + ((1<<slot)&~1)
   + (function<<8) + offset));

  JPRINTK("\n\n");

  return PCIBIOS_SUCCESSFUL;
}

int direct_pci_read_config_word(
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

  JPRINTK("==>\n");

  *val=in_le16((uint32_t)
      (pci.pci_config_data + ((1<<slot)&~1)
       + (function<<8) + offset));

  JPRINTK("\n\n");

  return PCIBIOS_SUCCESSFUL;
}

int direct_pci_read_config_dword(
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

  JPRINTK("==>\n");

  *val=in_le32((uint32_t)(pci.pci_config_data +
    ((1<<slot)&~1)+(function<<8) + offset));

  JPRINTK("\n\n");

  return PCIBIOS_SUCCESSFUL;
}

int direct_pci_write_config_byte(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint8_t       val
) {
  if (bus != 0 || (1<<slot & 0xff8007fe))
     return PCIBIOS_DEVICE_NOT_FOUND;

  JPRINTK("==>\n");

  out_8((uint32_t) (pci.pci_config_data + ((1<<slot)&~1) +
    (function<<8) + offset),
     val);

  JPRINTK("\n\n");

  return PCIBIOS_SUCCESSFUL;
}

int direct_pci_write_config_word(
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

  JPRINTK("==>\n");

  out_le16((uint32_t)(pci.pci_config_data + ((1<<slot)&~1) +
    (function<<8) + offset),
    val);

  JPRINTK("\n\n");

  return PCIBIOS_SUCCESSFUL;
}

int direct_pci_write_config_dword(
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

  JPRINTK("direct_pci_write_config_dword==>\n");

  out_le32((uint32_t)
     (pci.pci_config_data + ((1<<slot)&~1)
   + (function<<8) + offset),
     val);

  JPRINTK("\n\n");

  return PCIBIOS_SUCCESSFUL;
}

/*
 * Validate a test interrupt name and print a warning if its not one of
 * the names defined in the routing record.
 */
int test_intname(
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
      printk(
        "pci : Device %d:0x%02x:%d supplied a bogus interrupt_pin %d\n",
        pbus,
        pslot,
        pfun,
        int_pin
      );
      return -1;
   }
   else
   {
     if( _noname ) {
       unsigned char v = row->pin_route[j].int_name[0];
       printk(
         "pci : Device %d:0x%02x:%d supplied a suspicious interrupt_line %d, ",
         pbus,
         pslot,
         pfun,
         int_name
       );
       if ((row->opts & PCI_FIXUP_OPT_OVERRIDE_NAME) && 255 !=
           (v = row->pin_route[j].int_name[0])
          ) {
         printk("OVERRIDING with %d from fixup table\n", v);
         pci_write_config_byte(pbus,pslot,pfun,PCI_INTERRUPT_LINE,v);
       } else {
        printk("using it anyway\n");
       }
     }
   }
   return 0;
}

int FindPCIbridge( int mybus, struct pcibridge *pb )
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
          JPRINTK(
            "pci : Found bridge at %d:0x%02x, mybus %d, pribus %d, secbus %d ",
            pbus,
            pslot,
            mybus,
            buspri,
            bussec
          );
        #endif
        if ( bussec == mybus ) {
          #ifdef SHOW_PCI_SETTING
            JPRINTK("match\n");
          #endif
          /* found our nearest bridge going towards the root */
          pb->bus = pbus;
          pb->slot = pslot;
          return 0;
        }
        #ifdef SHOW_PCI_SETTING
          JPRINTK("no match\n");
        #endif
      }

     }
   }
   return -1;
}

void FixupPCI( const struct _int_map *bspmap, int (*swizzler)(int,int) )
{
  unsigned char cvalue;
  uint16_t      devid;
  int           ismatch, i, j, pbus, pslot, pfun, int_pin, int_name, nfuns;

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

        #ifdef SHOW_PCI_SETTING
        {
          unsigned short cmd,stat;
          unsigned char  lat, seclat, csize;

          pci_read_config_word(pbus,pslot,pfun,PCI_COMMAND, &cmd );
          pci_read_config_word(pbus,pslot,pfun,PCI_STATUS, &stat );
          pci_read_config_byte(pbus,pslot,pfun,PCI_LATENCY_TIMER, &lat );
          pci_read_config_byte(pbus,pslot,pfun,PCI_SEC_LATENCY_TIMER, &seclat);
          pci_read_config_byte(pbus,pslot,pfun,PCI_CACHE_LINE_SIZE, &csize );

          JPRINTK(
            "pci : device %d:0x%02x:%d  cmd %04X, stat %04X, latency %d, "
            " sec_latency %d, clsize %d\n",
            pbus,
            pslot,
            pfun,
            cmd,
            stat,
            lat,
            seclat,
            csize
          );
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
               * pins and interrupts for devices in this slot
               */
              if ( int_name == 255 ) {

                /* find the vector associated with whatever pin the
                 * device gives us
                 */
                for ( int_name=-1, j=0; bspmap[i].pin_route[j].pin > -1; j++ ){
                  if ( bspmap[i].pin_route[j].pin == int_pin ) {
                    int_name = bspmap[i].pin_route[j].int_name[0];
                    break;
                  }
                }

                if ( int_name == -1 ) {
                  printk(
                    "pci : Unable to resolve device %d:0x%02x:%d w/ "
                    "swizzled int pin %i to an interrupt_line.\n",
                    pbus,
                    pslot,
                    pfun,
                    int_pin
                  );
                } else {
                  PRINT_MSG();
                  pci_write_config_byte(
                    pbus,
                    pslot,
                    pfun,
                    PCI_INTERRUPT_LINE,(cvalue= int_name, cvalue)
                  );
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
                    (bspmap[i].slot == tslot || bspmap[i].slot == -1))
                {
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
                    for (int_name=-1, j=0;
                         bspmap[i].pin_route[j].pin > -1;
                         j++)
                    {
                      if ( bspmap[i].pin_route[j].pin == int_pin ) {
                        int_name = bspmap[i].pin_route[j].int_name[0];
                        break;
                      }
                    }

                    if ( int_name == -1 ) {
                      printk(
                        "pci : Unable to resolve device %d:0x%02x:%d w/ "
                        "swizzled int pin %i to an interrupt_line.\n",
                        pbus,
                        pslot,
                        pfun,
                        int_pin
                      );
                    } else {
                      PRINT_MSG();
                      pci_write_config_byte(pbus,pslot,pfun,
                      PCI_INTERRUPT_LINE,(cvalue=int_name, cvalue));
                    }
                  } else {
                    test_intname(
                      &bspmap[i],
                      pbus,
                      pslot,
                      pfun,
                      int_pin,
                      int_name
                    );
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
                  printk(
                    "pci : No bridge from bus %i towards root found\n",
                    tbus
                  );
                  goto donesearch;
                }
              }
            }
          }

          donesearch:
          if ( !ismatch && int_pin != 0 && int_name == 255 ) {
            printk(
              "pci : Unable to match device %d:0x%02x:%d with an int "
              "routing table entry\n",
              pbus,
              pslot,
              pfun
            );
          }
        }
      }
    }
  }
}

void print_bars(
  unsigned char slot,
  unsigned char func
)
{
  uint32_t addr;

  printk( "*** BARs for slot=%d func=%d\n", slot, func );
  pci_read_config_dword (0, slot, func, PCI_BASE_ADDRESS_0, &addr);
  printk("***    PCI DEVICE BAR0: 0x%x\n", addr);
  pci_read_config_dword (0, slot, func, PCI_BASE_ADDRESS_1, &addr);
  printk("***    PCI DEVICE BAR1: 0x%x\n", addr);
  pci_read_config_dword (0, slot, func, PCI_BASE_ADDRESS_2, &addr);
  printk("***    PCI DEVICE BAR2: 0x%x\n", addr);
  pci_read_config_dword (0, slot, func, PCI_BASE_ADDRESS_3, &addr);
  printk("***    PCI DEVICE BAR3: 0x%x\n", addr);
  pci_read_config_dword (0, slot, func, PCI_BASE_ADDRESS_4, &addr);
  printk("***    PCI DEVICE BAR4: 0x%x\n", addr);
  pci_read_config_dword (0, slot, func, PCI_BASE_ADDRESS_5, &addr);
  printk("***    PCI DEVICE BAR5: 0x%x\n", addr);
}

void pci_memory_enable(
  unsigned char bus,
  unsigned char slot,
  unsigned char function
)
{
  uint16_t data;

  pci_read_config_word(0, slot, function, PCI_COMMAND, &data);
  data |= PCI_COMMAND_MEMORY;
  pci_write_config_word(0, slot, function, PCI_COMMAND, data );
  pci_read_config_word(0, slot, function, PCI_COMMAND, &data);
}

void pci_io_enable(
  unsigned char bus,
  unsigned char slot,
  unsigned char function
)
{
  uint16_t data;

  pci_read_config_word(0, slot, function, PCI_COMMAND, &data);
  data |= PCI_COMMAND_IO;
  pci_write_config_word(0, slot, function, PCI_COMMAND, data );
}

void pci_busmaster_enable(
  unsigned char bus,
  unsigned char slot,
  unsigned char function
)
{
  uint16_t data;

  pci_read_config_word(0, slot, function, PCI_COMMAND, &data);
  data |= PCI_COMMAND_MASTER;
  pci_write_config_word(0, slot, function, PCI_COMMAND, data );
}

/*
 * This routine determines the maximum bus number in the system
 */
int pci_initialize(void)
{
  unsigned char slot, func, ucNumFuncs;
  unsigned char ucHeader;
  uint32_t class;
  uint32_t device;
  uint32_t vendor;

  /*
   * Initialize GT_PCI0IOREMAP
   */
  pci_out_32( BSP_PCI_BASE_ADDRESS, 0xf0, 0 );

  /*
   *  According to Linux and BSD sources, this is needed to cover up a bug
   *  in some versions of the hardware.
   */
  out_le32( PCI_CONFIG_ADDR, 0x80000020 );
  out_le32( PCI_CONFIG_DATA, 0x1be00000 );

  /*
   * Scan PCI bus 0 looking for the known Network devices and
   * initializing the PCI for them.
   */
  for (slot=0;slot<PCI_MAX_DEVICES;slot++) {
    pci_read_config_dword(0, slot, 0, PCI_VENDOR_ID, &device);
    if (device == PCI_INVALID_VENDORDEVICEID) {
      /* This slot is empty */
      continue;
    }

    pci_read_config_byte(0, slot, 0, PCI_HEADER_TYPE, &ucHeader);
    if (ucHeader & PCI_HEADER_TYPE_MULTI_FUNCTION)  {
      ucNumFuncs = PCI_MAX_FUNCTIONS;
    } else {
      ucNumFuncs=1;
    }
    for (func=0;func<ucNumFuncs;func++) {
      pci_read_config_dword(0, slot, func, PCI_VENDOR_ID, &device);
       if (device==PCI_INVALID_VENDORDEVICEID) {
        /* This slot/function is empty */
        continue;
      }
      vendor = device & 0xffff;
      device = device >> 16;

      /* This slot/function has a device fitted. */
      pci_read_config_dword(0, slot, func, PCI_CLASS_REVISION, &class);
      class >>= 16;

      // printk( "FOUND DEVICE 0x%04x/0x%04x class 0x%x\n",
      //          vendor, device, class );
      if (class == PCI_CLASS_NETWORK_ETHERNET) {
        JPRINTK("FOUND ETHERNET\n");

        pci_write_config_byte(
            0, slot, func, PCI_INTERRUPT_LINE, MALTA_IRQ_ETHERNET );

        /*
         * Rewrite BAR1 for RTL8139
         */
        if ( vendor == PCI_VENDOR_ID_REALTEK &&
             device == PCI_DEVICE_ID_REALTEK_8139 ) {

          pci_memory_enable(0, slot, func);
          pci_io_enable(0, slot, func);
          pci_busmaster_enable(0, slot, func);

          // BAR0: IO at 0x0000_1001
          pci_write_config_dword(0, slot, func, PCI_BASE_ADDRESS_0, 0x00001001);

          // BAR1: Memory at 0x1203_1000
          pci_write_config_dword(0, slot, func, PCI_BASE_ADDRESS_1, 0x12031000);

          // print_bars( slot, func );
       } else if ( vendor == PCI_VENDOR_ID_AMD &&
                   device == PCI_DEVICE_ID_AMD_LANCE ) {
         print_bars( slot, func );
         pci_memory_enable(0, slot, func);
         pci_io_enable(0, slot, func);
         pci_busmaster_enable(0, slot, func);

         // BAR0: IO at 0x0000_1041
         pci_write_config_dword(0, slot, func, PCI_BASE_ADDRESS_0, 0x00001041);

         // BAR1: Memory at 0x1201_1020
         pci_write_config_dword(0, slot, func, PCI_BASE_ADDRESS_1, 0x12011020);
         print_bars( slot, func );
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
