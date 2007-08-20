!macro FILE_WRITE_LINE Handle Text
  FileWrite     ${Handle} `${Text}`
  FileWriteByte ${Handle} "13"
  FileWriteByte ${Handle} "10"
!macroend
