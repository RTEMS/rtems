/*****************************************************************************/
/*
  Hooks for GDB

 */
/*****************************************************************************/

#include <bsp.h>
#include <rtems/m68k/m68302.h>
#include <m68302scc.h>

static int initialised = 0;

void putDebugChar(char ch)
{
  if (!initialised)
  {
    scc_initialise(DEBUG_PORT, DEBUG_BAUD, 0);
    initialised = 1;
  }

  scc_out(DEBUG_PORT, ch);
}

char getDebugChar(void)
{
  if (!initialised)
  {
    scc_initialise(DEBUG_PORT, DEBUG_BAUD, 0);
    initialised = 1;
  }

  while (!scc_status(DEBUG_PORT, 0));

  return scc_in(DEBUG_PORT);
}

/*
 * Need to create yet another jump table for gdb this time
 */

void (*exceptionHook)(unsigned int) = 0;

typedef struct {
  uint16_t         move_a7;            /* move #FORMAT_ID,%a7@- */
  uint16_t         format_id;
  uint16_t         jmp;                /* jmp  _ISR_Handlers */
  uint32_t         isr_handler;
} GDB_HANDLER_ENTRY;

#if !defined(M68K_MOVE_A7)
#define M68K_MOVE_A7 0x3F3C
#endif

#if !defined(M68K_JMP)
#define M68K_JMP     0x4EF9
#endif

/* points to jsr-exception-table in targets wo/ VBR register */
static GDB_HANDLER_ENTRY gdb_jump_table[256];

void exceptionHandler(unsigned int vector, void *handler)
{
  uint32_t         *interrupt_table = 0;

  gdb_jump_table[vector].move_a7 = M68K_MOVE_A7;
  gdb_jump_table[vector].format_id = vector;
  gdb_jump_table[vector].jmp = M68K_JMP;
  gdb_jump_table[vector].isr_handler = (uint32_t) handler;

  interrupt_table[vector] = (uint32_t) &gdb_jump_table[vector];
}
