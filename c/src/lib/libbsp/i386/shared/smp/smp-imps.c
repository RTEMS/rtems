/*
 * Author: Erich Boleyn  <erich@uruk.org>
 *         http://www.uruk.org/~erich/
 *
 * Copyright (c) 1997-2011 Erich Boleyn.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  Source file implementing Intel MultiProcessor Specification (MPS)
 *  version 1.1 and 1.4 SMP hardware control for Intel Architecture CPUs,
 *  with hooks for running correctly on a standard PC without the hardware.
 *
 *  This file was created from information in the Intel MPS version 1.4
 *  document, order number 242016-004, which can be ordered from the
 *  Intel literature center.
 *
 *  General limitations of this code:
 *
 *   (1) : This code has never been tested on an MPS-compatible system with
 *           486 CPUs, but is expected to work.
 *   (2) : Presumes "int", "long", and "unsigned" are 32 bits in size, and
 *       that 32-bit pointers and memory addressing is used uniformly.
 */

#define _SMP_IMPS_C

/*
 *  Includes here
 */
#if 0
#define IMPS_DEBUG
#endif

#include <bsp/apic.h>
#include <bsp/smp-imps.h>

/*
 *  XXXXX  The following absolutely must be defined!!!
 *
 *  The "KERNEL_PRINT" could be made a null macro with no danger, of
 *  course, but pretty much nothing would work without the other
 *  ones defined.
 */

#if 0
#define KERNEL_PRINT(x)       /* some kind of print function */
#define CMOS_WRITE_BYTE(x,y)  /* write unsigned char "y" at CMOS loc "x" */
#define CMOS_READ_BYTE(x)     /* read unsigned char at CMOS loc "x" */
#define PHYS_TO_VIRTUAL(x)    /* convert physical address "x" to virtual */
#define VIRTUAL_TO_PHYS(x)    /* convert virtual address "x" to physical */
#define UDELAY(x)             /* delay roughly at least "x" microsecs */
#define TEST_BOOTED(x)        /* test bootaddr x to see if CPU started */
#define READ_MSR_LO(x)        /* Read MSR low function */
#else
#include <string.h>
#include <unistd.h>
#include <rtems.h>
#include <rtems/bspsmp.h>
#include <rtems/bspIo.h>
#include <libcpu/cpu.h>

extern void _pc386_delay(void);

/* #define KERNEL_PRINT(_format)       printk(_format) */

static void CMOS_WRITE_BYTE(
  unsigned int  offset,
  unsigned char value
)
{
  if ( offset < 128 ) {
    outport_byte( 0x70, offset );
    outport_byte( 0x71, value );
  } else {
    outport_byte( 0x72, offset );
    outport_byte( 0x73, value );
  }
}

static unsigned char CMOS_READ_BYTE(
  unsigned int  offset
)
{
  unsigned char value;
  if ( offset < 128 ) {
    outport_byte( 0x70, offset );
    inport_byte( 0x71, value );
  } else {
    outport_byte( 0x72, offset );
    inport_byte( 0x73, value );
  }
  return value;
}

#define PHYS_TO_VIRTUAL(_x)    _x
#define VIRTUAL_TO_PHYS(_x)    _x
static void UDELAY(int x)
{ int _i = x;
  while ( _i-- )
    _pc386_delay();
}
 
#define READ_MSR_LO(_x) \
  (unsigned int)(read_msr(_x) & 0xffffffff)

#define TEST_BOOTED(_cpu) \
  (_Per_CPU_Information[_cpu].state == RTEMS_BSP_SMP_CPU_INITIALIZED)

static inline unsigned long long read_msr(unsigned int msr)
{
  unsigned long long value;
 
  asm volatile("rdmsr" : "=A" (value) : "c" (msr));
  return value;
}
#endif

/*
 *  Defines that are here so as not to be in the global header file.
 */
#define EBDA_SEG_ADDR       0x40E
#define BIOS_RESET_VECTOR   0x467
#define LAPIC_ADDR_DEFAULT  0xFEE00000uL
#define IOAPIC_ADDR_DEFAULT 0xFEC00000uL
#define CMOS_RESET_CODE     0xF
#define CMOS_RESET_JUMP     0xa
#define CMOS_BASE_MEMORY    0x15

/*
 *  Static defines here for SMP use.
 */

#define DEF_ENTRIES  23

static struct {
  imps_processor proc[2];
  imps_bus bus[2];
  imps_ioapic ioapic;
  imps_interrupt intin[16];
  imps_interrupt lintin[2];
} defconfig = {
  { { IMPS_BCT_PROCESSOR, 0, 0, 0, 0, 0},
    { IMPS_BCT_PROCESSOR, 1, 0, 0, 0, 0} },
  { { IMPS_BCT_BUS, 0, {'E', 'I', 'S', 'A', ' ', ' '}},
    { 255, 1, {'P', 'C', 'I', ' ', ' ', ' '}} },
  { IMPS_BCT_IOAPIC, 0, 0, IMPS_FLAG_ENABLED, IOAPIC_ADDR_DEFAULT },
  { { IMPS_BCT_IO_INTERRUPT, IMPS_INT_EXTINT, 0, 0, 0, 0xFF, 0},
    { IMPS_BCT_IO_INTERRUPT, IMPS_INT_INT, 0, 0, 1, 0xFF, 1},
    { IMPS_BCT_IO_INTERRUPT, IMPS_INT_INT, 0, 0, 0, 0xFF, 2},
    { IMPS_BCT_IO_INTERRUPT, IMPS_INT_INT, 0, 0, 3, 0xFF, 3},
    { IMPS_BCT_IO_INTERRUPT, IMPS_INT_INT, 0, 0, 4, 0xFF, 4},
    { IMPS_BCT_IO_INTERRUPT, IMPS_INT_INT, 0, 0, 5, 0xFF, 5},
    { IMPS_BCT_IO_INTERRUPT, IMPS_INT_INT, 0, 0, 6, 0xFF, 6},
    { IMPS_BCT_IO_INTERRUPT, IMPS_INT_INT, 0, 0, 7, 0xFF, 7},
    { IMPS_BCT_IO_INTERRUPT, IMPS_INT_INT, 0, 0, 8, 0xFF, 8},
    { IMPS_BCT_IO_INTERRUPT, IMPS_INT_INT, 0, 0, 9, 0xFF, 9},
    { IMPS_BCT_IO_INTERRUPT, IMPS_INT_INT, 0, 0, 10, 0xFF, 10},
    { IMPS_BCT_IO_INTERRUPT, IMPS_INT_INT, 0, 0, 11, 0xFF, 11},
    { IMPS_BCT_IO_INTERRUPT, IMPS_INT_INT, 0, 0, 12, 0xFF, 12},
    { IMPS_BCT_IO_INTERRUPT, IMPS_INT_INT, 0, 0, 13, 0xFF, 13},
    { IMPS_BCT_IO_INTERRUPT, IMPS_INT_INT, 0, 0, 14, 0xFF, 14},
    { IMPS_BCT_IO_INTERRUPT, IMPS_INT_INT, 0, 0, 15, 0xFF, 15} },
  { { IMPS_BCT_LOCAL_INTERRUPT, IMPS_INT_EXTINT, 0, 0, 15, 0xFF, 0},
    { IMPS_BCT_LOCAL_INTERRUPT, IMPS_INT_NMI, 0, 0, 15, 0xFF, 1} }
};

/*
 *  Exported globals here.
 */

volatile int imps_release_cpus = 0;
int imps_enabled = 0;
int imps_num_cpus = 1;
unsigned char imps_cpu_apic_map[IMPS_MAX_CPUS];
unsigned char imps_apic_cpu_map[IMPS_MAX_CPUS];

/* now defined in getcpuid.c */
extern unsigned imps_lapic_addr;

/*
 *  MPS checksum function
 *
 *  Function finished.
 */
static int
get_checksum(unsigned start, int length)
{
  unsigned sum = 0;

  while (length-- > 0) {
    sum += *((unsigned char *) (start++));
  }

  return (sum&0xFF);
}

/*
 *  APIC ICR write and status check function.
 */
static int
send_ipi(unsigned int dst, unsigned int v)
{
  int to, send_status;

  IMPS_LAPIC_WRITE(LAPIC_ICR+0x10, (dst << 24));
  IMPS_LAPIC_WRITE(LAPIC_ICR, v);

  /* Wait for send to finish */
  to = 0;
  do {
    UDELAY(100);
    send_status = IMPS_LAPIC_READ(LAPIC_ICR) & LAPIC_ICR_STATUS_PEND;
  } while (send_status && (to++ < 1000));

  return (to < 1000);
}

/*
 *  Primary function for booting individual CPUs.
 *
 *  This must be modified to perform whatever OS-specific initialization
 *  that is required.
 */
int
boot_cpu(imps_processor *proc)
{
  int apicid = proc->apic_id, success = 1;
  unsigned bootaddr, accept_status;
  unsigned bios_reset_vector = PHYS_TO_VIRTUAL(BIOS_RESET_VECTOR);

  /*
   * Copy boot code for secondary CPUs here.  Find it in between
   * "patch_code_start" and "patch_code_end" symbols.  The other CPUs
   * will start there in 16-bit real mode under the 1MB boundary.
   * "patch_code_start" should be placed at a 4K-aligned address
   * under the 1MB boundary.
   */

  uint32_t *reset;

  bootaddr = (512-64)*1024;
  reset= (uint32_t *)bootaddr;

  memcpy(
    (char *) bootaddr,
    _binary_appstart_bin_start,
    (size_t)_binary_appstart_bin_size
  );

  reset[1] = (uint32_t)rtems_smp_secondary_cpu_initialize;
  reset[2] = (uint32_t)_Per_CPU_Information[apicid].interrupt_stack_high;

  /*
   *  Generic CPU startup sequence starts here.
   */

  /* set BIOS reset vector */
  CMOS_WRITE_BYTE(CMOS_RESET_CODE, CMOS_RESET_JUMP);
  *((volatile unsigned *) bios_reset_vector) = ((bootaddr & 0xFF000) << 12);

  /* clear the APIC error register */
  IMPS_LAPIC_WRITE(LAPIC_ESR, 0);
  accept_status = IMPS_LAPIC_READ(LAPIC_ESR);

  /* assert INIT IPI */
  send_ipi(
    apicid,
    LAPIC_ICR_TM_LEVEL | LAPIC_ICR_LEVELASSERT | LAPIC_ICR_DM_INIT
  );
  UDELAY(10000);

  /* de-assert INIT IPI */
  send_ipi(apicid, LAPIC_ICR_TM_LEVEL | LAPIC_ICR_DM_INIT);

  UDELAY(10000);

  /*
   *  Send Startup IPIs if not an old pre-integrated APIC.
   */

  if (proc->apic_ver >= APIC_VER_NEW) {
    int i;
    for (i = 1; i <= 2; i++) {
      send_ipi(apicid, LAPIC_ICR_DM_SIPI | ((bootaddr >> 12) & 0xFF));
      UDELAY(1000);
    }
  }

  /*
   *  Check to see if other processor has started.
   */
  bsp_smp_wait_for(
    (volatile unsigned int *)&_Per_CPU_Information[imps_num_cpus].state,
    RTEMS_BSP_SMP_CPU_INITIALIZED,
    1600
  );
  if ( _Per_CPU_Information[imps_num_cpus].state ==
        RTEMS_BSP_SMP_CPU_INITIALIZED )
    printk("#%d  Application Processor (AP)", imps_num_cpus);
  else {
    printk("CPU Not Responding, DISABLED");
    success = 0;
  }

  /*
   *  Generic CPU startup sequence ends here, the rest is cleanup.
   */

  /* clear the APIC error register */
  IMPS_LAPIC_WRITE(LAPIC_ESR, 0);
  accept_status = IMPS_LAPIC_READ(LAPIC_ESR);

  /* clean up BIOS reset vector */
  CMOS_WRITE_BYTE(CMOS_RESET_CODE, 0);
  *((volatile unsigned *) bios_reset_vector) = 0;

  printk("\n");

  return success;
}

/*
 *  read bios stuff and fill tables
 */
static void
add_processor(imps_processor *proc)
{
  int apicid = proc->apic_id;

  printk("  Processor [APIC id %d ver %d]: ", apicid, proc->apic_ver);
  if (!(proc->flags & IMPS_FLAG_ENABLED)) {
    printk("DISABLED\n");
    return;
  }
  if (proc->flags & (IMPS_CPUFLAG_BOOT)) {
    printk("#0  BootStrap Processor (BSP)\n");
    return;
  }
  if (boot_cpu(proc)) {

    /*  XXXXX  add OS-specific setup for secondary CPUs here */

    imps_cpu_apic_map[imps_num_cpus] = apicid;
    imps_apic_cpu_map[apicid] = imps_num_cpus;
    imps_num_cpus++;
  }
}


static void
add_bus(imps_bus *bus)
{
  char str[8];

  memcpy(str, bus->bus_type, 6);
  str[6] = 0;
  printk("  Bus id %d is %s\n", bus->id, str);

  /*  XXXXX  add OS-specific code here */
}

static void
add_ioapic(imps_ioapic *ioapic)
{
  printk("  I/O APIC id %d ver %d, address: 0x%x  ",
          ioapic->id, ioapic->ver, ioapic->addr);
  if (!(ioapic->flags & IMPS_FLAG_ENABLED)) {
    printk("DISABLED\n");
    return;
  }
  printk("\n");

  /*  XXXXX  add OS-specific code here */
}

static void
imps_read_config_table(unsigned start, int count)
{
  while (count-- > 0) {
    switch (*((unsigned char *)start)) {
    case IMPS_BCT_PROCESSOR:
      if ( imps_num_cpus < rtems_configuration_smp_maximum_processors ) {
	add_processor((imps_processor *)start);
      } else 
        imps_num_cpus++;
      start += 12;  /* 20 total */
      break;
    case IMPS_BCT_BUS:
      add_bus((imps_bus *)start);
      break;
    case IMPS_BCT_IOAPIC:
      add_ioapic((imps_ioapic *)start);
      break;
#if 0  /*  XXXXX  uncomment this if "add_io_interrupt" is implemented */
    case IMPS_BCT_IO_INTERRUPT:
      add_io_interrupt((imps_interrupt *)start);
      break;
#endif
#if 0  /*  XXXXX  uncomment this if "add_local_interrupt" is implemented */
    case IMPS_BCT_LOCAL_INTERRUPT:
      add_local_interupt((imps_interrupt *)start);
      break;
#endif
    default:
      break;
    }
    start += 8;
  }
  if ( imps_num_cpus > rtems_configuration_smp_maximum_processors ) {
    printk(
      "WARNING!! Found more CPUs (%d) than configured for (%d)!!\n",
      imps_num_cpus - 1,
      rtems_configuration_smp_maximum_processors
    );
    imps_num_cpus = rtems_configuration_smp_maximum_processors;
    return;
  }
}

static int
imps_bad_bios(imps_fps *fps_ptr)
{
  int sum;
  imps_cth *local_cth_ptr
    = (imps_cth *) PHYS_TO_VIRTUAL(fps_ptr->cth_ptr);

  if (fps_ptr->feature_info[0] > IMPS_FPS_DEFAULT_MAX) {
    printk("    Invalid MP System Configuration type %d\n",
            fps_ptr->feature_info[0]);
    return 1;
  }

  if (fps_ptr->cth_ptr) {
    sum = get_checksum((unsigned)local_cth_ptr,
                                   local_cth_ptr->base_length);
    if (local_cth_ptr->sig != IMPS_CTH_SIGNATURE || sum) {
      printk(
        "    Bad MP Config Table sig 0x%x and/or checksum 0x%x\n",
        (unsigned)(fps_ptr->cth_ptr),
        sum
      );
      return 1;
    }
    if (local_cth_ptr->spec_rev != fps_ptr->spec_rev) {
      printk(
        "    Bad MP Config Table sub-revision # %d\n",
        local_cth_ptr->spec_rev
      );
      return 1;
    }
    if (local_cth_ptr->extended_length) {
      sum = (get_checksum(((unsigned)local_cth_ptr)
              + local_cth_ptr->base_length,
              local_cth_ptr->extended_length)
             + local_cth_ptr->extended_checksum) & 0xFF;
      if (sum) {
        printk("    Bad Extended MP Config Table checksum 0x%x\n", sum);
        return 1;
      }
    }
  } else if (!fps_ptr->feature_info[0]) {
    printk("    Missing configuration information\n");
    return 1;
  }

  return 0;
}

static void
imps_read_bios(imps_fps *fps_ptr)
{
  int apicid;
  unsigned cth_start, cth_count;
  imps_cth *local_cth_ptr
    = (imps_cth *)PHYS_TO_VIRTUAL(fps_ptr->cth_ptr);
  char *str_ptr;

  printk("Intel MultiProcessor Spec 1.%d BIOS support detected\n",
          fps_ptr->spec_rev);

  /*
   *  Do all checking of errors which would definitely
   *  lead to failure of the SMP boot here.
   */
  if (imps_bad_bios(fps_ptr)) {
    printk("    Disabling MPS support\n");
    return;
  }

  if (fps_ptr->feature_info[1] & IMPS_FPS_IMCRP_BIT) {
    str_ptr = "IMCR and PIC";
  } else {
    str_ptr = "Virtual Wire";
  }
  if (fps_ptr->cth_ptr) {
    imps_lapic_addr = local_cth_ptr->lapic_addr;
  } else {
    imps_lapic_addr = LAPIC_ADDR_DEFAULT;
  }
  printk("    APIC config: \"%s mode\"    Local APIC address: 0x%x\n",
          str_ptr, imps_lapic_addr);
  if (imps_lapic_addr != (READ_MSR_LO(0x1b) & 0xFFFFF000)) {
    printk("Inconsistent Local APIC address, Disabling SMP support\n");
    return;
  }
  imps_lapic_addr = PHYS_TO_VIRTUAL(imps_lapic_addr);

  /*
   *  Setup primary CPU.
   */
  apicid = IMPS_LAPIC_READ(LAPIC_SPIV);
  IMPS_LAPIC_WRITE(LAPIC_SPIV, apicid|LAPIC_SPIV_ENABLE_APIC);
  apicid = APIC_ID(IMPS_LAPIC_READ(LAPIC_ID));
  imps_cpu_apic_map[0] = apicid;
  imps_apic_cpu_map[apicid] = 0;

  if (fps_ptr->cth_ptr) {
    char str1[16], str2[16];
    memcpy(str1, local_cth_ptr->oem_id, 8);
    str1[8] = 0;
    memcpy(str2, local_cth_ptr->prod_id, 12);
    str2[12] = 0;
    printk("  OEM id: %s  Product id: %s\n", str1, str2);
    cth_start = ((unsigned) local_cth_ptr) + sizeof(imps_cth);
    cth_count = local_cth_ptr->entry_count;
  } else {
    *((volatile unsigned *) IOAPIC_ADDR_DEFAULT) =  IOAPIC_ID;
    defconfig.ioapic.id
      = APIC_ID(*((volatile unsigned *)
            (IOAPIC_ADDR_DEFAULT+IOAPIC_RW)));
    *((volatile unsigned *) IOAPIC_ADDR_DEFAULT) =  IOAPIC_VER;
    defconfig.ioapic.ver
      = APIC_VERSION(*((volatile unsigned *)
           (IOAPIC_ADDR_DEFAULT+IOAPIC_RW)));
    defconfig.proc[apicid].flags
      = IMPS_FLAG_ENABLED|IMPS_CPUFLAG_BOOT;
    defconfig.proc[!apicid].flags = IMPS_FLAG_ENABLED;
    imps_num_cpus = 2;
    if (fps_ptr->feature_info[0] == 1
     || fps_ptr->feature_info[0] == 5) {
      memcpy(defconfig.bus[0].bus_type, "ISA   ", 6);
    }
    if (fps_ptr->feature_info[0] == 4
     || fps_ptr->feature_info[0] == 7) {
      memcpy(defconfig.bus[0].bus_type, "MCA   ", 6);
    }
    if (fps_ptr->feature_info[0] > 4) {
      defconfig.proc[0].apic_ver = 0x10;
      defconfig.proc[1].apic_ver = 0x10;
      defconfig.bus[1].type = IMPS_BCT_BUS;
    }
    if (fps_ptr->feature_info[0] == 2) {
      defconfig.intin[2].type = 255;
      defconfig.intin[13].type = 255;
    }
    if (fps_ptr->feature_info[0] == 7) {
      defconfig.intin[0].type = 255;
    }
    cth_start = (unsigned) &defconfig;
    cth_count = DEF_ENTRIES;
  }
  imps_read_config_table(cth_start, cth_count);

  /* %%%%% ESB read extended entries here */

  imps_enabled = 1;
}

/*
 *  Given a region to check, this actually looks for the "MP Floating
 *  Pointer Structure".  The return value indicates if the correct
 *  signature and checksum for a floating pointer structure of the
 *  appropriate spec revision was found.  If so, then do not search
 *  further.
 *
 *  NOTE:  The memory scan will always be in the bottom 1 MB.
 *
 *  This function presumes that "start" will always be aligned to a 16-bit
 *  boundary.
 *
 *  Function finished.
 */
static int
imps_scan(unsigned start, unsigned length)
{
  printk("Scanning from 0x%x for %d bytes\n", start, length);

  while (length > 0) {
    imps_fps *fps_ptr = (imps_fps *) PHYS_TO_VIRTUAL(start);

    if (fps_ptr->sig == IMPS_FPS_SIGNATURE
     && fps_ptr->length == 1
     && (fps_ptr->spec_rev == 1 || fps_ptr->spec_rev == 4)
     && !get_checksum(start, 16)) {
      printk("Found MP Floating Structure Pointer at %x\n", start);
      imps_read_bios(fps_ptr);
      return 1;
    }

    length -= 16;
    start += 16;
  }

  return 0;
}

#if !defined(__rtems__)
/*
 *  This is the primary function to "force" SMP support, with
 *  the assumption that you have consecutively numbered APIC ids.
 */
int
imps_force(int ncpus)
{
  int apicid, i;
  imps_processor p;

  printk("Intel MultiProcessor \"Force\" Support\n");

  imps_lapic_addr = (READ_MSR_LO(0x1b) & 0xFFFFF000);
  imps_lapic_addr = PHYS_TO_VIRTUAL(imps_lapic_addr);

  /*
   *  Setup primary CPU.
   */
  apicid = IMPS_LAPIC_READ(LAPIC_SPIV);
  IMPS_LAPIC_WRITE(LAPIC_SPIV, apicid|LAPIC_SPIV_ENABLE_APIC);
  apicid = APIC_ID(IMPS_LAPIC_READ(LAPIC_ID));
  imps_cpu_apic_map[0] = apicid;
  imps_apic_cpu_map[apicid] = 0;

  p.type = 0;
  p.apic_ver = 0x10;
  p.signature = p.features = 0;

  for (i = 0; i < ncpus; i++) {
    if (apicid == i) {
      p.flags = IMPS_FLAG_ENABLED | IMPS_CPUFLAG_BOOT;
    } else {
      p.flags = IMPS_FLAG_ENABLED;
    }
    p.apic_id = i;
    add_processor(&p);
  }

  return imps_num_cpus;
}
#endif

/*
 *  This is the primary function for probing for MPS compatible hardware
 *  and BIOS information.  Call this during the early stages of OS startup,
 *  before memory can be messed up.
 *
 *  The probe looks for the "MP Floating Pointer Structure" at locations
 *  listed at the top of page 4-2 of the spec.
 *
 *  Environment requirements from the OS to run:
 *
 *   (1) : A non-linear virtual to physical memory mapping is probably OK,
 *       as (I think) the structures all fall within page boundaries,
 *       but a linear mapping is recommended.  Currently assumes that
 *       the mapping will remain identical over time (which should be
 *       OK since it only accesses memory which shouldn't be munged
 *       by the OS anyway).
 *   (2) : The OS only consumes memory which the BIOS says is OK to use,
 *       and not any of the BIOS standard areas (the areas 0x400 to
 *       0x600, the EBDA, 0xE0000 to 0xFFFFF, and unreported physical
 *       RAM).  Sometimes a small amount of physical RAM is not
 *       reported by the BIOS, to be used to store MPS and other
 *       information.
 *   (3) : It must be possible to read the CMOS.
 *   (4) : There must be between 512K and 640K of lower memory (this is a
 *       sanity check).
 *
 *  Function finished.
 */
int
imps_probe(void)
{
  /*
   *  Determine possible address of the EBDA
   */
  unsigned ebda_addr = *((unsigned short *)
             PHYS_TO_VIRTUAL(EBDA_SEG_ADDR)) << 4;

  /*
   *  Determine amount of installed lower memory (not *available*
   *  lower memory).
   *
   *  NOTE:  This should work reliably as long as we verify the
   *         machine is at least a system that could possibly have
   *         MPS compatibility to begin with.
   */
  unsigned mem_lower = ((CMOS_READ_BYTE(CMOS_BASE_MEMORY+1) << 8)
            | CMOS_READ_BYTE(CMOS_BASE_MEMORY))       << 10;

#ifdef IMPS_DEBUG
  imps_enabled = 0;
  imps_num_cpus = 1;
#endif

  /*
   *  Sanity check : if this isn't reasonable, it is almost impossibly
   *    unlikely to be an MPS compatible machine, so return failure.
   */
  if (mem_lower < 512*1024 || mem_lower > 640*1024) {
    return 0;
  }

  if (ebda_addr > mem_lower - 1024
   || ebda_addr + *((unsigned char *) PHYS_TO_VIRTUAL(ebda_addr))
         * 1024 > mem_lower) {
    ebda_addr = 0;
  }

  if (((ebda_addr && imps_scan(ebda_addr, 1024))
   || (!ebda_addr && imps_scan(mem_lower - 1024, 1024))
   || imps_scan(0xF0000, 0x10000)) && imps_enabled) {
    return imps_num_cpus;
  }

  /*
   *  If no BIOS info on MPS hardware is found, then return failure.
   */

  return 0;
}

/*
 *  RTEMS SMP BSP Support
 */
void smp_apic_ack(void)
{
  (void) IMPS_LAPIC_READ(LAPIC_SPIV);  /* dummy read */
  IMPS_LAPIC_WRITE(LAPIC_EOI, 0 );     /* ACK the interrupt */
}

rtems_isr ap_ipi_isr(
  rtems_vector_number vector
)
{
  smp_apic_ack();

  rtems_smp_process_interrupt();
}

#include <rtems/irq.h>

static rtems_irq_connect_data apIPIIrqData = {
  16,
  (void *)ap_ipi_isr,
  0,
  NULL,            /* On */
  NULL,            /* Off */
  NULL,            /* IsOn */
};

extern void bsp_reset(void);
void ipi_install_irq(void)
{
  if (!BSP_install_rtems_irq_handler (&apIPIIrqData)) {
    printk("Unable to initialize IPI\n");
    bsp_reset();
  }
}

#ifdef __SSE__
extern void enable_sse(void);
#endif

/* pc386 specific initialization */
void bsp_smp_secondary_cpu_initialize(int cpu)
{
  int apicid;

  asm volatile( "lidt IDT_Descriptor" );

  apicid = IMPS_LAPIC_READ(LAPIC_SPIV);
  IMPS_LAPIC_WRITE(LAPIC_SPIV, apicid|LAPIC_SPIV_ENABLE_APIC);

#ifdef __SSE__
  enable_sse();
#endif
}

#include <rtems/bspsmp.h>
int bsp_smp_initialize(
  int maximum
)
{
  int cores;
  /* XXX need to deal with finding too many cores */

  cores = imps_probe();

  if ( cores > 1 )
    ipi_install_irq();
  return cores;
}

void bsp_smp_interrupt_cpu(
  int cpu
)
{
  send_ipi( cpu, 0x30 );
}

void bsp_smp_broadcast_interrupt(void)
{
  /* Single broadcast interrupt */
  send_ipi( 0, LAPIC_ICR_DS_ALLEX | 0x30 );
}

void bsp_smp_wait_for(
  volatile unsigned int *address,
  unsigned int           desired,
  int                    maximum_usecs
)
{
  int iterations;
  volatile int i;
  volatile unsigned int *p = (volatile unsigned int *)address;

  for (iterations=0 ;  iterations < maximum_usecs ; iterations++ ) {
    if ( *p == desired )
      break;
    #ifdef __SSE3__
      __builtin_ia32_monitor( (const void *)address, 0, 0 );
      if ( *p == desired )
        break;
      __builtin_ia32_mwait( 0, 0 );
    #endif

    /*
     *  Until i386 ms delay does not depend upon the clock we
     *  will use this less sophisticated delay. 
     */
    for(i=5000; i>0; i--)
      ;
  }
}
