--  RTEMS / Specification
--
--  DESCRIPTION:
--
--  This package provides the interface to the RTEMS API.
--
--  DEPENDENCIES:
--
--  NOTES:
--    RTEMS initialization and configuration are called from
--    the BSP side, therefore should never be called from ADA.
--
--  COPYRIGHT (c) 1997-2011.
--  On-Line Applications Research Corporation (OAR).
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.com/license/LICENSE.
--

with System;
with System.Storage_Elements; use System.Storage_Elements;
with Interfaces;
with Interfaces.C;

package RTEMS is
pragma Elaborate_Body (RTEMS);

   Structure_Alignment : constant := 8;

   --
   --  RTEMS Base Types
   --

   subtype Unsigned8  is Interfaces.Unsigned_8;
   subtype Unsigned16 is Interfaces.Unsigned_16;
   subtype Unsigned32 is Interfaces.Unsigned_32;
   subtype Signed32   is Interfaces.Integer_32;

   type Unsigned32_Pointer     is access all RTEMS.Unsigned32;
   type Unsigned16_Pointer     is access all RTEMS.Unsigned16;
   type Unsigned8_Pointer      is access all RTEMS.Unsigned8;
   type Signed32_Pointer       is access all RTEMS.Signed32;

   subtype Boolean             is RTEMS.Unsigned8;
   subtype Address             is System.Address;
   subtype Single              is Interfaces.C.C_float;
   subtype Double              is Interfaces.C.Double;

   --
   --  The following define the size of each of the base types in
   --  both bits and system units.
   --

   Unsigned8_Bits   : constant := 7;
   Unsigned16_Bits  : constant := 15;
   Unsigned32_Bits  : constant := 31;
   Boolean_Bits     : constant := 7;
   Address_Bits     : constant := 31;
   Single_Bits      : constant := 31;
   Double_Bits      : constant := 63;

   Unsigned8_Units  : constant := 1;
   Unsigned16_Units : constant := 2;
   Unsigned32_Units : constant := 4;
   Boolean_Units    : constant := 1;
   Address_Units    : constant := 4;
   Single_Units     : constant := 4;
   Double_Units     : constant := 8;

   Null_Address     : constant RTEMS.Address :=
      System.Storage_Elements.To_Address(0);

   True  : constant RTEMS.Boolean := 1;
   False : constant RTEMS.Boolean := 0;

   --
   --  More Types
   --

   subtype Name                is RTEMS.Unsigned32;
   subtype ID                  is RTEMS.Unsigned32;
   subtype Interval            is RTEMS.Unsigned32;
   subtype Attribute           is RTEMS.Unsigned32;
   subtype Mode                is RTEMS.Unsigned32;
   subtype Option              is RTEMS.Unsigned32;
   subtype Notepad_Index       is RTEMS.Unsigned32 range 0 .. 15;

   subtype Event_Set           is RTEMS.Unsigned32;
   subtype Signal_Set          is RTEMS.Unsigned32;
   subtype Device_Major_Number is RTEMS.Unsigned32;
   subtype Device_Minor_Number is RTEMS.Unsigned32;
   subtype ISR_Level           is RTEMS.Unsigned32;

   subtype Node                is RTEMS.Unsigned32;

   type Driver_Name_t is
      record
         Device_Name        : RTEMS.Address;
         Device_Name_Length : RTEMS.Unsigned32;
         Major              : RTEMS.Device_Major_Number;
         Minor              : RTEMS.Device_Minor_Number;

      end record;


   --
   --  Task Related Types
   --

   subtype TCB                 is RTEMS.Unsigned32;
   type    TCB_Pointer         is access all RTEMS.TCB;

   --
   --  Time of Day Type
   --

   type Time_Of_Day is
      record
         Year    : RTEMS.Unsigned32; -- year, A.D.
         Month   : RTEMS.Unsigned32; -- month, 1 .. 12
         Day     : RTEMS.Unsigned32; -- day, 1 .. 31
         Hour    : RTEMS.Unsigned32; -- hour, 0 .. 23
         Minute  : RTEMS.Unsigned32; -- minute, 0 .. 59
         Second  : RTEMS.Unsigned32; -- second, 0 .. 59
         Ticks   : RTEMS.Unsigned32; -- elapsed ticks between seconds
      end record;

   type Time_T is new Interfaces.C.Long;

   type Timespec is record
      TV_Sec  : Time_T;
      TV_Nsec : Interfaces.C.Long;
   end record;
   pragma Convention (C, Timespec);

   type Time_Value is
      record
         Seconds      : RTEMS.Unsigned32;
         Microseconds : RTEMS.Unsigned32;
      end record;

   --
   --  Ident Options
   --

   Search_All_Nodes   : constant RTEMS.Node := 0;
   Search_Other_Nodes : constant RTEMS.Node := 16#7FFFFFFE#;
   Search_Local_Node  : constant RTEMS.Node := 16#7FFFFFFF#;
   Who_Am_I           : constant RTEMS.Node := 0;

   --
   --  Options
   --

   Default_Options    : constant RTEMS.Option    := 16#0000#;

   Wait      : constant RTEMS.Option := 16#0000#;
   No_Wait   : constant RTEMS.Option := 16#0001#;

   Event_All : constant RTEMS.Option := 16#0000#;
   Event_Any : constant RTEMS.Option := 16#0002#;

   --
   --  Mode constants
   --

   Default_Modes      : constant RTEMS.Mode      := 16#0000#;

   All_Mode_Masks     : constant RTEMS.Mode := 16#0000_ffff#;
   Current_Mode       : constant RTEMS.Mode := 16#0000_0000#;
   Preempt_Mask       : constant RTEMS.Mode := 16#0000_0100#;
   Timeslice_Mask     : constant RTEMS.Mode := 16#0000_0200#;
   ASR_Mask           : constant RTEMS.Mode := 16#0000_0400#;
   Interrupt_Mask     : RTEMS.Mode;
   Preempt            : constant RTEMS.Mode := 16#0000_0000#;
   No_Preempt         : constant RTEMS.Mode := 16#0000_0100#;
   No_Timeslice       : constant RTEMS.Mode := 16#0000_0000#;
   Timeslice          : constant RTEMS.Mode := 16#0000_0200#;
   ASR                : constant RTEMS.Mode := 16#0000_0000#;
   No_ASR             : constant RTEMS.Mode := 16#0000_0400#;

   pragma Import (C, Interrupt_Mask, "rtems_interrupt_mask");

   --
   --  Attribute constants
   --

   Default_Attributes      : constant RTEMS.Attribute := 16#00000000#;
   No_Floating_Point       : constant RTEMS.Attribute := 16#00000000#;
   Floating_Point          : constant RTEMS.Attribute := 16#00000001#;
   Local                   : constant RTEMS.Attribute := 16#00000000#;
   Global                  : constant RTEMS.Attribute := 16#00000002#;
   FIFO                    : constant RTEMS.Attribute := 16#00000000#;
   Priority                : constant RTEMS.Attribute := 16#00000004#;
   Counting_Semaphore      : constant RTEMS.Attribute := 16#00000000#;
   Binary_Semaphore        : constant RTEMS.Attribute := 16#00000010#;
   Simple_Binary_Semaphore : constant RTEMS.Attribute := 16#00000020#;
   No_Inherit_Priority     : constant RTEMS.Attribute := 16#00000000#;
   Inherit_Priority        : constant RTEMS.Attribute := 16#00000040#;
   No_Priority_Ceiling     : constant RTEMS.Attribute := 16#00000000#;
   Priority_Ceiling        : constant RTEMS.Attribute := 16#00000080#;

   function Interrupt_Level (
      Level : in     RTEMS.Unsigned32
   ) return RTEMS.Mode;
   pragma Import (C, Interrupt_Level, "rtems_interrupt_level_body");

   Minimum_Stack_Size : RTEMS.Unsigned32;
   pragma Import (C, Minimum_Stack_Size, "rtems_minimum_stack_size");

   --
   --  Notepad index constants
   --

   Notepad_0  : constant RTEMS.Unsigned32 := 0;
   Notepad_1  : constant RTEMS.Unsigned32 := 1;
   Notepad_2  : constant RTEMS.Unsigned32 := 2;
   Notepad_3  : constant RTEMS.Unsigned32 := 3;
   Notepad_4  : constant RTEMS.Unsigned32 := 4;
   Notepad_5  : constant RTEMS.Unsigned32 := 5;
   Notepad_6  : constant RTEMS.Unsigned32 := 6;
   Notepad_7  : constant RTEMS.Unsigned32 := 7;
   Notepad_8  : constant RTEMS.Unsigned32 := 8;
   Notepad_9  : constant RTEMS.Unsigned32 := 9;
   Notepad_10 : constant RTEMS.Unsigned32 := 10;
   Notepad_11 : constant RTEMS.Unsigned32 := 11;
   Notepad_12 : constant RTEMS.Unsigned32 := 12;
   Notepad_13 : constant RTEMS.Unsigned32 := 13;
   Notepad_14 : constant RTEMS.Unsigned32 := 14;
   Notepad_15 : constant RTEMS.Unsigned32 := 15;

   --
   --  Miscellaneous
   --

   No_Timeout                    : constant RTEMS.Interval := 0;
   Self                          : constant RTEMS.ID       := 0;
   Yield_Processor               : constant RTEMS.Interval := 0;
   Rate_Monotonic_Period_Status  : constant RTEMS.Interval := 0;


   --
   --  Extension Callouts and Table
   --

   type Thread_Create_Extension is access function (
      Current_Task : in     RTEMS.TCB_Pointer;
      New_Task     : in     RTEMS.TCB_Pointer
   ) return RTEMS.Boolean;
   pragma Convention (C, Thread_Create_Extension);

   type Thread_Start_Extension is access procedure (
      Current_Task : in     RTEMS.TCB_Pointer;
      Started_Task : in     RTEMS.TCB_Pointer
   );
   pragma Convention (C, Thread_Start_Extension);

   type Thread_Restart_Extension is access procedure (
      Current_Task   : in     RTEMS.TCB_Pointer;
      Restarted_Task : in     RTEMS.TCB_Pointer
   );
   pragma Convention (C, Thread_Restart_Extension);

   type Thread_Delete_Extension is access procedure (
      Current_Task : in     RTEMS.TCB_Pointer;
      Deleted_Task : in     RTEMS.TCB_Pointer
   );
   pragma Convention (C, Thread_Delete_Extension);

   type Thread_Switch_Extension is access procedure (
      Current_Task : in     RTEMS.TCB_Pointer;
      Heir_Task    : in     RTEMS.TCB_Pointer
   );
   pragma Convention (C, Thread_Switch_Extension);

   type Thread_Post_Switch_Extension is access procedure (
      Current_Task : in     RTEMS.TCB_Pointer
   );
   pragma Convention (C, Thread_Post_Switch_Extension);

   type Thread_Begin_Extension is access procedure (
      Current_Task : in     RTEMS.TCB_Pointer
   );
   pragma Convention (C, Thread_Begin_Extension);

   type Thread_Exitted_Extension is access procedure (
      Current_Task : in     RTEMS.TCB_Pointer
   );
   pragma Convention (C, Thread_Exitted_Extension);

   type Fatal_Error_Extension is access procedure (
      Error : in     RTEMS.Unsigned32
   );
   pragma Convention (C, Fatal_Error_Extension);

   type Extensions_Table is
      record
         Thread_Create      : RTEMS.Thread_Create_Extension;
         Thread_Start       : RTEMS.Thread_Start_Extension;
         Thread_Restart     : RTEMS.Thread_Restart_Extension;
         Thread_Delete      : RTEMS.Thread_Delete_Extension;
         Thread_Switch      : RTEMS.Thread_Switch_Extension;
         Thread_Post_Switch : RTEMS.Thread_Post_Switch_Extension;
         Thread_Begin       : RTEMS.Thread_Begin_Extension;
         Thread_Exitted     : RTEMS.Thread_Exitted_Extension;
         Fatal              : RTEMS.Fatal_Error_Extension;
      end record;

   type Extensions_Table_Pointer is access all Extensions_Table;

   --
   --  The following type define a pointer to a signal service routine.
   --

   type ASR_Handler is access procedure (
      Signals : in     RTEMS.Signal_Set
   );
   pragma Convention (C, ASR_Handler);

   --
   --  Method Completions Status Codes
   --

   type Status_Codes is (
      Successful,               -- successful completion
      Task_Exitted,             -- returned from a task
      MP_Not_Configured,        -- multiprocessing not configured
      Invalid_Name,             -- invalid object name
      Invalid_ID,               -- invalid object id
      Too_Many,                 -- too many
      Timeout,                  -- timed out waiting
      Object_Was_Deleted,       -- object deleted while waiting
      Invalid_Size,             -- specified size was invalid
      Invalid_Address,          -- address specified is invalid
      Invalid_Number,           -- number was invalid
      Not_Defined,              -- item has not been initialized
      Resource_In_Use,          -- resources still outstanding
      Unsatisfied,              -- request not satisfied
      Incorrect_State,          -- task is in wrong state
      Already_Suspended,        -- task already in state
      Illegal_On_Self,          -- illegal on calling task
      Illegal_On_Remote_Object, -- illegal for remote object
      Called_From_ISR,          -- called from wrong environment
      Invalid_Priority,         -- invalid task priority
      Invalid_Clock,            -- invalid date/time
      Invalid_Node,             -- invalid node id
      Not_Configured,           -- directive not configured
      Not_Owner_Of_Resource,    -- not owner of resource
      Not_Implemented,          -- directive not implemented
      Internal_Error,           -- RTEMS inconsistency detected
      No_Memory,                -- no memory left in heap
      IO_Error,                 -- driver IO error
      Proxy_Blocking            -- internal multiprocessing only
   );

   for Status_Codes'Size use 32;

   for Status_Codes use (
      Successful                  =>  0,
      Task_Exitted                =>  1,
      MP_Not_Configured           =>  2,
      Invalid_Name                =>  3,
      Invalid_ID                  =>  4,
      Too_Many                    =>  5,
      Timeout                     =>  6,
      Object_Was_Deleted          =>  7,
      Invalid_Size                =>  8,
      Invalid_Address             =>  9,
      Invalid_NumbeR              => 10,
      Not_Defined                 => 11,
      Resource_In_Use             => 12,
      Unsatisfied                 => 13,
      Incorrect_State             => 14,
      Already_Suspended           => 15,
      Illegal_On_Self             => 16,
      Illegal_On_Remote_Object    => 17,
      Called_From_ISR             => 18,
      Invalid_Priority            => 19,
      Invalid_Clock               => 20,
      Invalid_Node                => 21,
      Not_Configured              => 22,
      Not_Owner_Of_Resource       => 23,
      Not_ImplementeD             => 24,
      Internal_Error              => 25,
      No_Memory                   => 26,
      IO_Error                    => 27,
      Proxy_Blocking              => 28
   );

   --
   --  RTEMS Events
   --

   Pending_Events : constant RTEMS.Event_Set := 16#0000_0000#;
   All_Events     : constant RTEMS.Event_Set := 16#FFFF_FFFF#;
   Event_0        : constant RTEMS.Event_Set := 16#0000_0001#;
   Event_1        : constant RTEMS.Event_Set := 16#0000_0002#;
   Event_2        : constant RTEMS.Event_Set := 16#0000_0004#;
   Event_3        : constant RTEMS.Event_Set := 16#0000_0008#;
   Event_4        : constant RTEMS.Event_Set := 16#0000_0010#;
   Event_5        : constant RTEMS.Event_Set := 16#0000_0020#;
   Event_6        : constant RTEMS.Event_Set := 16#0000_0040#;
   Event_7        : constant RTEMS.Event_Set := 16#0000_0080#;
   Event_8        : constant RTEMS.Event_Set := 16#0000_0100#;
   Event_9        : constant RTEMS.Event_Set := 16#0000_0200#;
   Event_10       : constant RTEMS.Event_Set := 16#0000_0400#;
   Event_11       : constant RTEMS.Event_Set := 16#0000_0800#;
   Event_12       : constant RTEMS.Event_Set := 16#0000_1000#;
   Event_13       : constant RTEMS.Event_Set := 16#0000_2000#;
   Event_14       : constant RTEMS.Event_Set := 16#0000_4000#;
   Event_15       : constant RTEMS.Event_Set := 16#0000_8000#;
   Event_16       : constant RTEMS.Event_Set := 16#0001_0000#;
   Event_17       : constant RTEMS.Event_Set := 16#0002_0000#;
   Event_18       : constant RTEMS.Event_Set := 16#0004_0000#;
   Event_19       : constant RTEMS.Event_Set := 16#0008_0000#;
   Event_20       : constant RTEMS.Event_Set := 16#0010_0000#;
   Event_21       : constant RTEMS.Event_Set := 16#0020_0000#;
   Event_22       : constant RTEMS.Event_Set := 16#0040_0000#;
   Event_23       : constant RTEMS.Event_Set := 16#0080_0000#;
   Event_24       : constant RTEMS.Event_Set := 16#0100_0000#;
   Event_25       : constant RTEMS.Event_Set := 16#0200_0000#;
   Event_26       : constant RTEMS.Event_Set := 16#0400_0000#;
   Event_27       : constant RTEMS.Event_Set := 16#0800_0000#;
   Event_28       : constant RTEMS.Event_Set := 16#1000_0000#;
   Event_29       : constant RTEMS.Event_Set := 16#2000_0000#;
   Event_30       : constant RTEMS.Event_Set := 16#4000_0000#;
   Event_31       : constant RTEMS.Event_Set := 16#8000_0000#;

   --
   --  RTEMS Signals
   --

   All_Signals : constant RTEMS.Signal_Set := 16#7FFFFFFF#;
   Signal_0    : constant RTEMS.Signal_Set := 16#00000001#;
   Signal_1    : constant RTEMS.Signal_Set := 16#00000002#;
   Signal_2    : constant RTEMS.Signal_Set := 16#00000004#;
   Signal_3    : constant RTEMS.Signal_Set := 16#00000008#;
   Signal_4    : constant RTEMS.Signal_Set := 16#00000010#;
   Signal_5    : constant RTEMS.Signal_Set := 16#00000020#;
   Signal_6    : constant RTEMS.Signal_Set := 16#00000040#;
   Signal_7    : constant RTEMS.Signal_Set := 16#00000080#;
   Signal_8    : constant RTEMS.Signal_Set := 16#00000100#;
   Signal_9    : constant RTEMS.Signal_Set := 16#00000200#;
   Signal_10   : constant RTEMS.Signal_Set := 16#00000400#;
   Signal_11   : constant RTEMS.Signal_Set := 16#00000800#;
   Signal_12   : constant RTEMS.Signal_Set := 16#00001000#;
   Signal_13   : constant RTEMS.Signal_Set := 16#00002000#;
   Signal_14   : constant RTEMS.Signal_Set := 16#00004000#;
   Signal_15   : constant RTEMS.Signal_Set := 16#00008000#;
   Signal_16   : constant RTEMS.Signal_Set := 16#00010000#;
   Signal_17   : constant RTEMS.Signal_Set := 16#00020000#;
   Signal_18   : constant RTEMS.Signal_Set := 16#00040000#;
   Signal_19   : constant RTEMS.Signal_Set := 16#00080000#;
   Signal_20   : constant RTEMS.Signal_Set := 16#00100000#;
   Signal_21   : constant RTEMS.Signal_Set := 16#00200000#;
   Signal_22   : constant RTEMS.Signal_Set := 16#00400000#;
   Signal_23   : constant RTEMS.Signal_Set := 16#00800000#;
   Signal_24   : constant RTEMS.Signal_Set := 16#01000000#;
   Signal_25   : constant RTEMS.Signal_Set := 16#02000000#;
   Signal_26   : constant RTEMS.Signal_Set := 16#04000000#;
   Signal_27   : constant RTEMS.Signal_Set := 16#08000000#;
   Signal_28   : constant RTEMS.Signal_Set := 16#10000000#;
   Signal_29   : constant RTEMS.Signal_Set := 16#20000000#;
   Signal_30   : constant RTEMS.Signal_Set := 16#40000000#;
   Signal_31   : constant RTEMS.Signal_Set := 16#80000000#;

   --
   --  Utility Functions
   --

   function From_Ada_Boolean (
      Ada_Boolean : Standard.Boolean
   ) return RTEMS.Boolean;

   function To_Ada_Boolean (
      RTEMS_Boolean : RTEMS.Boolean
   ) return Standard.Boolean;

   function Milliseconds_To_Microseconds (
      Milliseconds : RTEMS.Unsigned32
   ) return RTEMS.Unsigned32;

   function Microseconds_To_Ticks (
      Microseconds : RTEMS.Unsigned32
   ) return RTEMS.Interval;

   function Milliseconds_To_Ticks (
      Milliseconds : RTEMS.Unsigned32
   ) return RTEMS.Interval;

   procedure Name_To_Characters (
      Name : in     RTEMS.Name;
      C1   :    out Character;
      C2   :    out Character;
      C3   :    out Character;
      C4   :    out Character
   );

   function Get_Node (
      ID : in     RTEMS.ID
   ) return RTEMS.Unsigned32;

   function Get_Index (
      ID : in     RTEMS.ID
   ) return RTEMS.Unsigned32;

   function Are_Statuses_Equal (
      Status  : in     RTEMS.Status_Codes;
      Desired : in     RTEMS.Status_Codes
   ) return Standard.Boolean;

   function Is_Status_Successful (
      Status  : in     RTEMS.Status_Codes
   ) return Standard.Boolean;

   function Subtract (
      Left   : in     RTEMS.Address;
      Right  : in     RTEMS.Address
   ) return RTEMS.Unsigned32;

   function Are_Equal (
      Left   : in     RTEMS.Address;
      Right  : in     RTEMS.Address
   ) return Standard.Boolean;

   --
   --  RTEMS API
   --

   function Build_Name (
      C1 : in     Character;
      C2 : in     Character;
      C3 : in     Character;
      C4 : in     Character
   ) return RTEMS.Name;

   --
   --  Initialization Manager -- Shutdown Only
   --
   procedure Shutdown_Executive (
      Status : in     RTEMS.Unsigned32
   );

end RTEMS;

