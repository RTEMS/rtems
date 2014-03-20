/**
 * @file
 *
 * @brief Header File for Status Checks
 *
 * @warning Do not include this file in other header files.  Use it only in
 * source files.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef RTEMS_STATUS_CHECKS_H
#define RTEMS_STATUS_CHECKS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup rtems_status_checks Status Checks
 */
/**@{**/

/**
 * @name Print Macros
 */
/**@{**/

/**
 * @brief General purpose debug print macro.
 */
#ifdef DEBUG
  #ifndef RTEMS_DEBUG_PRINT
    #ifdef RTEMS_STATUS_CHECKS_USE_PRINTK
      #define RTEMS_DEBUG_PRINT( fmt, ...) \
        printk( "%s: " fmt, __func__, ##__VA_ARGS__)
    #else /* RTEMS_STATUS_CHECKS_USE_PRINTK */
      #include <stdio.h>
      #define RTEMS_DEBUG_PRINT( fmt, ...) \
        printf( "%s: " fmt, __func__, ##__VA_ARGS__)
    #endif /* RTEMS_STATUS_CHECKS_USE_PRINTK */
  #endif /* RTEMS_DEBUG_PRINT */
#else /* DEBUG */
  #ifdef RTEMS_DEBUG_PRINT
    #warning RTEMS_DEBUG_PRINT was defined, but DEBUG was undefined
    #undef RTEMS_DEBUG_PRINT
  #endif /* RTEMS_DEBUG_PRINT */
  #define RTEMS_DEBUG_PRINT( fmt, ...)
#endif /* DEBUG */

/**
 * @brief Macro to print debug messages for successful operations.
 */
#define RTEMS_DEBUG_OK( msg) \
  RTEMS_DEBUG_PRINT( "Ok: %s\n", msg)

/**
 * @brief General purpose system log print macro.
 */
#ifndef RTEMS_SYSLOG_PRINT
  #ifdef RTEMS_STATUS_CHECKS_USE_PRINTK
    #define RTEMS_SYSLOG_PRINT( fmt, ...) \
      printk( fmt, ##__VA_ARGS__)
  #else /* RTEMS_STATUS_CHECKS_USE_PRINTK */
    #include <stdio.h>
    #define RTEMS_SYSLOG_PRINT( fmt, ...) \
      printf( fmt, ##__VA_ARGS__)
  #endif /* RTEMS_STATUS_CHECKS_USE_PRINTK */
#endif /* RTEMS_SYSLOG_PRINT */

/**
 * @brief General purpose system log macro.
 */
#define RTEMS_SYSLOG( fmt, ...) \
  RTEMS_SYSLOG_PRINT( "%s: " fmt, __func__, ##__VA_ARGS__)

/**
 * @brief General purpose system log macro for warnings.
 */
#define RTEMS_SYSLOG_WARNING( fmt, ...) \
  RTEMS_SYSLOG( "Warning: " fmt, ##__VA_ARGS__)

/**
 * @brief Macro to generate a system log warning message if the status code @a
 * sc is not equal to @ref RTEMS_SUCCESSFUL.
 */
#define RTEMS_SYSLOG_WARNING_SC( sc, msg) \
  if ((rtems_status_code) (sc) != RTEMS_SUCCESSFUL) { \
    RTEMS_SYSLOG_WARNING( "SC = %i: %s\n", (int) sc, msg); \
  }

/**
 * @brief General purpose system log macro for errors.
 */
#define RTEMS_SYSLOG_ERROR( fmt, ...) \
  RTEMS_SYSLOG( "Error: " fmt, ##__VA_ARGS__)

/**
 * @brief Macro for system log error messages with status code.
 */
#define RTEMS_SYSLOG_ERROR_WITH_SC( sc, msg) \
  RTEMS_SYSLOG_ERROR( "SC = %i: %s\n", (int) sc, msg);

/**
 * @brief Macro for system log error messages with return value.
 */
#define RTEMS_SYSLOG_ERROR_WITH_RV( rv, msg) \
  RTEMS_SYSLOG_ERROR( "RV = %i: %s\n", (int) rv, msg);

/**
 * @brief Macro to generate a system log error message if the status code @a
 * sc is not equal to @ref RTEMS_SUCCESSFUL.
 */
#define RTEMS_SYSLOG_ERROR_SC( sc, msg) \
  if ((rtems_status_code) (sc) != RTEMS_SUCCESSFUL) { \
    RTEMS_SYSLOG_ERROR_WITH_SC( sc, msg); \
  }

/**
 * @brief Macro to generate a system log error message if the return value @a
 * rv is less than zero.
 */
#define RTEMS_SYSLOG_ERROR_RV( rv, msg) \
  if ((int) (rv) < 0) { \
    RTEMS_SYSLOG_ERROR_WITH_RV( rv, msg); \
  }

/** @} */

/**
 * @name Check Macros
 */
/**@{**/

/**
 * @brief Prints message @a msg and returns with status code @a sc if the status
 * code @a sc is not equal to @ref RTEMS_SUCCESSFUL.
 */
#define RTEMS_CHECK_SC( sc, msg) \
  if ((rtems_status_code) (sc) != RTEMS_SUCCESSFUL) { \
    RTEMS_SYSLOG_ERROR_WITH_SC( sc, msg); \
    return (rtems_status_code) sc; \
  } else { \
    RTEMS_DEBUG_OK( msg); \
  }

/**
 * @brief Prints message @a msg and returns with a return value of negative @a sc
 * if the status code @a sc is not equal to @ref RTEMS_SUCCESSFUL.
 */
#define RTEMS_CHECK_SC_RV( sc, msg) \
  if ((rtems_status_code) (sc) != RTEMS_SUCCESSFUL) { \
    RTEMS_SYSLOG_ERROR_WITH_SC( sc, msg); \
    return -((int) (sc)); \
  } else { \
    RTEMS_DEBUG_OK( msg); \
  }

/**
 * @brief Prints message @a msg and returns if the status code @a sc is not equal
 * to @ref RTEMS_SUCCESSFUL.
 */
#define RTEMS_CHECK_SC_VOID( sc, msg) \
  if ((rtems_status_code) (sc) != RTEMS_SUCCESSFUL) { \
    RTEMS_SYSLOG_ERROR_WITH_SC( sc, msg); \
    return; \
  } else { \
    RTEMS_DEBUG_OK( msg); \
  }

/**
 * @brief Prints message @a msg and delete the current task if the status code
 * @a sc is not equal to @ref RTEMS_SUCCESSFUL.
 */
#define RTEMS_CHECK_SC_TASK( sc, msg) \
  if ((rtems_status_code) (sc) != RTEMS_SUCCESSFUL) { \
    RTEMS_SYSLOG_ERROR_WITH_SC( sc, msg); \
    (void) rtems_task_delete( RTEMS_SELF); \
    return; \
  } else { \
    RTEMS_DEBUG_OK( msg); \
  }

/**
 * @brief Prints message @a msg and returns with a return value @a rv if the
 * return value @a rv is less than zero.
 */
#define RTEMS_CHECK_RV( rv, msg) \
  if ((int) (rv) < 0) { \
    RTEMS_SYSLOG_ERROR_WITH_RV( rv, msg); \
    return (int) rv; \
  } else { \
    RTEMS_DEBUG_OK( msg); \
  }

/**
 * @brief Prints message @a msg and returns with status code @ref RTEMS_IO_ERROR
 * if the return value @a rv is less than zero.
 */
#define RTEMS_CHECK_RV_SC( rv, msg) \
  if ((int) (rv) < 0) { \
    RTEMS_SYSLOG_ERROR_WITH_RV( rv, msg); \
    return RTEMS_IO_ERROR; \
  } else { \
    RTEMS_DEBUG_OK( msg); \
  }

/**
 * @brief Prints message @a msg and returns if the return value @a rv is less
 * than zero.
 */
#define RTEMS_CHECK_RV_VOID( rv, msg) \
  if ((int) (rv) < 0) { \
    RTEMS_SYSLOG_ERROR_WITH_RV( rv, msg); \
    return; \
  } else { \
    RTEMS_DEBUG_OK( msg); \
  }

/**
 * @brief Prints message @a msg and delete the current task if the return value
 * @a rv is less than zero.
 */
#define RTEMS_CHECK_RV_TASK( rv, msg) \
  if ((int) (rv) < 0) { \
    RTEMS_SYSLOG_ERROR_WITH_RV( rv, msg); \
    (void) rtems_task_delete( RTEMS_SELF); \
    return; \
  } else { \
    RTEMS_DEBUG_OK( msg); \
  }

/** @} */

/**
 * @name Cleanup Macros
 */
/**@{**/

/**
 * @brief Prints message @a msg and jumps to @a label if the status code @a sc
 * is not equal to @ref RTEMS_SUCCESSFUL.
 */
#define RTEMS_CLEANUP_SC( sc, label, msg) \
  if ((rtems_status_code) (sc) != RTEMS_SUCCESSFUL) { \
    RTEMS_SYSLOG_ERROR_WITH_SC( sc, msg); \
    goto label; \
  } else { \
    RTEMS_DEBUG_OK( msg); \
  }

/**
 * @brief Prints message @a msg and jumps to @a label if the status code @a sc
 * is not equal to @ref RTEMS_SUCCESSFUL.  The return value variable @a rv will
 * be set to a negative @a sc in this case.
 */
#define RTEMS_CLEANUP_SC_RV( sc, rv, label, msg) \
  if ((rtems_status_code) (sc) != RTEMS_SUCCESSFUL) { \
    RTEMS_SYSLOG_ERROR_WITH_SC( sc, msg); \
    rv = -((int) (sc)); \
    goto label; \
  } else { \
    RTEMS_DEBUG_OK( msg); \
  }

/**
 * @brief Prints message @a msg and jumps to @a label if the return value @a rv
 * is less than zero.
 */
#define RTEMS_CLEANUP_RV( rv, label, msg) \
  if ((int) (rv) < 0) { \
    RTEMS_SYSLOG_ERROR_WITH_RV( rv, msg); \
    goto label; \
  } else { \
    RTEMS_DEBUG_OK( msg); \
  }

/**
 * @brief Prints message @a msg and jumps to @a label if the return value @a rv
 * is less than zero.  The status code variable @a sc will be set to @ref
 * RTEMS_IO_ERROR in this case.
 */
#define RTEMS_CLEANUP_RV_SC( rv, sc, label, msg) \
  if ((int) (rv) < 0) { \
    RTEMS_SYSLOG_ERROR_WITH_RV( rv, msg); \
    sc = RTEMS_IO_ERROR; \
    goto label; \
  } else { \
    RTEMS_DEBUG_OK( msg); \
  }

/**
 * @brief Prints message @a msg and jumps to @a label.
 */
#define RTEMS_DO_CLEANUP( label, msg) \
  do { \
    RTEMS_SYSLOG_ERROR( msg); \
    goto label; \
  } while (0)

/**
 * @brief Prints message @a msg, sets the status code variable @a sc to @a val
 * and jumps to @a label.
 */
#define RTEMS_DO_CLEANUP_SC( val, sc, label, msg) \
  do { \
    sc = (rtems_status_code) val; \
    RTEMS_SYSLOG_ERROR_WITH_SC( sc, msg); \
    goto label; \
  } while (0)

/**
 * @brief Prints message @a msg, sets the return value variable @a rv to @a val
 * and jumps to @a label.
 */
#define RTEMS_DO_CLEANUP_RV( val, rv, label, msg) \
  do { \
    rv = (int) val; \
    RTEMS_SYSLOG_ERROR_WITH_RV( rv, msg); \
    goto label; \
  } while (0)

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_STATUS_CHECKS_H */
