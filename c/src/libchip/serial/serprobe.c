#include <rtems.h>
#include <libchip/serial.h>
#include <libchip/sersupp.h>

bool libchip_serial_default_probe(int minor)
{
  /*
   * If the configuration dependent probe has located the device then
   * assume it is there
   */

  return true;
}
