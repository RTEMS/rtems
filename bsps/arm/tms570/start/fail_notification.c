#include <stdint.h>
#include <stdbool.h>
#include <bsp/tms570.h>
#include <bsp/tms570_selftest.h>

void bsp_selftest_fail_notification( uint32_t flag )
{
}

void tms570_memory_port0_fail_notification(
  uint32_t groupSelect,
  uint32_t dataSelect,
  uint32_t address,
  uint32_t data
)
{
}
