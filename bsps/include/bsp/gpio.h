/**
 * @file
 *
 * @ingroup rtems_gpio
 *
 * @brief RTEMS GPIO API definition.
 */

/*
 *  Copyright (c) 2014-2015 Andre Marques <andre.lousa.marques at gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_SHARED_GPIO_H
#define LIBBSP_SHARED_GPIO_H

#include <bsp.h>
#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if !defined(BSP_GPIO_PIN_COUNT) || !defined(BSP_GPIO_PINS_PER_BANK)
  #error "BSP_GPIO_PIN_COUNT or BSP_GPIO_PINS_PER_BANK is not defined."
#endif

#if BSP_GPIO_PIN_COUNT <= 0 || BSP_GPIO_PINS_PER_BANK <= 0
  #error "Invalid BSP_GPIO_PIN_COUNT or BSP_GPIO_PINS_PER_BANK."
#endif

#if BSP_GPIO_PINS_PER_BANK > 32
  #error "Invalid BSP_GPIO_PINS_PER_BANK. Must be in the range of 1 to 32."
#endif

#define GPIO_LAST_BANK_PINS BSP_GPIO_PIN_COUNT % BSP_GPIO_PINS_PER_BANK

#if GPIO_LAST_BANK_PINS > 0
  #define GPIO_BANK_COUNT (BSP_GPIO_PIN_COUNT / BSP_GPIO_PINS_PER_BANK) + 1
#else
  #define GPIO_BANK_COUNT BSP_GPIO_PIN_COUNT / BSP_GPIO_PINS_PER_BANK
  #undef GPIO_LAST_BANK_PINS
  #define GPIO_LAST_BANK_PINS BSP_GPIO_PINS_PER_BANK
#endif

#if defined(BSP_GPIO_PINS_PER_SELECT_BANK) && BSP_GPIO_PINS_PER_SELECT_BANK > 32
  #error "Invalid BSP_GPIO_PINS_PER_SELECT_BANK. Must under and including 32."
#elif defined(BSP_GPIO_PINS_PER_SELECT_BANK) <= 32
  #define GPIO_SELECT_BANK_COUNT \
    BSP_GPIO_PINS_PER_BANK / BSP_GPIO_PINS_PER_SELECT_BANK
#endif

#define INTERRUPT_SERVER_PRIORITY 1
#define INTERRUPT_SERVER_STACK_SIZE 2 * RTEMS_MINIMUM_STACK_SIZE
#define INTERRUPT_SERVER_MODES RTEMS_TIMESLICE | RTEMS_PREEMPT
#define INTERRUPT_SERVER_ATTRIBUTES RTEMS_DEFAULT_ATTRIBUTES

#define GPIO_INPUT_ERROR ~0

/**
 * @name GPIO data structures
 *
 * @{
 */

/**
 * @brief The set of possible configurations for a GPIO pull-up resistor.
 *
 * Enumerated type to define the possible pull-up resistor configurations
 * for a GPIO pin.
 */
typedef enum
{
  PULL_UP = 1,
  PULL_DOWN,
  NO_PULL_RESISTOR
} rtems_gpio_pull_mode;

/**
 * @brief The set of possible functions a pin can have.
 *
 * Enumerated type to define a pin function.
 */
typedef enum
{
  DIGITAL_INPUT = 0,
  DIGITAL_OUTPUT,
  BSP_SPECIFIC,
  NOT_USED
} rtems_gpio_function;

/**
 * @brief The set of possible interrupts a GPIO pin can generate.
 *
 * Enumerated type to define a GPIO pin interrupt.
 */
typedef enum
{
  FALLING_EDGE = 0,
  RISING_EDGE,
  LOW_LEVEL,
  HIGH_LEVEL,
  BOTH_EDGES,
  BOTH_LEVELS,
  NONE
} rtems_gpio_interrupt;

/**
 * @brief The set of possible handled states an user-defined interrupt
 *        handler can return.
 *
 * Enumerated type to define an interrupt handler handled state.
 */
typedef enum
{
  IRQ_HANDLED,
  IRQ_NONE
} rtems_gpio_irq_state;

/**
 * @brief The set of flags to specify an user-defined interrupt handler
 *        uniqueness on a GPIO pin.
 *
 * Enumerated type to define an interrupt handler shared flag.
 */
typedef enum
{
  SHARED_HANDLER,
  UNIQUE_HANDLER
} rtems_gpio_handler_flag;

/**
 * @brief Object containing relevant information for assigning a BSP specific
 *        function to a pin.
 *
 * Encapsulates relevant data for a BSP specific GPIO function.
 */
typedef struct
{
  /* The BSP defined function code. */
  uint32_t io_function;

  void *pin_data;
} rtems_gpio_specific_data;

/**
 * @brief Object containing configuration information
 *        regarding interrupts.
 */
typedef struct
{
  rtems_gpio_interrupt active_interrupt;

  rtems_gpio_handler_flag handler_flag;

  bool threaded_interrupts;

  /* Interrupt handler function. */
  rtems_gpio_irq_state (*handler) (void *arg);

  /* Interrupt handler function arguments. */
  void *arg;

  /* Software switch debounce settings. It should contain the amount of clock
   * ticks that must pass between interrupts to ensure that the interrupt
   * was not caused by a switch bounce.
   * If set to 0 this feature is disabled . */
  uint32_t debounce_clock_tick_interval;
} rtems_gpio_interrupt_configuration;

/**
 * @brief Object containing configuration information
 *        to request/update a GPIO pin.
 */
typedef struct
{
  /* Processor pin number. */
  uint32_t pin_number;
  rtems_gpio_function function;

  /* Pull resistor setting. */
  rtems_gpio_pull_mode pull_mode;

  /* If digital out pin, set to TRUE to set the pin to logical high,
   * or FALSE for logical low. If not a digital out then this
   * is ignored. */
  bool output_enabled;

  /* If true inverts digital in/out applicational logic. */
  bool logic_invert;

  /* Pin interrupt configuration. Should be NULL if not used. */
  rtems_gpio_interrupt_configuration *interrupt;

  /* Structure with BSP specific data, to use during the pin request.
   * If function == BSP_SPECIFIC this should have a pointer to
   * a rtems_gpio_specific_data structure.
   *
   * If not this field may be NULL. This is passed to the BSP function
   * so any BSP specific data can be passed to it through this pointer. */
  void *bsp_specific;
} rtems_gpio_pin_conf;

/**
 * @brief Object containing configuration information
 *        to assign GPIO functions to multiple pins
 *        at the same time. To be used by BSP code only.
 */
typedef struct
{
  /* Global GPIO pin number. */
  uint32_t pin_number;

  /* RTEMS GPIO pin function code. */
  rtems_gpio_function function;

  /* BSP specific function code. Only used if function == BSP_SPECIFIC */
  uint32_t io_function;

  /* BSP specific data. */
  void *bsp_specific;
} rtems_gpio_multiple_pin_select;

/**
 * @brief Object containing configuration information
 *        to request a GPIO pin group.
 */
typedef struct
{
  const rtems_gpio_pin_conf *digital_inputs;
  uint32_t input_count;

  const rtems_gpio_pin_conf *digital_outputs;
  uint32_t output_count;

  const rtems_gpio_pin_conf *bsp_specifics;
  uint32_t bsp_specific_pin_count;
} rtems_gpio_group_definition;

/**
 * @brief Opaque type for a GPIO pin group.
 */
typedef struct rtems_gpio_group rtems_gpio_group;

/** @} */

/**
 * @name gpio Usage
 *
 * @{
 */

/**
 * @brief Initializes the GPIO API.
 *
 * @retval RTEMS_SUCCESSFUL API successfully initialized.
 * @retval * @see rtems_semaphore_create().
 */
extern rtems_status_code rtems_gpio_initialize(void);

/**
 * @brief Instantiates a GPIO pin group.
 *        To define the group @see rtems_gpio_define_pin_group().
 *
 * @retval rtems_gpio_group pointer.
 */
extern rtems_gpio_group *rtems_gpio_create_pin_group(void);

/**
 * @brief Requests a GPIO pin group configuration.
 *
 * @param[in] group_definition rtems_gpio_group_definition structure filled with
 *                             the group pins configurations.
 * @param[out] group Reference to the created group.
 *
 * @retval RTEMS_SUCCESSFUL Pin group was configured successfully.
 * @retval RTEMS_UNSATISFIED @var group_definition or @var group is NULL,
 *                           the @var pins are not from the same bank,
 *                           no pins were defined or could not satisfy at
 *                           least one given configuration.
 * @retval RTEMS_RESOURCE_IN_USE At least one pin is already being used.
 * @retval * @see rtems_semaphore_create().
 */
extern rtems_status_code rtems_gpio_define_pin_group(
  const rtems_gpio_group_definition *group_definition,
  rtems_gpio_group *group
);

/**
 * @brief Writes a value to the group's digital outputs. The pins order
 *        is as defined in the group definition.
 *
 * @param[in] data Data to write/send.
 * @param[in] group Reference to the group.
 *
 * @retval RTEMS_SUCCESSFUL Data successfully written.
 * @retval RTEMS_NOT_DEFINED Group has no output pins.
 * @retval RTEMS_UNSATISFIED Could not operate on at least one of the pins.
 */
extern rtems_status_code rtems_gpio_write_group(
  uint32_t data,
  rtems_gpio_group *group
);

/**
 * @brief Reads the value/level of the group's digital inputs. The pins order
 *        is as defined in the group definition.
 *
 * @param[in] group Reference to the group.
 *
 * @retval The function returns a 32-bit bitmask with the group's input pins
 *         current logical values.
 * @retval GPIO_INPUT_ERROR Group has no input pins.
 */
extern uint32_t rtems_gpio_read_group(rtems_gpio_group *group);

/**
 * @brief Performs a BSP specific operation on a group of pins. The pins order
 *        is as defined in the group definition.
 *
 * @param[in] group Reference to the group.
 * @param[in] arg Pointer to a BSP defined structure with BSP-specific
 *                data. This field is handled by the BSP.
 *
 * @retval RTEMS_SUCCESSFUL Operation completed with success.
 * @retval RTEMS_NOT_DEFINED Group has no BSP specific pins, or the BSP does not
 *                           support BSP specific operations for groups.
 * @retval RTEMS_UNSATISFIED Could not operate on at least one of the pins.
 */
extern rtems_status_code rtems_gpio_group_bsp_specific_operation(
  rtems_gpio_group *group,
  void *arg
);

/**
 * @brief Requests a GPIO pin configuration.
 *
 * @param[in] conf rtems_gpio_pin_conf structure filled with the pin information
 *                 and desired configurations.
 *
 * @retval RTEMS_SUCCESSFUL Pin was configured successfully.
 * @retval RTEMS_UNSATISFIED Could not satisfy the given configuration.
 */
extern rtems_status_code rtems_gpio_request_configuration(
  const rtems_gpio_pin_conf *conf
);

/**
 * @brief Updates the current configuration of a GPIO pin .
 *
 * @param[in] conf rtems_gpio_pin_conf structure filled with the pin information
 *                 and desired configurations.
 *
 * @retval RTEMS_SUCCESSFUL Pin configuration was updated successfully.
 * @retval RTEMS_INVALID_ID Pin number is invalid.
 * @retval RTEMS_NOT_CONFIGURED The pin is not being used.
 * @retval RTEMS_UNSATISFIED Could not update the pin's configuration.
 */
extern rtems_status_code rtems_gpio_update_configuration(
  const rtems_gpio_pin_conf *conf
);

/**
 * @brief Sets multiple output GPIO pins with the logical high.
 *
 * @param[in] pin_numbers Array with the GPIO pin numbers to set.
 * @param[in] count Number of GPIO pins to set.
 *
 * @retval RTEMS_SUCCESSFUL All pins were set successfully.
 * @retval RTEMS_INVALID_ID At least one pin number is invalid.
 * @retval RTEMS_NOT_CONFIGURED At least one of the received pins
 *                              is not configured as a digital output.
 * @retval RTEMS_UNSATISFIED Could not set the GPIO pins.
 */
extern rtems_status_code rtems_gpio_multi_set(
  uint32_t *pin_numbers,
  uint32_t pin_count
);

/**
 * @brief Sets multiple output GPIO pins with the logical low.
 *
 * @param[in] pin_numbers Array with the GPIO pin numbers to clear.
 * @param[in] count Number of GPIO pins to clear.
 *
 * @retval RTEMS_SUCCESSFUL All pins were cleared successfully.
 * @retval RTEMS_INVALID_ID At least one pin number is invalid.
 * @retval RTEMS_NOT_CONFIGURED At least one of the received pins
 *                              is not configured as a digital output.
 * @retval RTEMS_UNSATISFIED Could not clear the GPIO pins.
 */
extern rtems_status_code rtems_gpio_multi_clear(
  uint32_t *pin_numbers,
  uint32_t pin_count
);

/**
 * @brief Returns the value (level) of multiple GPIO input pins.
 *
 * @param[in] pin_numbers Array with the GPIO pin numbers to read.
 * @param[in] count Number of GPIO pins to read.
 *
 * @retval Bitmask with the values of the corresponding pins.
 *         0 for logical low and 1 for logical high.
 * @retval GPIO_INPUT_ERROR Could not read at least one pin level.
 */
extern uint32_t rtems_gpio_multi_read(
  uint32_t *pin_numbers,
  uint32_t pin_count
);

/**
 * @brief Sets an output GPIO pin with the logical high.
 *
 * @param[in] pin_number GPIO pin number.
 *
 * @retval RTEMS_SUCCESSFUL Pin was set successfully.
 * @retval RTEMS_INVALID_ID Pin number is invalid.
 * @retval RTEMS_NOT_CONFIGURED The received pin is not configured
 *                              as a digital output.
 * @retval RTEMS_UNSATISFIED Could not set the GPIO pin.
 */
extern rtems_status_code rtems_gpio_set(uint32_t pin_number);

/**
 * @brief Sets an output GPIO pin with the logical low.
 *
 * @param[in] pin_number GPIO pin number.
 *
 * @retval RTEMS_SUCCESSFUL Pin was cleared successfully.
 * @retval RTEMS_INVALID_ID Pin number is invalid.
 * @retval RTEMS_NOT_CONFIGURED The received pin is not configured
 *                              as a digital output.
 * @retval RTEMS_UNSATISFIED Could not clear the GPIO pin.
 */
extern rtems_status_code rtems_gpio_clear(uint32_t pin_number);

/**
 * @brief Returns the value (level) of a GPIO input pin.
 *
 * @param[in] pin_number GPIO pin number.
 *
 * @retval The function returns 0 or 1 depending on the pin current
 *         logical value.
 * @retval -1 Pin number is invalid, or not a digital input pin.
 */
extern int rtems_gpio_get_value(uint32_t pin_number);

/**
 * @brief Requests multiple GPIO pin configurations. If the BSP provides
 *        support for parallel selection each call to this function will
 *        result in a single call to the GPIO hardware, else each pin
 *        configuration will be done in individual and sequential calls.
 *        All pins must belong to the same GPIO bank.
 *
 * @param[in] pins Array of rtems_gpio_pin_conf structures filled with the pins
 *                 information and desired configurations. All pins must belong
 *                 to the same GPIO bank.
 * @param[in] pin_count Number of pin configurations in the @var pins array.
 *
 * @retval RTEMS_SUCCESSFUL All pins were configured successfully.
 * @retval RTEMS_INVALID_ID At least one pin number in the @var pins array
 *                          is invalid.
 * @retval RTEMS_RESOURCE_IN_USE At least one pin is already being used.
 * @retval RTEMS_UNSATISFIED Could not satisfy at least one given configuration.
 */
extern rtems_status_code rtems_gpio_multi_select(
  const rtems_gpio_pin_conf *pins,
  uint8_t pin_count
);

/**
 * @brief Assigns a certain function to a GPIO pin.
 *
 * @param[in] pin_number GPIO pin number.
 * @param[in] function The new function for the pin.
 * @param[in] output_enabled If TRUE and @var function is DIGITAL_OUTPUT,
 *                           then the pin is set with the logical high.
 *                           Otherwise it is set with logical low.
 * @param[in] logic_invert Reverses the digital I/O logic for DIGITAL_INPUT
 *                         and DIGITAL_OUTPUT pins.
 * @param[in] bsp_specific Pointer to a BSP defined structure with BSP-specific
 *                         data. This field is handled by the BSP.
 *
 * @retval RTEMS_SUCCESSFUL Pin was configured successfully.
 * @retval RTEMS_INVALID_ID Pin number is invalid.
 * @retval RTEMS_RESOURCE_IN_USE The received pin is already being used.
 * @retval RTEMS_UNSATISFIED Could not assign the GPIO function.
 * @retval RTEMS_NOT_DEFINED GPIO function not defined, or NOT_USED.
 */
extern rtems_status_code rtems_gpio_request_pin(
  uint32_t pin_number,
  rtems_gpio_function function,
  bool output_enable,
  bool logic_invert,
  void *bsp_specific
);

/**
 * @brief Configures a single GPIO pin pull resistor.
 *
 * @param[in] pin_number GPIO pin number.
 * @param[in] mode The pull resistor mode.
 *
 * @retval RTEMS_SUCCESSFUL Pull resistor successfully configured.
 * @retval RTEMS_INVALID_ID Pin number is invalid.
 * @retval RTEMS_UNSATISFIED Could not set the pull mode.
 */
extern rtems_status_code rtems_gpio_resistor_mode(
  uint32_t pin_number,
  rtems_gpio_pull_mode mode
);

/**
 * @brief Releases a GPIO pin, making it available to be used again.
 *
 * @param[in] pin_number GPIO pin number.
 *
 * @retval RTEMS_SUCCESSFUL Pin successfully disabled.
 * @retval RTEMS_INVALID_ID Pin number is invalid.
 * @retval * Could not disable an active interrupt on this pin,
 *           @see rtems_gpio_disable_interrupt().
 */
extern rtems_status_code rtems_gpio_release_pin(uint32_t pin_number);

/**
 * @brief Releases a GPIO pin, making it available to be used again.
 *
 * @param[in] conf GPIO pin configuration to be released.
 *
 * @retval RTEMS_SUCCESSFUL Pin successfully disabled.
 * @retval RTEMS_UNSATISFIED Pin configuration is NULL.
 * @retval * @see rtems_gpio_release_pin().
 */
extern rtems_status_code rtems_gpio_release_configuration(
  const rtems_gpio_pin_conf *conf
);

/**
 * @brief Releases multiple GPIO pins, making them available to be used again.
 *
 * @param[in] pins Array of rtems_gpio_pin_conf structures.
 * @param[in] pin_count Number of pin configurations in the @var pins array.
 *
 * @retval RTEMS_SUCCESSFUL Pins successfully disabled.
 * @retval RTEMS_UNSATISFIED @var pins array is NULL.
 * @retval * @see rtems_gpio_release_pin().
 */
extern rtems_status_code rtems_gpio_release_multiple_pins(
  const rtems_gpio_pin_conf *pins,
  uint32_t pin_count
);

/**
 * @brief Releases a GPIO pin group, making the pins used available to be
 *        repurposed.
 *
 * @param[in] conf GPIO pin configuration to be released.
 *
 * @retval RTEMS_SUCCESSFUL Pins successfully disabled.
 * @retval * @see rtems_gpio_release_pin(), @see rtems_semaphore_delete() or
 *           @see rtems_semaphore_flush().
 */
extern rtems_status_code rtems_gpio_release_pin_group(
  rtems_gpio_group *group
);

/**
 * @brief Attaches a debouncing function to a given pin/switch.
 *        Debouncing is done by requiring a certain number of clock ticks to
 *        pass between interrupts. Any interrupt fired too close to the last
 *        will be ignored as it is probably the result of an involuntary
 *        switch/button bounce after being released.
 *
 * @param[in] pin_number GPIO pin number.
 * @param[in] ticks Minimum number of clock ticks that must pass between
 *                  interrupts so it can be considered a legitimate
 *                  interrupt.
 *
 * @retval RTEMS_SUCCESSFUL Debounce function successfully attached to the pin.
 * @retval RTEMS_INVALID_ID Pin number is invalid.
 * @retval RTEMS_NOT_CONFIGURED The current pin is not configured as a digital
 *                              input, hence it can not be connected to a switch,
 *                              or interrupts are not enabled for this pin.
 */
extern rtems_status_code rtems_gpio_debounce_switch(
  uint32_t pin_number,
  int ticks
);

/**
 * @brief Connects a new user-defined interrupt handler to a given pin.
 *
 * @param[in] pin_number GPIO pin number.
 * @param[in] handler Pointer to a function that will be called every time
 *                    the enabled interrupt for the given pin is generated.
 *                    This function must return information about its
 *                    handled/unhandled state.
 * @param[in] arg Void pointer to the arguments of the user-defined handler.
 *
 * @retval RTEMS_SUCCESSFUL Handler successfully connected to this pin.
 * @retval RTEMS_NO_MEMORY Could not connect more user-defined handlers to
 *                         the given pin.
 * @retval RTEMS_NOT_CONFIGURED The given pin has no interrupt configured.
 * @retval RTEMS_INVALID_ID Pin number is invalid.
 * @retval RTEMS_TOO_MANY The pin's current handler is set as unique.
 * @retval RTEMS_RESOURCE_IN_USE The current user-defined handler for this pin
 *                               is unique.
 */
extern rtems_status_code rtems_gpio_interrupt_handler_install(
  uint32_t pin_number,
  rtems_gpio_irq_state (*handler) (void *arg),
  void *arg
);

/**
 * @brief Enables interrupts to be generated on a given GPIO pin.
 *        When fired that interrupt will call the given handler.
 *
 * @param[in] pin_number GPIO pin number.
 * @param[in] interrupt Type of interrupt to enable for the pin.
 * @param[in] flag Defines the uniqueness of the interrupt handler for the pin.
 * @param[in] threaded_handling Defines if the handler should be called from a
 *                              thread/task or from normal ISR contex.
 * @param[in] handler Pointer to a function that will be called every time
 *                    @var interrupt is generated. This function must return
 *                    information about its handled/unhandled state.
 * @param[in] arg Void pointer to the arguments of the user-defined handler.
 *
 * @retval RTEMS_SUCCESSFUL Interrupt successfully enabled for this pin.
 * @retval RTEMS_UNSATISFIED Could not install the GPIO ISR, create/start
 *                           the handler task, or enable the interrupt
 *                           on the pin.
 * @retval RTEMS_INVALID_ID Pin number is invalid.
 * @retval RTEMS_NOT_CONFIGURED The received pin is not configured
 *                              as a digital input, the pin is on a
 *                              pin grouping.
 * @retval RTEMS_RESOURCE_IN_USE The pin already has an enabled interrupt,
 *                               or the handler threading policy does not match
 *                               the bank's policy.
 * @retval RTEMS_NO_MEMORY Could not store the pin's interrupt configuration.
 */
extern rtems_status_code rtems_gpio_enable_interrupt(
  uint32_t pin_number,
  rtems_gpio_interrupt interrupt,
  rtems_gpio_handler_flag flag,
  bool threaded_handling,
  rtems_gpio_irq_state (*handler) (void *arg),
  void *arg
);

/**
 * @brief Disconnects an user-defined interrupt handler from the given pin.
 *        If in the end there are no more user-defined handlers connected
 *        to the pin, interrupts are disabled on the given pin.
 *
 * @param[in] pin_number GPIO pin number.
 * @param[in] handler Pointer to the user-defined handler
 * @param[in] arg Void pointer to the arguments of the user-defined handler.
 *
 * @retval RTEMS_SUCCESSFUL Handler successfully disconnected from this pin.
 * @retval RTEMS_INVALID_ID Pin number is invalid.
 * @retval RTEMS_NOT_CONFIGURED Pin has no active interrupts.
 * @retval * @see rtems_gpio_disable_interrupt()
 */
extern rtems_status_code rtems_gpio_interrupt_handler_remove(
  uint32_t pin_number,
  rtems_gpio_irq_state (*handler) (void *arg),
  void *arg
);

/**
 * @brief Stops interrupts from being generated on a given GPIO pin
 *        and removes the corresponding handler.
 *
 * @param[in] pin_number GPIO pin number.
 *
 * @retval RTEMS_SUCCESSFUL Interrupt successfully disabled for this pin.
 * @retval RTEMS_INVALID_ID Pin number is invalid.
 * @retval RTEMS_NOT_CONFIGURED Pin has no active interrupts.
 * @retval RTEMS_UNSATISFIED Could not remove the current interrupt handler,
 *                           could not recognize the current active interrupt
 *                           on this pin or could not disable interrupts on
 *                           this pin.
 */
extern rtems_status_code rtems_gpio_disable_interrupt(uint32_t pin_number);

/**
 * @brief Sets multiple output GPIO pins with the logical high.
 *        This must be implemented by each BSP.
 *
 * @param[in] bank GPIO bank number.
 * @param[in] bitmask Bitmask of GPIO pins to set in the given bank.
 *
 * @retval RTEMS_SUCCESSFUL All pins were set successfully.
 * @retval RTEMS_UNSATISFIED Could not set at least one of the pins.
 */
extern rtems_status_code rtems_gpio_bsp_multi_set(
  uint32_t bank,
  uint32_t bitmask
);

/**
 * @brief Sets multiple output GPIO pins with the logical low.
 *        This must be implemented by each BSP.
 *
 * @param[in] bank GPIO bank number.
 * @param[in] bitmask Bitmask of GPIO pins to clear in the given bank.
 *
 * @retval RTEMS_SUCCESSFUL All pins were cleared successfully.
 * @retval RTEMS_UNSATISFIED Could not clear at least one of the pins.
 */
extern rtems_status_code rtems_gpio_bsp_multi_clear(
  uint32_t bank,
  uint32_t bitmask
);

/**
 * @brief Returns the value (level) of multiple GPIO input pins.
 *        This must be implemented by each BSP.
 *
 * @param[in] bank GPIO bank number.
 * @param[in] bitmask Bitmask of GPIO pins to read in the given bank.
 *
 * @retval The function must return a bitmask with the values of the
 *         corresponding pins. 0 for logical low and 1 for logical high.
 * @retval GPIO_INPUT_ERROR Could not read at least one pin level.
 */
extern uint32_t rtems_gpio_bsp_multi_read(uint32_t bank, uint32_t bitmask);

/**
 * @brief Performs a BSP specific operation on a group of pins.
 *        The implementation for this function may be omitted if the target
 *        does not support the feature, by returning RTEMS_NOT_DEFINED.
 *
 * @param[in] bank GPIO bank number.
 * @param[in] pins Array filled with BSP specific pin numbers. All pins belong
 *                 to the same select bank.
 * @param[in] pin_count Number of pin configurations in the @var pins array.
 * @param[in] arg Pointer to a BSP defined structure with BSP-specific
 *                data. This field is handled by the BSP.
 *
 * @retval RTEMS_SUCCESSFUL Operation completed with success.
 * @retval RTEMS_NOT_DEFINED Group has no BSP specific pins, or the BSP does not
 *                           support BSP specific operations for groups.
 * @retval RTEMS_UNSATISFIED Could not operate on at least one of the pins.
 */
extern rtems_status_code rtems_gpio_bsp_specific_group_operation(
  uint32_t bank,
  uint32_t *pins,
  uint32_t pin_count,
  void *arg
);

/**
 * @brief Assigns GPIO functions to all the given pins in a single register
 *        operation.
 *        The implementation for this function may be omitted if the target
 *        does not support the feature, by returning RTEMS_NOT_DEFINED.
 *
 * @param[in] pins Array of rtems_gpio_multiple_pin_select structures filled
 *                 with the pins desired functions. All pins belong to the
 *                 same select bank.
 * @param[in] pin_count Number of pin configurations in the @var pins array.
 * @param[in] select_bank Select bank number of the received pins.
 *
 * @retval RTEMS_SUCCESSFUL Functions were assigned successfully.
 * @retval RTEMS_NOT_DEFINED The BSP does not support multiple pin function
 *                           assignment.
 * @retval RTEMS_UNSATISFIED Could not assign the functions to the pins.
 */
extern rtems_status_code rtems_gpio_bsp_multi_select(
  rtems_gpio_multiple_pin_select *pins,
  uint32_t pin_count,
  uint32_t select_bank
);

/**
 * @brief Sets an output GPIO pin with the logical high.
 *        This must be implemented by each BSP.
 *
 * @param[in] bank GPIO bank number.
 * @param[in] pin GPIO pin number within the given bank.
 *
 * @retval RTEMS_SUCCESSFUL Pin was set successfully.
 * @retval RTEMS_UNSATISFIED Could not set the given pin.
 */
extern rtems_status_code rtems_gpio_bsp_set(uint32_t bank, uint32_t pin);

/**
 * @brief Sets an output GPIO pin with the logical low.
 *        This must be implemented by each BSP.
 *
 * @param[in] bank GPIO bank number.
 * @param[in] pin GPIO pin number within the given bank.
 *
 * @retval RTEMS_SUCCESSFUL Pin was cleared successfully.
 * @retval RTEMS_UNSATISFIED Could not clear the given pin.
 */
extern rtems_status_code rtems_gpio_bsp_clear(uint32_t bank, uint32_t pin);

/**
 * @brief Returns the value (level) of a GPIO input pin.
 *        This must be implemented by each BSP.
 *
 * @param[in] bank GPIO bank number.
 * @param[in] pin GPIO pin number within the given bank.
 *
 * @retval The function must return 0 if the pin level is a logical low,
 *         or non zero if it has a logical high.
 * @retval GPIO_INPUT_ERROR Could not read the pin level.
 */
extern uint32_t rtems_gpio_bsp_get_value(uint32_t bank, uint32_t pin);

/**
 * @brief Assigns the digital input function to the given pin.
 *        This must be implemented by each BSP.
 *
 * @param[in] bank GPIO bank number.
 * @param[in] pin GPIO pin number within the given bank.
 * @param[in] bsp_specific Pointer to a BSP defined structure with BSP-specific
 *                         data.
 *
 * @retval RTEMS_SUCCESSFUL Function was assigned successfully.
 * @retval RTEMS_UNSATISFIED Could not assign the function to the pin.
 */
extern rtems_status_code rtems_gpio_bsp_select_input(
  uint32_t bank,
  uint32_t pin,
  void *bsp_specific
);

/**
 * @brief Assigns the digital output function to the given pin.
 *        This must be implemented by each BSP.
 *
 * @param[in] bank GPIO bank number.
 * @param[in] pin GPIO pin number within the given bank.
 * @param[in] bsp_specific Pointer to a BSP defined structure with BSP-specific
 *                         data.
 *
 * @retval RTEMS_SUCCESSFUL Function was assigned successfully.
 * @retval RTEMS_UNSATISFIED Could not assign the function to the pin.
 */
extern rtems_status_code rtems_gpio_bsp_select_output(
  uint32_t bank,
  uint32_t pin,
  void *bsp_specific
);

/**
 * @brief Assigns a BSP specific function to the given pin.
 *        This must be implemented by each BSP.
 *
 * @param[in] bank GPIO bank number.
 * @param[in] pin GPIO pin number within the given bank.
 * @param[in] function BSP defined GPIO function.
 * @param[in] pin_data Pointer to a BSP defined structure with BSP-specific
 *                     data.
 *
 * @retval RTEMS_SUCCESSFUL Function was assigned successfully.
 * @retval RTEMS_UNSATISFIED Could not assign the function to the pin.
 */
extern rtems_status_code rtems_gpio_bsp_select_specific_io(
  uint32_t bank,
  uint32_t pin,
  uint32_t function,
  void *pin_data
);

/**
 * @brief Configures a single GPIO pin pull resistor.
 *        This must be implemented by each BSP.
 *
 * @param[in] bank GPIO bank number.
 * @param[in] pin GPIO pin number within the given bank.
 * @param[in] mode The pull resistor mode.
 *
 * @retval RTEMS_SUCCESSFUL Pull resistor successfully configured.
 * @retval RTEMS_UNSATISFIED Could not set the pull mode.
 */
extern rtems_status_code rtems_gpio_bsp_set_resistor_mode(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_pull_mode mode
);

/**
 * @brief Reads and returns a vector/bank interrupt event line.
 *        The bitmask should indicate with a 1 if the corresponding pin
 *        as a pending interrupt, or 0 if otherwise. The function
 *        should clear the interrupt event line before returning.
 *        This must be implemented by each BSP.
 *
 * @param[in] vector GPIO vector/bank.
 *
 * @retval Bitmask (max 32-bit) representing a GPIO bank, where a bit set
 *         indicates an active interrupt on that pin.
 */
extern uint32_t rtems_gpio_bsp_interrupt_line(rtems_vector_number vector);

/**
 * @brief Calculates a vector number for a given GPIO bank.
 *        This must be implemented by each BSP.
 *
 * @param[in] bank GPIO bank number.
 *
 * @retval The corresponding rtems_vector_number.
 */
extern rtems_vector_number rtems_gpio_bsp_get_vector(uint32_t bank);

/**
 * @brief Enables interrupts to be generated on a given GPIO pin.
 *        This must be implemented by each BSP.
 *
 * @param[in] bank GPIO bank number.
 * @param[in] pin GPIO pin number within the given bank.
 * @param[in] interrupt Type of interrupt to enable for the pin.
 *
 * @retval RTEMS_SUCCESSFUL Interrupt successfully enabled for this pin.
 * @retval RTEMS_UNSATISFIED Could not enable the interrupt on the pin.
 */
extern rtems_status_code rtems_gpio_bsp_enable_interrupt(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_interrupt interrupt
);

/**
 * @brief Stops interrupts from being generated on a given GPIO pin.
 *        This must be implemented by each BSP.
 *
 * @param[in] bank GPIO bank number.
 * @param[in] pin GPIO pin number within the given bank.
 * @param[in] active_interrupt Interrupt type currently active on this pin.
 *
 * @retval RTEMS_SUCCESSFUL Interrupt successfully disabled for this pin.
 * @retval RTEMS_UNSATISFIED Could not disable interrupts on this pin.
 */
extern rtems_status_code rtems_gpio_bsp_disable_interrupt(
  uint32_t bank,
  uint32_t pin,
  rtems_gpio_interrupt interrupt
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_GPIO_H */
