/*
 *  Default implementation of __gnat_install_handler to satisfy
 *  reference in a-init.c in GNAT's run-time.  Each BSP really
 *  should provide its own version of this routine but this
 *  version lets programs link.
 *
 *  This routine is responsible for installing fault/exception/trap
 *  handlers that map them onto POSIX signals so they can be
 *  propagated to the GNAT run-time.  See the sparc/erc32 BSP
 *  for an example.
 */

/*
void __bsp_gnat_install_handler( void )
{
}
*/
