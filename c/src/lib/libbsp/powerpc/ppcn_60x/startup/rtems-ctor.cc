/*
 *  rtems-ctor.cc
 *
 *  Description:
 *	This file exists solely to (try to) ensure RTEMS is initialized
 *      before any global constructors are run.
 *
 *  The problem:
 *      Global constructors might reasonably expect that new() will
 *      work, but since new() uses malloc() which uses RTEMS regions,
 *      it can not be called until after initialize_executive().
 *
 *      Global constructors are called in GNU systems one of 2 ways:
 *
 *              an "invisible" call to __main() inserted by compiler
 *              This __main() calls __do_global_ctors() which
 *              walks thru the table and calls all global
 *              constructors.
 *
 *       or -
 *              A special section is put into the linked binary.  The
 *              system startup code knows to run the constructors in
 *              this special section before calling main().
 *
 *      By making RTEMS initialization a constructor, we avoid having
 *      too much about all this.  All we have to guarantee is that
 *      this constructor is the first one run.
 *
 *
 *  So for the first case above, this is what happens
 *
 *    host crt0
 *      main()
 *          __main()
 *              __do_global_ctors()
 *                  bsp_start()
 *                      init_executive_early()
 *                  <<any other constructors>>
 *
 *          rtems_init_executive_late()
 *          bsp_cleanup()
 *
 *  TODO:
 *
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: 
 */

#include <bsp.h>

/*
 * RTEMS program name
 * Probably not used by anyone, but it is nice to have it.
 * Actually the UNIX version of CPU_INVOKE_DEBUGGER will probably
 * need to use it
 */

char *rtems_progname;
char **rtems_environp;

#ifdef USE_CONSTRUCTORS_FOR_INIT_EXEC

class RTEMS {
    public:
	RTEMS();
        ~RTEMS();
};

RTEMS  rtems_constructor;

RTEMS::RTEMS()
{
    bsp_start();
}

RTEMS::~RTEMS()
{
    bsp_cleanup();
}
#endif

extern "C" {
    int
    main(int argc,
         char **argv,
         char **environp)
    {

#ifndef USE_CONSTRUCTORS_FOR_INIT_EXEC
        bsp_start();
#endif

        if ((argc > 0) && argv && argv[0])
            rtems_progname = argv[0];
        else
            rtems_progname = "RTEMS";

        rtems_environp = environp;

        /*
         *  Start multitasking
         */
        rtems_initialize_executive_late( bsp_isr_level );

#ifndef USE_CONSTRUCTORS_FOR_INIT_EXEC
        bsp_cleanup();
#endif

        /*
         * Returns when multitasking is stopped
         * This allows our destructors to get run normally
         */

        return 0;
    }
}
