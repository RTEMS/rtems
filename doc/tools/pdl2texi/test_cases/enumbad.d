OBJECT: Testfile
  DESCRIPTION:
    This file contains one abstract type without a range and two 
    abstract types with filler ranges.
  THEORY OF OPERATION: 
    This file will be used to test the .d compiler.

ATTRIBUTE DESCRIPTIONS: none 

ASSOCIATION DESCRIPTIONS: none 

ABSTRACT TYPE DESCRIPTIONS:

ABSTRACT TYPE: ID
  DESCRIPTION:
    This type defines an identifier for an event set that may be manipulated.
  VISIBILITY: public
  DERIVATION: Bin_type handle
  RANGE: 99 - 99

ABSTRACT TYPE: Test_ID
  DESCRIPTION:
    This is the abstract type with the range made optional.
  VISIBILITY: public
  DERIVATION: Bin_type handle
    
ABSTRACT TYPE: Input_Buffer
  DESCRIPTION:
    This type indicates the maximum input buffer.
  VISIBILITY: public
  DERIVATION: string
  RANGE: 99 - 99       
    
ABSTRACT TYPE: Image_t 
  DESCRIPTION:         
    This type specifies the data maintained for the screen.
  VISIBILITY: public
  DERIVATION: array [LINES][COLUMNS] of character
  RANGE: 0 - 1 QQQ

DATA ITEM DESCRIPTIONS: none

METHOD DESCRIPTIONS: none
  
TASK DESCRIPTIONS: none
  
ENDOBJECT: Testfile

