#include <bsp/residual.h>
#include <stdint.h>

/* Magic knowledge - qemu loads image here. 
 * However, we use the value from NVRAM if possible...
 */
#define KERNELBASE 0x01000000

/* When starting qemu make sure to give the correct
 * amount of memory!
 *
 * NOTE: Code now supports reading the actual memory
 *       amount from NVRAM. The residual copy in RAM
 *       is fixed-up accordingly.
 */
#define MEM_MEGS 32

/* Mock up a minimal/fake residual; just enough to make the
 * bootloader happy.
 */
struct fake_data {
	unsigned long dat_len;
	unsigned long res_off;
	unsigned long cmd_off;
	unsigned long cmd_len;
	unsigned long img_adr;
	RESIDUAL      fake_residual;
	char          cmdline[1024];
} fake_data = {
dat_len: sizeof(fake_data),
res_off: (unsigned long) &fake_data.fake_residual
        -(unsigned long) &fake_data,
cmd_off: (unsigned long) &fake_data.cmdline
        -(unsigned long) &fake_data,
cmd_len: sizeof(fake_data.cmdline),
img_adr: KERNELBASE,
fake_residual: 
{
  ResidualLength: sizeof(RESIDUAL),
  Version:          0,
  Revision:         0,
  EC:               0,
  VitalProductData: {
	FirmwareSupplier:  QEMU,
	ProcessorHz:    300000000, /* fantasy */
	ProcessorBusHz: 100000000, /* qemu timebase clock */
	TimeBaseDivisor:1*1000,
  },
  MaxNumCpus:       1,
  ActualNumCpus:    1,
  Cpus: {
    {
	CpuType:        0x00040103, /* FIXME: fill from PVR */
	CpuNumber:      0,  
    CpuState:       0,
    },
  },
  /* Memory */
  TotalMemory:      1024*1024*MEM_MEGS,
  GoodMemory:       1024*1024*MEM_MEGS,
  ActualNumMemSegs: 13,
  Segs: {
    { 0x2000, 0xFFF00, 0x00100 },
    { 0x0020, MEM_MEGS*0x100, 0x80000 - MEM_MEGS*0x100 },
    { 0x0008, 0x00800, 0x00168 },
    { 0x0004, 0x00000, 0x00005 },
    { 0x0001, 0x006F1, 0x0010F },
    { 0x0002, 0x006AD, 0x00044 },
    { 0x0010, 0x00005, 0x006A8 },
    { 0x0010, 0x00968, MEM_MEGS*0x100 - 0x00968 },
    { 0x0800, 0xC0000, 0x3F000 },
    { 0x0600, 0xBF800, 0x00800 },
    { 0x0500, 0x81000, 0x3E800 },
    { 0x0480, 0x80800, 0x00800 },
    { 0x0440, 0x80000, 0x00800 }  
  },
  ActualNumMemories: 0,
  Memories: {
	{0},
  },
  /* Devices */
  ActualNumDevices:  1,
  Devices: {
	{
	DeviceId: {
		BusId:     PROCESSORDEVICE,
		BaseType:  BridgeController,
		SubType:   PCIBridge,
		Interface: PCIBridgeIndirect,
	},
	}
  },
  DevicePnPHeap: {0}
},
/* This is overwritten by command line passed by qemu. */
cmdline: {
	'-','-','n','e','2','k','-','i','r','q','=','9',
	0,
}
};

/* Read one byte from NVRAM */
static inline uint8_t
nvram_rd(void)
{
uint8_t rval = *(volatile uint8_t*)0x80000077;
	asm volatile("eieio");
	return rval;
}

/* Set NVRAM address pointer */
static inline void
nvram_addr(uint16_t addr)
{
	*(volatile uint8_t*)0x80000074 = (addr & 0xff);
	asm volatile("eieio");
	*(volatile uint8_t*)0x80000075 = ((addr>>8) & 0xff);
	asm volatile("eieio");
}

/* Read a 32-bit (big-endian) work from NVRAM */
uint32_t
nvram_rdl_be(uint16_t addr)
{
int i;
uint32_t rval = 0;
	for ( i=0; i<sizeof(rval); i++ ) { 
		nvram_addr( addr + i );
		rval = (rval<<8) | nvram_rd();
	}
	return rval;
}


/* !!! NOTE !!!
 *
 * We use a special hack to propagate command-line info to the bootloader.
 * This is NOT PreP compliant (but who cares).
 * We set R6 and R7 to point to the start/end of the command line string
 * and hacked the bootloader so it uses R6/R7 (provided that the firmware
 * is detected as 'QEMU').
 *
 * (see bootloader/mm.c, bootloader/misc.c,  bootloader/bootldr.h, -- boot_data.cmd_line[])
 */
uint32_t
res_copy(void)
{
struct   fake_data *p;
uint32_t addr, cmdl, l, imga;
uint32_t mem_sz, pgs;
int      i;
int      have_nvram;

	/* Make sure we have a valid NVRAM -- just check for 'QEMU' at the
	 * beginning 
	 */
	have_nvram = ( (('Q'<<24) | ('E'<<16) | ('M'<< 8) | ('U'<< 0)) == nvram_rdl_be( 0x0000 ) );

	if ( !have_nvram ) {
		/* reading NVRAM failed - fall back to using the static residual copy;
		 * this means no support for variable memory size or 'true' command line.
		 */
		return (uint32_t)&fake_data;
	}

	/* Dilemma - we don't really know where to put the residual copy
     * (original is in ROM and cannot be modified).
	 * We can't put it at the top of memory since the bootloader starts
	 * allocating memory from there, before saving the residual, that is.
	 * Too close to the final image might not work either because RTEMS
	 * zeroes its BSS *before* making its copies of the residual and commandline.
	 *
 	 * For now we hope that appending to the kernel image works (and that
	 * the bootloader puts it somewhere safe).
	 */
	imga  = nvram_rdl_be( 0x0038 );
	addr  = imga + nvram_rdl_be( 0x003c );
	addr += 0x1f;
	addr &= ~(0x1f);

	p     = (struct fake_data *)addr;

	/* commandline + length from NVRAM */
	cmdl  = nvram_rdl_be( 0x0040 );
	l     = nvram_rdl_be( 0x0044 );

	if ( l > 0 ) {
		/* have a command-line; copy it into our local buffer */
		if ( l > sizeof( p->cmdline ) - 1 ) {
			l = sizeof( p->cmdline ) - 1;
		}
		/* original may overlap our buffer; must safely move around */
		if ( p->cmdline < (char*)cmdl ) {
			for ( i=0; i<l; i++ ) {
				p->cmdline[i] = ((char*)cmdl)[i];
			}
		} else {
			for ( i=l-1; i>=0; i-- ) {
				p->cmdline[i] = ((char*)cmdl)[i];
			}
		}
	}
	p->cmdline[l]      = 0;
	/* Copy rest of residual */
	for ( i=0; i<sizeof(p->fake_residual); i++ )
		((char*)&p->fake_residual)[i] = ((char*)&fake_data.fake_residual)[i];
	p->dat_len         = fake_data.dat_len;
	p->res_off         = fake_data.res_off;
	p->cmd_off         = fake_data.cmd_off;
	p->cmd_len         = l+1;
	p->img_adr         = imga;

	/* Fix up memory in residual from NVRAM settings */

	mem_sz = nvram_rdl_be( 0x0030 );
	pgs    = mem_sz >> 12;

	p->fake_residual.TotalMemory = mem_sz;
	p->fake_residual.GoodMemory  = mem_sz;

	p->fake_residual.Segs[1].BasePage  = pgs;
	p->fake_residual.Segs[1].PageCount = 0x80000 - pgs;
	p->fake_residual.Segs[7].PageCount = pgs - 0x00968;

	return (uint32_t)p;
}
