(All fields marked with an '*' are optional and can be deleted if
 there is no applicable information.
 All entity names (OBJECT, ATTRIBUTE, METHOD, etc.)  are proper nouns
 and thus should only have the first letter capitalized.)

OBJECT: Object_name
  DESCRIPTION:
    This object ...
    (briefly describe the object's primary responsibility or purpose)
 *COPYRIGHT:
    Copyright (c) 1995, On-Line Applications Research Corporation (OAR)
 *PORTING:
  THEORY OF OPERATION:
 *DERIVATION:
 *DEPENDENCIES:
 *NOTES:

ATTRIBUTE DESCRIPTIONS:

ATTRIBUTE: An_attribute
  DESCRIPTION:
    This attribute ...
    (briefly describe the attribute's primary purpose)
  TYPE: float [constant]|integer [constant]
    (indicate one of the above)
  MEMBERS|RANGE: 0 - +INFINITY  or  MEMBER1, MEMBER2, MEMBER3
    (indicate RANGE or MEMBERS and specify, MEMBERS should be listed
     in all caps, RANGE can use +/-INFINITY)
 *UNITS:
 *SCALE FACTOR:
 *DEFAULTS:
 *TOLERANCE:
 *REQUIREMENTS:
    ABCD 3.2.4 A paragraph title
    (indicate document, paragraph number, paragraph title)
 *REFERENCES:
 *NOTES:

ASSOCIATION DESCRIPTIONS:

ASSOCIATION: name
  DESCRIPTION:
    This association ...
    (briefly describe the association's primary purpose)
  VISIBILITY: private|public
    (indicate one of the above)
  ASSOCIATED WITH: object_name
  MULTIPLICITY:
 *REQUIREMENTS:
    ABCD 3.2.4 A paragraph title
    (indicate document, paragraph number, paragraph title)
 *REFERENCES:
 *NOTES:  

ABSTRACT TYPE DESCRIPTIONS:

ABSTRACT TYPE: name
  DESCRIPTION:
    This type ...
    (briefly describe the type's primary purpose)
  VISIBILITY: private|public
    (indicate one of the above)
  DERIVATION:  
  MEMBERS|RANGE:  0 - +INFINITY  or  MEMBER1, MEMBER2, MEMBER3
    (indicate RANGE or MEMBERS and specify, MEMBERS should be listed
     in all caps, RANGE can use +/-INFINITY)
 *UNITS:
 *SCALE FACTOR:
 *DEFAULTS:
 *TOLERANCE:
 *REQUIREMENTS:
    ABCD 3.2.4 A paragraph title
    (indicate document, paragraph number, paragraph title)
 *REFERENCES:
 *NOTES:

DATA ITEM DESCRIPTIONS:

DATA ITEM: name
  DESCRIPTION:
    This data item ...
    (briefly describe the data item's primary purpose)
  TYPE:
 *UNITS:
 *SCALE FACTOR:
 *DEFAULTS:
 *TOLERANCE:
 *NOTES:

METHOD DESCRIPTIONS:
  (List methods alphabetically grouping public methods followed
   by private methods.)

METHOD: Some_method
  DESCRIPTION:
    This method ...
    (briefly describe the method's primary responsibility)
  VISIBILITY: private|public
    (indicate one of the above)
  INPUTS:
    input_one - the first and only input
    (specify the logical inputs followed by a description,
     indicate 'none' if there are no inputs)
  OUTPUTS:
    output_one - the first and only output
    (specify the logical outputs followed by a description,
     indicate 'none' if there are no outputs, use 'result' if the
     method is a boolean function)
 *REQUIREMENTS:
    ABCD 3.2.4 A paragraph title
    (indicate document, paragraph number, paragraph title)
 *REFERENCES:
 *NOTES:
  PDL:

TASK DESCRIPTIONS:

TASK: name
  DESCRIPTION:
    This task ...
    (briefly describe the task's primary responsibility)
  INPUTS:
  SYNCHRONIZATION: delay|event|message|semaphore|signal|period
    (indicate one or more of the above and list any events,
     messages, or signals that can be received)
  TIMING:
    (specify any timing information that is related to the
     synchronization specified above)
 *REQUIREMENTS:
    ABCD 3.2.4 A paragraph title
    (indicate document, paragraph number, paragraph title)
 *REFERENCES:
 *NOTES:
  PDL:

ENDOBJECT: Object_name
