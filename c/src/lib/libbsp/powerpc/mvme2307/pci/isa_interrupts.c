#include <bsp.h>

#define PCI_Interrupt_Ack (*(volatile unsigned32 *) 0xFEFF0030)
#define PIC1_OCW2 (*(volatile char *)IO_TO_LOCAL(0x0020))
#define PIC2_OCW2 (*(volatile char *)IO_TO_LOCAL(0x00A0))
#define NonspecificEOI 0x20

#define Master_PIC_Mask (*(unsigned8 *) IO_TO_LOCAL(0x0021))
#define Slave_PIC_Mask  (*(unsigned8 *) IO_TO_LOCAL(0x00A1))

rtems_isr_entry isa_handlers[16] = {0, 0, 0, 0, 0, 0, 0, 0, 
                                    0, 0, 0, 0, 0, 0, 0, 0};

rtems_status_code isa_interrupt_enable(rtems_vector_number vector) {
    if (vector < 0 || vector > 15 || vector == 2) {
        return RTEMS_INVALID_NUMBER;
    }
    if (vector < 8) {
        Master_PIC_Mask &= ~ (1 << vector);
    } else {
        Slave_PIC_Mask &= ~ (1 << (vector - 8));
    }
    return RTEMS_SUCCESSFUL;
}

rtems_status_code isa_interrupt_disable(rtems_vector_number vector) {
    if (vector < 0 || vector > 15 || vector == 2) {
        return RTEMS_INVALID_NUMBER;
    }
    if (vector < 8) {
        Master_PIC_Mask |= (1 << vector);
    } else {
        Slave_PIC_Mask |= (1 << (vector - 8));
    }
    return RTEMS_SUCCESSFUL;
}

rtems_status_code isa_interrupt_catch(rtems_isr_entry      new_isr_handler,
                                      rtems_vector_number  vector,
                                      rtems_isr_entry     *old_isr_handler) {
    if (vector < 0 || vector > 15 || vector == 2) {
        return RTEMS_INVALID_NUMBER;
    }
    if (!old_isr_handler || ((int)new_isr_handler & 3) != 0) {
        return RTEMS_INVALID_ADDRESS;
    }
    *old_isr_handler = isa_handlers[vector];
    isa_handlers[vector] = new_isr_handler;
    return RTEMS_SUCCESSFUL;
}

rtems_isr isa_interrupt_handler(rtems_vector_number vector) {
    unsigned32          piack_image = PCI_Interrupt_Ack;
    int                 pic_id      = piack_image >> 27;
    rtems_vector_number isa_vector  = (piack_image >> 24) & 7;

    if (pic_id == 0x10) {
        isa_vector += 8;
    } else if (pic_id != 0x01) {
        printk("unrecognized PIACK value: %08x\n", piack_image);
        return;
    }
    if (isa_handlers[isa_vector] != 0) {
        isa_handlers[isa_vector](isa_vector);
    }
    PIC1_OCW2 = NonspecificEOI;
    if (isa_vector >= 8) {
        PIC2_OCW2 = NonspecificEOI;
    }
}
