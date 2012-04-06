#include <rtems.h>
#include <bsp.h>
#include <bsp/irq-generic.h>

static inline void bsp_dispatch_irq(int irq)
{
       bsp_interrupt_handler_entry *e =
               &bsp_interrupt_handler_table[bsp_interrupt_handler_index(irq)];

       while (e != NULL) {
               (*e->handler)(e->arg);
               e = e->next;
       }
}

/* Called directly from IRQ trap handler TRAP[0x10..0x1F] = IRQ[0..15] */
static void BSP_ISR_handler(rtems_vector_number vector)
{
       int irq = vector - 0x10;

       /* Let BSP fixup and/or handle incomming IRQ */
       irq = bsp_irq_fixup(irq);

       bsp_dispatch_irq(irq);
}

/* Initialize interrupts */
int BSP_shared_interrupt_init(void)
{
       rtems_vector_number vector;
       rtems_isr_entry previous_isr;
       int sc, i;

       for (i=0; i <= BSP_INTERRUPT_VECTOR_MAX_STD; i++) {
               vector = SPARC_ASYNCHRONOUS_TRAP(i) + 0x10;
               rtems_interrupt_catch(BSP_ISR_handler, vector, &previous_isr);
       }

       /* Initalize interrupt support */
       sc = bsp_interrupt_initialize();
       if (sc != RTEMS_SUCCESSFUL)
               return -1;

       return 0;
}

/* Callback from bsp_interrupt_initialize() */
rtems_status_code bsp_interrupt_facility_initialize(void)
{
       return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable(rtems_vector_number vector)
{
       BSP_Unmask_interrupt((int)vector);

       return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable(rtems_vector_number vector)
{
       BSP_Mask_interrupt((int)vector);

       return RTEMS_SUCCESSFUL;
}

void BSP_shared_interrupt_mask(int irq)
{
       BSP_Mask_interrupt(irq);
}

void BSP_shared_interrupt_unmask(int irq)
{
       BSP_Unmask_interrupt(irq);
}

void BSP_shared_interrupt_clear(int irq)
{
       /* We don't have to interrupt lock here, because the register is only
        * written and self clearing
        */
       BSP_Clear_interrupt(irq);
}
