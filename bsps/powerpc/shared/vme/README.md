vme
===

bsps/powerpc/include/bsp/VME.h: VME API; BSP and bridge-chip independent
bsps/powerpc/beatnik/include/VMEConfig.h:    defines BSP specific constants for VME configuration
bsps/powerpc/beatnik/vme/vmeconfig.c     configures the VME bridge using the VME.h API calls
                and BSP specific constants from VMEConfig.h.
                Independent of the bridge chip, however.
vme_universe.c: implements VME.h for the vmeUniverse driver.
vme_universe_dma.c: implements VMEDMA.h for the vmeUniverse driver.

o  other universe BSP     --> use its own VMEConfig.h; may reuse vmeconfig.c, vme_universe.c
o  other non-universe BSP --> use its own VMEConfig.h and vme_xxx.c; may reuse vmeconfig.c


Porting
=======
NOTE: (T.S, 2007/1) The information in this file is outdated
      (but some portions may still be useful). Some more information
	  about how to use the Universe and Tsi148 drivers in new BSPs
	  can be found in

	  	README.universe,
		bsps/powerpc/beatnik/include/bsp/VMEConfig.h,

		source files in this directory and bsps/powerpc/shared/vme

The vmeUniverse driver needs some support from the BSP for

a) PCI configuration space access
b) PCI interrupt acknowledgement
c) PCI interrupt handler installation

The driver was developed using the powerpc/shared/ BSP
(it also supports vxWorks) and by default uses that BSP's
a) PCI access API
b,c) irq handling API (AKA 'new' style BSP_install_rtems_irq_handler()
   API).

Some hooks exist in the driver to ease porting to other BSPs.
The following information has been assembled when answering a
question regarding a ppcn_60x BSP port:

I looked through the ppcn_60x BSP. Here's what I found:

 - this BSP does NOT adhere to neither the 'old' nor the 'new' API
   but provides its own (startup/setvec.c: set_vector()).
 - the BSP has a 'driver' for vmeUniverse although mine is far more
   complete (including support for VME interrupts, DMA etc.).
 - Porting my driver to your BSP should not be too hard:
  
   1) vmeUniverse needs PCI configuration space support from the
      BSP:
        a) a routine 'pciFindDevice' (need to be macro-aliased
           to the proper routine/wrapper of your BSP) who scans
           PCI config space for the universe bridge.
           You could add 'bsps/powerpc/shared/pci/pcifinddevice.c'
           to your BSP substituting the pci_read_config_xxx calls
           by the ones present on your BSP (see step 2))
        b) routines to read PCI config registers (byte and longword)
           [on your BSP these are PCIConfigRead32/PCIConfigRead8;
           hence you could replace the macros on top with
            #define pciConfigInLong PCIConfigRead32
   2) vmeUniverse needs to know how to acknowledge a PCI interrupt
      In your case, nothing needs to be done
            #define BSP_PIC_DO_EOI do {} while (0)
   3) Install the VME ISR dispatcher: replace the 'new' style
      interrupt installer (BSP_install_rtems_irq_handler()) by
      a proper call to 'set_vector()'
   4) I might have missed something... 

I attach the latest version of the vmeUniverse driver in case you want
to try to do the port (should be easy).

For the sake of ease of maintenance, I just added a few hooks making it
possible to override some things without having to modify the driver code.

 1,2) PCI config space access macros may be overriden via CFLAGS
      when compiling vmeUniverse.c, hence:
      CFLAGS += -DBSP_PIC_DO_EOI=do{}while(0)
      CFLAGS += -DBSP_PCI_CONFIG_IN_LONG=PCIConfigRead32
      CFLAGS += -DBSP_PCI_CONFIG_IN_BYTE=PCIConfigRead8
      (you still need to supply pci_find_device)
 3)   create your own version of vmeUniverseInstallIrqMgr():
      copy to a separate file and replace   
      BSP_rtems_install_irq_handler() by a proper call to set_vector.
 
 4)   Send me email :-)

USAGE NOTE: To fully initialize the driver, the following steps can/must
be performed:

 vmeUniverseInit();  /* MANDATORY: Driver Initialization */
 vmeUniverseReset(); /* OPTIONAL: Reset most registers to a known state; 
                      * if this step is omitted, firmware setup is
                      * preserved
                      */
 vmeUniverseMasterPortCfg(...); /* OPTIONAL: setup the master windows
                                 * (current setup preserved if omitted)
                                 */
 vmeUniverseSlavePortCfg(...);  /* OPTIONAL: setup the slave windows
                                 * (current setup preserved if omitted)
                                 */
 vmeUniverseInstallIrqMgr();    /* NEEDED FOR VME INTERRUPT SUPPRORT
                                 * initialize the interrupt manager.
                                 * NOTE: you need to call your own
                                 * version of this routine here
                                 */

For an example of init/setup, consult bsps/powerpc/beatnik/vme/vmeconfig.c


Universe
========
The tundra drivers are in a separate subdir
because they are maintained at SSRL outside of the
rtems CVS tree. The directory is called 'vmeUniverse'
for historic reasons. 'tundra' would be better
since we now support the tundra tsi148 as well...

Till Straumann <strauman@slac.stanford.edu> 1/2002, 2005, 2007

A BSP that wants to use these drivers
must implement the following headers / functionality:
 - <bsp/pci.h> offering an API like 'bsps/powerpc/shared/pci'
 - <bsp/irq.h> offering the 'new style' RTEMS irq API
   (like 'bsps/powerpc/shared/irq').
 - <libcpu/io.h>   for the I/O operations (out_le32,in_le32, ..., out_be32,...)
 - <libcpu/byteorder.h> for byte-swapping (st_le32, ld_le32, ..., st_be32,...)
 - glue code that implements the 'VME.h' and 'VMEDMA.h' APIs
   using the vmeUniverse and/or vmeTsi148 driver code.
   The 'glue' code initializes appropriate VME/PCI windows when booting
   and installs the VME interrupt manager.

   The 'glue' may also use the 'bspVmeDmaList' code to implement generic
   parts of linked-list DMA.

   Boards with a 'universe' chip may use a pretty generic version of
   the glue code that is defined in bsps/powerpc/shared/vme/vmeconfig.c,
   bsps/powerpc/shared/vme/vme_universe.c, and
   bsps/powerpc/shared/vme_universe_dma.c. The board-specific parameters
   are defined in a single BSP-specific file 'VMEConfig.h'. That's where
   the actual addresses of VME/PCI windows are configured and where
   interrupt wires can be assigned etc.

   Read bsp/powerpc/beatnik/include/bsp/VMEConfig.h for more information and use
   it as a template. Note that BSP implementors should try *not* to
   clone 'vmeconfig.c' but use the constants in VMEConfig.h

 - The BSP should export 'VME.h' and 'VMEDMA.h' to applications
   and encourage them to only use the API defined there in order
   to make application code driver-independent. This will ensure
   seamless portability of applications between the universe and Tsi148
   drivers.
   

TESTING: A valuable tool for testing are the (substitute XXX for
         'Universe' or 'Tsi148') routines:
			vmeXXXMapCRG() 
				maps the controller registers to VME space so you
				can test if you successfully can read/write from VME.
				You can read or DMA the PCI configuration registers
				and compare to what you expect (beware of endianness).

		 	vmeXXXIntLoopbackTest()
				this installs an ISR and then asserts an IRQ on the VME
				backplane so you can verify that your interrupt routing
				and handling really works.

NOTES: The universe may always issue MBLTs if a data width of 64-bit
       is enabled (default for non-BLT addressing modes -- the 
	   VME_AM_STD_xx_BLT / VME_AM_EXT_xx_BLT enforce 32-bit transfers).

	   Therefore, if you want to setup a outbound window that always
	   uses single cycles then you must explicitely request a data
	   width < 64, e.g.,
```c
       vmeUniverseMasterPortCfg(port, VME_AM_EXT_SUP_DATA | VME_MODE_DBW32, vme_addr, pci_addr, size);
```	   
