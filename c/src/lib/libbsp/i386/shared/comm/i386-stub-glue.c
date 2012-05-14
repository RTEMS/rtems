/*
 * This software is Copyright (C) 1998 by T.sqware - all rights limited
 * It is provided in to the public domain "as is", can be freely modified
 * as far as this copyight notice is kept unchanged, but does not imply
 * an endorsement by T.sqware of the product in which it is included.
 */

#include <rtems/system.h>
#include <rtems/score/cpu.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <uart.h>
#include <assert.h>

int  putDebugChar(int ch);     /* write a single character      */
int  getDebugChar(void);       /* read and return a single char */

/* assign an exception handler */
void exceptionHandler(int, void (*handler)(void));

static void
nop(const rtems_raw_irq_connect_data* notused)
{
}

static int
isOn(const rtems_raw_irq_connect_data* notused)
{
  return 1;
}

void BSP_loop(int uart);

/* Current uart used by gdb stub */
static int uart_current = 0;

/*
 * Initialize glue code linking i386-stub with the rest of
 * the system
 */
void
i386_stub_glue_init(int uart)
{
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

  uart_current = uart;

  /* BSP_uart_init(uart, 38400, CHR_8_BITS, 0, 0, 0);*/
  BSP_uart_init(uart, 115200, CHR_8_BITS, 0, 0, 0);
}

void BSP_uart_on(const rtems_raw_irq_connect_data* used)
{
  BSP_irq_enable_at_i8259s(used->idtIndex - BSP_IRQ_VECTOR_BASE);
}

void BSP_uart_off(const rtems_raw_irq_connect_data* used)
{
  BSP_irq_disable_at_i8259s(used->idtIndex - BSP_IRQ_VECTOR_BASE);
}

int BSP_uart_isOn(const rtems_raw_irq_connect_data* used)
{
  return BSP_irq_enabled_at_i8259s(used->idtIndex - BSP_IRQ_VECTOR_BASE);
}

/*
 * In order to have a possibility to break into
 * running program, one has to call this function
 */
void i386_stub_glue_init_breakin(void)
{
  rtems_raw_irq_connect_data uart_raw_irq_data;

  assert(uart_current == BSP_UART_COM1 || uart_current == BSP_UART_COM2);

  if(uart_current == BSP_UART_COM1)
    {
      uart_raw_irq_data.idtIndex = BSP_UART_COM1_IRQ + BSP_IRQ_VECTOR_BASE;
    }
  else
    {
      uart_raw_irq_data.idtIndex = BSP_UART_COM2_IRQ + BSP_IRQ_VECTOR_BASE;
    }

  if(!i386_get_current_idt_entry(&uart_raw_irq_data))
    {
      printk("cannot get idt entry\n");
      rtems_fatal_error_occurred(1);
    }

  if(!i386_delete_idt_entry(&uart_raw_irq_data))
    {
      printk("cannot delete idt entry\n");
      rtems_fatal_error_occurred(1);
    }

  uart_raw_irq_data.on  = BSP_uart_on;
  uart_raw_irq_data.off = BSP_uart_off;
  uart_raw_irq_data.isOn= BSP_uart_isOn;

  /* Install ISR  */
  if(uart_current == BSP_UART_COM1)
    {
      uart_raw_irq_data.idtIndex = BSP_UART_COM1_IRQ + BSP_IRQ_VECTOR_BASE;
      uart_raw_irq_data.hdl = BSP_uart_dbgisr_com1;
    }
  else
    {
      uart_raw_irq_data.idtIndex = BSP_UART_COM2_IRQ + BSP_IRQ_VECTOR_BASE;
      uart_raw_irq_data.hdl = BSP_uart_dbgisr_com2;
    }

  if (!i386_set_idt_entry (&uart_raw_irq_data))
    {
      printk("raw exception handler connection failed\n");
      rtems_fatal_error_occurred(1);
    }

  /* Enable interrupts */
  BSP_uart_intr_ctrl(uart_current, BSP_UART_INTR_CTRL_GDB);

  return;
}

int
putDebugChar(int ch)
{
  assert(uart_current == BSP_UART_COM1 || uart_current == BSP_UART_COM2);

  BSP_uart_polled_write(uart_current, ch);

  return 1;
}

int getDebugChar(void)
{
  int val;

  assert(uart_current == BSP_UART_COM1 || uart_current == BSP_UART_COM2);

  val = BSP_uart_polled_read(uart_current);

  return val;
}

void exceptionHandler(int vector, void (*handler)(void))
{
  rtems_raw_irq_connect_data excep_raw_irq_data;

  excep_raw_irq_data.idtIndex = vector;

  if(!i386_get_current_idt_entry(&excep_raw_irq_data))
    {
      printk("cannot get idt entry\n");
      rtems_fatal_error_occurred(1);
    }

  if(!i386_delete_idt_entry(&excep_raw_irq_data))
    {
      printk("cannot delete idt entry\n");
      rtems_fatal_error_occurred(1);
    }

  excep_raw_irq_data.on = nop;
  excep_raw_irq_data.off = nop;
  excep_raw_irq_data.isOn = isOn;
  excep_raw_irq_data.hdl = handler;

  if (!i386_set_idt_entry (&excep_raw_irq_data)) {
      printk("raw exception handler connection failed\n");
      rtems_fatal_error_occurred(1);
    }
  return;
}
