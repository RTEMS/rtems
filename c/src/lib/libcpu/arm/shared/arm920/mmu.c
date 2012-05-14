/*
 *  ARM920 MMU functions
 *
 *  Copyright (c) 2004 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com>
 */
#include <libcpu/mmu.h>
#include <libcpu/arm-cp15.h>

typedef uint32_t mmu_lvl1_t;

extern uint32_t _ttbl_base;

static void mmu_set_map_inval(mmu_lvl1_t *base);

#define MMU_CTRL_MMU_EN             (1 << 0)
#define MMU_CTRL_ALIGN_FAULT_EN     (1 << 1)
#define MMU_CTRL_D_CACHE_EN         (1 << 2)
#define MMU_CTRL_DEFAULT            (0xf << 3)
#define MMU_CTRL_LITTLE_ENDIAN      (0 << 7)
#define MMU_CTRL_BIG_ENDIAN         (1 << 7)
#define MMU_CTRL_SYS_PROT           (1 << 8)
#define MMU_CTRL_ROM_PROT           (1 << 9)
#define MMU_CTRL_I_CACHE_EN         (1 << 12)
#define MMU_CTRL_LOW_VECT           (0 << 13)
#define MMU_CTRL_HIGH_VECT          (1 << 13)


#define MMU_SET_LVL1_SECT(addr, ap, dom, ce, be) \
          (((addr) & 0xfff00000) |     \
           (ap)                  |     \
           (dom)                 |     \
           ((ce) << 3)           |     \
           ((be) << 2)           |     \
           0x12)

#define MMU_SET_LVL1_INVAL (0x0)

#define MMU_SECT_AP_ALL (0x3 << 10)

void mmu_init(mmu_sect_map_t *map)
{
    mmu_lvl1_t *lvl1_base;
    int i;

    /* flush the cache and TLB */
    arm_cp15_cache_invalidate();
    arm_cp15_tlb_invalidate();

    /* set manage mode access for all domains */
    arm_cp15_set_domain_access_control(0xffffffff);

    lvl1_base = (mmu_lvl1_t *)&_ttbl_base;

    /* set up the trans table */
    mmu_set_map_inval(lvl1_base);
    arm_cp15_set_translation_table_base(lvl1_base);

    /* create a 1:1 mapping of the entire address space */
    i = 0;
    while(map[i].size != 0) {
        int c = 0;  /* to avoid uninitialized warnings */
        int b = 0;  /* to avoid uninitialized warnings */
        int pbase;
        int vbase;
        int sects;

        switch (map[i].cache_flags) {
        case MMU_CACHE_NONE:
            c = 0;
            b = 0;
            break;
        case MMU_CACHE_BUFFERED:
            c = 0;
            b = 1;
            break;
        case MMU_CACHE_WTHROUGH:
            c = 1;
            b = 0;
            break;
        case MMU_CACHE_WBACK:
            c = 1;
            b = 1;
            break;
        }

        pbase = (map[i].paddr & 0xfff00000) >> 20;
        vbase = (map[i].vaddr & 0xfff00000) >> 20;
        sects = map[i].size;

        while (sects > 0) {
            lvl1_base[vbase] = MMU_SET_LVL1_SECT(pbase << 20,
                                                 MMU_SECT_AP_ALL,
                                                 0,
                                                 c,
                                                 b);
            pbase++;
            vbase++;
            sects--;
        }
        i++;
    }

    /* flush the cache and TLB */
    arm_cp15_cache_invalidate();
    arm_cp15_tlb_invalidate();

    /*  I & D caches turned on */
    arm_cp15_set_control(MMU_CTRL_DEFAULT |
                         MMU_CTRL_D_CACHE_EN |
                         MMU_CTRL_I_CACHE_EN |
                         MMU_CTRL_ALIGN_FAULT_EN |
                         MMU_CTRL_LITTLE_ENDIAN |
                         MMU_CTRL_MMU_EN);

    return;
}

/* set all the level 1 entrys to be invalid descriptors */
static void mmu_set_map_inval(mmu_lvl1_t *base)
{
    int i;
    for (i = 0; i < (0x4000 / 4); i++) {
        base[i] = MMU_SET_LVL1_INVAL;
    }
}

void mmu_set_cpu_async_mode(void)
{
    uint32_t reg;
    reg = arm_cp15_get_control();
    reg |= 0xc0000000;
    arm_cp15_set_control(reg);
}
