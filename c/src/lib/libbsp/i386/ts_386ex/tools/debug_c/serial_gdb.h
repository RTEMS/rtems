/*  serial_gdb.h
 *
 *  Interface to invoke and initialize GDB support.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

void init_serial_gdb( void );

#define breakpoint() __asm__ ("int $3")

#ifdef __cplusplus
}
#endif
