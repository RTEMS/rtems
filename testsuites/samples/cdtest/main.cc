/*
 *  This routine is the initialization task for this test program.
 *  It is called from init_exec and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *  Based in part on OAR works.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *
 *  by Rosimildo da Silva:
 *  Modified the test a bit to indicate when an instance is
 *  global or not, and added code to test C++ exception.
 */

#include <rtems.h>
#include <rtems/bspIo.h>
#include <rtems/test.h>

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#ifdef RTEMS_TEST_IO_STREAM
#include <iostream>
#endif

const char rtems_test_name[] = "CONSTRUCTOR/DESTRUCTOR";

extern "C"
{
#include <tmacros.h>
extern rtems_task main_task(rtems_task_argument);
}

static int num_inst = 0;

static void check_begin_of_test(void)
{
  if ( num_inst == 0 ) {
    TEST_BEGIN();
  }
}

static void check_end_of_test(void)
{
  if ( num_inst == 0 ) {
    TEST_END();
  }
}

class AClass {
public:
  AClass(const char *p = "LOCAL" ) : ptr( p )
    {
        check_begin_of_test();
        num_inst++;
        printf(
          "%s: Hey I'm in base class constructor number %d for %p.\n",
          p, num_inst, this
        );

	/*
	 * Make sure we use some space
	 */

        string = new char[50];
	sprintf(string, "Instantiation order %d", num_inst);
    };

    virtual ~AClass()
    {
	// MUST USE PRINTK -- RTEMS IS SHUTTING DOWN WHEN THIS RUNS
        printk(
          "%s: Hey I'm in base class destructor number %d for %p.\n",
          ptr, num_inst, this
        );
        printk("Derived class - %s\n", string);
        num_inst--;
        check_end_of_test();
    };

#if __cplusplus >= 201103L
    AClass& operator=(const AClass&) = default;
#endif

    virtual void print()  { printf("%s\n", string); };

protected:
    char  *string;
    const char *ptr;
};

class BClass : public AClass {
public:
  BClass(const char *p = "LOCAL" ) : AClass( p )
    {
        check_begin_of_test();
        num_inst++;
        printf(
          "%s: Hey I'm in derived class constructor number %d for %p.\n",
          p, num_inst,  this
        );

	/*
	 * Make sure we use some space
	 */

        string = new char[50];
	sprintf(string, "Instantiation order %d", num_inst);
    };

    ~BClass()
    {
        printk(
          "%s: Hey I'm in derived class destructor number %d for %p.\n",
          ptr, num_inst,
          this
        );
        printk("Derived class - %s\n", string);
        num_inst--;
        check_end_of_test();
    };

    void print()  { printf("Derived class - %s\n", string); }
};


class RtemsException
{
public:

    RtemsException( const char *module, int ln, int err = 0 )
    : error( err ), line( ln ), file( module )
    {
      printf( "RtemsException raised=File:%s, Line:%d, Error=%X\n",
               file, line, error );
    }

    void show()
    {
      printf( "RtemsException ---> File:%s, Line:%d, Error=%X\n",
               file, line, error );
    }

private:
   int  error;
   int  line;
   const char *file;

};



AClass foo( "GLOBAL" );
BClass foobar( "GLOBAL" );

void
cdtest(void)
{
    AClass bar, blech, blah;
    BClass bleak;

#ifdef RTEMS_TEST_IO_STREAM
    std::cout << "Testing a C++ I/O stream" << std::endl;
#else
    printf("IO Stream not tested\n");
#endif
    bar = blech;
    rtems_task_wake_after( 5 * rtems_clock_get_ticks_per_second() );
}

//
// main equivalent
//      It can not be called 'main' since the bsp owns that name
//      in many implementations in order to get global constructors
//      run.
//

static void foo_function()
{
    try
    {
      throw "foo_function() throw this exception";
    }
    catch( const char *e )
    {
     printf( "foo_function() catch block called:\n   < %s  >\n", e );
     throw "foo_function() re-throwing execption...";
    }
}

rtems_task main_task(
  rtems_task_argument
)
{
    cdtest();

    printf( "*** TESTING C++ EXCEPTIONS ***\n\n" );

    try
    {
      foo_function();
    }
    catch( const char *e )
    {
       printf( "Success catching a char * exception\n%s\n", e );
    }

    try
    {
      throw std::runtime_error("thrown std::runtime object");
    }
    catch (std::exception const& e)
    {
       printf("throw std::runtime: caught: %s\n", e.what());
    }

    try
    {
      printf( "throw an instance based exception\n" );
		throw RtemsException( __FILE__, __LINE__, 0x55 );
    }
    catch( RtemsException & ex )
    {
       printf( "Success catching RtemsException...\n" );
       ex.show();
    }
    catch(...)
    {
      printf( "Caught another exception.\n" );
    }
    printf( "Exceptions are working properly.\n" );
    rtems_task_wake_after( 5 * rtems_clock_get_ticks_per_second() );
    printf( "Global Dtors should be called after this line....\n" );
    exit(0);
}
