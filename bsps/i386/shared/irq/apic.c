#include <bsp/apic.h>
#include <bsp.h>

volatile uint32_t *i386_lapic_base; /* Utilized in assembly */

#ifdef RTEMS_SMP
/* Maps the processor index to the Local APIC ID */
uint8_t i386_lapic_to_cpu_map[APIC_BCAST_ID]; /* utilized in assembly */
static uint8_t i386_cpu_to_lapic_map[APIC_BCAST_ID];
static uint32_t lapic_count = 0;

static void send_ipi(uint8_t dest_id, uint32_t icr_low)
{
  i386_lapic_base[LAPIC_REGISTER_ICR_HIGH] = dest_id << 24;
  i386_lapic_base[LAPIC_REGISTER_ICR_LOW] = icr_low;
}

static void UDELAY(int x)
{
  int _i = x;

  while ( _i-- ) {
    _pc386_delay();
  }
}

static void wait_ipi(void)
{
  int to;
  int send_status;

  to = 0;
  do {
    UDELAY(100);
    send_status = i386_lapic_base[LAPIC_REGISTER_ICR_LOW] & LAPIC_ICR_STATUS_PEND;
  } while (send_status && (to++ < 1000));
}
#endif

void lapic_enable(void)
{
  /* Enable the Local APIC by setting the spurious interrupt vector register */
  uint32_t value = i386_lapic_base[LAPIC_REGISTER_SPURIOUS];
  value |= LAPIC_SPURIOUS_ENABLE;
  i386_lapic_base[LAPIC_REGISTER_SPURIOUS] = value;
}

uint8_t lapic_get_id(void)
{
  return i386_lapic_base[LAPIC_REGISTER_ID] >> 24;
}

void lapic_send_eoi(void)
{
  i386_lapic_base[LAPIC_REGISTER_EOI] = LAPIC_EOI_ACK;
}

void lapic_clear_errors(void)
{
  i386_lapic_base[LAPIC_REGISTER_ESR] = 0;
}

bool lapic_initialize(uint32_t lapic_addr)
{
  /* Set the LAPIC base pointer */
  i386_lapic_base = (volatile uint32_t *) lapic_addr;

  /* Enable the Local APIC */
  lapic_enable();

#ifdef RTEMS_SMP
  /* Register BSP in cpu/apic maps */
  uint8_t apicid = lapic_get_id();
  lapic_set_cpu_index(apicid, lapic_count);
  lapic_set_lapic_id(lapic_count++, apicid);
#endif

  return true;
}

#ifdef RTEMS_SMP

uint8_t lapic_get_cpu_index(uint8_t lapic_id)
{
  return i386_lapic_to_cpu_map[lapic_id];
}

uint8_t lapic_get_lapic_id(uint32_t cpu_index)
{
  return i386_cpu_to_lapic_map[cpu_index];
}

void lapic_set_cpu_index(uint8_t lapic_id, uint32_t cpu_index)
{
  i386_lapic_to_cpu_map[lapic_id] = (uint8_t) cpu_index;
}

void lapic_set_lapic_id(uint32_t cpu_index, uint8_t lapic_id)
{
  i386_cpu_to_lapic_map[cpu_index] = lapic_id;
}

void lapic_send_ipi(uint32_t target_cpu_index, uint8_t isr_vector)
{
  uint8_t target_lapic_id = lapic_get_lapic_id(target_cpu_index);
  send_ipi(target_lapic_id, isr_vector);
  wait_ipi();
}

void lapic_reset_and_start_ap(uint32_t cpu_index, uint8_t page_vector)
{
  uint8_t apicid = lapic_get_lapic_id(cpu_index);

  /* assert INIT IPI */
  send_ipi(
    apicid,
    LAPIC_ICR_TM_LEVEL | LAPIC_ICR_LEVELASSERT | LAPIC_ICR_DM_INIT
  );
  UDELAY(10000);

  /* Deassert INIT IPI */
  send_ipi(
    apicid,
    LAPIC_ICR_TM_LEVEL | LAPIC_ICR_DM_INIT
  );
  UDELAY(10000);

  /*
   *  Send Startup IPIs.
   */
  int i;
  for (i = 1; i <= 2; i++) {
    send_ipi(apicid, LAPIC_ICR_DM_SIPI | page_vector);
    wait_ipi();
    UDELAY(1000);
  }
}

void lapic_reset_ap(uint32_t cpu_index)
{
  uint8_t apicid = lapic_get_lapic_id(cpu_index);

  /* assert INIT IPI */
  send_ipi(
    apicid,
    LAPIC_ICR_TM_LEVEL | LAPIC_ICR_LEVELASSERT | LAPIC_ICR_DM_INIT
  );
  UDELAY(10000);

  /* Deassert INIT IPI */
  send_ipi(
    apicid,
    LAPIC_ICR_TM_LEVEL | LAPIC_ICR_DM_INIT
  );
  UDELAY(10000);
}
#endif
