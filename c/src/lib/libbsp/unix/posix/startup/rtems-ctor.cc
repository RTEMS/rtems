/*
 *  rtems-ctor.cc
 *
 *  Description:
 *  This file exists solely to (try to) ensure RTEMS is initialized
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
 *          init_executive_late()
 *          bsp_cleanup()
 *
 *  TODO:
 *
 *  $Id$
 */

#include <bsp.h>
#include <stdio.h>
#include <stdlib.h>

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

extern "C" {
    #ifndef __GNUC__
      extern void invoke_non_gnu_constructors(void);
    #endif

    int
    main(int argc,
         char **argv,
         char ** /* environp is unused */)
    {
        rtems_argc = argc;
        rtems_argv = argv;

        rtems_interrupt_level bsp_isr_level;

        /*
         *  Make sure interrupts are disabled.
         */

        rtems_interrupt_disable( bsp_isr_level );

        if ((argc > 0) && argv && argv[0])
            rtems_progname = argv[0];
        else
            rtems_progname = "RTEMS";

        /*
         * run any non-gnu constructors we may need
         */
        
        #ifndef __GNUC__
          invoke_non_gnu_constructors();
	#endif

        rtems_initialize_executive_early( &Configuration );

        /*
         *  Start multitasking
         */

        rtems_initialize_executive_late( bsp_isr_level );

        /*
         * Returns when multitasking is stopped
         * This allows our destructors to get run normally
         */

        fflush( stdout );
        fflush( stderr );
        return 0;
    }
}
