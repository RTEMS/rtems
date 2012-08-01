/*COPYRIGHT (c) 1989-2009.
*  On-Line Applications Research Corporation (OAR).
*  *
*  *  The license and distribution terms for this file may be
*  *  found in the file LICENSE in this distribution or at
*  *  http://www.rtems.com/license/LICENSE.
*  *
*  *  $Id$
*  */
#ifndef _LIBCPU_MMU_SUPPORT_H
#define _LIBCPU_MMU_SUPPORT_H


#ifdef __cplusplus
  extern "C" {
#endif


/* Access definition macros below */
#define _PPC_MMU_ACCESS_SUPERVISOR_ONLY     0
#define _PPC_MMU_ACCESS_SUPERVISOR_WRITE_ONLY   1
#define _PPC_MMU_ACCESS_NO_PROT        2//no protection
#define _PPC_MMU_ACCESS_READ_ONLY      3

#define _PPC_CACHE_WRITETHROUGH                0x8
#define _PPC_CACHE_WRITEBACK                       0x0
#define _PPC_CACHE_ALLOW                               0x0
#define _PPC_CACHE_DISABLE                            0x4
#define _PPC_CACHE_MEM_COHERENCY              0x2
#define _PPC_CACHE_MEM_NONCOHERENCY       0x0
#define _PPC_CACHE_GUARDED                          0x1
#define _PPC_CACHE_UNGUARDED                      0x0

/* Defining masks for the various bits of the PTE
 *    Referenced from OEA PowerPC Manual */

#define PTEW0_VALID            0x80000000
#define PTEW0_VSID             0x7fffff80
#define PTEW0_HASHF            0x00000040
#define PTEW0_API              0x0000003f
#define PTEW1_RPN              0xfffff000
#define PTEW1_REF              0x00000100
#define PTEW1_CHG              0x00000080
#define PTEW1_WIMG             0x00000078 
#define PTEW1_PROTP            0x00000003

#define SR_VSID                0x00ffffff
#define SR_KS                  0x40000000                                           
#define SR_KP                  0x20000000  

#define RTEMS_MPE_PAGE_SIZE    0x1000

/* Splitting the 64 bit PTE into two 32 bit words. As shown in the OEA
 *    Manual of PowerPC */
#define KEY_SUP      (1<<30) /* supervisor mode key */
#define KEY_USR      (1<<29) /* user mode key */
#define LD_PG_SIZE              12 /* In logarithm base */
#define LD_PI_SIZE              16
#define LD_VSID_SIZE            24
#define LD_HASH_SIZE            19
#define MMUS_DEBUG
/* Primary and secondary PTE hash functions */

/* Compute the primary hash from a VSID and a PI */
#define PTE_HASH_FUNC1(vsid, pi) (((vsid)^(pi))&(0x0007FFFF))

/* Compute the secondary hash from a primary hash */
#define PTE_HASH_FUNC2(hash1) ((~(hash1))&(0x0007FFFF))

typedef struct  
{
  uint32_t ptew0; /* Word 0 */
  uint32_t ptew1; /* Word 1 */
} ppc_bsp_mm_mpe;

extern char RamBase[];
extern char RamSize[];

/* Function prototypes that can be shared with high
 *    level modules go in here */
void
mmu_init(void);

void
mmu_irq_init(void);

extern ppc_bsp_mm_mpe* BSP_ppc_add_pte(ppc_bsp_mm_mpe *ppteg,
    ppc_bsp_mm_mpe *spteg,
    uint32_t vsid,
    uint32_t pi, 
    uint32_t wimg,
    uint32_t protp
);

void get_pteg_addr(ppc_bsp_mm_mpe** pteg, uint32_t hash);
#ifdef __cplusplus
  }
#endif

#endif

