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
with System.Task_Primitives.Operations;
with System.Tasking;

procedure SPATCB01 is
  Self_Id : constant System.Tasking.Task_Id := System.Task_Primitives.Operations.Self;
begin
  TEXT_IO.PUT_LINE( "*** BEGIN OF TEST SPATCB 1 ***" );
  TEXT_IO.PUT_LINE( Self_Id.Common.Task_Image( 1 .. Self_Id.Common.Task_Image_Len ) );
  TEXT_IO.PUT_LINE( "*** END OF TEST SPATCB 1 ***" );
end SPATCB01;
