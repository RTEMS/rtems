--
--  MAIN / BODY
--
--  DESCRIPTION:
--
--  Ensure that the SYSTEM.TASK_PRIMITIVES.OPERATIONS.SPECIFIC implementation
--  works.
--
--  Copyright (c) 2017 embedded brains GmbH
--
--  The license and distribution terms for this file may in
--  the file LICENSE in this distribution or at
--  http://www.rtems.org/license/LICENSE.
--

with TEXT_IO;
with TEST_SUPPORT;
with System.Task_Primitives.Operations;
with System.Tasking;

procedure SPATCB01 is
  Self_Id : constant System.Tasking.Task_Id := System.Task_Primitives.Operations.Self;
begin
  TEST_SUPPORT.ADA_TEST_BEGIN;
  TEXT_IO.PUT_LINE( Self_Id.Common.Task_Image( 1 .. Self_Id.Common.Task_Image_Len ) );
  TEST_SUPPORT.ADA_TEST_END;
end SPATCB01;
