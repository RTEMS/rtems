/*  serial_gdb.h
 *
 *  Interface to invoke and initialize GDB support.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

void init_serial_gdb( void );

#define breakpoint() asm("int $3")

#ifdef __cplusplus
}
#endif
