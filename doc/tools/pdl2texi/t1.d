OBJECT:  Drive
  DESCRIPTION:
    This object provides an interface to the elevation/azimuth
    drive system(ECA).  
  THEORY OF OPERATION:
    This object encapsulates the turret drive.  The object provides
    an interface to control the turret drive.  A method is provided
    to set the azimuth and elevation rate of the drive.  Methods are
    provided to request the drive to run in stabilized or power mode
    and set the drive speed to high or low.  A method is also provided
    to allow for drift adjust.  

    This object also tracks drive system on hours.  It gets the hours
    from the EEPROM data, allows the hours to be set, updates the
    hours via the activation and deactivation of the drive, and
    provides access to the hours.

    The object also provides methods to access azimuth and elevation
    displacement, and methods to process changes in the hardware
    discretes drive on, stabilized mode, thermal fault and fault.
  
    The object also provides methods to enable and disable
    both the remote and turret power interlocks which determine if
    the drive can be driven by the remote or turret handstation
    respectively.   

ATTRIBUTE DESCRIPTIONS:


ATTRIBUTE:  Azimuth_displacement
  DESCRIPTION:
    This attribute represents the current azimuth displacement
    of the turret.
  TYPE: float constant
  RANGE: 0-0
  UNITS: degrees
  REQUIREMENTS:
    AFS 8.2.11 Receive Azimuth Displacement and Elevation Displacement
    AIS 5.2.6 Azimuth Displacement analog


ASSOCIATION DESCRIPTIONS: none

ABSTRACT TYPE DESCRIPTIONS: none

DATA ITEM DESCRIPTIONS: none

METHOD DESCRIPTIONS: none

TASK DESCRIPTIONS: none

END OBJECT: Drive
