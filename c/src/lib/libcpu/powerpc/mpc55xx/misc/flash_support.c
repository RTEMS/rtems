/**
 * @file
 *
 * @ingroup mpc55xx
 *
 *  @brief MPC55XX flash memory support.
 *
 *  I set my MMU up to map what will finally be in flash into RAM and at the
 *  same time I map the flash to a different location.  When the software
 *  is tested I can use this to copy the RAM version of the program into
 *  the flash and when I reboot I'm running out of flash.
 *
 *  I use a flag word located after the boot configuration half-word to
 *  indicate that the MMU should be left alone, and I don't include the RCHW
 *  or that flag in my call to this routine.
 *
 *  There are obviously other uses for this.
 **/

/*
 * Copyright (c) 2009-2011
 * HD Associates, Inc.
 * 18 Main Street
 * Pepperell, MA 01463
 * USA
 * dufault@hda.com
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#include <errno.h>
#include <sys/types.h>
#include <mpc55xx/regs.h>
#include <mpc55xx/mpc55xx.h>

#include <libcpu/powerpc-utility.h>
#include <rtems/powerpc/registers.h>

#if MPC55XX_CHIP_TYPE / 100 == 55

/* Set up the memory ranges for the flash on
 * the MPC5553, MPC5554, MPC5566 and MPC5567.
 * I check if it is an unknown CPU and return an error.
 *
 * These CPUS have a low, mid, and high space of memory.
 *
 * Only the low space really needs a table like this, but for simplicity
 * I do low, mid, and high the same way.
 */
struct range {          /* A memory range. */
    uint32_t lower;
    uint32_t upper;
};

/* The ranges of the memory banks for the low space.  All the
 * chips I'm looking at share this low format, identified by LAS=6:
 * 2 16K banks,
 * 2 48K banks,
 * 2 64K banks.
 */
static const struct range lsel_ranges[] = {
    {                        0, (1*16              )*1024 - 1},
    {(1*16              )*1024, (2*16              )*1024 - 1},
    {(2*16              )*1024, (2*16 + 1*48       )*1024 - 1},
    {(2*16 + 1*48       )*1024, (2*16 + 2*48       )*1024 - 1},
    {(2*16 + 2*48       )*1024, (2*16 + 2*48 + 1*64)*1024 - 1},
    {(2*16 + 2*48 + 1*64)*1024, (2*16 + 2*48 + 2*64)*1024 - 1},
};

/* The ranges of the memory blocks for the mid banks, 2 128K banks.
 * Again, all the chips share this, identified by MAS=0.
 */
#define MBSTART ((2*16+2*48+2*64)*1024)
static const struct range msel_ranges[] = {
    {MBSTART             , MBSTART + 1*128*1024 - 1},
    {MBSTART + 1*128*1024, MBSTART + 2*128*1024 - 1},
};

/* The ranges of the memory blocks for the high banks.
 * There are N 128K banks, where N <= 20,
 * and is identified by looking at the SIZE field.
 *
 * This could benefit from being redone to save a few bytes
 * and provide for bigger flash spaces.
 */
#define HBSTART (MBSTART+2*128*1024)
static const struct range hbsel_ranges[] = {
    {HBSTART              , HBSTART +  1*128*1024 - 1},
    {HBSTART +  1*128*1024, HBSTART +  2*128*1024 - 1},
    {HBSTART +  2*128*1024, HBSTART +  3*128*1024 - 1},
    {HBSTART +  3*128*1024, HBSTART +  4*128*1024 - 1},
    {HBSTART +  4*128*1024, HBSTART +  5*128*1024 - 1},
    {HBSTART +  5*128*1024, HBSTART +  6*128*1024 - 1},
    {HBSTART +  6*128*1024, HBSTART +  7*128*1024 - 1},
    {HBSTART +  7*128*1024, HBSTART +  8*128*1024 - 1},
    {HBSTART +  8*128*1024, HBSTART +  9*128*1024 - 1},
    {HBSTART +  9*128*1024, HBSTART + 10*128*1024 - 1},
    {HBSTART + 10*128*1024, HBSTART + 11*128*1024 - 1},
    {HBSTART + 11*128*1024, HBSTART + 12*128*1024 - 1},
    {HBSTART + 12*128*1024, HBSTART + 13*128*1024 - 1},
    {HBSTART + 13*128*1024, HBSTART + 14*128*1024 - 1},
    {HBSTART + 14*128*1024, HBSTART + 15*128*1024 - 1},
    {HBSTART + 15*128*1024, HBSTART + 16*128*1024 - 1},
    {HBSTART + 16*128*1024, HBSTART + 17*128*1024 - 1},
    {HBSTART + 17*128*1024, HBSTART + 18*128*1024 - 1},
    {HBSTART + 18*128*1024, HBSTART + 19*128*1024 - 1},
    {HBSTART + 19*128*1024, HBSTART + 20*128*1024 - 1},
};

/* Set bits in a bitmask to indicate which banks are
 * within the range "first" and "last".
 */
static void
range_set(
  uint32_t first,
  uint32_t last,
  int *p_bits,
  const struct range *pr,
  int n_range
)
{
    int i;
    int bits = 0;
    for (i = 0; i < n_range; i++) {
        /* If the upper limit is less than "first" or the lower limit
         * is greater than "last" then the block is not in range.
         */
        if ( !(pr[i].upper < first || pr[i].lower > last)) {
            bits |= (1 << i);   /* This block is in the range, set the bit. */
        }

    }
    *p_bits = bits;
}

#define N(ARG) (sizeof(ARG)/sizeof(ARG[0]))

/** Return the size of the on-chip flash
 *  verifying that this is a device that we know about.
 * @return 0 for OK, non-zero for error:
 *  - MPC55XX_FLASH_VERIFY_ERR for LAS not 6 or MAS not 0.
 *    @note This is overriding what verify means!
 *  - MPC55XX_FLASH_SIZE_ERR Not a chip I've checked against the manual,
 *                           athat is, SIZE not 5, 7, or 11.
 */
int
mpc55xx_flash_size(
  uint32_t *p_size  /**< The size is returned here. */
)
{
    /* On the MPC5553, MPC5554, MPC5566, and MP5567 the
     *  low address space LAS field is 0x6 and all have
     *  six blocks sized 2*16k, 2*48k, and 2*64k.
     *
     * All the mid and high address spaces have 128K blocks.
     *
     * The mid address space MAS size field is 0 for the above machines,
     * and they all have 2 128K blocks.
     *
     * For the high address space we look at the
     * size field to figure out the size.  The SIZE field is:
     *
     * 5 for 1.5MB (MPC5553)
     * 7 for 2MB (MPC5554, MPC5567)
     * 11 for 3MB  (MPC5566)
     */
    int hblocks;    /* The number of blocks in the high address space. */

    /* Verify the configuration matches one of the chips that I've checked out.
     */
    if (FLASH.MCR.B.LAS != 6 || FLASH.MCR.B.MAS != 0) {
        return MPC55XX_FLASH_VERIFY_ERR;
    }

    switch(FLASH.MCR.B.SIZE) {
        case 5:
        hblocks = 8;
        break;

        case 7:
        hblocks = 12;
        break;

        case 11:
        hblocks = 20;
        break;

        default:
        return MPC55XX_FLASH_SIZE_ERR;
    }

    /* The first two banks are 256K.
     * The high block has "hblocks" 128K blocks.
     */
    *p_size = 256*1024 + 256*1024 + hblocks * 128*1024;
    return 0;
}

/* Unlock the flash blocks if "p_locked" points to something that is 0.
 * If it is a NULL pointer then we aren't allowed to do the unlock.
 */
static int
unlock_once(int lsel, int msel, int hbsel, int *p_locked)
{
    union LMLR_tag lmlr;
    union SLMLR_tag slmlr;
    union HLR_tag hlr;
    rtems_interrupt_level level;

    /* If we're already locked return.
     */
    if (p_locked && (*p_locked == 1)) {
        return 0;
    }

    /* Do we have to lock something in the low or mid block?
     */
    rtems_interrupt_disable(level);
    lmlr = FLASH.LMLR;
    if ((lsel || msel) && (lmlr.B.LME == 0)) {
        union LMLR_tag lmlr_unlock;
        lmlr_unlock.B.LLOCK=~lsel;
        lmlr_unlock.B.MLOCK=~msel;
        lmlr_unlock.B.SLOCK=1;

        if (lmlr.B.LLOCK != lmlr_unlock.B.LLOCK ||
        lmlr.B.MLOCK != lmlr_unlock.B.MLOCK) {
            if (p_locked == 0) {
                rtems_interrupt_enable(level);
                return MPC55XX_FLASH_LOCK_ERR;
            } else {
                *p_locked = 1;
            }
            FLASH.LMLR.R = 0xA1A11111;  /* Unlock. */
            FLASH.LMLR = lmlr_unlock;
        }
    }
    rtems_interrupt_enable(level);

    rtems_interrupt_disable(level);
    slmlr = FLASH.SLMLR;
    if ((lsel || msel) && (slmlr.B.SLE == 0)) {
        union SLMLR_tag slmlr_unlock;
        slmlr_unlock.B.SLLOCK=~lsel;
        slmlr_unlock.B.SMLOCK=~msel;
        slmlr_unlock.B.SSLOCK=1;

        if (slmlr.B.SLLOCK != slmlr_unlock.B.SLLOCK ||
        slmlr.B.SMLOCK != slmlr_unlock.B.SMLOCK) {
            if (p_locked == 0) {
                rtems_interrupt_enable(level);
                return MPC55XX_FLASH_LOCK_ERR;
            } else {
                *p_locked = 1;
            }
            FLASH.SLMLR.R = 0xC3C33333;  /* Unlock. */
            FLASH.SLMLR = slmlr_unlock;
        }
    }
    rtems_interrupt_enable(level);

    /* Do we have to unlock something in the high block?
     */
    rtems_interrupt_disable(level);
    hlr = FLASH.HLR;
    if (hbsel && (hlr.B.HBE == 0)) {
        union HLR_tag hlr_unlock;
        hlr_unlock.B.HBLOCK = ~hbsel;

        if (hlr.B.HBLOCK != hlr_unlock.B.HBLOCK) {
            if (p_locked == 0) {
                return MPC55XX_FLASH_LOCK_ERR;
                rtems_interrupt_enable(level);
            } else {
                *p_locked = 1;
            }
            FLASH.HLR.R = 0xB2B22222;   /* Unlock. */
            FLASH.HLR = hlr_unlock;
        }
    }
    rtems_interrupt_enable(level);

    return 0;
}

static inline uint32_t
tsize(int i)
{
  return 1 << (10 + 2 * i);
}

static int
addr_map(
  int to_phys,         /* If 1 lookup physical else lookup mapped. */
  const void *addr,    /* The address to look up. */
  uint32_t *p_result   /* Result is here. */
)
{
    uint32_t u_addr = (uint32_t)addr;
    uint32_t mas0, mas1, mas2, mas3;
    uint32_t start, end;
    rtems_interrupt_level level;
    int i;

    for (i = 0; i < 32; i++) {
      mas0 = 0x10000000 | (i << 16);
      rtems_interrupt_disable(level);
      PPC_SET_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS0, mas0);
      asm volatile("tlbre");
      mas1 = PPC_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS1);
      mas2 = PPC_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS2);
      mas3 = PPC_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS3);
      rtems_interrupt_enable(level);

      if (mas1 & 0x80000000) {
        /* Valid. */
        start = (to_phys ? mas2 : mas3) & 0xFFFFF000;
        end = start + tsize((mas1 >> 8) & 0x0000000F);
        /* Are we within range?
         */
        if (start <= u_addr && end >= u_addr) {
          uint32_t offset = (to_phys ? mas3 : mas2) & 0xFFFFF000;
          *p_result = u_addr - offset;
          return 0;
        }
      }
    }

    /* Not found in a TLB.
     */
    return ESRCH;
}

/** Return the physical address corresponding to a mapped address.
  @return 0 if OK, ESRCH if not found in TLB1.
 **/
int
mpc55xx_physical_address(
  const void *addr,     /**< Mapped address. */
  uint32_t *p_result    /**< Result returned here. */
)
{
    return addr_map(1, addr, p_result);
}

/** Return the mapped address corresponding to a mapped address.
  @return 0 if OK, ESRCH if not found in TLB1.
 **/
int
mpc55xx_mapped_address(
  const void *addr,     /**< Mapped address. */
  uint32_t *p_result    /**< Result returned here. */
)
{
    return addr_map(0, addr, p_result);
}

/**
 * Copy memory from an address into the flash when flash is relocated
 * If programming fails the address that it failed at can be returned.
 @note At end of operation the flash may be left writable.
 *     Use mpc55xx_flash_read_only() to set read-only.
 @return Zero for OK, non-zero for error:
 * - ESRCH                       Can't lookup where something lives.
 * - EPERM                       Attempt to write to non-writable flash.
 * - ETXTBSY                     Attempt to flash overlapping regions.
 * - MPC55XX_FLASH_CONFIG_ERR    for LAS not 6 or MAS not 0.
 * - MPC55XX_FLASH_SIZE_ERR      for SIZE not 5, 7, or 11.
 * - MPC55XX_FLASH_RANGE_ERR     for illegal access:
 *                               - first or first+last outside of flash;
 *                               - first not on a mod(8) boundary;
 *                               - nbytes not multiple of 8.
 * - MPC55XX_FLASH_ERASE_ERR     Erase requested but failed.
 * - MPC55XX_FLASH_PROGRAM_ERR   Program requested but failed.
 * - MPC55XX_FLASH_NOT_BLANK_ERR Blank check requested but not blank.
 * - MPC55XX_FLASH_VERIFY_ERR    Verify requested but failed.
 * - MPC55XX_FLASH_LOCK_ERR      Unlock requested but failed.
 **/

int
mpc55xx_flash_copy_op(
  void *dest,       /**< An address in the flash to copy to. */
  const void *src,  /**< An address in memory to copy from. */
  size_t nbytes,    /**< The number of bytes to copy. */
  uint32_t opmask,  /**< Bitmask of operations to perform.
                     * - MPC55XX_FLASH_UNLOCK:      Unlock the blocks.
                     * - MPC55XX_FLASH_ERASE:       Erase the blocks.
                     * - MPC55XX_FLASH_BLANK_CHECK: Verify the blocks are blank.
                     * - MPC55XX_FLASH_PROGRAM:     Program the FLASH.
                     * - MPC55XX_FLASH_VERIFY:      Verify the regions match.
                     **/
  uint32_t *p_fail  /**< If not NULL then the address where the operation
                     *   failed is returned here.
                     **/
)
{
    uint32_t udest, usrc, flash_size;
    int r;
    int peg;            /* Program or Erase Good - Did it work? */

    int lsel;           /* Low block select bits. */
    int msel;           /* Mid block select bits. */
    int hbsel;          /* High block select bits. */

    int s_lsel;           /* Source Low block select bits. */
    int s_msel;           /* Source Mid block select bits. */
    int s_hbsel;          /* Source High block select bits. */

    int unlocked = 0;
    int *p_unlocked;
    int i;
    int nwords;         /* The number of 32 bit words to write. */
    volatile uint32_t *flash;    /* Where the flash is mapped in. */
    volatile uint32_t *memory;   /* What to copy into flash. */
    uint32_t offset;    /* Where the FLASH is mapped into memory. */
    rtems_interrupt_level level;

    if ( (r = mpc55xx_flash_size(&flash_size))) {
        return r;
    }

    /* Get where the flash is mapped in.
     */
    offset = mpc55xx_flash_address();

    udest = ((uint32_t)dest) - offset;
    if ( (r = mpc55xx_physical_address(src, &usrc)) ) {
      return r;
    }

    /* Verify that the address being programmed is in flash and that it is
     * a multiple of 64 bits.
     * Someone else can remove the 64-bit restriction.
     */
    if (udest > flash_size ||
    udest + nbytes > flash_size ||
    (udest & 0x7) != 0 ||
    (nbytes & 0x7) != 0) {
        return MPC55XX_FLASH_RANGE_ERR;
    }

    if (opmask == 0) {
        return 0;
    }

    /* If we're going to do a write-style operation the flash must be writable.
     */
    if ((opmask &
        (MPC55XX_FLASH_UNLOCK | MPC55XX_FLASH_ERASE | MPC55XX_FLASH_PROGRAM)) &&
        !mpc55xx_flash_writable()
    ) {
      return EPERM;
    }

    /* If we aren't allowed to unlock then set the pointer to zero.
     * That is how "unlock_once" decides we can't unlock.
     */
    p_unlocked = (opmask & MPC55XX_FLASH_UNLOCK) ? &unlocked : 0;

    /* Set up the bit masks for the blocks to program or erase.
     */
    range_set(udest, udest + nbytes, &lsel,   lsel_ranges, N( lsel_ranges));
    range_set(udest, udest + nbytes, &msel,   msel_ranges, N( msel_ranges));
    range_set(udest, udest + nbytes, &hbsel, hbsel_ranges, N(hbsel_ranges));

    range_set(usrc, usrc + nbytes, &s_lsel,   lsel_ranges, N( lsel_ranges));
    range_set(usrc, usrc + nbytes, &s_msel,   msel_ranges, N( msel_ranges));
    range_set(usrc, usrc + nbytes, &s_hbsel, hbsel_ranges, N(hbsel_ranges));

    /* Are we attempting overlapping flash?
     */
    if ((lsel & s_lsel) | (msel & s_msel) | (hbsel & s_hbsel)) {
      return ETXTBSY;
    }

    nwords = nbytes / 4;
    flash = (volatile uint32_t *)dest;
    memory = (volatile uint32_t *)src;

  /* In the following sections any "Step N" notes refer to
   * the steps in "13.4.2.3 Flash Programming" in the reference manual.
   * XXX Do parts of this neeed to be protected by interrupt locks?
   */

    if (opmask & MPC55XX_FLASH_ERASE) {   /* Erase. */
        if ( (r = unlock_once(lsel, msel, hbsel, p_unlocked)) ) {
            return r;
        }

        rtems_interrupt_disable(level);
        FLASH.MCR.B.ERS = 1;        /* Step 1: Select erase. */

        FLASH.LMSR.B.LSEL = lsel;   /* Step 2: Select blocks to be erased. */
        FLASH.LMSR.B.MSEL = msel;
        FLASH.HSR.B.HBSEL = hbsel;

        flash[0] = 1;               /* Step 3: Write to any address in the flash
                                     * (the "erase interlock write)".
                                     */
        FLASH.MCR.B.EHV = 1;         /* Step 4: Enable high V to start erase. */
        rtems_interrupt_enable(level);
        while (FLASH.MCR.B.DONE == 0) { /* Step 5: Wait until done. */
        }
        rtems_interrupt_disable(level);
        peg = FLASH.MCR.B.PEG;       /* Save result. */
        FLASH.MCR.B.EHV = 0;         /* Disable high voltage. */
        FLASH.MCR.B.ERS = 0;         /* De-select erase. */
        rtems_interrupt_enable(level);
        if (peg == 0) {
            return MPC55XX_FLASH_ERASE_ERR; /* Flash erase failed. */
        }
    }

    if (opmask & MPC55XX_FLASH_BLANK_CHECK) {    /* Verify blank. */
        for (i = 0; i < nwords; i++) {
           if (flash[i] != 0xffffffff) {
                if (p_fail) {
                    *p_fail = (uint32_t)(flash + i);
                }
                return MPC55XX_FLASH_NOT_BLANK_ERR; /* Not blank. */
           }
        }
    }

  /* Program.
   */
    if (opmask & MPC55XX_FLASH_PROGRAM) {
        int chunk = 0;  /* Used to collect programming into 256 bit chunks. */

        if ( (r = unlock_once(lsel, msel, hbsel, p_unlocked)) ) {
            return r;
        }
        FLASH.MCR.B.PGM = 1;                /* Step 1 */

       rtems_interrupt_disable(level);

        for (i = 0; i < nwords; i += 2) {
           flash[i] = memory[i];            /* Step 2 */
           flash[i + 1] = memory[i + 1];    /* Always program in min 64 bits. */

          /* Step 3 is "write additional words" */

           /* Try to program in chunks of 256 bits.
            * Collect the 64 bit writes into 256 bit ones:
            */
           chunk++;
           if (chunk == 4) {
                /* Collected 4 64-bits for a 256 bit chunk. */
                FLASH.MCR.B.EHV = 1;            /* Step 4: Enable high V. */

                rtems_interrupt_enable(level);
                while (FLASH.MCR.B.DONE == 0) { /* Step 5: Wait until done. */
                }
                rtems_interrupt_disable(level);

                peg = FLASH.MCR.B.PEG;          /* Step 6: Save result. */
                FLASH.MCR.B.EHV = 0;            /* Step 7: Disable high V. */
                if (peg == 0) {
                    FLASH.MCR.B.PGM = 0;
                    rtems_interrupt_enable(level);
                    if (p_fail) {
                        *p_fail = (uint32_t)(flash + i);
                    }
                    return MPC55XX_FLASH_PROGRAM_ERR; /* Programming failed. */
                }
                chunk = 0;                       /* Reset chunk counter. */
            }
                                                 /* Step 8: Back to step 2. */
        }

       if (!chunk) {
            FLASH.MCR.B.PGM = 0;
            rtems_interrupt_enable(level);
       } else {
           /* If there is anything left in that last chunk flush it out:
            */
            FLASH.MCR.B.EHV = 1;

            rtems_interrupt_enable(level);
            while (FLASH.MCR.B.DONE == 0) {     /* Wait until done. */
            }
            rtems_interrupt_disable(level);

            peg = FLASH.MCR.B.PEG;              /* Save result. */
            FLASH.MCR.B.EHV = 0;                /* Disable high voltage. */
            FLASH.MCR.B.PGM = 0;
            rtems_interrupt_enable(level);

            if (peg == 0) {
                if (p_fail) {
                    *p_fail = (uint32_t)(flash + i);
                }
                return MPC55XX_FLASH_PROGRAM_ERR; /* Programming failed. */
            }
        }
    }

    if (opmask & MPC55XX_FLASH_VERIFY) {        /* Verify memory matches. */
        for (i = 0; i < nwords; i++) {
           if (flash[i] != memory[i]) {
                if (p_fail) {              /* Return the failed address. */
                    *p_fail = (uint32_t)(flash + i);
                }
                return MPC55XX_FLASH_VERIFY_ERR; /* Verification failed. */
           }
        }
    }

    return 0;
}

/** Simple flash copy with a signature that matches memcpy.
 @note At end of operation the flash may be left writable.
 *     Use mpc55xx_flash_read_only() to set read-only.
 @return Zero for OK, non-zero for error.
 *       see flash_copy_op() for possible errors.
 **/
int
mpc55xx_flash_copy(
  void *dest,       /**< An address in the flash to copy to. */
  const void *src,  /**< An address in the flash copy from. */
  size_t nbytes     /**< The number of bytes to copy. */
)
{
    return mpc55xx_flash_copy_op(dest, src, nbytes,
        (MPC55XX_FLASH_UNLOCK      |
         MPC55XX_FLASH_ERASE       |
         MPC55XX_FLASH_BLANK_CHECK |
         MPC55XX_FLASH_PROGRAM     |
         MPC55XX_FLASH_VERIFY      ), 0);
}

/** Make the flash read-write.
 @note This assumes the flash is mapped by TLB1 entry 1.
 */
void
mpc55xx_flash_set_read_write(void)
{
    rtems_interrupt_level level;
    rtems_interrupt_disable(level);
    PPC_SET_SPECIAL_PURPOSE_REGISTER( FSL_EIS_MAS0, 0x10010000);
    asm volatile("tlbre");
    PPC_SET_SPECIAL_PURPOSE_REGISTER_BITS(FSL_EIS_MAS3, 0x0000000C);
    asm volatile("tlbwe");
    rtems_interrupt_enable(level);
}

/** Make the flash read-only.
 @note This assumes the flash is mapped by TLB1 entry 1.
 */
void
mpc55xx_flash_set_read_only(void)
{
    rtems_interrupt_level level;
    rtems_interrupt_disable(level);
    PPC_SET_SPECIAL_PURPOSE_REGISTER( FSL_EIS_MAS0, 0x10010000);
    asm volatile("tlbre");
    PPC_CLEAR_SPECIAL_PURPOSE_REGISTER_BITS(FSL_EIS_MAS3, 0x0000000C);
    asm volatile("tlbwe");
    rtems_interrupt_enable(level);
}

/** See if the flash is writable.
 *  @note This assumes the flash is mapped by TLB1 entry 1.
 *  @note It needs to be writable by both user and supervisor.
 */
int
mpc55xx_flash_writable(void)
{
    uint32_t mas3;
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    PPC_SET_SPECIAL_PURPOSE_REGISTER( FSL_EIS_MAS0, 0x10010000);
    asm volatile("tlbre");
    mas3 = PPC_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS3);
    rtems_interrupt_enable(level);

    return ((mas3 & 0x0000000C) == 0x0000000C) ? 1 : 0;
}

/** Return the address where the flash is mapped in.
 @note This assumes the flash is mapped by TLB1 entry 1.
 **/
uint32_t
mpc55xx_flash_address(void)
{
    uint32_t mas2;
    rtems_interrupt_level level;

    rtems_interrupt_disable(level);
    PPC_SET_SPECIAL_PURPOSE_REGISTER( FSL_EIS_MAS0, 0x10010000);
    asm volatile("tlbre");
    mas2 = PPC_SPECIAL_PURPOSE_REGISTER(FSL_EIS_MAS2);
    rtems_interrupt_enable(level);

    return mas2 & 0xFFFFF000;
}

#endif /* MPC55XX_CHIP_TYPE / 100 == 55 */
