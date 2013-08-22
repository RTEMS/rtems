/*
 * Authorship
 * ----------
 * This software was created by
 *     R. Claus <claus@slac.stanford.edu>, 2013,
 *       Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 *     under Contract DE-AC03-76SFO0515 with the Department of Energy.
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

#ifndef LIBBSP_ARM_ZYNQ_CACHE__H
#define LIBBSP_ARM_ZYNQ_CACHE__H

#include <libcpu/arm-cp15.h>

/* These two defines also ensure that the rtems_cache_* functions have bodies */
#define CPU_DATA_CACHE_ALIGNMENT 32
#define CPU_INSTRUCTION_CACHE_ALIGNMENT 32

#define CPU_CACHE_SUPPORT_PROVIDES_RANGE_FUNCTIONS

#define L2CC_BASE_ADDR 0xF8F02000U

#define ZYNQ_L2_CACHE_LINE_SIZE 32

/* L2CC Register Offsets */
typedef struct {
  uint32_t cache_id;                                    /* Cache ID */
  uint32_t cache_type;                                  /* Cache type */

  uint8_t  reserved_8[0x100 - 8];
  uint32_t ctrl;                                        /* Control */
#define L2CC_ENABLE_MASK                 0x00000001     /* Enables the L2CC */

  uint32_t aux_ctrl;                                    /* Auxiliary control */
#define L2CC_AUX_EBRESPE_MASK            0x40000000     /* Early BRESP Enable */
#define L2CC_AUX_IPFE_MASK               0x20000000     /* Instruction Prefetch Enable */
#define L2CC_AUX_DPFE_MASK               0x10000000     /* Data Prefetch Enable */
#define L2CC_AUX_NSIC_MASK               0x08000000     /* Non-secure interrupt access control */
#define L2CC_AUX_NSLE_MASK               0x04000000     /* Non-secure lockdown enable */
#define L2CC_AUX_CRP_MASK                0x02000000     /* Cache replacement policy */
#define L2CC_AUX_FWE_MASK                0x01800000     /* Force write allocate */
#define L2CC_AUX_SAOE_MASK               0x00400000     /* Shared attribute override enable */
#define L2CC_AUX_PE_MASK                 0x00200000     /* Parity enable */
#define L2CC_AUX_EMBE_MASK               0x00100000     /* Event monitor bus enable */
#define L2CC_AUX_WAY_SIZE_MASK           0x000E0000     /* Way-size */
#define L2CC_AUX_ASSOC_MASK              0x00010000     /* Associativity */
#define L2CC_AUX_SAIE_MASK               0x00002000     /* Shared attribute invalidate enable */
#define L2CC_AUX_EXCL_CACHE_MASK         0x00001000     /* Exclusive cache configuration */
#define L2CC_AUX_SBDLE_MASK              0x00000800     /* Store buffer device limitation Enable */
#define L2CC_AUX_HPSODRE_MASK            0x00000400     /* High Priority for SO and Dev Reads Enable */
#define L2CC_AUX_FLZE_MASK               0x00000001     /* Full line of zero enable */

#define L2CC_AUX_REG_DEFAULT_MASK        0x72360000     /* Enable all prefetching, */
                                                        /* Cache replacement policy, Parity enable, */
                                                        /* Event monitor bus enable and Way Size (64 KB) */
#define L2CC_AUX_REG_ZERO_MASK           0xFFF1FFFF

  uint32_t tag_ram_ctrl;
#define L2CC_TAG_RAM_DEFAULT_MASK        0x00000111     /* Latency for tag RAM */
  uint32_t data_ram_ctrl;
#define L2CC_DATA_RAM_DEFAULT_MASK       0x00000121     /* Latency for data RAM */

  uint8_t  reserved_110[0x200 - 0x110];
  uint32_t ev_ctrl;                                     /* Event counter control */
  uint32_t ev_cnt1_cfg;                                 /* Event counter 1 configuration */
  uint32_t ev_cnt0_cfg;                                 /* Event counter 0 configuration */
  uint32_t ev_cnt1;                                     /* Event counter 1 value */
  uint32_t ev_cnt0;                                     /* Event counter 0 value */
  uint32_t int_mask;                                    /* Interrupt enable mask */
  uint32_t int_mask_status;                             /* Masked   interrupt status (read-only)*/
  uint32_t int_raw_status;                              /* Unmasked interrupt status */
  uint32_t int_clr;                                     /* Interrupt clear */
/* Interrupt bit masks */
#define L2CC_INT_DECERR_MASK             0x00000100     /* DECERR from L3 */
#define L2CC_INT_SLVERR_MASK             0x00000080     /* SLVERR from L3 */
#define L2CC_INT_ERRRD_MASK              0x00000040     /* Error on L2 data RAM (Read) */
#define L2CC_INT_ERRRT_MASK              0x00000020     /* Error on L2 tag RAM (Read) */
#define L2CC_INT_ERRWD_MASK              0x00000010     /* Error on L2 data RAM (Write) */
#define L2CC_INT_ERRWT_MASK              0x00000008     /* Error on L2 tag RAM (Write) */
#define L2CC_INT_PARRD_MASK              0x00000004     /* Parity Error on L2 data RAM (Read) */
#define L2CC_INT_PARRT_MASK              0x00000002     /* Parity Error on L2 tag RAM (Read) */
#define L2CC_INT_ECNTR_MASK              0x00000001     /* Event Counter1/0 Overflow Increment */

  uint8_t  reserved_224[0x730 - 0x224];
  uint32_t cache_sync;                                  /* Drain the STB */
  uint8_t  reserved_734[0x770 - 0x734];
  uint32_t inv_pa;                                      /* Invalidate line by PA */
  uint8_t  reserved_774[0x77c - 0x774];
  uint32_t inv_way;                                     /* Invalidate by Way */
  uint8_t  reserved_780[0x7b0 - 0x780];
  uint32_t clean_pa;                                    /* Clean Line by PA */
  uint8_t  reserved_7b4[0x7b8 - 0x7b4];
  uint32_t clean_index;                                 /* Clean Line by Set/Way */
  uint32_t clean_way;                                   /* Clean by Way */
  uint8_t  reserved_7c0[0x7f0 - 0x7c0];
  uint32_t clean_inv_pa;                                /* Clean and Invalidate Line by PA */
  uint8_t  reserved_7f4[0x7f8 - 0x7f4];
  uint32_t clean_inv_indx;                              /* Clean and Invalidate Line by Set/Way */
  uint32_t clean_inv_way;                               /* Clean and Invalidate by Way */

  uint8_t  reserved_800[0x900 - 0x800];
  uint32_t d_lockdown_0;                                /* Data        lock down 0 */
  uint32_t i_lockdown_0;                                /* Instruction lock down 0 */
  uint32_t d_lockdown_1;                                /* Data        lock down 1 */
  uint32_t i_lockdown_1;                                /* Instruction lock down 1 */
  uint32_t d_lockdown_2;                                /* Data        lock down 2 */
  uint32_t i_lockdown_2;                                /* Instruction lock down 2 */
  uint32_t d_lockdown_3;                                /* Data        lock down 3 */
  uint32_t i_lockdown_3;                                /* Instruction lock down 3 */
  uint32_t d_lockdown_4;                                /* Data        lock down 4 */
  uint32_t i_lockdown_4;                                /* Instruction lock down 4 */
  uint32_t d_lockdown_5;                                /* Data        lock down 5 */
  uint32_t i_lockdown_5;                                /* Instruction lock down 5 */
  uint32_t d_lockdown_6;                                /* Data        lock down 6 */
  uint32_t i_lockdown_6;                                /* Instruction lock down 6 */
  uint32_t d_lockdown_7;                                /* Data        lock down 7 */
  uint32_t i_lockdown_7;                                /* Instruction lock down 7 */

  uint8_t  reserved_940[0x950 - 0x940];
  uint32_t lock_line_en;                                /* Lockdown by Line Enable */
  uint32_t unlock_way;                                  /* Cache lockdown by way */

  uint8_t  reserved_958[0xc00 - 0x958];
  uint32_t addr_filtering_start;                        /* Address range redirect, part 1 */
  uint32_t addr_filtering_end;                          /* Address range redirect, part 2 */
#define L2CC_ADDR_FILTER_VALID_MASK      0xFFF00000     /* Address filtering valid bits*/
#define L2CC_ADDR_FILTER_ENABLE_MASK     0x00000001     /* Address filtering enable bit*/

  uint8_t  reserved_c08[0xf40 - 0xc08];
  uint32_t debug_ctrl;                                  /* Debug control */
#define L2CC_DEBUG_SPIDEN_MASK           0x00000004     /* Debug SPIDEN bit */
#define L2CC_DEBUG_DWB_MASK              0x00000002     /* Debug DWB bit, forces write through */
#define L2CC_DEBUG_DCL_MASK              0x00000002     /* Debug DCL bit, disables cache line fill */

  uint8_t  reserved_f44[0xf60 - 0xf44];
  uint32_t prefetch_ctrl;                               /* Purpose prefetch enables */
  uint8_t  reserved_f64[0xf80 - 0xf64];
  uint32_t power_ctrl;                                  /* Purpose power controls */
} L2CC;

static inline void
zynq_cache_l1_cache_properties(uint32_t *l1LineSize,
                         uint32_t *l1NumWays,
                         uint32_t *l1NumSets)
{
  uint32_t id;

  /* Select cache level 1 and Data cache in CSSELR */
  arm_cp15_set_cache_size_selection(0);
  _ARM_Instruction_synchronization_barrier();
  id = arm_cp15_get_cache_size_id();

  *l1LineSize =  (id        & 0x0007U) + 2 + 2; /* Cache line size (+2 -> bytes) */
  *l1NumWays  = ((id >>  3) & 0x03ffU) + 1;     /* Number of Ways */
  *l1NumSets  = ((id >> 13) & 0x7fffU) + 1;     /* Number of Sets */
}


static inline void
zynq_cache_l1_cache_flush_1_data_line(const void *d_addr)
{
  /* Select cache Level 1 and Data cache in CSSELR */
  arm_cp15_set_cache_size_selection(0);

  /* Flush the Data cache */
  arm_cp15_data_cache_clean_and_invalidate_line(d_addr);

  /* Wait for L1 flush to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l1_cache_flush_data_range(const void *d_addr, size_t n_bytes)
{
  const void * final_address;

 /*
  * Set d_addr to the beginning of the cache line; final_address indicates
  * the last address_t which needs to be pushed. Increment d_addr and push
  * the resulting line until final_address is passed.
  */

  if( n_bytes == 0 )
    /* Do nothing if number of bytes to flush is zero */
    return;

  /* Select cache Level 1 and Data cache in CSSELR */
  arm_cp15_set_cache_size_selection(0);

  final_address = (void *)((size_t)d_addr + n_bytes - 1);
  d_addr = (void *)((size_t)d_addr & ~(CPU_DATA_CACHE_ALIGNMENT - 1));
  while( d_addr <= final_address )  {
    arm_cp15_data_cache_clean_and_invalidate_line( d_addr );
    d_addr = (void *)((size_t)d_addr + CPU_DATA_CACHE_ALIGNMENT);
  }

  /* Wait for L1 flush to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l1_cache_flush_entire_data(void)
{
  uint32_t l1LineSize, l1NumWays, l1NumSets;
  uint32_t sets, ways, s, w;

  /* Select cache Level 1 and Data cache in CSSELR */
  arm_cp15_set_cache_size_selection(0);
  _ARM_Instruction_synchronization_barrier();

  /* Get the L1 cache properties */
  zynq_cache_l1_cache_properties(&l1LineSize, &l1NumWays, &l1NumSets);

  ways = l1NumWays * (1 << 30);
  sets = l1NumSets * (1 << l1LineSize);

  /* Invalidate all the cache lines */
  for (w = 0; w < ways; w += (1 << 30)) {
    for (s = 0; s < sets; s += (1 << l1LineSize)) {
      /* Flush by Set/Way */
      arm_cp15_data_cache_clean_and_invalidate_line_by_set_and_way(w | s);
    }
  }

  /* Wait for L1 flush to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l1_cache_invalidate_1_data_line(const void *d_addr)
{
  /* Select cache Level 1 and Data cache in CSSELR */
  arm_cp15_set_cache_size_selection(0);

  /* Invalidate the cache line */
  arm_cp15_data_cache_invalidate_line(d_addr);

  /* Wait for L1 invalidate to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l1_cache_invalidate_data_range(const void *d_addr, size_t n_bytes)
{
  const void * final_address;

 /*
  * Set d_addr to the beginning of the cache line; final_address indicates
  * the last address_t which needs to be invalidated. Increment d_addr and
  * invalidate the resulting line until final_address is passed.
  */

  if( n_bytes == 0 )
    /* Do nothing if number of bytes to invalidate is zero */
    return;

  /* Select cache Level 1 and Data cache in CSSELR */
  arm_cp15_set_cache_size_selection(0);

  final_address = (void *)((size_t)d_addr + n_bytes - 1);
  d_addr = (void *)((size_t)d_addr & ~(CPU_DATA_CACHE_ALIGNMENT - 1));
  while( final_address >= d_addr ) {
    arm_cp15_data_cache_invalidate_line( d_addr );
    d_addr = (void *)((size_t)d_addr + CPU_DATA_CACHE_ALIGNMENT);
  }

  /* Wait for L1 invalidate to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l1_cache_invalidate_entire_data(void)
{
  uint32_t l1LineSize, l1NumWays, l1NumSets;
  uint32_t sets, ways, s, w;

  /* Select cache Level 1 and Data cache in CSSELR */
  arm_cp15_set_cache_size_selection(0);
  _ARM_Instruction_synchronization_barrier();

  /* Get the L1 cache properties */
  zynq_cache_l1_cache_properties(&l1LineSize, &l1NumWays, &l1NumSets);

  ways = l1NumWays * (1 << 30);
  sets = l1NumSets * (1 << l1LineSize);

  /* Invalidate all the cache lines */
  for (w = 0; w < ways; w += (1 << 30)) {
    for (s = 0; s < sets; s += (1 << l1LineSize)) {
      /* Invalidate by Set/Way */
      arm_cp15_data_cache_invalidate_line_by_set_and_way(w | s);
    }
  }

  /* Wait for L1 invalidate to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l1_cache_store_data(const void *d_addr)
{
  /* Select cache Level 1 and Data cache in CSSELR */
  arm_cp15_set_cache_size_selection(0);

  /* Store the Data cache line */
  arm_cp15_data_cache_clean_line(d_addr);

  /* Wait for L1 store to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l1_cache_freeze_data(void)
{
  /* TODO */
}

static inline void
zynq_cache_l1_cache_unfreeze_data(void)
{
  /* TODO */
}

static inline void
zynq_cache_l1_cache_invalidate_1_instruction_line(const void *i_addr)
{
  /* Select cache Level 1 and Instruction cache in CSSELR */
  arm_cp15_set_cache_size_selection(1);

  /* Invalidate the Instruction cache line */
  arm_cp15_instruction_cache_invalidate_line(i_addr);

  /* Wait for L1 invalidate to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l1_cache_invalidate_instruction_range(const void *i_addr, size_t n_bytes)
{
  const void * final_address;

 /*
  * Set i_addr to the beginning of the cache line; final_address indicates
  * the last address_t which needs to be invalidated. Increment i_addr and
  * invalidate the resulting line until final_address is passed.
  */

  if( n_bytes == 0 )
    /* Do nothing if number of bytes to invalidate is zero */
    return;

  /* Select cache Level 1 and Instruction cache in CSSELR */
  arm_cp15_set_cache_size_selection(1);

  final_address = (void *)((size_t)i_addr + n_bytes - 1);
  i_addr = (void *)((size_t)i_addr & ~(CPU_INSTRUCTION_CACHE_ALIGNMENT - 1));
  while( final_address > i_addr ) {
    arm_cp15_instruction_cache_invalidate_line( i_addr );
    i_addr = (void *)((size_t)i_addr + CPU_INSTRUCTION_CACHE_ALIGNMENT);
  }

  /* Wait for L1 invalidate to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l1_cache_invalidate_entire_instruction(void)
{
  /* Select cache Level 1 and Instruction cache in CSSELR */
  arm_cp15_set_cache_size_selection(1);

  /* Invalidate the Instruction cache */
  arm_cp15_instruction_cache_invalidate();

  /* Wait for L1 invalidate to complete */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l1_cache_freeze_instruction(void)
{
  /* TODO */
}

static inline void
zynq_cache_l1_cache_unfreeze_instruction(void)
{
  /* TODO */
}

static inline void
zynq_cache_l1_cache_enable_data(void)
{
  rtems_interrupt_level level;
  uint32_t ctrl;

  rtems_interrupt_disable(level);

  /* Enable caches only if they are disabled */
  ctrl = arm_cp15_get_control();
  if (!(ctrl & ARM_CP15_CTRL_C)) {
    /* Clean and invalidate the Data cache */
    zynq_cache_l1_cache_invalidate_entire_data();

    /* Enable the Data cache */
    ctrl |= ARM_CP15_CTRL_C;

    arm_cp15_set_control(ctrl);
  }

  rtems_interrupt_enable(level);
}

static inline void
zynq_cache_l1_cache_disable_data(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  /* Clean and invalidate the Data cache */
  zynq_cache_l1_cache_flush_entire_data();

  /* Disable the Data cache */
  arm_cp15_set_control(arm_cp15_get_control() & ~ARM_CP15_CTRL_C);

  rtems_interrupt_enable(level);
}

static inline void
zynq_cache_l1_cache_enable_instruction(void)
{
  rtems_interrupt_level level;
  uint32_t              ctrl;

  rtems_interrupt_disable(level);

  /* Enable Instruction cache only if it is disabled */
  ctrl = arm_cp15_get_control();
  if (!(ctrl & ARM_CP15_CTRL_I)) {
    /* Invalidate the Instruction cache */
    zynq_cache_l1_cache_invalidate_entire_instruction();

    /* Enable the Instruction cache */
    ctrl |= ARM_CP15_CTRL_I;

    arm_cp15_set_control(ctrl);
  }

  rtems_interrupt_enable(level);
}

static inline void
zynq_cache_l1_cache_disable_instruction(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);

  /* Synchronize the processor */
  _ARM_Data_synchronization_barrier();

  /* Invalidate the Instruction cache */
  zynq_cache_l1_cache_invalidate_entire_instruction();

  /* Disable the Instruction cache */
  arm_cp15_set_control(arm_cp15_get_control() & ~ARM_CP15_CTRL_I);

  rtems_interrupt_enable(level);
}


static inline void
zynq_cache_l2_cache_flush_1_line(const void *d_addr)
{
  volatile L2CC* l2cc = (volatile L2CC *)L2CC_BASE_ADDR;

  l2cc->clean_inv_pa = (uint32_t)d_addr;

  /* Synchronize the processor */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l2_cache_flush_range(const void *d_addr, size_t n_bytes)
{
  volatile L2CC *l2cc = (volatile L2CC *)L2CC_BASE_ADDR;

  if (n_bytes != 0) {
    uint32_t       adx = (uint32_t)d_addr;
    const uint32_t end = adx + n_bytes;

    /* Back starting address up to start of a line and flush until end */
    for (adx &= ~(ZYNQ_L2_CACHE_LINE_SIZE - 1);
         adx < end;
         adx += ZYNQ_L2_CACHE_LINE_SIZE) {
      l2cc->clean_inv_pa = adx;
    }
  }

  /* Wait for L2 flush to complete */
  while (l2cc->cache_sync != 0);

  /* Synchronize the processor */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l2_cache_flush_entire(void)
{
  volatile L2CC* l2cc = (volatile L2CC *)L2CC_BASE_ADDR;

  /* Flush the caches */
  l2cc->clean_inv_way = 0x0000FFFFU;

  /* Wait for the flush to complete */
  while (l2cc->cache_sync != 0);

  /* Synchronize the processor */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l2_cache_invalidate_1_line(const void *d_addr)
{
  volatile L2CC* l2cc = (volatile L2CC *)L2CC_BASE_ADDR;

  l2cc->inv_pa = (uint32_t)d_addr;

  /* Synchronize the processor */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l2_cache_invalidate_range(const void* d_addr, size_t n_bytes)
{
  volatile L2CC* l2cc = (volatile L2CC *)L2CC_BASE_ADDR;

  if (n_bytes != 0) {
    uint32_t       adx = (uint32_t)d_addr;
    const uint32_t end = adx + n_bytes;

    /* Back starting address up to start of a line and invalidate until end */
    for (adx &= ~(ZYNQ_L2_CACHE_LINE_SIZE - 1);
         adx < end;
         adx += ZYNQ_L2_CACHE_LINE_SIZE) {
      l2cc->inv_pa = adx;
    }
  }

  /* Wait for L2 invalidate to complete */
  while (l2cc->cache_sync != 0);

  /* Synchronize the processor */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l2_cache_invalidate_entire(void)
{
  volatile L2CC *l2cc = (volatile L2CC *)L2CC_BASE_ADDR;

  /* Invalidate the caches */
  l2cc->inv_way = 0xFFFFU;

  /* Wait for the invalidate to complete */
  while (l2cc->cache_sync != 0);

  /* Synchronize the processor */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l2_cache_store(const void *d_addr)
{
  volatile L2CC *l2cc = (volatile L2CC *)L2CC_BASE_ADDR;

  l2cc->clean_pa = (uint32_t)d_addr;

  /* Synchronize the processor */
  _ARM_Data_synchronization_barrier();
}

static inline void
zynq_cache_l2_cache_freeze(void)
{
  /* TODO */
}

static inline void
zynq_cache_l2_cache_unfreeze(void)
{
  /* TODO */
}

static inline void
zynq_cache_l2_cache_enable(void)
{
  volatile L2CC *l2cc = (volatile L2CC *)L2CC_BASE_ADDR;

  /* Only enable if L2CC is currently disabled */
  if ((l2cc->ctrl & L2CC_ENABLE_MASK) == 0) {
    rtems_interrupt_level level;
    uint32_t value;

    rtems_interrupt_disable(level);

    /* Set up the way size and latencies */
    value               = l2cc->aux_ctrl;
    value              &= L2CC_AUX_REG_ZERO_MASK;
    value              |= L2CC_AUX_REG_DEFAULT_MASK;
    l2cc->aux_ctrl      = value;
    l2cc->tag_ram_ctrl  = L2CC_TAG_RAM_DEFAULT_MASK;
    l2cc->data_ram_ctrl = L2CC_DATA_RAM_DEFAULT_MASK;

    /* Clear the pending interrupts */
    l2cc->int_clr       = l2cc->int_raw_status;

    /* Enable the L2CC */
    l2cc->ctrl         |= L2CC_ENABLE_MASK;

    /* Synchronize the processor */
    _ARM_Data_synchronization_barrier();

    /* Enable the Data cache */
    arm_cp15_set_control(arm_cp15_get_control() | ARM_CP15_CTRL_C);

    /* Synchronize the processor */
    _ARM_Data_synchronization_barrier();

    rtems_interrupt_enable(level);
  }
}

static inline void
zynq_cache_l2_cache_disable(void)
{
  volatile L2CC *l2cc = (volatile L2CC *)L2CC_BASE_ADDR;

  if (l2cc->ctrl & L2CC_ENABLE_MASK) {
    rtems_interrupt_level level;
    uint32_t              ctrl;

    rtems_interrupt_disable(level);

    ctrl = arm_cp15_get_control();

    /* Disable the L1 Data cache */
    ctrl &= ~ARM_CP15_CTRL_C;

    arm_cp15_set_control(ctrl);

    /* Synchronize the processor */
    _ARM_Data_synchronization_barrier();

    /* Clean and Invalidate L2 Cache */
    zynq_cache_l2_cache_flush_entire();

    /* Disable the L2 cache */
    l2cc->ctrl &= ~L2CC_ENABLE_MASK;

    /* Enable the L1 Data cache */
    ctrl |= ARM_CP15_CTRL_C;

    arm_cp15_set_control(ctrl);

    /* Synchronize the processor */
    _ARM_Data_synchronization_barrier();

    rtems_interrupt_enable(level);
  }
}


static inline void
_CPU_cache_enable_data(void)
{
  zynq_cache_l1_cache_enable_data();
  zynq_cache_l2_cache_enable();
}

static inline void
_CPU_cache_disable_data(void)
{
  zynq_cache_l1_cache_disable_data();
  zynq_cache_l2_cache_disable();
}

static inline void
_CPU_cache_enable_instruction(void)
{
  zynq_cache_l1_cache_enable_instruction();
  zynq_cache_l2_cache_enable();
}

static inline void
_CPU_cache_disable_instruction(void)
{
  zynq_cache_l1_cache_disable_instruction();
  zynq_cache_l2_cache_disable();
}

static inline void
_CPU_cache_flush_data_range(const void *d_addr, size_t n_bytes)
{
  volatile L2CC *l2cc = (volatile L2CC *)L2CC_BASE_ADDR;

  if (n_bytes != 0) {
    uint32_t       adx = (uint32_t)d_addr;
    const uint32_t end = adx + n_bytes;

    /* Select cache Level 1 and Data cache in CSSELR */
    arm_cp15_set_cache_size_selection(0);

    /* Back starting address up to start of a line and flush until end */
    for (adx &= ~(CPU_DATA_CACHE_ALIGNMENT - 1);
         adx < end;
         adx += CPU_DATA_CACHE_ALIGNMENT) {
      /* Flush L1 Data cache line */
      arm_cp15_data_cache_clean_and_invalidate_line( (const void*)adx );

      /* Flush L2 cache line */
      l2cc->clean_inv_pa = adx;

      _ARM_Data_synchronization_barrier();
    }
  }

  /* Wait for L1 and L2 flush to complete */
  _ARM_Data_synchronization_barrier();
  while (l2cc->cache_sync != 0);
}

static inline void
_CPU_cache_flush_entire_data(void)
{
  zynq_cache_l1_cache_flush_entire_data();
  zynq_cache_l2_cache_flush_entire();
}

static inline void
_CPU_cache_invalidate_data_range(const void *d_addr, size_t n_bytes)
{
  volatile L2CC *l2cc = (volatile L2CC *)L2CC_BASE_ADDR;

  if (n_bytes != 0) {
    uint32_t       adx = (uint32_t)d_addr;
    const uint32_t end = adx + n_bytes;

    /* Select cache Level 1 and Data cache in CSSELR */
    arm_cp15_set_cache_size_selection(0);

    /* Back starting address up to start of a line and invalidate until end */
    for (adx &= ~(CPU_DATA_CACHE_ALIGNMENT - 1);
         adx < end;
         adx += CPU_DATA_CACHE_ALIGNMENT) {
      /* Invalidate L2 cache line */
      l2cc->inv_pa = adx;
      _ARM_Data_synchronization_barrier();

      /* Invalidate L1 Data cache line */
      arm_cp15_data_cache_invalidate_line( (const void *)adx );
    }
  }

  /* Wait for L1 and L2 invalidate to complete */
  _ARM_Data_synchronization_barrier();
  while (l2cc->cache_sync != 0);
}

static inline void
_CPU_cache_invalidate_entire_data(void)
{
  zynq_cache_l2_cache_invalidate_entire();
  zynq_cache_l1_cache_invalidate_entire_data();
}

static inline void
_CPU_cache_store_data_line(const void *d_addr)
{
  zynq_cache_l1_cache_store_data(d_addr);
  zynq_cache_l2_cache_store(d_addr);
}

static inline void
_CPU_cache_freeze_data(void)
{
  zynq_cache_l1_cache_freeze_data();
  zynq_cache_l2_cache_freeze();
}

static inline void
_CPU_cache_unfreeze_data(void)
{
  zynq_cache_l1_cache_unfreeze_data();
  zynq_cache_l2_cache_unfreeze();
}

static inline void
_CPU_cache_invalidate_instruction_range(const void *i_addr, size_t n_bytes)
{
  volatile L2CC *l2cc = (volatile L2CC *)L2CC_BASE_ADDR;

  if (n_bytes != 0) {
    uint32_t       adx = (uint32_t)i_addr;
    const uint32_t end = adx + n_bytes;

    /* Select cache Level 1 and Instruction cache in CSSELR */
    arm_cp15_set_cache_size_selection(1);

    /* Back starting address up to start of a line and invalidate until end */
    for (adx &= ~(CPU_INSTRUCTION_CACHE_ALIGNMENT - 1);
         adx < end;
         adx += CPU_INSTRUCTION_CACHE_ALIGNMENT) {
      /* Invalidate L2 cache line */
      l2cc->inv_pa = adx;
      _ARM_Data_synchronization_barrier();

      /* Invalidate L1 I-cache line */
      arm_cp15_instruction_cache_invalidate_line( (const void *)adx );
    }
  }

  /* Wait for L1 and L2 invalidate to complete */
  _ARM_Data_synchronization_barrier();
  while (l2cc->cache_sync != 0);
}

static inline void
_CPU_cache_invalidate_entire_instruction(void)
{
  zynq_cache_l2_cache_invalidate_entire();
  zynq_cache_l1_cache_invalidate_entire_instruction();
}

static inline void
_CPU_cache_freeze_instruction(void)
{
  zynq_cache_l1_cache_freeze_instruction();
  zynq_cache_l2_cache_freeze();
}

static inline void
_CPU_cache_unfreeze_instruction(void)
{
  zynq_cache_l1_cache_unfreeze_instruction();
  zynq_cache_l2_cache_unfreeze();
}

#endif /* LIBBSP_ARM_ZYNQ_CACHE__H */
