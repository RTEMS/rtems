/*
 *
 * Support for gnat/rtems interrupts and exception handling.
 * Jiri Gaisler, ESA/ESTEC, 17-02-1999.
 *
 */

void
__gnat_install_handler ()
{
  __gnat_install_handler_common (0x1d, 0x15);
}
