; Push $filenamestring (e.g. 'c:\this\and\that\filename.htm')
; Push '\\'
; Pop $R0
; Call StrSlash
; Pop $R0
; ;Now $R0 contains 'c:/this/and/that/filename.htm'
Function StrSlash
 Exch $R0
 Push $R1
 Push $R2
 StrCpy $R1 0
loop:
  IntOp $R1 $R1 - 1
  StrCpy $R2 $R0 1 $R1
  StrCmp $R2 "" done
 StrCmp $R2 "\" 0 loop  ; "
  StrCpy $R2 $R0 $R1
   Push $R1
  IntOp $R1 $R1 + 1
  StrCpy $R1 $R0 "" $R1
 StrCpy $R0 "$R2/$R1"
   Pop $R1
  IntOp $R1 $R1 - 1
 Goto loop
done:
 Pop $R2
 Pop $R1
 Exch $R0
FunctionEnd
