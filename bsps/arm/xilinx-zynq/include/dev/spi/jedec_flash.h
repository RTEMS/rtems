#if !defined(_ZQSPIFLASH_JEDEC_H_)
#define _ZQSPIFLASH_JEDEC_H_

#include <stdint.h>

typedef struct {
  uint32_t jedec_id;
  uint32_t sec_size;
  uint32_t page_size;
  uint32_t flash_size;
  uint32_t num_die;
  char label[16];
} flash_definition;

typedef flash_definition* flash_def;

flash_definition flash_dev_table[] = {
  {0x012018, 0x10000, 0x100, 0x1000000, 1, "S25FL128P_64K"},
  {0x010218, 0x400000, 0x200, 0x4000000, 1, "S25FL512P_256K"},
  {0x0, 0x0, 0x0, 0x0, 0, "end of table"}
};

#endif
