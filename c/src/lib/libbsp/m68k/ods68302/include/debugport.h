/*****************************************************************************/
/*
  Debug Port Support

*/
/*****************************************************************************/

#if !defined(_DEBUGPORT_H_)
 #define _DEBUGPORT_H_

#if __cplusplus
extern "C"
{
#endif

  /* normall automatic, only need when re-initialising */
  void debug_port_initialise(void);

  unsigned char debug_port_status(const unsigned char status);
  unsigned char debug_port_in(void);
  void debug_port_out(const unsigned char character);

  void debug_port_write(const char *buffer);
  void debug_port_write_buffer(const char *buffer, unsigned int size);

  void debug_port_write_hex_uint(const unsigned int value);
  void debug_port_write_hex_ulong(const unsigned long value);

   /*
    * special banner message for CPU specific boot code,
    * initialises the debug port
    */
  void debug_port_banner(void);
  void debug_port_printf(const char *format, ...);

#if __cplusplus
}
#endif

#endif
