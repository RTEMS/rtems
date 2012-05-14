/*  mvme16x_hw.h
 *
 *  This include file contains all MVME16x board IO definitions
 *  and was derived by combining the common items in the
 *  mvme162 and mvme167 BSPs.
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __MVME16xHW_h
#define __MVME16xHW_h

#include <bsp.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Network driver configuration
 */

struct rtems_bsdnet_ifconfig;
int uti596_attach(struct rtems_bsdnet_ifconfig * pConfig, int attaching );
#define RTEMS_BSP_NETWORK_DRIVER_NAME   "uti1"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH uti596_attach

/*
 *  This is NOT the base address of local RAM!
 *  This is the base local address of the VMEbus short I/O space. A local
 *  access to this space results in a A16 VMEbus I/O cycle. This base address
 *  is NOT configurable on the MVME167, although the types of VMEbus short I/O
 *  cycles generated when a cycle in the local 0xFFFF0000-0xFFFFFFFF address
 *  range is generated is under control of bits 8-15 of LCSR 0xFFF4002C. The
 *  GCSRs of other boards are accessible only through the VMEbus short I/O
 *  space. See pages 2-45 and 2-7.
 */
#define BOARD_BASE_ADDRESS 0xFFFF0000

/*
 *  This address must be added to the BOARD_BASE_ADDRESS to access the GCSR of
 *  other MVMEs in the group, i.e. it represents the offset of the GCSRs in the
 *  VMEbus short I/O space. It also should represent the group address of this
 *  MVME167! The group address is configurable, and must match the address
 *  programmed into the MVME167 through the 167Bug monitor. 0xCC is the address
 *  recommended by Motorola. It is arbitrary.
 *  See pages 2-42 and 2-97 to 2-104.
 */
#define GROUP_BASE_ADDRESS 0x0000CC00

/*
 *  Representation of the VMEchip2 LCSR.
 *  Could be made more detailed.
 */

typedef volatile struct {
  unsigned long     slave_adr[2];
  unsigned long     slave_trn[2];
  unsigned long     slave_ctl;
  unsigned long     mastr_adr[4];
  unsigned long     mastr_trn;
  unsigned long     mastr_att;
  unsigned long     mastr_ctl;
  unsigned long     dma_ctl_1;
  unsigned long     dma_ctl_2;
  unsigned long     dma_loc_cnt;
  unsigned long     dma_vme_cnt;
  unsigned long     dma_byte_cnt;
  unsigned long     dma_adr_cnt;
  unsigned long     dma_status;
  unsigned long     to_ctl;
  unsigned long     timer_cmp_1;
  unsigned long     timer_cnt_1;
  unsigned long     timer_cmp_2;
  unsigned long     timer_cnt_2;
  unsigned long     board_ctl;
  unsigned long     prescaler_cnt;
  unsigned long     intr_stat;
  unsigned long     intr_ena;
  unsigned long     intr_soft_set;
  unsigned long     intr_clear;
  unsigned long     intr_level[4];
  unsigned long     vector_base;
} lcsr_regs;

/*
 *  Base address of VMEchip2 LCSR
 *  Not configurable on the MVME167.
 *  XXX what about 162?
 */
#define lcsr        ((lcsr_regs * const) 0xFFF40000)

/*
 *  Vector numbers for the interrupts from the VMEchip2. Use the values
 *  "recommended" by Motorola.
 *  See pages 2-70 to 2-92, and table 2-3.
 */

/* MIEN (Master Interrupt Enable) bit in LCSR 0xFFF40088. */
#define MASK_INT    0x00800000

/* The content of VBR0 corresponds to "X" in table 2-3 */
#define VBR0        0x6

/* The content of VBR1 corresponds to "Y" in table 2-3 */
#define VBR1        0x7

/*
 *  Representation of the PCCchip2
 */
typedef volatile struct pccchip2_regs_ {
  unsigned char     chip_id;            /* 0xFFF42000 */
  unsigned char     chip_revision;      /* 0xFFF42001 */
  unsigned char     gen_control;        /* 0xFFF42002 */
  unsigned char     vector_base;        /* 0xFFF42003 */
  unsigned long     timer_cmp_1;        /* 0xFFF42004 */
  unsigned long     timer_cnt_1;        /* 0xFFF42008 */
  unsigned long     timer_cmp_2;        /* 0xFFF4200C */
  unsigned long     timer_cnt_2;        /* 0xFFF42010 */
  unsigned char     LSB_prescaler_count;/* 0xFFF42014 */
  unsigned char     prescaler_clock_adjust; /* 0xFFF42015 */
  unsigned char     timer_ctl_2;        /* 0xFFF42016 */
  unsigned char     timer_ctl_1;        /* 0xFFF42017 */
  unsigned char     gpi_int_ctl;        /* 0xFFF42018 */
  unsigned char     gpio_ctl;           /* 0xFFF42019 */
  unsigned char     timer_int_ctl_2;    /* 0xFFF4201A */
  unsigned char     timer_int_ctl_1;    /* 0xFFF4201B */
  unsigned char     SCC_error;          /* 0xFFF4201C */
  unsigned char     SCC_modem_int_ctl;  /* 0xFFF4201D */
  unsigned char     SCC_tx_int_ctl;     /* 0xFFF4201E */
  unsigned char     SCC_rx_int_ctl;     /* 0xFFF4201F */
  unsigned char     reserved1[3];
  unsigned char     modem_piack;        /* 0xFFF42023 */
  unsigned char     reserved2;
  unsigned char     tx_piack;           /* 0xFFF42025 */
  unsigned char     reserved3;
  unsigned char     rx_piack;           /* 0xFFF42027 */
  unsigned char     LANC_error;         /* 0xFFF42028 */
  unsigned char     reserved4;
  unsigned char     LANC_int_ctl;       /* 0xFFF4202A */
  unsigned char     LANC_berr_ctl;      /* 0xFFF4202B */
  unsigned char     SCSI_error;         /* 0xFFF4202C */
  unsigned char     reserved5[2];
  unsigned char     SCSI_int_ctl;       /* 0xFFF4202F */
  unsigned char     print_ack_int_ctl;  /* 0xFFF42030 */
  unsigned char     print_fault_int_ctl;/* 0xFFF42031 */
  unsigned char     print_sel_int_ctl;  /* 0xFFF42032 */
  unsigned char     print_pe_int_ctl;   /* 0xFFF42033 */
  unsigned char     print_busy_int_ctl; /* 0xFFF42034 */
  unsigned char     reserved6;
  unsigned char     print_input_status; /* 0xFFF42036 */
  unsigned char     print_ctl;          /* 0xFFF42037 */
  unsigned char     chip_speed;         /* 0xFFF42038 */
  unsigned char     reserved7;
  unsigned char     print_data;         /* 0xFFF4203A */
  unsigned char     reserved8[3];
  unsigned char     int_level;          /* 0xFFF4203E */
  unsigned char     int_mask;           /* 0xFFF4203F */
} pccchip2_regs;

/*
 *  Base address of the PCCchip2.
 *  This is not configurable in the MVME167.
 */
#define pccchip2    ((pccchip2_regs * const) 0xFFF42000)

/*
 *  On the MVME162, we have the mcchip and the pccchip2 on
 *  the 167.  They are similar but different enough where
 *  we have to reconcile them later.
 */

/*
 * Vector numbers for the interrupts from the PCCchip2. Use the values
 * "recommended" by Motorola.
 * See page 3-15.
 */
#define PCCCHIP2_VBR    0x5

/*
 * The following registers are located in the VMEbus short
 * IO space and respond to address modifier codes $29 and $2D.
 * On FORCE CPU use address gcsr_vme and device /dev/vme16d32.
 */

typedef volatile struct {
  unsigned char       chip_revision;
  unsigned char       chip_id;
  unsigned char       lmsig;
  unsigned char       board_scr;
  unsigned short      gpr[6];
} gcsr_regs;

#define gcsr_vme ((gcsr_regs * const) (GROUP_BASE_ADDRESS + BOARD_BASE_ADDRESS))
#define gcsr     ((gcsr_regs * const) 0xFFF40100)

/*
 *  Representation of 82596CA LAN controller: Memory Map
 */
typedef volatile struct i82596_regs_ {
  unsigned short  port_lower;             /* 0xFFF46000 */
  unsigned short  port_upper;             /* 0xFFF46002 */
  unsigned long   chan_attn;              /* 0xFFF46004 */
} i82596_regs;

/*
 *  Base address of the 82596.
 */

#define i82596    ((i82596_regs * const) 0xFFF46000)

/*
 *  Representation of initialization data in NVRAM
 */

#if defined(mvme167)
typedef volatile struct nvram_config_ {
  unsigned char   cache_mode;          /* 0xFFFC0000 */
  unsigned char   console_mode;        /* 0xFFFC0001 */
  unsigned char   console_printk_port; /* 0xFFFC0002 */
  unsigned char   pad1;                /* 0xFFFC0003 */
  unsigned long   ipaddr;              /* 0xFFFC0004 */
  unsigned long   netmask;             /* 0xFFFC0008 */
  unsigned char   enaddr[6];           /* 0xFFFC000C */
  unsigned short  processor_id;        /* 0xFFFC0012 */
  unsigned long   rma_start;           /* 0xFFFC0014 */
  unsigned long   vma_start;           /* 0xFFFC0018 */
  unsigned long   ramsize;             /* 0xFFFC001C */
} nvram_config;

/*
 *  Pointer to the base of User Area NVRAM
 */

#define nvram      ((nvram_config * const) 0xFFFC0000)

#endif

/*
 *  Flag to indicate if J1-4 is on (and parameters should be
 *  sought in User Area NVRAM)
 *
 *  NOTE:  If NVRAM has bad settings, the you want to disable this
 *         on the MVME167.
 */
#if defined(mvme167)
  #define NVRAM_CONFIGURE \
    ( !( ( (unsigned char)(lcsr->vector_base & 0xFF) ) & 0x10 ) )
#else
  #define NVRAM_CONFIGURE 0
#endif

#ifdef __cplusplus
}
#endif

#endif
