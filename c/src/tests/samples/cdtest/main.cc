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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef RTEMS_TEST_IO_STREAM
#include <iostream.h>
#endif

extern "C" {
extern rtems_task main_task(rtems_task_argument);
}

static int num_inst = 0;

class A {
public:
    A(void)
    {
        num_inst++;
        printf(
          "Hey I'm in base class constructor number %d for %p.\n",
          num_inst,
          this
        );

	/*
	 * Make sure we use some space
	 */

        string = new char[50];
	sprintf(string, "Instantiation order %d", num_inst);
    };

    virtual ~A()
    {
        printf(
          "Hey I'm in base class destructor number %d for %p.\n",
          num_inst,
          this
        );
	print();
        num_inst--;
    };

    virtual void print()  { printf("%s\n", string); };

protected:
    char  *string;
};

class B : public A {
public:
    B(void)
    {
        num_inst++;
        printf(
          "Hey I'm in derived class constructor number %d for %p.\n",
          num_inst,
          this
        );

	/*
	 * Make sure we use some space
	 */

        string = new char[50];
	sprintf(string, "Instantiation order %d", num_inst);
    };

    ~B()
    {
        printf(
          "Hey I'm in derived class destructor number %d for %p.\n",
          num_inst,
          this
        );
	      print();
        num_inst--;
    };

    void print()  { printf("Derived class - %s\n", string); }
};


A foo;
B foobar;

void
cdtest(void)
{
    A bar, blech, blah;
    B bleak;

#ifdef RTEMS_TEST_IO_STREAM
    cout << "Testing a C++ I/O stream" << endl;
#else
    printf("IO Stream not tested\n");
#endif

    bar = blech;
}

//
// main equivalent
//      It can not be called 'main' since the bsp owns that name
//      in many implementations in order to get global constructors
//      run.
//


rtems_task main_task(
  rtems_task_argument 
)
{
    printf( "\n\n*** CONSTRUCTOR/DESTRUCTOR TEST ***\n" );

    cdtest();

    printf( "*** END OF CONSTRUCTOR/DESTRUCTOR TEST ***\n\n\n" );

    exit(0);
}
