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

      + This is the first bullet.
      + This is the second bullet.
      + This is the third bullet.
      + This is the fourth bullet.
      + This is the fifth bullet.

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
  TYPE: floating point
  RANGE: 0-0
  UNITS: degrees
  REQUIREMENTS:
    AFS 8.2.11 Receive Azimuth Displacement and Elevation Displacement
    AIS 5.2.6 Azimuth Displacement analog


ATTRIBUTE:  Azimuth_rate
  DESCRIPTION:
    Azimuth rate command for movement of the turret. 
  TYPE: floating point
  RANGE: X-Y
  UNITS: degrees / second
  DEFAULT: none
  REQUIREMENTS:
    AFS 8.2.3 Send Azimuth and Elevation Rate to ECA
    AIS 5.3.4 Elevation Rate analog
    AIS 5.3.5 Azimuth Rate analog


ATTRIBUTE:  Drift_adjust
  DESCRIPTION:
    This attribute represents the current drift adjustment mode
    of the ECA turret drive.
  TYPE: enumerated
  MEMBERS:
    ADJUST_OFF - drift adjust mode is OFF
    ADJUST_ON  - drift adjust mode is ON
  REQUIREMENTS:
    AFS 8.2.6 Capable of turret drift adjustment
    AIS 5.3.1 Drift Adjust discrete
 

ATTRIBUTE:  Drive_on
  DESCRIPTION:
    Current status of the turret drive.  The turret drive may either be
    switched on or off.
  TYPE: boolean
  MEMBERS:
    FALSE - turret drive is off
    TRUE  - turret drive is on
  DEFAULT: FALSE
  REQUIREMENTS:
    AFS 8.2.9 Monitor Drive System On
    AIS 5.2.1 Drive System On discrete


ATTRIBUTE:  Drive_system_hours
  DESCRIPTION: 
    Total number of hours drive has been activated. 
  TYPE: integer
  RANGE: 0 - +INFINITY 
  UNITS: seconds
  REQUIREMENTS:
    AFS 4.11 Respond to CDT elapsed time indicator key


ATTRIBUTE:  Elevation_displacement 
  DESCRIPTION:
    This attribute represents the current elevation displacement
    of the turret.
  TYPE: floating point
  RANGE: 0-0
  UNITS: degrees
  REQUIREMENTS:
    AFS 8.2.11 Receive Azimuth Displacement and Elevation Displacement
    AIS 5.2.7 Elevation Displacement analog


ATTRIBUTE:  Elevation_rate
  DESCRIPTION:
    Elevation rate command for movement of the turret.
  TYPE: floating point
  RANGE: X - Y
  UNITS: degrees / second
  DEFAULT: none
  REQUIREMENTS:
    AFS 8.2.3 Send Azimuth and Elevation Rate to ECA
    AIS 5.3.4 Elevation Rate analog
    AIS 5.3.5 Azimuth Rate analog
 

ATTRIBUTE:  Fault
  DESCRIPTION:
    This attribute denotes whether a turret malfunction has occurred.
  TYPE: boolean
  MEMBERS:
    TRUE  - a turret malfunction has occurred
    FALSE - no turret malfunction has occurred
  DEFAULT: FALSE
  REQUIREMENTS:
    AFS 8.2.7 Check for ECA failure indication
    AIS 5.2.4 Turret Drive Malfunction discrete


ATTRIBUTE:  Mode
  DESCRIPTION:
    Current requested drive mode.
  TYPE: enumerated
  MEMBERS:
    POWER - operate in POWER mode
    STAB  - operate in STABILIZED mode
  DEFAULT: POWER
  REQUIREMENTS:
    bogus requirement


ATTRIBUTE:  Remote_power_interlock_position
  DESCRIPTION:
    This is the status of the remote's power interlock.
  TYPE: enumerated
  MEMBERS:
    OPEN   - relay is open
    CLOSED - relay is closed
  REQUIREMENTS:  
    AFS 8.2.12 Control Power Interlock
 

ATTRIBUTE:  Speed
  DESCRIPTION:
    Current speed mode of the ECA turret drive.
  TYPE: enumerated
  MEMBERS:
    LOW_SPEED  - turret drive does not allow the maximum rate
    HIGH_SPEED - turret drive allows the maximum rate
  DEFAULT:  HIGH 
  REQUIREMENTS:
    AFS 8.2.5 Control Turret
    AIS 5.3.3 Turret High Speed Mode discrete


ATTRIBUTE:  Stabilized_mode
  DESCRIPTION:
    Current drive mode indicating whether the ECA electronics are
    operating in Stabilized mode or Power mode.
  TYPE: boolean
  MEMBERS:
    FALSE - not operating in STABILIZED mode
    TRUE  - operating in STABILIZED mode
  DEFAULT: FALSE
  REQUIREMENTS:
    AFS 8.2.8 Monitor Turret Stab Mode Indicate
    AIS 5.2.3 Turret Stab Mode Indicate discrete


ATTRIBUTE:  State
  DESCRIPTION:
    Current state of the turret drive.
  TYPE: enumerated
  MEMBERS:
    ACTIVATED   - turret drive is ON and active
    DEACTIVATED - turret drive is ON but deactivated
  DEFAULT: DEACTIVATED
  REQUIREMENTS:
    AFS 8.2.4 Set Plam Grips Active
    AIS 5.3.2 Palm Grips Active discrete


ATTRIBUTE:  Thermal_fault
  DESCRIPTION:
    This attribute denotes whether a turret thermal fault has occurred.
  TYPE: boolean
  MEMBERS:
    TRUE  - a turret thermal fault has occurred
    FALSE - no turret thermal fault has occurred
  DEFAULT: FALSE
  REQUIREMENTS:
    AFS 8.2.7 Check for ECA failure indication
    AIS 5.2.2 Turret ECA Thermal Failure discrete


ATTRIBUTE:  Turret_power_interlock_position
  DESCRIPTION:
    This is the status of the turret's power interlock.
  TYPE: enumerated
  MEMBERS:
    OPEN   - relay is open
    CLOSED - relay is closed
  REQUIREMENTS:  
    AFS 8.2.12 Control Power Interlock
 

ASSOCIATION DESCRIPTIONS: none

ABSTRACT TYPE DESCRIPTIONS:

ABSTRACT TYPE:  Drift_adjust_t
  DESCRIPTION:
    This abstract type represents the current drift adjustment mode
    of the ECA turret drive.
  VISIBILITY: public
  DERIVATION:  enumerated
  MEMBERS:  
    ADJUST_OFF = 0 - drift adjust mode is OFF
    ADJUST_ON  = 1 - drift adjust mode is ON
  REQUIREMENTS:
    AIS 5.3.1 Drift Adjust discrete

ABSTRACT TYPE: Interlock_status
  DESCRIPTION:
    An interlock state being OPEN or CLOSED.
  VISIBILITY: private
  DERIVATION: enumerated
  MEMBERS:
    OPEN   - interlock relay is in the open state
    CLOSED - interlock relay is in the closed state
  REQUIREMENTS: 
    AFS 8.2.12 Control Power Interlock

ABSTRACT TYPE:  Mode
  DESCRIPTION:
    This type specifies the drive mode of the ECA turret drive.
  VISIBILITY: public  
  DERIVATION: enumerated
  MEMBERS:
    POWER - operate in POWER mode
    STAB  - operate in STABILIZED mode

ABSTRACT TYPE:  Speed
  DESCRIPTION:
    This type specifies the Speed mode of the ECA turret drive.
  VISIBILITY: public
  DERIVATION:  enumerated
  MEMBERS:  
    LOW_SPEED  = 0 - turret drive does not allow the maximum rate
    HIGH_SPEED = 1 - turret drive allows the maximum rate
  REQUIREMENTS:
    AIS 5.3.3 Turret High Speed Mode discrete

ABSTRACT TYPE:  State_t
  DESCRIPTION:
    This type specifies the state of the turret drive.
  VISIBILITY: private
  DERIVATION:  enumerated
  MEMBERS:  
    DEACTIVATED - turret drive is ON but deactivated
    ACTIVATED   - turret drive is ON and active

DATA ITEM DESCRIPTIONS:

DATA ITEM: Azimuth_displacement_control
  DESCRIPTION:
    This data item contains the Azimuth_displacement analog control
    information.
  TYPE: handle


DATA ITEM: Azimuth_rate_control
  DESCRIPTION:
    This data item contains the Azimuth_rate analog control
    information.
  TYPE: handle


DATA ITEM: Drift_adjust_control
  DESCRIPTION:
    This data item contains the Drift_adjust discrete control
    information.
  TYPE: handle


DATA ITEM: Drive_activate_control
  DESCRIPTION:
    This data item contains the activate drive relay control information. 
  TYPE: handle


DATA ITEM: Drive_activated_timestamp
  DESCRIPTION:
    This data item stores the timestamp of the last time the drive was
    activated.  It will be used to compute the activated time when a
    deactivate is received.
  TYPE: time_t
  DEFAULT: none
  NOTES:
    see C library for data type information 


DATA ITEM: Drive_on
  DESCRIPTION:
    The current status of the turret drive representing whether
    the drive is on or off.
  TYPE: boolean
  DEFAULT: FALSE


DATA ITEM: Drive_on_control
  DESCRIPTION:
    This data item contains the Drive_on discrete flag control
    information.
  TYPE: handle


DATA ITEM: Drive_system_hours
  DESCRIPTION:
    This data item stores the actual time the drive has been active.
    This value will be output by the Get_time method and will be set
    by the Set_time method.
  TYPE: integer 
  RANGE: 0 - +INFINITY 
  UNITS: seconds
  DEFAULT: 0


DATA ITEM: Elevation_rate_control
  DESCRIPTION:
    This data item contains the Elevation_rate analog control 
    information.
  TYPE: handle


DATA ITEM: Elevation_displacement_control
  DESCRIPTION:
    This data item contains the Elevation_displacement analog control
    information.
  TYPE: handle


DATA ITEM: Fault_control
  DESCRIPTION:
    This data item contains the Fault discrete flag control
    information.
  TYPE: handle


DATA ITEM: Remote_power_interlock_control
  DESCRIPTION:
    This data item contains the Remote_power_interlock relay control
    information.
  TYPE: handle


DATA ITEM: Stabilized_mode_control
  DESCRIPTION:
    This data item contains the Stabilized_mode discrete flag control
    information.
  TYPE: handle


DATA ITEM: State 
  DESCRIPTION:
    The current state of the turret drive representing an activated or
    deactivated drive.
  TYPE: State_t
  DEFAULT: DEACTIVATED
  

DATA ITEM: Thermal_fault_control
  DESCRIPTION:
    This data item contains the Thermal_fault discrete flag control
    information.
  TYPE: handle


DATA ITEM: Turret_power_interlock_control
  DESCRIPTION:
    This data item contains the Turret_power_interlock relay control
    information.
  TYPE: handle


DATA ITEM: Turret_power_interlock_status
  DESCRIPTION:
    The accurate status of the turret power interlock relay.  This value
    will be used to control the relay during a transition from the remote
    back to the turret console.
  TYPE: Interlock_status
  DEFAULT: none


METHOD DESCRIPTIONS:

METHOD:  Activate 
  DESCRIPTION:
    This method activates the turret drive system.  A timestamp of the
    activation is recorded to keep a running count of hours the drive has
    been active.
  VISIBILITY:  public
  INPUTS:  none
  OUTPUTS: none
  REQUIREMENTS:
    AFS 8.2.4 Set Palm Grips Active
    AIS 5.3.2 Palm Grips Active discrete
  PDL:

    if Drive_on
      if State is DEACTIVATED
        close the Drive_activate_control relay using the Relay object
        timestamp Drive_activated_timestamp for Drive_system_hours
        running total
        set State to ACTIVATED
      else
        already in the ACTIVATED state


METHOD:  Create
  DESCRIPTION:
    This method initializes the Turret Drive object.  All monitors of 
    turret drive signals are initialized and/or registered.  All turret
    drive characteristics are set to their initial values.
  VISIBILITY:  public
  INPUTS: none
  OUTPUTS: none
  PDL:

    set State to DEACTIVATED

    get Drive_system_hours to initialize the elapsed drive time
      from the Statistics (eeprom database) object - this is
        stored upon shutdown in eeprom

    create Fault_control using Discrete_flag object with the
     following attributes 
      true_sense     - DISCRETE_FLAG_TRUE_IS_1
      is_buffered    - false
      is_monitored   - true
      period         - 10ms
      change_routine - Change_fault
    change fault using initial value returned from create Fault_control
 
    create Thermal_fault_control using Discrete_flag object with
     the following attributes 
      true_sense     - DISCRETE_FLAG_TRUE_IS_0
      is_buffered    - false
      is_monitored   - true
      period         - 10ms
      change_routine - Change_thermal_fault
    change thermal fault using initial value returned from create
     Thermal_fault_control

    create Stabilized_mode_control using Discrete_flag object with the
     following attributes  
      true_sense     - DISCRETE_FLAG_TRUE_IS_1
      is_buffered    - false
      is_monitored   - true
      period         - 10ms 
      change_routine - Change_stabilized_mode
    change stabilized mode using initial value returned from create
     Stabilized_mode_control

    create Drive_on_control using Discrete_flag object with the 
     following attributes 
      true_sense     - DISCRETE_FLAG_TRUE_IS_1
      is_buffered    - false
      is_monitored   - true
      period         - 10ms
      change_routine - Change_drive_on
    change drive on using initial value returned from create
     Drive_on_control

    create Azimuth_rate_control using the Linear_dac object
     with the following attributes:
      voltage_low   - -10
      voltage_high  - +10
      logical_low   - -60
      logical_high  - +60
      is_buffered   - false
      period        - none
      initial_value - 0

    create Elevation_rate_control using the Linear_dac object with the
     following attributes:
      voltage_low   - -10
      voltage_high  - +10
      logical_low   - -60
      logical_high  - +60
      is_buffered   - false
      period        - none
      initial_value - 0

    create Azimuth_displacement_control using the Linear_adc object with
     the following attributes:
      voltage low    - 
      voltage high   - 
      logical low    - 
      logical high   - 
      is_buffered    - false 
      is_monitored   - false
      period         - none
      change_routine - none
      delta          - none

    create Elevation_displacement_control using the Linear_adc object 
     with the following attributes:
      voltage low    - 
      voltage high   - 
      logical low    - 
      logical high   - 
      is_buffered    - false
      is_monitored   - false
      period         - none
      change_routine - none
      delta          - none

    create Drive_activate_control using the Relay object with the
     following attributes:
      open_sense    - RELAY_OPEN_IS_0
      is_buffered   - false
      period        - none
      initial_state - RELAY_OPEN

    create Turret_power_interlock_control using the Relay object with 
     the following attributes:
      open_sense    - RELAY_OPEN_IS_0
      is_buffered   - false
      period        - none
      initial_state - RELAY_OPEN
    set Turret_power_interlock_status to OPEN

    create Remote_power_interlock_control using the Relay
     object with the following attributes:
      open_sense    - RELAY_OPEN_IS_0
      is_buffered   - false
      period        - none
      initial_state - RELAY_OPEN

    create Drift_adjust_control using the Discrete_out object with 
     following attributes:
      is_buffered   - false
      period        - 10ms
      initial_value - ADJUST_OFF


METHOD:  Deactivate
  DESCRIPTION:
    This method deactivates the turret drive system.  A timestamp of the
    activation is recorded to keep a running count of hours the drive
    has been active.
  VISIBILITY:  public
  INPUTS:  none
  OUTPUTS: none
  REQUIREMENTS:
    AFS 8.2.4 Set Plam Grips Active
    AIS 5.3.2 Palm Grips Active discrete
  PDL:

    if Drive_on
      if State is DEACTIVATED 
        timestamp deactivate for Drive_system_hours running total
        deactivate drive by opening the Drive_activate_control relay
        using the Relay object
        compute running time since last activate
        add running time to Drive_system_hours
        set State to DEACTIVATED
      else
        already in the DEACTIVATED state
 

METHOD:  Disable_remote_power_interlock
  DESCRIPTION:
    This method disables the turret power interlock safety preventing
    the turret from being driven by the remote's handstation. 
  VISIBILITY:  public
  INPUTS:  none
  OUTPUTS: none
  REQUIREMENTS:
    AFS 8.2.12 Control Power Interlock
  PDL:

    open the remote power interlock relay
    return the turret relay to its designated state defined by the
      Turret_power_interlock_status
 

METHOD:  Disable_turret_power_interlock
  DESCRIPTION:
    This method disables the turret power interlock safety preventing
    the turret from being driven by the turret's handstation. 
  VISIBILITY:  public
  INPUTS: none
  OUTPUTS: none
  REQUIREMENTS:
    AFS 8.2.12 Control Power Interlock
  PDL:

    Deactivate the Drive object
    open the turret power interlock relay
    set Turret_power_interlock_status to OPEN


METHOD:  Enable_remote_power_interlock
  DESCRIPTION:
    This method will enable the remote power interlock to allow
    the turret to be driven from the remote station.
  VISIBILITY:  public
  INPUTS: none
  OUTPUTS: none
  REQUIREMENTS:
    AFS 8.2.12 Control Power Interlock
  NOTES:
    At this time the turret's power interlock is opened to avoid
    having both relays closed at a time.
  PDL:

    if Drive_on
      open the turret power interlock relay using the Relay object
      close the remote power interlock relay using the Relay object


METHOD:  Enable_turret_power_interlock
  DESCRIPTION:
    This method will enable the turret power interlock to allow
    the turret to be driven from the turret station.
  VISIBILITY:  public
  INPUTS: none
  OUTPUTS: none
  REQUIREMENTS:
    AFS 8.2.12 Control Power Interlock
  NOTES:
    At this time the closed state is saved to allow the correct state
    to be returned to in the event a remote overrides the turret and the 
    remote gives up its control, we must return to the previous state of
    the turret power interlock.
  PDL:

    if Drive_on
      close the turret power interlock relay using the Relay object

      set the Turret_power_interlock_status to CLOSED to return to
        when remote gives up control


METHOD: Get_AZ_EL_displacement  
  DESCRIPTION:
    This method returns the current azimuth displacement and elevation
    displacement from the ECA.
  VISIBILITY:  public
  INPUTS:  none
  OUTPUTS:
    current azimuth displacement value
    current elevation displacement value
  REQUIREMENTS:
    AFS 8.2.11 Receive Azimuth Displacement and Elevation Displacement
    AIS 5.2.6 Azimuth Displacement analog
    AIS 5.2.7 Elevation Displacement analog
  NOTES:
    Currently supported as an on-demand type of signal.  May change to a
    monitored analog signal.
  PDL:
    
    if Drive_on
      get and return current azimuth displacement and current elevation
      displacement from ECA using the Linear_ADC object
 

METHOD:  Get_hours
  DESCRIPTION:
    This method returns the current value contained in the elapsed
    turret drive active time.
  VISIBILITY:  public
  INPUTS: none
  OUTPUTS:
    elapsed turret drive time - the current elasped turret drive time
  REQUIREMENTS:
    bogus requirements
  REFERENCES:
    AFS 4.11 Respond to CDT elapsed time indicator key
  NOTES:
    bogus notes
  PDL:

    return the current value for Drive_system_hours


METHOD:  Set_drift_adjust
  DESCRIPTION:
    This method provides the ability to set the drift adjust to the 
    value represented by the handstation.  Drift adjust is either ON
    or OFF.
  VISIBILITY:  public
  INPUTS:
    value - new drift adjust value 
      ADJUST_OFF - drift adjust mode is OFF
      ADJUST_ON  - drift adjust mode is ON
  OUTPUTS: none
  REQUIREMENTS:
    AFS 8.2.6 Capable of turret drift adjustment
    AIS 5.3.1 Drift Adjust discrete
  PDL:
 
    if Drive_on
      set drift adjust discrete to value using Discrete_out object

METHOD:  Set_hours
  DESCRIPTION:
    This method will set the turret drive activated elapsed time value
    to the specified value.
  VISIBILITY:  public
  INPUTS:
    new_elapsed_time_value - specified turret drive elapsed time value
  OUTPUTS: none
  REQUIREMENTS:
    AFS 4.11 Respond to CDT elapsed time indicator key
  PDL:
 
    set the value of Drive_system_hours to the value specified by
      new_elapsed_time_value
 

METHOD: Set_mode
  DESCRIPTION:
    This method sets the requested mode of the turret drive.
  VISIBILITY: public
  INPUTS:
    mode - new requested mode
      POWER - operating in POWER mode
      STAB  - operating in STABILIZED mode
  OUTPUTS:  none
  REQUIREMENTS:
    bogus requirements
  PDL:
    if mode is POWER
      set drive mode switchlight to PWR_LAMP using the Console object
    else if mode is STAB
      set drive mode switchlight to STB_LAMP using the Console object

    
METHOD:  Set_rate
  DESCRIPTION:
    This method provides the ability to send azimuth and elevation rates
    to the drive.  Two parameters are used as input to designate the
    corresponding rates.  These parameters must be in degrees per second.
  VISIBILITY:  public
  INPUTS:
    azimuth rate   - new azimuth rate in degress per second
    elevation rate - new elevation rate in degress per second
  OUTPUTS: none
  REQUIREMENTS:
    AFS 8.2.3 Send Azimuth and Elevation Rate to ECA
    AIS 5.3.4 Elevation Rate analog
    AIS 5.3.5 Azimuth Rate analog
  NOTES:
    May need to couple these analogs with an interface driver to close
    the window between writes and conversions.
  PDL:

    if Drive_on
      send the new azimuth rate to the Azimuth analog
      send the new elevation rate to Elevation analog


METHOD:  Set_speed
  DESCRIPTION:
    This method sets the turret drive speed mode to the specified value
    either high or low based on user input.
  VISIBILITY:  public
  INPUTS:
    new_speed_value - new value to set the turret speed to
      LOW_SPEED  - turret drive does not allow the maximum rate
      HIGH_SPEED - turret drive allows the maximum rate
  OUTPUTS: none
  REQUIREMENTS:
    AFS 8.2.5 Control Turret
    AIS 5.3.3 Turret High Speed Mode discrete
  PDL:

    if Drive_on
      set turret drive high speed mode discrete to the new_speed_value
       using Discrete_out object


METHOD:  Change_drive_on
  DESCRIPTION:
    This method will be used to report a transition of the drive system
    on discrete.
  VISIBILITY:  private
  INPUTS:
    value - new drive system on value 
      FALSE - indicates drive not on
      TRUE  - indicates drive on
  OUTPUTS: none
  REQUIREMENTS:
    AFS 8.2.9 Monitor Drive System On
    AIS 5.2.1 Drive System On discrete
  REFERENCES:
    bogus references
  NOTES:
    bogus notes
  PDL:

    set Drive_on to value
    if Drive_on is FALSE
      deactivate the turret drive using the Drive object


METHOD:  Change_fault
  DESCRIPTION:
    This method will be used to indicate that a turret malfunction
    has been recognized by the turret drive.
  VISIBILITY:  private
  INPUTS:
    value - new value resulting from a fault discrete change
      FALSE - indicates no fault
      TRUE  - indicates fault
  OUTPUTS: none
  REQUIREMENTS:
    AFS 8.2.7 Check for ECA failure indication
    AIS 5.2.4 Turret Drive Malfunction discrete
  PDL:
 
    if value
      send turret fault message using the Display object 
    else 
      return


METHOD:  Change_stabilized_mode
  DESCRIPTION:
    This method will determine a transition from STAB mode to POWER mode.
    It will set the stab mode lamp on the console.
  VISIBILITY:  private
  INPUTS:
    value - new value resulting from a stab mode discrete change
      FALSE - indicates not stabilized mode
      TRUE -  indicates stabilized mode
  OUTPUTS: none
  REQUIREMENTS:
    AFS 8.2.8 Monitor Turret Stab Mode Indicate
    AIS 5.2.3 Turret Stab Mode Indicate discrete
  PDL:

    if value 
      set STAB mode light to LAMP_ON using the Console object
    else value indicates not stabilized mode
      set STAB mode light to LAMP_OFF using the Console object


METHOD:  Change_thermal_fault
  DESCRIPTION:
    This method will be used to indicate that a thermal fault has been
     recognized by the turret drive.
  VISIBILITY:  private
  INPUTS:
    value - new value resulting from a thermal fault discrete change
      FALSE - indicates no thermal fault
      TRUE  - indicates thermal fault
  OUTPUTS: none
  REQUIREMENTS:
    AFS 8.2.7 Check for ECA failure indication
    AIS 5.2.2 Turret ECA Thermal Failure discrete
  PDL:
 
    if value
      send thermal fault message using the Display object
    else 
      return

TASK DESCRIPTIONS: none

ENDOBJECT: Drive
