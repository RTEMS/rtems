@c
@c  COPYRIGHT (c) 1988-2002.
@c  On-Line Applications Research Corporation (OAR).
@c  All rights reserved.
@c
@c  $Id$
@c

@chapter Real-Time Clock Driver

@section Introduction

The Real-Time Clock (@b{RTC}) driver is responsible for providing an
interface to an @b{RTC} device.  [NOTE: In this chapter, the abbreviation
@b{TOD} is used for @b{Time of Day}.]  The capabilities provided by this
driver are:

@itemize @bullet
@item Set the RTC TOD to RTEMS TOD
@item Set the RTEMS TOD to the RTC TOD
@item Get the RTC TOD
@item Set the RTC TOD to the Specified TOD
@item Get the Difference Between the RTEMS and RTC TOD
@end itemize

The reference implementation for a real-time clock driver can 
be found in @code{c/src/lib/libbsp/shared/tod.c}.  This driver
is based on the libchip concept and can be easily configured 
to work with any of the RTC chips supported by the RTC
chip drivers in the directory @code{c/src/lib/lib/libchip/rtc}.
There is a README file in this directory for each supported
RTC chip.  Each of these README explains how to configure the
shared libchip implementation of the RTC driver for that particular
RTC chip.

The DY-4 DMV177 BSP uses the shared libchip implementation of the RTC
driver.  Its @code{RTC_Table} configuration table can be found in
@code{c/src/lib/libbsp/powerpc/dmv177/tod/config.c}.

@section Initialization

The @code{rtc_initialize} routine is responsible for initializing the
RTC chip so it can be used.  The shared libchip implementation of this
driver supports multiple RTCs and bases its initialization order on
the order the chips are defined in the @code{RTC_Table}.  Each chip
defined in the table may or may not be present on this particular board.
It is the responsibility of the @code{deviceProbe} to indicate the
presence of a particular RTC chip.  The first RTC found to be present
is considered the preferred RTC.

In the shared libchip based implementation
of the driver, the following actions are performed:

@example
@group
rtems_device_driver rtc_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor_arg,
  void                      *arg
)
@{
   for each RTC configured in RTC_Table
     if the deviceProbe for this RTC indicates it is present
       set RTC_Minor to this device
       set RTC_Present to TRUE
       break out of this loop

   if RTC_Present is not TRUE
     return RTEMS_INVALID_NUMBER to indicate that no RTC is present

   register this minor number as the "/dev/rtc"

   perform the deviceInitialize routine for the preferred RTC chip

   for RTCs past this one in the RTC_Table
     if the deviceProbe for this RTC indicates it is present
       perform the deviceInitialize routine for this RTC chip
       register the configured name for this RTC
@}
@end group
@end example

The @code{deviceProbe} routine returns TRUE if the device
configured by this entry in the @code{RTC_Table} is present.  
This configuration scheme allows one to support multiple versions
of the same board with a single BSP.  For example, if the first
generation of a board had Vendor A's RTC chip and the second
generation had Vendor B's RTC chip, RTC_Table could contain 
information for both.  The @code{deviceProbe} configured
for Vendor A's RTC chip would need to return TRUE if the
board was a first generation one.  The @code{deviceProbe}
routines are very board dependent and must be provided by
the BSP.

@section setRealTimeToRTEMS

The @code{setRealTimeToRTEMS} routine sets the current RTEMS TOD to that
of the preferred RTC.  

@example
@group
void setRealTimeToRTEMS(void)
@{
  if no RTCs are present
    return

  invoke the deviceGetTime routine for the preferred RTC
  set the RTEMS TOD using rtems_clock_set
@}
@end group
@end example

@section setRealTimeFromRTEMS

The @code{setRealTimeFromRTEMS} routine sets the preferred RTC TOD to the
current RTEMS TOD.

@example
@group
void setRealTimeFromRTEMS(void)
@{
  if no RTCs are present
    return

  obtain the RTEMS TOD using rtems_clock_get
  invoke the deviceSetTime routine for the preferred RTC
@}
@end group
@end example

@section getRealTime

The @code{getRealTime} returns the preferred RTC TOD to the
caller.

@example
@group
void getRealTime( rtems_time_of_day *tod )
@{
  if no RTCs are present
    return

  invoke the deviceGetTime routine for the preferred RTC
@}
@end group
@end example

@section setRealTime

The @code{setRealTime} routine sets the preferred RTC TOD to the
TOD specified by the caller.

@example
@group
void setRealTime( rtems_time_of_day *tod )
@{
  if no RTCs are present
    return

  invoke the deviceSetTime routine for the preferred RTC
@}
@end group
@end example

@section checkRealTime

The @code{checkRealTime} routine returns the number of seconds
difference between the RTC TOD and the current RTEMS TOD.

@example
@group
int checkRealTime( void )
@{
  if no RTCs are present
    return -1

  
  obtain the RTEMS TOD using rtems_clock_get
  get the TOD from the preferred RTC using the deviceGetTime routine

  convert the RTEMS TOD to seconds
  convert the RTC TOD to seconds
 
  return the RTEMS TOD in seconds - RTC TOD in seconds
@}
@end group
@end example

