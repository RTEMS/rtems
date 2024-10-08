MVME147S
========

MVME147 port for TNI - Telecom Bretagne
by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
May 1996


This bsp is essentially based on the mvme136 bsp,
and is only an extention of the MVME147 bsp. There
are slight differences in the VMEchips used in the MVME147S and
the MVME147 that should prevent the shmsupport and the startup
code from running on a MVME147.

Summary
-------
 * include
 
   - bsp.h 
      Peripheral Channel Controller memory mapping
      Z8530 memory mapping
      VMEchip memory mapping
 
 * startup
 
   - bspstart.c
      main () setup for VME roundrobin mode
	      setup for the PCC interrupt vector base 
              setup of the VME shared memory
   - bspclean.c
      bsp_cleanup () disable timer 1 & 2 interruptions
   - linkcmds set the RAM start (0x7000) and size (4Meg - 0x7000)
   - setvec.c unchanged
   - sbrk.c unchanged
 
 * console
 
   - console.c taken from the dmv152 bsp (Zilog Z8530)
               with no modification
 
 * clock
 
   - ckinit.c entirely rewritten for the PCC tick timer 2
 
 * timer
 
   - timerisr.s and timer.c 
     entirely rewritten for the PCC tick timer 1
     now gives results un 6.25 us units (mininum timer delay,
     suprising big grain)
 
 * times
 
   - updated results for the mvme147 (beware of the 6.25 us grain)

 * shmsupp :
   Specific to the S version of the MVME147
   Only tested with 2 boards, in interrupt mode.
   Uses the top 128k of the VME system controller board RAM
   as the shared space.

   - mpisr.c : uses the SIGLP interruption

   - Makefile : unchanged

   - getcfg.c : rewritten 

   - lock.c unchanged

   - addrconv.c unchanged

 * Makefiles
   
   - unchanged


TODO
----
 * update the overheads in coverhead.h 

 * add support for serial ports 2,3 and 4.


Other notes
-----------
 * All the timing tests and sp tests have been run
