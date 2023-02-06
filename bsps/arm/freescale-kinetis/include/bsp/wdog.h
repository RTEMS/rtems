#ifndef KINETIS_WATCH_DOG_H
#define KINETIS_WATCH_DOG_H

/** WDOG - Register Layout Typedef */
typedef struct {
  uint16_t stat_ctrl_high;                    /**< Watchdog Status and Control Register High, offset: 0x0 */
  uint16_t stat_ctrl_low;                     /**< Watchdog Status and Control Register Low, offset: 0x2 */
  uint16_t timeout_val_high;                  /**< Watchdog Time-out Value Register High, offset: 0x4 */
  uint16_t timeout_val_low;                   /**< Watchdog Time-out Value Register Low, offset: 0x6 */
  uint16_t window_high;                       /**< Watchdog Window Register High, offset: 0x8 */
  uint16_t window_low;                        /**< Watchdog Window Register Low, offset: 0xA */
  uint16_t refresh;                           /**< Watchdog Refresh register, offset: 0xC */
  uint16_t unlock;                            /**< Watchdog Unlock register, offset: 0xE */
  uint16_t timer_out_high;                    /**< Watchdog Timer Output Register High, offset: 0x10 */
  uint16_t timer_out_low;                     /**< Watchdog Timer Output Register Low, offset: 0x12 */
  uint16_t reset_count;                       /**< Watchdog Reset Count register, offset: 0x14 */
  uint16_t prescaler;                         /**< Watchdog Prescaler register, offset: 0x16 */
} kinetis_wdog_t;

/* 
 * WDOG - Peripheral instance base addresses
 */
#define KINETIS_WDOG  ((kinetis_wdog_t*)0x40052000u)

#endif