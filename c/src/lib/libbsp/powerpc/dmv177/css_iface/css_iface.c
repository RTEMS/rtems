#include <Css.h>

rtems_unsigned32 Css_Id(
  rtems_vector_number vector        /* vector number      */
)
{
  rtems_unsigned32 id;

  switch ( vector ) {
    case DMV170_DUART_IRQ:
    case DMV170_ETHERNET_IRQ:
    case DMV170_SCSI_IRQ:
    case DMV170_SCC_IRQ:
      id = CSS_DARF_INT;
      break;

    case DMV170_MEZZANINE_IRQ:
      id = CSS_MAXPACK_INT;
      break;

    case DMV170_TICK_IRQ:
      id = CSS_TICK_INT;
      break;

    case DMV170_LOCATION_MON_IRQ:
      id = CSS_LM_INT;
      break;

    case DMV170_SCV64_IRQ:
      id = CSS_SCV_VME_INT;
      break;

    case DMV170_RTC_IRQ:
      id = CSS_RTC_INT;
      break;
  }

  return id;
}

rtems_vector_number Vector_id(
  rtems_unsigned32 id
)
{
  rtems_vector_number vector;

  switch ( id ) {
    case CSS_ACFAIL_INT:
      vector = DMV170_ACFAIL_IRQ;
      break;
    case CSS_SYSFAIL_INT:
      vector = DMV170_SYSFAIL_IRQ;
      break;
    case CSS_WATCHDOG_INT:
      vector = DMV170_WATCHDOG_IRQ;
      break;
    case CSS_BI_INT:
      vector = DMV170_BI_IRQ;
      break;
    case CSS_LM_INT:
      vector = DMV170_LOCATION_MON_IRQ;
      break;
    case CSS_TICK_INT:
      vector = DMV170_TICK_IRQ;
      break;
    case CSS_DARF_INT:
      vector = DMV170_DUART_IRQ;
      break;
    case CSS_RAM_PARITY_INT:
      vector = DMV170_RAM_PARITY_IRQ;
      break;
    case CSS_DARF_BUS_ERROR:
      vector = DMV170_DARF_BUS_ERROR_IRQ;
      break;
    case CSS_PERIPHERAL_INT:
      vector = DMV170_PERIPHERAL_IRQ;
      break;
    case CSS_MAXPACK_INT:
      vector = DMV170_MEZZANINE_IRQ_0;assert(0);
      break;
    case CSS_SCV_VME_INT:
      vector = DMV170_MEZZANINE_IRQ_1;assert(0);
      break;
    case CSS_RTC_INT:
      vector = DMV170_RTC_IRQ;
      break;
  }

  return vector;
}

void enable_card_interrupt( 
  rtems_vector_number vector        /* vector number      */
)
{
  rtems_unsigned32 Id;
  Id = Css_Id(vector);

  Enable_int(Id);
}

rtems_vector_number Get_interrupt()
{
  rtems_vector_number vector;
  rtems_unsigned32    id;

  if ( Get_int_status(CSS_ACFAIL_INT) )
    vector = DMV170_ACFAIL_IRQ;
  else if ( Get_int_status( CSS_SYSFAIL_INT) )
    vector = DMV170_SYSFAIL_IRQ;
  else if ( Get_int_status( CSS_WATCHDOG_INT) )
    vector = DMV170_WATCHDOG_IRQ;
  else if ( Get_int_status( CSS_BI_INT) )
    vector = DMV170_BI_IRQ;
  else if ( Get_int_status( CSS_LM_INT) )
    vector = DMV170_LOCATION_MON_IRQ;
  else if ( Get_int_status( CSS_TICK_INT) )
    vector = DMV170_TICK_IRQ;
  else if ( Get_int_status( CSS_DARF_INT) )
    vector = DMV170_DUART_IRQ;
  else if ( Get_int_status( CSS_RAM_PARITY_INT) )
    vector = DMV170_RAM_PARITY_IRQ;
  else if ( Get_int_status( CSS_DARF_BUS_ERROR) )
    vector = DMV170_DARF_BUS_ERROR_IRQ;
  else if ( Get_int_status( CSS_PERIPHERAL_INT) )
    vector = DMV170_PERIPHERAL_IRQ;
  else if ( Get_int_status( CSS_MAXPACK_INT) ) {
    vector = DMV170_MEZZANINE_IRQ_0;
    assert(0);
  }
  else if ( Get_int_status( CSS_SCV_VME_INT) ) {
    vector = DMV170_MEZZANINE_IRQ_1;
    assert(0);
  }
  else if ( Get_int_status( CSS_RTC_INT) )
    vector = DMV170_RTC_IRQ;

  return vector;
}

void Clear_interrupt( 
  rtems_vector_number vector
)
{
 rtems_unsigned32 Id;
  Id = Css_Id(vector);

  Clear_int(Id);
}
