OBJECT: AVDAS
  DESCRIPTION:
    This object collects various values from other objects in the system
    and outputs the values in a message on a serial interface.  The message
    can be read and used as diagnostic information.
  THEORY OF OPERATION:
    This object contains a Server task which periodically collects 
    diagnostic values from other objects in the system and outputs the 
    values in a message on a serial interface.  The diagnostic values
    collected and formatted reflect system status.
  NOTES:
    The following engineering note consists of a chart to aid in the
    decision making process of whether the AVDAS should reflect System
    status or Switch status.  As it is right now, it was decided to 
    reflect System status.

    @table
                               System                  Switch
    Gun Manual              Weapon_control          Gunner_console
    Gun Air                 -------                 -------
    Gun Ground              -------                 -------
    *Remote Status          Remote                  Remote
    *Msl Armed Indicate     Missiles                Missiles
    *Rain Mode              FOV                     FOV
    *Msl Uncage Verified    Missiles                Missiles
    *Msl Activate Status    Missiles                Missiles
    *RSO Status             Weapon_control          Weapon_control
    *Fire Permit            Weapon_control_(new)    Weapon_control_(new)
    *Stab Mode Status       Drive_(new)             Drive_(new)
    Uncage Mode Status      Weapon_control          Gunner_console_(new)
    Engage Mode Status      Mode                    Gunner_console_(new)
    *Palm Grip Status       Gunner_handstation      Gunner_handstation
    *System Fault Status    Fault (Add)             Fault_(new)
    FLIR FOV Status         FOV                     Gunner_console
    *Autotrack Status       Video_tracker           Video_tracker
    Arm Switch Status       Weapon_control          Gunner_console
    *Uncage Switch Status   Gunner_handstation      Gunner_handstation
    *Trig Switch Status     Gunner_handstation      Gunner_handstation
    *Msl Fire Cmd Status    Missiles_(new)          Missiles_(new)
    Helicopter Mode         Weapon_control          Gunner_console_(new)
    IFF Challenge Switch    IFF_(new)               Gunner_console
    *Auto Slew Status       Autoslew_Is_on          Autoslew_Is_on

    @end table

    Message bytes 26 through 29 are not set at this time pending 
    completion of the Super Elevate object.
  
ATTRIBUTE DESCRIPTIONS: none

ASSOCIATION DESCRIPTIONS: none

ABSTRACT TYPE DESCRIPTIONS: none

DATA ITEM DESCRIPTIONS:
 
DATA ITEM: Message
  DESCRIPTION:
    This data item is the buffer used to build an AVDAS data message.
    The first two bytes are place-holders for the header bytes.  The 
    last byte is a place-holder for the checksum byte.
  DERIVATION: array[ 36 ]
  TYPE: bytes
  DEFAULT:
    all bytes = 0x00

DATA ITEM: The_Channel
  DESCRIPTION:
    This data item contains the handle to the AVDAS channel.
  TYPE: Channel_Control handle
  DEFAULT: NULL
 
METHOD DESCRIPTIONS:

METHOD: Break_Up
  DESCRIPTION:
    This method converts a floating point number to two ascii bytes
    based on an input range and weighting factor.
  VISIBILITY: private
  INPUTS:
    max_range
    min_range
    value
    weight_factor
  OUTPUTS:
    lower_byte
    upper_byte
  PDL:
    if value between 0 and min_range
      set temp2 to min_range
      set bit_ratio to absolute value( temp2 / 0x8000 )
    else if value between 0 and max_range
      set temp2 to max_range
      set bit_ratio to absolute value( temp2 / 0x7FF0 )

    set temp3 to (value / temp2) * weight_factor

    set temp4 to temp3 converted to unsigned 16 bit value

    set temp4 to two's compliment of temp4

    set lower_byte to lower 8 bits of temp4

    set upper_byte to upper 8 bits of temp4

METHOD: Create
  DESCRIPTION:
    This method performs the necessary actions to create this object.
  VISIBILITY: public
  INPUTS: none
  OUTPUTS: none
  PDL:
    initialize Message to default

    get AVDAS protocol information using the AVDAS_protocol object
    create a channel for the AVDAS interface using the Channel object
    attach the channel to the AVDAS interface using the Channel object
     with the following parameters:
       flush        - FALSE
       major_number - CONFIGURATION_AVDAS_MAJOR
       minor_number - CONFIGURATION_AVDAS_MINOR
 
TASK DESCRIPTIONS:

TASK: Server 
  DESCRIPTION: 
    This task periodically collects diagnostic values and outputs the
    values in a message on a serial interface.
  INPUTS: none
  SYNCHRONIZATION: period
  TIMING: 100 ms
  REQUIREMENTS: 
    AFS 3.1.4 Provide AVDAS
    AFS 3.1.7 Update AVDAS port 
  PDL:
    create a period

    wait for completion using the Initialization object

    loop forever
       wait for period to expire

       initialize Message

       get absolute azimuth using the Turret object
       format absolute azimuth and store in Message[2] and Message[3]

       get elevation using the Turret object
       format elevation and store in Message[4] and Message[5]

       get drive rate using the Drive object
       format azimuth rate and store in Message[6] and Message[7]
       format elevation rate and store in Message[8] and Message[9]

       get drive displacement using the Drive object
       format azimuth displacement and store in Message[10] and Message[11]
       format elevation displacement and
        store in Message[12] and Message[13]

       get seeker position error using the Seeker object
       format azimuth position error and
        store in Message[14] and Message[15]
       format elevation position error and
        store in Message[16] and Message[17]

       get range using the Laser object
       format range and store in Message[18] and Message[19]

       get north reference using the Turret object
       format north reference and store in Message[20] and Message[21]

       get video tracker position error using the Video_tracker object
       format azimuth position error and
        store in Message[22] and Message[23]
       format elevation position error and
        store in Message[24] and Message[25]

       ?get lead command?

       get gun mode switch using the Weapon_control object
       format gun mode switch and store in Message[30]

       determine if remote is active using the Remote object
       format remote active and store in Message[30]

       determine if missile is safed using the Missiles object
       format missile safed and store in Message[30]

       determine if rain mode is active using the FOV object
       format rain mode active and store in Message[30]

       get missile inventory using the Missiles object
       format present indicators and store in Message[31]
       format selected indicators and store in Message[32]

       determine if missile uncage verified using the Missiles object
       format missile uncage verified and store in Message[32]

       determine if missile activated using the Missiles object
       format missile activated and store in Message[32]

       determine if RSO is authorized using the Weapon_control object
       format RSO authorized and store in Message[32]

       determine if fire permit using the Weapon_control object
       format fire permit and store in Message[32]

       determine if stab mode using the Drive object
       format stab mode and store in Message[33]

       determine if uncage mode is WEAPON_CONTROL_MANUAL_UNCAGE using 
        the Weapon_control object
       format uncage mode and store in Message[33]

       determine if current mode is MODE_ENGAGE using the Mode object
       format current mode and store in Message[33]

       determine if palmgrip switch is pressed using the
        Gunner_handstation object
       format palmgrip switch pressed and store in Message[33]

       determine if fault is active using the Fault object
       format fault active and store in Message[33]

       determine if FOV FLIR zoom is FOV_ZOOM_NARROW using the FOV object
       format FOV FLIR zoom and store in Message[33]

       determine if video tracker is on using the Video_tracker object
       format video tracker on and store in Message[34]

       determine if armed using the Weapon_control object
       format armed and store in Message[34]

       determine if uncage trigger switch pressed using the
        Gunner_handstation object
       format uncage trigger switch pressed and store in Message[34]

       determine if fire trigger switch pressed using the
        Gunner_handstation object
       format fire trigger switch pressed and store in Message[34]

       determine if missiles is firing using the Missiles object
       format missiles firing and store in Message[34]

       determine if helicopter mode is WEAPON_CONTROL_ON using the 
        Weapon_control object
       format helicopter mode and store in Message[34]

       determine if IFF challenge using the IFF object
       format IFF challenge and store in Message[34]

       determine if autoslew is on using the Autoslew object
       format autoslew is on and store in Message[34]

       loop until successful
          write Message using the Channel object
          if unsuccessful
             reset the Channel object

ENDOBJECT: AVDAS
