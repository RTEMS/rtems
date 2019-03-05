/**
 * @file
 *
 * @ingroup RTEMSBSPsI386
 *
 * @brief Real mode interrupt call implementation
 */

/*
 * Copyright (c) 2014 - CTU in Prague
 *                      Jan Doležal ( dolezj21@fel.cvut.cz )
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp/realmode_int.h>
#include <string.h>
#include <rtems/score/cpu.h>

/*
 * offsets to \a i386_realmode_interrupt_registers declared in realmode_int.h
 * used in inline assmbler for better readability
 */
#define IR_EAX_OFF      "0x00"
#define IR_EBX_OFF      "0x04"
#define IR_ECX_OFF      "0x08"
#define IR_EDX_OFF      "0x0C"
#define IR_ESI_OFF      "0x10"
#define IR_EDI_OFF      "0x14"
#define IR_DS_OFF       "0x18"
#define IR_ES_OFF       "0x1A"
#define IR_FS_OFF       "0x1C"
#define IR_GS_OFF       "0x1E"

/*
 * offsets to \a rm_int_regs_bkp_param
 */
#define BKP_ESP_OFF     "0x20"
#define BKP_SS_OFF      "0x24"
#define BKP_DS_OFF      "0x26"
#define RM_ENTRY        "0x28"
#define PM_ENTRY        "0x2C"

/**
 * @brief parameters, results, backup values accessible in real mode
 *
 * @note Struct members not necessarily used in C. This serves also as
 *       layout of memory and it is used within inline assembler.
 */
typedef struct {
    i386_realmode_interrupt_registers inoutregs;
    /** spot for back up of protected mode stack pointer */
    uint32_t pm_esp_bkp;
    /** spot for back up of protected mode stack selector */
    uint16_t pm_ss_bkp;
    /** spot for back up of protected mode data selector */
    uint16_t ds_bkp;
    /** spot for setting up long indirect jump offset
        to real mode from 16bit protected mode */
    uint16_t rm_entry;
    /** spot for setting up long indirect jump segment
        to real mode from 16bit protected mode */
    uint16_t rm_code_segment;
    /** returning offset for long indirect jump back
        to 32bit protected mode */
    uint32_t pm_entry;
    /** returning selector for long indirect jump back
        to 32bit protected mode */
    uint16_t pm_code_selector;
    /* if this struct is to be modified update offset definitions as well */
} RTEMS_PACKED rm_int_regs_bkp_param;

/* offsets to \a pm_bkp_and_param */
#define BKP_IDTR_LIM    "0x00"
#define BKP_IDTR_BASE   "0x02"
#define BKP_ES_OFF      "0x06"
#define BKP_FS_OFF      "0x08"
#define BKP_GS_OFF      "0x0A"
#define RML_ENTRY       "0x0C"
#define RML_D_SEL       "0x12"
#define RM_SS           "0x14"
#define RM_SP           "0x16"
#define RM_DS           "0x18"

/**
 * @brief backup values, pointers/parameters accessible in protected mode
 *
 * @note Struct members not necessarily used in C. This serves also as
 *       layout of memory and it is used within inline assembler.
 */
typedef struct {
    /** spot for backup protected mode interrupt descriptor table register */
    uint16_t idtr_lim_bkp;
    /** @see idtr_lim_bkp */
    uint32_t idtr_base_bkp;
    /** spot to backup of ES register value in 32bit protected mode */
    uint16_t es_bkp;
    /** spot to backup of FS register value in 32bit protected mode */
    uint16_t fs_bkp;
    /** spot to backup of GS register value in 32bit protected mode */
    uint16_t gs_bkp;
    /** values for indirect jump to 16bit protected mode */
    uint32_t rml_entry;
    /** @see rml_entry */
    uint16_t rml_code_selector;
    /** data selector for 16bit protected mode */
    uint16_t rml_data_selector;
    /** values determinig location of real mode stack */
    uint16_t rm_stack_segment;
    /** @see rm_stack_segment */
    uint16_t rm_stack_pointer;
    /** data segment for real mode */
    uint16_t rm_data_segment;
} RTEMS_PACKED pm_bkp_and_param;

/* addresses where we are going to put Interrupt buffer,
 * parameter/returned/preserved values, stack and copy code
 * for calling BIOS interrupt real mode interface
 * The value is chosen arbitrarily in the first 640kB
 * to be accessible for real mode. It should be out of range
 * used by RTEMS because its base address is above 1MB.
 * It has to be above first 4kB (or better 64kB) which could
 * be used by BIOS.
 */
#define REAL_MODE_SPOT   0x12000
/* REAL_MODE_SPOT value is also top of real mode stack */

/* buffers positions and lengths */
#define DEFAULT_BUFFER_SIZE             512
static void *default_rm_buffer_spot = (void *)REAL_MODE_SPOT;
static uint16_t default_rm_buffer_size = DEFAULT_BUFFER_SIZE;

/* real mode stack */
#define STACK_SIZE                      8192
#define INT_STACK_TOP                   REAL_MODE_SPOT

/******************************
 * STACK            *         *
 ****************************** REAL_MODE_SPOT
 * INT_BUF          * 512 B   *
 ******************************
 * INT_REGs         *  50 B   *
 ******************************
 * INT_FNC          *~149 B   *
 ******************************/

#define __DP_TYPE       uint8_t
#define __DP_YES        ((__DP_TYPE)1)
#define __DP_NO         ((__DP_TYPE)-1)
#define __DP_FAIL       ((__DP_TYPE)0)
static __DP_TYPE descsPrepared = __DP_NO;

/* rml - real mode alike */
#define rml_limit 0xFFFF
static uint16_t rml_code_dsc_index = 0;
static uint16_t rml_data_dsc_index = 0;

/**
 * @brief Prepares real-mode like descriptors to be used for switching
 * to real mode.
 *
 * Descriptors will be placed to the GDT.
 *
 * @param[in] base32 32-bit physical address to be used as base for 16-bit
 *               protected mode descriptors
 * @retval __DP_YES descriptors are prepared
 * @retval __DP_FAIL descriptors allocation failed (GDT too small)
 */
static __DP_TYPE prepareRMDescriptors (void *base32) {
    static void *prevBase = (void *)-1;
    /* check if descriptors were prepared already */
    if (descsPrepared == __DP_YES && prevBase == base32)
        return descsPrepared;

    if (descsPrepared == __DP_FAIL)
        return descsPrepared;

  /* create 'real mode like' segment descriptors, for switching to real mode */
    rml_code_dsc_index = i386_next_empty_gdt_entry();
    if (rml_code_dsc_index == 0)
    {
        /* not enough space in GDT */
        descsPrepared = __DP_FAIL;
        return descsPrepared;
    }

    segment_descriptors flags_desc;
    memset(&flags_desc, 0, sizeof(flags_desc));
    flags_desc.type                = 0xE;      /* bits 4  */
    flags_desc.descriptor_type     = 0x1;      /* bits 1  */
    flags_desc.privilege           = 0x0;      /* bits 2  */
    flags_desc.present             = 0x1;      /* bits 1  */
    flags_desc.available           = 0x0;      /* bits 1  */
    flags_desc.fixed_value_bits    = 0x0;      /* bits 1  */
    flags_desc.operation_size      = 0x0;      /* bits 1  */
    flags_desc.granularity         = 0x0;      /* bits 1  */
    i386_fill_segment_desc_base((unsigned)base32, &flags_desc);
    i386_fill_segment_desc_limit(rml_limit, &flags_desc);
    if (i386_raw_gdt_entry(rml_code_dsc_index, &flags_desc) == 0)
    {
        /* selector to GDT out of range */
        descsPrepared = __DP_FAIL;
        return descsPrepared;
    }

    rml_data_dsc_index = i386_next_empty_gdt_entry();
    if (rml_data_dsc_index == 0)
    {
        /* not enough space in GDT for both descriptors */
        descsPrepared = __DP_FAIL;
        return descsPrepared;
    }

    flags_desc.type                = 0x2;      /* bits 4  */
    if (i386_raw_gdt_entry(rml_data_dsc_index, &flags_desc) == 0)
    {
        /* selector to GDT out of range */
        descsPrepared = __DP_FAIL;
        return descsPrepared;
    }
    prevBase = base32;
    descsPrepared = __DP_YES;
    return descsPrepared;
}

void *i386_get_default_rm_buffer(uint16_t *size) {
    *size = default_rm_buffer_size;
    return default_rm_buffer_spot;
}

int i386_real_interrupt_call(uint8_t interrupt_number,
                             i386_realmode_interrupt_registers *ir)
{
    uint32_t pagingon;
    rm_int_regs_bkp_param *int_passed_regs_spot;
    /* place where the code switching to realmode and executing
       interrupt is coppied */
    void *rm_swtch_code_dst;
    void *rm_stack_top;

    size_t cpLength;
    void *cpBeg;

    /* values that can be passed from protected mode are stored in this struct
       and they are passed later to the inline assembler executing interrupt */
    volatile pm_bkp_and_param pm_bkp, *pm_bkp_addr;
    unsigned short unused_offset;

    __asm__ volatile(   "\t"
        "movl    %%cr0, %%eax\n\t"
        "andl    %1, %%eax\n"
        : "=a"(pagingon)
        : "i"(CR0_PAGING)
    );
    if (pagingon)
        return 0;

    /* located under 1MB for real mode to be able to get/set values */
    int_passed_regs_spot = (rm_int_regs_bkp_param *)
                                (default_rm_buffer_spot+default_rm_buffer_size);
    /* position for real mode code reallocation to the first 1MB of RAM */
    rm_swtch_code_dst = (void *)((uint32_t)int_passed_regs_spot +
                                 sizeof(*int_passed_regs_spot));
    rm_stack_top = (void *)INT_STACK_TOP;

    if (prepareRMDescriptors(int_passed_regs_spot) != __DP_YES)
        return 0;

    pm_bkp_addr = &pm_bkp;
    i386_Physical_to_real(
        rm_stack_top - STACK_SIZE,
        (unsigned short *)&pm_bkp.rm_stack_segment,
        (unsigned short *)&pm_bkp.rm_stack_pointer
    );
    pm_bkp.rm_stack_pointer += STACK_SIZE;
    pm_bkp.rml_code_selector = (rml_code_dsc_index<<3);
    pm_bkp.rml_entry = ((uint32_t)rm_swtch_code_dst -
                        (uint32_t)int_passed_regs_spot);
    pm_bkp.rml_data_selector = (rml_data_dsc_index<<3);
    i386_Physical_to_real(
        int_passed_regs_spot,
        (unsigned short *)&pm_bkp.rm_data_segment,
        &unused_offset
    );

    int_passed_regs_spot->inoutregs = *ir;
    /* offset from the beginning of coppied code */
    uint16_t rm_entry_offset;
    __asm__ volatile(
        "movw   $(rment-cp_beg), %0\n\t"
        : "=r"(rm_entry_offset)
    );
    i386_Physical_to_real(
        rm_swtch_code_dst+rm_entry_offset,
        (unsigned short *)&int_passed_regs_spot->rm_code_segment,
        (unsigned short *)&int_passed_regs_spot->rm_entry
    );
    __asm__ volatile(
        "movl   $(cp_end), %0\n\t"
        "movw   %%cs, %1\n\t"
        : "=mr"(int_passed_regs_spot->pm_entry),
          "=mr"(int_passed_regs_spot->pm_code_selector)
    );
    /* copy code for switch to real mode and
       executing interrupt to first MB of RAM */
    __asm__ volatile(   "\t"
        "mov    $cp_end-cp_beg, %0\n\t"
        "mov    $cp_beg, %1\n\t"
        : "=rm"(cpLength), "=rm"(cpBeg)
    );
    memcpy(rm_swtch_code_dst, cpBeg, cpLength);
    /* write interrupt number to be executed */
    uint16_t interrupt_number_off;
    uint8_t *interrupt_number_ptr;
    __asm__ volatile(   "\t"
        "movw   $intnum-cp_beg, %0\n\t"
        : "=rm"(interrupt_number_off)
    );
    interrupt_number_ptr = (uint8_t *)(rm_swtch_code_dst+interrupt_number_off);
    *interrupt_number_ptr = interrupt_number;
    /* execute code that jumps to coppied function, which switches to real mode,
       loads registers with values passed to interrupt and executes interrupt */
    __asm__ volatile(   "\t"
        /* backup stack */
        "movl    %[regs_spot], %%ebx\n\t"
        "movl    %%esp, "BKP_ESP_OFF"(%%ebx)\n\t"
        "movw    %%ss,  "BKP_SS_OFF"(%%ebx)\n\t"
        /* backup data selector */
        "movw    %%ds,  "BKP_DS_OFF"(%%ebx)\n\t"
        /* backup other selectors */
        "movl    %[pm_bkp], %%esi\n\t"
        "movw    %%es, "BKP_ES_OFF"(%%esi)\n\t"
        "movw    %%fs, "BKP_FS_OFF"(%%esi)\n\t"
        "movw    %%gs, "BKP_GS_OFF"(%%esi)\n\t"
        /* hopefully loader does not damage interrupt table on the beginning of
           memory; that means length: 0x3FF, base: 0x0 */
        /* preserve idtr */
        "movl    %%esi, %%eax\n\t"
        "addl    $"BKP_IDTR_LIM", %%eax\n\t"
        "cli\n\t"
        "sidt    (%%eax)\n\t"
        "movl    $rmidt, %%eax\n\t"
        "lidt    (%%eax)\n\t"
        /* prepare 'real mode like' data selector */
        "movw    "RML_D_SEL"(%%esi), %%ax\n\t"
        /* prepare real mode data segment value */
        "xorl    %%edx,%%edx\n\t"
        "movw    "RM_DS"(%%esi), %%dx\n\t"
        /* prepare real mode stack values */
        "movw    "RM_SS"(%%esi), %%cx\n\t"
        "movzwl  "RM_SP"(%%esi), %%esp\n\t"
        /* jump to copied function and */
        /* load 'real mode like' code selector */
        "ljmp   *"RML_ENTRY"(%%esi)\n"
"rmidt:"/* limit and base for realmode interrupt descriptor table */
        ".word 0x3FF\n\t"
        ".long 0\n\t"
        /* load 'real mode like' data selectors */
"cp_beg: .code16\n\t"
        "movw    %%ax, %%ss\n\t"
        "movw    %%ax, %%ds\n\t"
        "movw    %%ax, %%es\n\t"
        "movw    %%ax, %%fs\n\t"
        "movw    %%ax, %%gs\n\t"
        /* disable protected mode */
        "movl    %%cr0, %%eax\n\t"
        "and     %[cr0_prot_dis], %%ax\n\t"
        "movl    %%eax, %%cr0\n\t"
        /* base for data selector of 16-bit protected mode is
           at beginning of passed regs */
        /* flush prefetch queue by far jumping */
        "ljmp    *"RM_ENTRY"\n\t"
"rment: "
        /* establish rm stack - esp was already set in 32-bit protected mode*/
        "movw    %%cx, %%ss\n\t"
        /* set data segment (value prepared in 32-bit prot mode) */
        "movw    %%dx, %%ds\n\t"
        /* count real mode pointer so we don't need to overuse address
           prefix (by using 32bit addresses in 16bit context) */
        "shll    $4,%%edx\n\t"
        "subl    %%edx,%%ebx\n\t"
        /* prepare values to be used after interrupt call */
        "pushw   %%bx\n\t"
        "pushw   %%ds\n\t"
        /* fill registers with parameters */
        "movw    " IR_DS_OFF"(%%bx), %%ax\n\t"
        "pushw   %%ax\n\t"
        "movl    "IR_EAX_OFF"(%%bx), %%eax\n\t"
        "movl    "IR_ECX_OFF"(%%bx), %%ecx\n\t"
        "movl    "IR_EDX_OFF"(%%bx), %%edx\n\t"
        "movl    "IR_EDI_OFF"(%%bx), %%edi\n\t"
        "movl    "IR_ESI_OFF"(%%bx), %%esi\n\t"
        "movw    " IR_ES_OFF"(%%bx), %%es\n\t"
        "movw    " IR_FS_OFF"(%%bx), %%fs\n\t"
        "movw    " IR_GS_OFF"(%%bx), %%gs\n\t"
        /* prepare ebx register */
        "movl    "IR_EBX_OFF"(%%bx), %%ebx\n\t"
        /* prepare ds */
        "popw    %%ds\n\t"
        /* interrupt instruction */
        ".byte   0xCD\n\t"
"intnum: .byte   0x0\n\t"
        /* fill return structure */
        "pushw   %%ds\n\t"
        "pushl   %%ebx\n\t"
        "movw    0x6(%%esp), %%ds\n\t"
        "movw    0x8(%%esp),%%bx\n\t" /* regs_spot */
        "movl    %%eax,"IR_EAX_OFF"(%%bx)\n\t"
        "popl    %%eax\n\t"
        "movl    %%eax,"IR_EBX_OFF"(%%bx)\n\t"
        "movl    %%ecx,"IR_ECX_OFF"(%%bx)\n\t"
        "movl    %%edx,"IR_EDX_OFF"(%%bx)\n\t"
        "movl    %%esi,"IR_ESI_OFF"(%%bx)\n\t"
        "movl    %%edi,"IR_EDI_OFF"(%%bx)\n\t"
        "popw    %%ax\n\t"
        "movw    %%ax, " IR_DS_OFF"(%%bx)\n\t"
        "movw    %%es, " IR_ES_OFF"(%%bx)\n\t"
        "movw    %%fs, " IR_FS_OFF"(%%bx)\n\t"
        "movw    %%gs, " IR_GS_OFF"(%%bx)\n\t"
        /* prepare protected mode data segment */
        "movw    "BKP_DS_OFF"(%%bx), %%ax\n\t"
        /* restore protected mode stack values */
        "movl    "BKP_ESP_OFF"(%%bx),%%esp\n\t"
        "movw    "BKP_SS_OFF"(%%bx), %%dx\n\t"
        /* return to protected mode */
        "movl    %%cr0, %%ecx     \n\t"
        "or      %[cr0_prot_ena], %%cx\n\t"
        "movl    %%ecx, %%cr0     \n\t"
        "ljmpl   *"PM_ENTRY"(%%bx)\n\t"
        ".code32\n"
        /* reload segmentation registers */
"cp_end:"
        "movw    %%ax, %%ds\n\t"
        /* restore stack segment in protected mode context */
        "movw    %%dx, %%ss\n\t"
        "movl    %[pm_bkp], %%esi\n\t"
        "movw    "BKP_ES_OFF"(%%esi), %%es\n\t"
        "movw    "BKP_FS_OFF"(%%esi), %%fs\n\t"
        "movw    "BKP_GS_OFF"(%%esi), %%gs\n\t"
        /* restore IDTR */
        "addl    $"BKP_IDTR_LIM", %%esi\n\t"
        "lidt    (%%esi)\n\t"
        :
        : [regs_spot]"m"(int_passed_regs_spot),
          [pm_bkp]"m"(pm_bkp_addr),
          [cr0_prot_ena]"i"(CR0_PROTECTION_ENABLE),
          [cr0_prot_dis]"i"(~CR0_PROTECTION_ENABLE)
        : "memory", "ebx", "ecx", "edx", "esi", "edi"
    );
    *ir = int_passed_regs_spot->inoutregs;
    return 1;
}
