#include <rtems.h>
#include <rtems/error.h>
#include <bsp.h>

#define NUMBER_INTERRUPTS (IRQ_RAVEN_ERROR + 1)

#define DISABLE_IRQ 0x80000000

static rtems_isr raven_interrupt_handler(rtems_vector_number vector);
static rtems_isr default_interrupt_handler(rtems_vector_number vector);
static rtems_isr spurious_interrupt_handler(rtems_vector_number vector);


static rtems_isr_entry handlers[NUMBER_INTERRUPTS];
static int *vec_prio_reg_addresses[NUMBER_INTERRUPTS] = {0};
static void *raven_base = 0;
static int timer_frequency = 0;

rtems_status_code interrupt_controller_init(void) {
    int i;
    int raven_config = pci_find_by_devid(RAVEN_VENDOR_ID, RAVEN_DEVICE_ID, 0);
    rtems_isr_entry old_handler;
    rtems_status_code sc;

    /* get base address of raven registers in LOCAL space */
    raven_base = 
        (void *)PCI_TO_LOCAL(pci_conf_read32(raven_config, PCI_BASE_ADDRESS_1));

    /* set cascade mode in global configuration register */
    st_le32(raven_base + 0x01020, 0x20000000);

    /* set spurious interrupt vector */
    st_le32(raven_base + 0x010e0, IRQ_SPURIOUS);

    /* get the timer frequency */
    timer_frequency = ld_le32(raven_base + 0x010f0);

    /* fill in vector/priority register addresses */
    vec_prio_reg_addresses[IRQ_PIC_CASCADE] = raven_base + 0x10000;
    vec_prio_reg_addresses[IRQ_FALCON_ECC]  = raven_base + 0x10020;
    vec_prio_reg_addresses[IRQ_ETHERNET]    = raven_base + 0x10040;
    vec_prio_reg_addresses[IRQ_VME_LINT0]   = raven_base + 0x100a0;
    vec_prio_reg_addresses[IRQ_VME_LINT1]   = raven_base + 0x100c0;
    vec_prio_reg_addresses[IRQ_VME_LINT2]   = raven_base + 0x100e0;
    vec_prio_reg_addresses[IRQ_VME_LINT3]   = raven_base + 0x10100;
    vec_prio_reg_addresses[IRQ_PMC1A_PMC2B] = raven_base + 0x10120;
    vec_prio_reg_addresses[IRQ_PMC1B_PMC2C] = raven_base + 0x10140;
    vec_prio_reg_addresses[IRQ_PMC1C_PMC2D] = raven_base + 0x10160;
    vec_prio_reg_addresses[IRQ_PMC1D_PMC2A] = raven_base + 0x10180;
    vec_prio_reg_addresses[IRQ_LM_SIG_0]    = raven_base + 0x101a0;
    vec_prio_reg_addresses[IRQ_LM_SIG_1]    = raven_base + 0x101c0;
    vec_prio_reg_addresses[IRQ_TIMER_0]     = raven_base + 0x01120;
    vec_prio_reg_addresses[IRQ_TIMER_1]     = raven_base + 0x01160;
    vec_prio_reg_addresses[IRQ_TIMER_2]     = raven_base + 0x011a0;
    vec_prio_reg_addresses[IRQ_TIMER_3]     = raven_base + 0x011e0;
    vec_prio_reg_addresses[IRQ_IPI_0]       = raven_base + 0x010a0;
    vec_prio_reg_addresses[IRQ_IPI_1]       = raven_base + 0x010b0;
    vec_prio_reg_addresses[IRQ_IPI_2]       = raven_base + 0x010c0;
    vec_prio_reg_addresses[IRQ_IPI_3]       = raven_base + 0x010d0;
    vec_prio_reg_addresses[IRQ_RAVEN_ERROR] = raven_base + 0x10200;

    /* initialize all vector/priority registers to level 0, disabled */
    for (i = 0; i < NUMBER_INTERRUPTS; i++) {
        if (vec_prio_reg_addresses[i]) {
            st_le32(vec_prio_reg_addresses[i], DISABLE_IRQ | i);
        }
    }

    /* initialize all interrupt destination registers to processor 0 */
    st_le32(raven_base + 0x01130, 1);
    st_le32(raven_base + 0x01170, 1);
    st_le32(raven_base + 0x011b0, 1);
    st_le32(raven_base + 0x011f0, 1);
    st_le32(raven_base + 0x10010, 1);
    st_le32(raven_base + 0x10030, 1);
    st_le32(raven_base + 0x10050, 1);
    st_le32(raven_base + 0x10070, 1);
    st_le32(raven_base + 0x10090, 1);
    st_le32(raven_base + 0x100b0, 1);
    st_le32(raven_base + 0x100d0, 1);
    st_le32(raven_base + 0x100f0, 1);
    st_le32(raven_base + 0x10110, 1);
    st_le32(raven_base + 0x10130, 1);
    st_le32(raven_base + 0x10150, 1);
    st_le32(raven_base + 0x10170, 1);
    st_le32(raven_base + 0x10190, 1);
    st_le32(raven_base + 0x101b0, 1);
    st_le32(raven_base + 0x101d0, 1);
    st_le32(raven_base + 0x101f0, 1);
    st_le32(raven_base + 0x10210, 1);

    /* set up default interrupt handlers */
    for (i = 0; i < NUMBER_INTERRUPTS; i++) {
        handlers[i] = default_interrupt_handler;
    }
    handlers[IRQ_SPURIOUS] = spurious_interrupt_handler;

    /* enable 8259 PIC interrupt at level 8 */
    st_le32(vec_prio_reg_addresses[IRQ_PIC_CASCADE], 
            0x00c00000 | (PRIORITY_ISA_INT << 16) | IRQ_PIC_CASCADE);

    /* attach interrupt handler and enable interrupts */
    sc = rtems_interrupt_catch(raven_interrupt_handler, PPC_IRQ_EXTERNAL, 
                               &old_handler);
    if (sc != RTEMS_SUCCESSFUL) {
        rtems_panic("can't catch interrupt for raven pic\n");
    }
    return set_interrupt_task_priority(0);
}

#define PCI_Interrupt_Ack_Addr ((unsigned32 *) 0xFEFF0030)
#define NonspecificEOI 0x20
#define PIC1_OCW2 (*(volatile char *)IO_TO_LOCAL(0x20))
#define PIC2_OCW2 (*(volatile char *)IO_TO_LOCAL(0xA0))

static rtems_isr raven_interrupt_handler(rtems_vector_number vector) {
    int raven_vector;
    int msr_value;

    /* read Raven interrupt acknowledge register */
    raven_vector = ld_le32(raven_base + 0x200a0);

    if (raven_vector == IRQ_PIC_CASCADE) {
        /* read PCI interrupt acknowledge register */
        int piack_image = ld_le32(PCI_Interrupt_Ack_Addr);
        int isa_vector = piack_image & 0x07;
        int pic_id = (piack_image >> 3) & 0x1f;
        
        if (pic_id == 0x10) {
            isa_vector += 8;
        } else if (pic_id != 0x01) {
            rtems_panic("unrecognized PIACK value: %08x\n", piack_image);
        }

        /* set MSRee = 1 */
        _CPU_MSR_Value(msr_value);
        msr_value |= PPC_MSR_EE;
        _CPU_MSR_SET(msr_value);

        /* signal EOI to Raven */
        st_le32(raven_base + 0x200b0, 0);

        /* call handler */
        handlers[isa_vector](isa_vector);

        /* signal EOI to 8259 PIC */
        PIC1_OCW2 = NonspecificEOI;
        if (isa_vector >= 8) {
            PIC2_OCW2 = NonspecificEOI;
        }
    } else {
        /* set MSRee = 1 */
        _CPU_MSR_Value(msr_value);
        msr_value |= PPC_MSR_EE;
        _CPU_MSR_SET(msr_value);

        /* call handler */
        handlers[raven_vector](raven_vector);

        /* signal EOI to Raven */
        st_le32(raven_base + 0x200b0, 0);

    }
}

static rtems_isr default_interrupt_handler(rtems_vector_number vector) {
    rtems_panic("unhandled interrupt: %d\n", vector);
}

static rtems_isr spurious_interrupt_handler(rtems_vector_number vector) {
    rtems_panic("spurious interrupt\n");
}

rtems_status_code bsp_interrupt_catch(rtems_isr_entry      new_isr_handler,
                                      rtems_vector_number  vector,
                                      rtems_isr_entry     *old_isr_handler) {
    if (!old_isr_handler || ((int)new_isr_handler & 3) != 0) {
        return RTEMS_INVALID_ADDRESS;
    }
    switch (vector) {
		case IRQ_ISA_TIMER:     case IRQ_UART:          case IRQ_ISA_LM_SIG:
		case IRQ_ABORT_SWITCH:  case IRQ_ISA_ETHERNET:  case IRQ_ISA_UNIVERSE:
		case IRQ_ISA_PMC_PCIX:  case IRQ_FALCON_ECC:    case IRQ_ETHERNET:
		case IRQ_VME_LINT0:     case IRQ_VME_LINT1:     case IRQ_VME_LINT2:
		case IRQ_VME_LINT3:     case IRQ_PMC1A_PMC2B:   case IRQ_PMC1B_PMC2C:
		case IRQ_PMC1C_PMC2D:   case IRQ_PMC1D_PMC2A:   case IRQ_LM_SIG_0:
		case IRQ_LM_SIG_1:      case IRQ_TIMER_0:       case IRQ_TIMER_1:
		case IRQ_TIMER_2:       case IRQ_TIMER_3:       case IRQ_IPI_0:
		case IRQ_IPI_1:         case IRQ_IPI_2:         case IRQ_IPI_3:
		case IRQ_RAVEN_ERROR:
            *old_isr_handler = handlers[vector];
            handlers[vector] = new_isr_handler;
            return RTEMS_SUCCESSFUL;
        default:
            return RTEMS_INVALID_NUMBER;
    }
}

#define PIC1_Mask (*(unsigned8 *) IO_TO_LOCAL(0x21))
#define PIC2_Mask  (*(unsigned8 *) IO_TO_LOCAL(0xA1))


rtems_status_code bsp_interrupt_enable(rtems_vector_number vector,
                                       int priority) {
    switch (vector) {
		case IRQ_ISA_TIMER:     case IRQ_UART:          case IRQ_ISA_LM_SIG:
		case IRQ_ABORT_SWITCH:  case IRQ_ISA_ETHERNET:  case IRQ_ISA_UNIVERSE:
		case IRQ_ISA_PMC_PCIX:
            if (priority != PRIORITY_ISA_INT) {
                return RTEMS_INVALID_NUMBER;
            }
            if (vector < 8) {
                PIC1_Mask &= ~ (1 << vector);
            } else {
                PIC2_Mask &= ~ (1 << (vector - 8));
            }
            return RTEMS_SUCCESSFUL;

		case IRQ_ETHERNET:     case IRQ_VME_LINT0:    case IRQ_VME_LINT1:
		case IRQ_VME_LINT2:    case IRQ_VME_LINT3:    case IRQ_PMC1A_PMC2B:
		case IRQ_PMC1B_PMC2C:  case IRQ_PMC1C_PMC2D:  case IRQ_PMC1D_PMC2A:
		case IRQ_LM_SIG_0:     case IRQ_LM_SIG_1:
            if (priority & ~15) {
                return RTEMS_INVALID_NUMBER;
            }
            st_le32(vec_prio_reg_addresses[vector], 
                    0x00400000 | (priority << 16) | vector);
            return RTEMS_SUCCESSFUL;

		case IRQ_FALCON_ECC:  case IRQ_TIMER_0:  case IRQ_TIMER_1:
		case IRQ_TIMER_2:     case IRQ_TIMER_3:  case IRQ_IPI_0:
		case IRQ_IPI_1:       case IRQ_IPI_2:    case IRQ_IPI_3:
		case IRQ_RAVEN_ERROR:
            if (priority & ~15) {
                return RTEMS_INVALID_NUMBER;
            }
            st_le32(vec_prio_reg_addresses[vector], (priority << 16) | vector);
            return RTEMS_SUCCESSFUL;

        default:
            return RTEMS_INVALID_NUMBER;
    }
}

rtems_status_code bsp_interrupt_disable(rtems_vector_number vector) {
    switch (vector) {
		case IRQ_ISA_TIMER:     case IRQ_UART:          case IRQ_ISA_LM_SIG:
		case IRQ_ABORT_SWITCH:  case IRQ_ISA_ETHERNET:  case IRQ_ISA_UNIVERSE:
		case IRQ_ISA_PMC_PCIX:
            if (vector < 8) {
                PIC1_Mask |= (1 << vector);
            } else {
                PIC2_Mask |= (1 << (vector - 8));
            }
            return RTEMS_SUCCESSFUL;

		case IRQ_FALCON_ECC:   case IRQ_ETHERNET:     case IRQ_VME_LINT0:
		case IRQ_VME_LINT1:    case IRQ_VME_LINT2:    case IRQ_VME_LINT3:
		case IRQ_PMC1A_PMC2B:  case IRQ_PMC1B_PMC2C:  case IRQ_PMC1C_PMC2D:
		case IRQ_PMC1D_PMC2A:  case IRQ_LM_SIG_0:     case IRQ_LM_SIG_1:
		case IRQ_TIMER_0:      case IRQ_TIMER_1:      case IRQ_TIMER_2:
		case IRQ_TIMER_3:      case IRQ_IPI_0:        case IRQ_IPI_1:
		case IRQ_IPI_2:        case IRQ_IPI_3:        case IRQ_RAVEN_ERROR:
            st_le32(vec_prio_reg_addresses[vector], DISABLE_IRQ | vector);
            return RTEMS_SUCCESSFUL;

        default:
            return RTEMS_INVALID_NUMBER;
    }
}

rtems_status_code bsp_start_timer(int timer, int period_usec) {
    double counts = (double)period_usec * timer_frequency / 1.0e6 + 0.5;
    if (counts < 1.0 || counts > (double)0x7fffffff) {
        return RTEMS_INVALID_NUMBER;
    }
    switch (timer) {
        case 0:
            st_le32(raven_base + 0x01110, (int)counts);
            break;
        case 1:
            st_le32(raven_base + 0x01150, (int)counts);
            break;
        case 2:
            st_le32(raven_base + 0x01190, (int)counts);
            break;
        case 3:
            st_le32(raven_base + 0x011d0, (int)counts);
            break;
        default:
            return RTEMS_INVALID_NUMBER;
    }
    return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_stop_timer(int timer) {
    switch (timer) {
        case 0:
            st_le32(raven_base + 0x01110, 0x80000000);
            break;
        case 1:
            st_le32(raven_base + 0x01150, 0x80000000);
            break;
        case 2:
            st_le32(raven_base + 0x01190, 0x80000000);
            break;
        case 3:
            st_le32(raven_base + 0x011d0, 0x80000000);
            break;
        default:
            return RTEMS_INVALID_NUMBER;
    }
    return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_read_timer(int timer, int *value) {
    switch (timer) {
        case 0:
            *value = ld_le32(raven_base + 0x01100) & 0x7fffffff;
            break;
        case 1:
            *value = ld_le32(raven_base + 0x01140) & 0x7fffffff;
            break;
        case 2:
            *value = ld_le32(raven_base + 0x01180) & 0x7fffffff;
            break;
        case 3:
            *value = ld_le32(raven_base + 0x011c0) & 0x7fffffff;
            break;
        default:
            return RTEMS_INVALID_NUMBER;
    }
    return RTEMS_SUCCESSFUL;
}

rtems_status_code generate_interprocessor_interrupt(int interrupt) {
    int *dispatch_reg = raven_base + 0x20040;

    if (interrupt & ~3) {
        return RTEMS_INVALID_NUMBER;
    }
    dispatch_reg[interrupt << 2] = 1;
    return RTEMS_SUCCESSFUL;
}

rtems_status_code set_interrupt_task_priority(int priority) {
    if (priority & ~15) {
        return RTEMS_INVALID_NUMBER;
    }
    st_le32(raven_base + 0x20080, priority);
    return RTEMS_SUCCESSFUL;
}

