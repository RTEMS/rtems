/*  main
 *
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
 *
 *  Based in part on OAR works.
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of Division Incorporated not be
 *      used in advertising or publicity pertaining to distribution
 *      of the software without specific, written prior permission.
 *      Division Incorporated makes no representations about the
 *      suitability of this software for any purpose.
 *
 *  $Id$
 */

#include <rtems.h>
#include <stdio.h>
#include <libcsupport.h>
#include <iostream.h>

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

    cout << "Testing a C++ I/O stream" << endl;

    bar = blech;
}

//
// main equivalent
//      It can not be called 'main' since the bsp owns that name
//      in many implementations in order to get global constructors
//      run.
//
//      Ref: c/src/lib/libbsp/hppa1_1/simhppa/startup/bspstart.c
//


rtems_task main_task(
  rtems_task_argument ignored
)
{
    printf( "\n\n*** CONSTRUCTOR/DESTRUCTOR TEST ***\n" );

    cdtest();

    printf( "*** END OF CONSTRUCTOR/DESTRUCTOR TEST ***\n\n\n" );

    exit(0);
}
