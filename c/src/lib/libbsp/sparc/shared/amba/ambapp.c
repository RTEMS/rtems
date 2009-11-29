/*
 *  AMBA Plag & Play Bus Driver
 *
 *  This driver hook performs bus scanning.
 *
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/bspIo.h>
#include <ambapp.h>

#define amba_insert_device(tab, address) \
{ \
  if (*(address)) \
  { \
    (tab)->addr[(tab)->devnr] = (address); \
    (tab)->devnr ++; \
  } \
} while(0)

#define amba_insert_apb_device(tab, address, apbmst) \
{ \
  if (*(address)) \
  { \
    (tab)->addr[(tab)->devnr] = (address); \
		(tab)->apbmst[(tab)->devnr] = (apbmst); \
    (tab)->devnr ++; \
  } \
} while(0)

static unsigned int
addr_from (struct amba_mmap *mmaps, unsigned int address)
{
  /* no translation? */
  if (!mmaps)
    return address;

  while (mmaps->size) {
    if ((address >= mmaps->remote_amba_adr)
        && (address <= (mmaps->remote_amba_adr + (mmaps->size - 1)))) {
      return (address - mmaps->remote_amba_adr) + mmaps->cpu_adr;
    }
    mmaps++;
  }
  return 1;
}


void
amba_scan (amba_confarea_type * amba_conf, unsigned int ioarea,
           struct amba_mmap *mmaps)
{
  unsigned int *cfg_area;       /* address to configuration area */
  unsigned int mbar, conf, custom;
  int i, j;
  unsigned int apbmst;
  int maxloops = amba_conf->notroot ? 16 : 64; /* scan first bus for 64 devices, rest for 16 devices */

  amba_conf->ahbmst.devnr = 0;
  amba_conf->ahbslv.devnr = 0;
  amba_conf->apbslv.devnr = 0;
  cfg_area = (unsigned int *) (ioarea | AMBA_CONF_AREA);
  amba_conf->ioarea = ioarea;
  amba_conf->mmaps = mmaps;

  for (i = 0; i < maxloops; i++) {
    amba_insert_device (&amba_conf->ahbmst, cfg_area);
    cfg_area += AMBA_AHB_CONF_WORDS;
  }

  cfg_area =
    (unsigned int *) (ioarea | AMBA_CONF_AREA | AMBA_AHB_SLAVE_CONF_AREA);
  for (i = 0; i < maxloops; i++) {
    amba_insert_device (&amba_conf->ahbslv, cfg_area);
    cfg_area += AMBA_AHB_CONF_WORDS;
  }

  for (i = 0; i < amba_conf->ahbslv.devnr; i++){
    conf = amba_get_confword(amba_conf->ahbslv, i, 0);
    mbar = amba_ahb_get_membar(amba_conf->ahbslv, i, 0);
    if ( (amba_vendor(conf) == VENDOR_GAISLER) && (amba_device(conf) == GAISLER_AHB2AHB) ){
      /* Found AHB->AHB bus bridge, scan it if more free amba_confarea_type:s available
       * Custom config 1 contain ioarea.
       */
      custom = amba_ahb_get_custom(amba_conf->ahbslv,i,1);

      if ( amba_ver(conf) && amba_conf->next ){
        amba_conf->next->notroot = 1;
        amba_scan(amba_conf->next,custom,mmaps);
      }
    }else if ((amba_vendor(conf) == VENDOR_GAISLER) && (amba_device(conf) == GAISLER_APBMST))
    {
      apbmst = amba_membar_start(mbar);
			if ( (apbmst=addr_from(mmaps,apbmst)) == 1 )
				continue; /* no available memory translation available, will not be able to access
				           * Plug&Play information for this AHB/APB bridge. Skip it.
				           */
			cfg_area = (unsigned int *)( apbmst | AMBA_CONF_AREA);
      for (j=0; (amba_conf->apbslv.devnr<AMBA_APB_SLAVES) && (j<AMBA_APB_SLAVES); j++){
        amba_insert_apb_device(&amba_conf->apbslv, cfg_area, apbmst);
        cfg_area += AMBA_APB_CONF_WORDS;
      }
    }
  }
}

void
amba_print_dev(int devno, unsigned int conf){
	int irq = amba_irq(conf);
	if ( irq > 0 ){
		printk("%x.%x.%x: irq %d\n",devno,amba_vendor(conf),amba_device(conf),irq);
	}else{
		printk("%x.%x.%x: no irq\n",devno,amba_vendor(conf),amba_device(conf));
	}
}

void
amba_apb_print_dev(int devno, unsigned int conf, unsigned int address){
	int irq = amba_irq(conf);
	if ( irq > 0 ){
		printk("%x.%x.%x: irq %d, apb: 0x%lx\n",devno,amba_vendor(conf),amba_device(conf),irq,address);
	}else{
		printk("%x.%x.%x: no irq, apb: 0x%lx\n",devno,amba_vendor(conf),amba_device(conf),address);
	}
}

/* Print AMBA Plug&Play info on terminal */
void
amba_print_conf (amba_confarea_type * amba_conf)
{
	int i,base=0;
	unsigned int conf, iobar, address;
	unsigned int apbmst;

	/* print all ahb masters */
	printk("--- AMBA AHB Masters ---\n");
	for(i=0; i<amba_conf->ahbmst.devnr; i++){
		conf = amba_get_confword(amba_conf->ahbmst, i, 0);
		amba_print_dev(i,conf);
	}

	/* print all ahb slaves */
	printk("--- AMBA AHB Slaves ---\n");
	for(i=0; i<amba_conf->ahbslv.devnr; i++){
		conf = amba_get_confword(amba_conf->ahbslv, i, 0);
		amba_print_dev(i,conf);
	}

	/* print all apb slaves */
	apbmst = 0;
	for(i=0; i<amba_conf->apbslv.devnr; i++){
		if ( apbmst != amba_conf->apbslv.apbmst[i] ){
			apbmst = amba_conf->apbslv.apbmst[i];
			printk("--- AMBA APB Slaves on 0x%x ---\n",apbmst);
			base=i;
		}
		conf = amba_get_confword(amba_conf->apbslv, i, 0);
    iobar = amba_apb_get_membar(amba_conf->apbslv, i);
    address = amba_iobar_start(amba_conf->apbslv.apbmst[i], iobar);
		amba_apb_print_dev(i-base,conf,address);
	}

}
/**** APB Slaves ****/

/* Return number of APB Slave devices which has given vendor and device */
int
amba_get_number_apbslv_devices (amba_confarea_type * amba_conf, int vendor,
                                int device)
{
  unsigned int conf;
  int cnt, i;

  for (cnt = i = 0; i < amba_conf->apbslv.devnr; i++) {
    conf = amba_get_confword (amba_conf->apbslv, i, 0);
    if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device))
      cnt++;
  }
  return cnt;
}

/* Get First APB Slave device of this vendor&device id */
int
amba_find_apbslv (amba_confarea_type * amba_conf, int vendor, int device,
                  amba_apb_device * dev)
{
  unsigned int conf, iobar;
  int i;

  for (i = 0; i < amba_conf->apbslv.devnr; i++) {
    conf = amba_get_confword (amba_conf->apbslv, i, 0);
    if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device)) {
      iobar = amba_apb_get_membar (amba_conf->apbslv, i);
      dev->start = amba_iobar_start (amba_conf->apbslv.apbmst[i], iobar);
      dev->irq = amba_irq (conf);
      return 1;
    }
  }
  return 0;
}

/* Get APB Slave device of this vendor&device id. (setting nr to 0 is eqivalent to calling amba_find_apbslv() ) */
int
amba_find_next_apbslv (amba_confarea_type * amba_conf, int vendor, int device,
                       amba_apb_device * dev, int index)
{
  unsigned int conf, iobar;
  int cnt, i;

  for (cnt = i = 0; i < amba_conf->apbslv.devnr; i++) {
    conf = amba_get_confword (amba_conf->apbslv, i, 0);
    if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device)) {
      if (cnt == index) {
        /* found device */
        iobar = amba_apb_get_membar (amba_conf->apbslv, i);
        dev->start = amba_iobar_start (amba_conf->apbslv.apbmst[i], iobar);
        dev->irq = amba_irq (conf);
        return 1;
      }
      cnt++;
    }
  }
  return 0;
}

/* Get first nr APB Slave devices, put them into dev (which is an array of nr length) */
int
amba_find_apbslvs (amba_confarea_type * amba_conf, int vendor, int device,
                   amba_apb_device * devs, int maxno)
{
  unsigned int conf, iobar;
  int cnt, i;

  for (cnt = i = 0; (i < amba_conf->apbslv.devnr) && (cnt < maxno); i++) {
    conf = amba_get_confword (amba_conf->apbslv, i, 0);
    if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device)) {
      /* found device */
      iobar = amba_apb_get_membar (amba_conf->apbslv, i);
      devs[cnt].start = amba_iobar_start (amba_conf->apbslv.apbmst[i], iobar);
      devs[cnt].irq = amba_irq (conf);
      cnt++;
    }
  }
  return cnt;
}

/***** AHB SLAVES *****/

/* Return number of AHB Slave devices which has given vendor and device */
int
amba_get_number_ahbslv_devices (amba_confarea_type * amba_conf, int vendor,
                                int device)
{
  unsigned int conf;
  int cnt, i;

  for (cnt = i = 0; i < amba_conf->ahbslv.devnr; i++) {
    conf = amba_get_confword (amba_conf->ahbslv, i, 0);
    if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device))
      cnt++;
  }
  return cnt;
}

/* Get First AHB Slave device of this vendor&device id */
int
amba_find_ahbslv (amba_confarea_type * amba_conf, int vendor, int device,
                  amba_ahb_device * dev)
{
  unsigned int conf, mbar, addr;
  int j, i;

  for (i = 0; i < amba_conf->ahbslv.devnr; i++) {
    conf = amba_get_confword (amba_conf->ahbslv, i, 0);
    if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device)) {
      for (j = 0; j < 4; j++) {
        mbar = amba_ahb_get_membar (amba_conf->ahbslv, i, j);
        addr = amba_membar_start (mbar);
        if (amba_membar_type (mbar) == AMBA_TYPE_AHBIO) {
          addr = AMBA_TYPE_AHBIO_ADDR (addr, amba_conf->ioarea);
        } else {                /* convert address if needed */
          if ((addr = addr_from (amba_conf->mmaps, addr)) == 1) {
            addr = 0;           /* no available memory translation available, will not be able to access
                                 * Plug&Play information for this AHB address. Skip it.
                                 */
          }
        }
        dev->start[j] = addr;
      }
      dev->irq = amba_irq (conf);
      dev->ver = amba_ver (conf);
      return 1;
    }
  }
  return 0;
}

/* Get AHB Slave device of this vendor&device id. (setting nr to 0 is eqivalent to calling amba_find_ahbslv() ) */
int
amba_find_next_ahbslv (amba_confarea_type * amba_conf, int vendor, int device,
                       amba_ahb_device * dev, int index)
{
  unsigned int conf, mbar, addr;
  int i, j, cnt;

  for (cnt = i = 0; i < amba_conf->ahbslv.devnr; i++) {
    conf = amba_get_confword (amba_conf->ahbslv, i, 0);
    if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device)) {
      if (cnt == index) {
        for (j = 0; j < 4; j++) {
          mbar = amba_ahb_get_membar (amba_conf->ahbslv, i, j);
          addr = amba_membar_start (mbar);
          if (amba_membar_type (mbar) == AMBA_TYPE_AHBIO) {
            addr = AMBA_TYPE_AHBIO_ADDR (addr, amba_conf->ioarea);
          } else {
            /* convert address if needed */
            if ((addr = addr_from (amba_conf->mmaps, addr)) == 1) {
              addr = 0;         /* no available memory translation available, will not be able to access
                                 * Plug&Play information for this AHB address. Skip it.
                                 */
            }
          }
          dev->start[j] = addr;
        }
        dev->irq = amba_irq (conf);
        dev->ver = amba_ver (conf);
        return 1;
      }
      cnt++;
    }
  }
  return 0;
}

/* Get first nr AHB Slave devices, put them into dev (which is an array of nr length) */
int
amba_find_ahbslvs (amba_confarea_type * amba_conf, int vendor, int device,
                   amba_ahb_device * devs, int maxno)
{
  unsigned int conf, mbar, addr;
  int i, j, cnt;

  for (cnt = i = 0; (i < amba_conf->ahbslv.devnr) && (maxno < cnt); i++) {
    conf = amba_get_confword (amba_conf->ahbslv, i, 0);
    if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device)) {
      for (j = 0; j < 4; j++) {
        mbar = amba_ahb_get_membar (amba_conf->ahbslv, i, j);
        addr = amba_membar_start (mbar);
        if (amba_membar_type (mbar) == AMBA_TYPE_AHBIO) {
          addr = AMBA_TYPE_AHBIO_ADDR (addr, amba_conf->ioarea);
        } else {
          /* convert address if needed */
          if ((addr = addr_from (amba_conf->mmaps, addr)) == 1) {
            addr = 0;           /* no available memory translation available, will not be able to access
                                 * Plug&Play information for this AHB address. Skip it.
                                 */
          }
        }
        devs[cnt].start[j] = addr;
      }
      devs[cnt].irq = amba_irq (conf);
      devs[cnt].ver = amba_ver (conf);
      cnt++;
    }
  }
  return cnt;
}


/***** AHB Masters *****/

/* Return number of AHB Slave devices which has given vendor and device */
int
amba_get_number_ahbmst_devices (amba_confarea_type * amba_conf, int vendor,
                                int device)
{
  unsigned int conf;
  int cnt, i;

  for (cnt = i = 0; i < amba_conf->ahbmst.devnr; i++) {
    conf = amba_get_confword (amba_conf->ahbmst, i, 0);
    if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device))
      cnt++;
  }
  return cnt;
}

/* Get First AHB Slave device of this vendor&device id */
int
amba_find_ahbmst (amba_confarea_type * amba_conf, int vendor, int device,
                  amba_ahb_device * dev)
{
  unsigned int conf, mbar, addr;
  int j, i;

  for (i = 0; i < amba_conf->ahbmst.devnr; i++) {
    conf = amba_get_confword (amba_conf->ahbslv, i, 0);
    if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device)) {
      for (j = 0; j < 4; j++) {
        mbar = amba_ahb_get_membar (amba_conf->ahbmst, i, j);
        addr = amba_membar_start (mbar);
        if (amba_membar_type (mbar) == AMBA_TYPE_AHBIO) {
          addr = AMBA_TYPE_AHBIO_ADDR (addr, amba_conf->ioarea);
        } else {
          /* convert address if needed */
          if ((addr = addr_from (amba_conf->mmaps, addr)) == 1) {
            addr = 0;           /* no available memory translation available, will not be able to access
                                 * Plug&Play information for this AHB address. Skip it.
                                 */
          }
        }
        dev->start[j] = addr;
      }
      dev->irq = amba_irq (conf);
      dev->ver = amba_ver (conf);
      return 1;
    }
  }
  return 0;
}

/* Get AHB Slave device of this vendor&device id. (setting nr to 0 is eqivalent to calling amba_find_ahbslv() ) */
int
amba_find_next_ahbmst (amba_confarea_type * amba_conf, int vendor, int device,
                       amba_ahb_device * dev, int index)
{
  unsigned int conf, mbar, addr;
  int i, j, cnt;

  for (cnt = i = 0; i < amba_conf->ahbmst.devnr; i++) {
    conf = amba_get_confword (amba_conf->ahbmst, i, 0);
    if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device)) {
      if (cnt == index) {
        for (j = 0; j < 4; j++) {
          mbar = amba_ahb_get_membar (amba_conf->ahbmst, i, j);
          addr = amba_membar_start (mbar);
          if (amba_membar_type (mbar) == AMBA_TYPE_AHBIO) {
            addr = AMBA_TYPE_AHBIO_ADDR (addr, amba_conf->ioarea);
          } else {
            /* convert address if needed */
            if ((addr = addr_from (amba_conf->mmaps, addr)) == 1) {
              addr = 0;         /* no available memory translation available, will not be able to access
                                 * Plug&Play information for this AHB address. Skip it.
                                 */
            }
          }
          dev->start[j] = addr;
        }
        dev->irq = amba_irq (conf);
        dev->ver = amba_ver (conf);
        return 1;
      }
      cnt++;
    }
  }
  return 0;
}

/* Get first nr AHB Slave devices, put them into dev (which is an array of nr length) */
int
amba_find_ahbmsts (amba_confarea_type * amba_conf, int vendor, int device,
                   amba_ahb_device * devs, int maxno)
{
  unsigned int conf, mbar, addr;
  int i, j, cnt;

  for (cnt = i = 0; (i < amba_conf->ahbmst.devnr) && (maxno < cnt); i++) {
    conf = amba_get_confword (amba_conf->ahbslv, i, 0);
    if ((amba_vendor (conf) == vendor) && (amba_device (conf) == device)) {
      for (j = 0; j < 4; j++) {
        mbar = amba_ahb_get_membar (amba_conf->ahbmst, i, j);
        addr = amba_membar_start (mbar);
        if (amba_membar_type (mbar) == AMBA_TYPE_AHBIO) {
          addr = AMBA_TYPE_AHBIO_ADDR (addr, amba_conf->ioarea);
        } else {
          /* convert address if needed */
          if ((addr = addr_from (amba_conf->mmaps, addr)) == 1) {
            addr = 0;           /* no available memory translation available, will not be able to access
                                 * Plug&Play information for this AHB address. Skip it.
                                 */
          }
        }
        devs[cnt].start[j] = addr;
      }
      devs[cnt].irq = amba_irq (conf);
      devs[cnt].ver = amba_ver (conf);
      cnt++;
    }
  }
  return cnt;
}
