PWM
===

Pulse Width Modulation subsystem includes EPWM, ECAP , EQEP. There are
different instances available for each one. For PWM there are three
different individual EPWM module 0 , 1 and 2. So wherever pwmss word is
used that affects whole PWM sub system such as EPWM, ECAP and EQEP. This code
has only implementation Non high resolution PWM module. APIs for high
resolution PWM has been yet to develop.

For Each EPWM instance, has two PWM channels, e.g. EPWM0 has two channel
EPWM0A and EPWM0B. If you configure two PWM outputs(e.g. EPWM0A , EPWM0B)
in the same device, then they *must* be configured with the same frequency.
Changing frequency on one channel (e.g EPWMxA) will automatically change
frequency on another channel(e.g. EPWMxB). However, it is possible to set
different pulse-width/duty cycle to different channel at a time. So always
set the frequency first and then pulse-width/duty cycle.

For more you can refer :
http://www.ofitselfso.com/BBBCSIO/Source/PWMPortEnum.cs.html
 
Pulse Width Modulation uses the system frequency of Beagle Bone Black.

System frequency = SYSCLKOUT, that is, CPU clock. TBCLK = SYSCLKOUT(By Default)
SYCLKOUT = 100 MHz

Please visit following link to check why SYSCLKDIV = 100MHz:
https://groups.google.com/forum/#!topic/beagleboard/Ed2J9Txe_E4
(Refer Technical Reference Manual (TRM) Table 15-41 as well)

To generate different frequencies with the help of PWM module , SYSCLKOUT
need to be scaled down, which will act as TBCLK and TBCLK will be base clock
for the pwm subsystem.

```
TBCLK = SYSCLKOUT/(HSPCLKDIV * CLKDIV)

                 |----------------| 
                 |     clock      | 
 SYSCLKOUT--->   |                |---> TBCLK 
                 |   prescale     |
                 |----------------|         
                   ^           ^
                   |           |
 TBCTL[CLKDIV]-----            ------TBCTL[HSPCLKDIV]

```

CLKDIV and HSPCLKDIV bits are part of the TBCTL register (Refer TRM).
CLKDIV - These bits determine part of the time-base clock prescale value.
Please use the following values of CLKDIV to scale down sysclk respectively.

```
0h (R/W) = /1 
1h (R/W) = /2
2h (R/W) = /4
3h (R/W) = /8
4h (R/W) = /16
5h (R/W) = /32
6h (R/W) = /64
7h (R/W) = /128
```
These bits determine part of the time-base clock prescale value.
Please use following value of HSPCLKDIV to scale down sysclk respectively

```
0h (R/W) = /1
1h (R/W) = /2
2h (R/W) = /4
3h (R/W) = /6
4h (R/W) = /8
5h (R/W) = /10
6h (R/W) = /12
7h (R/W) = /14
```
For example, if you set CLKDIV = 3h and HSPCLKDIV= 2h Then
SYSCLKOUT will be divided by (1/8)(1/4). It means SYSCLKOUT/32

How to generate frequency ?

freq = 1/Period

TBPRD register is responsible to generate the frequency. These bits determine
the period of the time-base counter.

By default TBCLK = SYSCLKOUT = 100 MHz

Here by default period is 1/100MHz = 10 nsec

Following example shows value to be loaded into TBPRD
 
```
e.g. TBPRD = 1 = 1 count
  count x Period = 1 x 1ns = 1ns
  freq = 1/Period = 1 / 1ns = 100 MHz
```

For duty cycle CMPA and CMPB are the responsible registers.

To generate single with 50% Duty cycle & 100MHz freq.
 
```
 CMPA = count x Duty Cycle
       = TBPRD x Duty Cycle
       = 1 x 50/100
       = 0.2
```
The value in the active CMPA register is continuously compared to
the time-base counter (TBCNT). When the values are equal, the
counter-compare module generates a "time-base counter equal to
counter compare A" event. This event is sent to the action-qualifier
where it is qualified and converted it into one or more actions.
These actions can be applied to either the EPWMxA or the
EPWMxB output depending on the configuration of the AQCTLA and
AQCTLB registers.
 
List of pins for that can be used for different PWM instance :

  ------------------------------------------------ 
  |  EPWM2        | EPWM1         | EPWM0        |
  ------------------------------------------------
  |  BBB_P8_13_2B | BBB_P8_34_1B  | BBB_P9_21_0B |  
  |  BBB_P8_19_2A | BBB_P8_36_1A  | BBB_P9_22_0A |
  |  BBB_P8_45_2A | BBB_P9_14_1A  | BBB_P9_29_0B |
  |  BBB_P8_46_2B | BBB_P9_16_1B  | BBB_P9_31_0A |
  ------------------------------------------------

BBB_P8_13_2B represents P8 Header , pin number 13 , 2nd PWM instance and B channel. 

Following sample program can be used to generate 7 Hz frequency.

```c
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/test.h>
#include <bsp.h>
#include <bsp/gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <bsp/bbb-pwm.h>

const char rtems_test_name[] = "Testing PWM driver";
rtems_printer rtems_test_printer;

static void inline delay_sec(int sec)
{
  rtems_task_wake_after(sec*rtems_clock_get_ticks_per_second());
}  

rtems_task Init(rtems_task_argument argument);

rtems_task Init(
	rtems_task_argument ignored
)
{
  rtems_test_begin(); 
  printf("Starting PWM Testing");
  
  /*Initialize GPIO pins in BBB*/
  rtems_gpio_initialize();
  
  /* Set P9 Header , 21 Pin number , PWM B channel and 0 PWM instance to generate frequency*/ 
  beagle_epwm_pinmux_setup(BBB_P9_21_0B,BBB_PWMSS0);
  
/** Initialize clock for PWM sub system
  * Turn on time base clock for PWM o instance
  */
  beagle_pwm_init(BBB_PWMSS0);

  float PWM_HZ = 7.0f ;           /* 7 Hz */
  float duty_A = 20.0f ;          /* 20% Duty cycle for PWM 0_A output */
  const float duty_B = 50.0f ;    /* 50% Duty cycle for PWM 0_B output*/
  
  /*Note: Always check whether pwmss clocks are enabled or not before configuring PWM*/
  bool is_running = beagle_pwmss_is_running(BBB_PWMSS2);
  
  if(is_running) {
  
  /*To analyse the two different duty cycle Output should be observed at P8_45 and P8_46 pin number */
  beagle_pwm_configure(BBB_PWMSS0, PWM_HZ ,duty_A , duty_B);
  printf("PWM  enable for 10s ....\n");
 
  /*Set Up counter and enable pwm module */
  beagle_pwm_enable(BBB_PWMSS0);
  delay_sec(10);
  
  /*freeze the counter and disable pwm module*/
  beagle_epwm_disable(BBB_PWMSS0);
  }
}

/* NOTICE: the clock driver is enabled */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

#define CONFIGURE_MAXIMUM_TASKS            1
#define CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM

#define CONFIGURE_MAXIMUM_SEMAPHORES    1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE 

#define CONFIGURE_EXTRA_TASK_STACKS         (2 * RTEMS_MINIMUM_STACK_SIZE)

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
```
