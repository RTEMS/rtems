/**
 * @file
 *
 * @brief Header file for status checks.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef RTEMS_STATUS_CHECKS_H
#define RTEMS_STATUS_CHECKS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef DEBUG
  #ifndef DEBUG_PRINT
    #ifdef RTEMS_STATUS_CHECKS_USE_PRINTK
      #define DEBUG_PRINT( fmt, ...) \
        printk( "%s: " fmt, __func__, ##__VA_ARGS__)
    #else /* RTEMS_STATUS_CHECKS_USE_PRINTK */
      #define DEBUG_PRINT( fmt, ...) \
        printf( "%s: " fmt, __func__, ##__VA_ARGS__)
    #endif /* RTEMS_STATUS_CHECKS_USE_PRINTK */
  #endif /* DEBUG_PRINT */
#else /* DEBUG */
  #ifdef DEBUG_PRINT
    #warning DEBUG_PRINT was defined, but DEBUG was undefined
    #undef DEBUG_PRINT
  #endif /* DEBUG_PRINT */
  #define DEBUG_PRINT( fmt, ...)
#endif /* DEBUG */

#ifndef SYSLOG_PRINT
  #ifdef RTEMS_STATUS_CHECKS_USE_PRINTK
    #define SYSLOG_PRINT( fmt, ...) \
      printk( fmt, ##__VA_ARGS__)
  #else /* RTEMS_STATUS_CHECKS_USE_PRINTK */
    #define SYSLOG_PRINT( fmt, ...) \
      printf( fmt, ##__VA_ARGS__)
  #endif /* RTEMS_STATUS_CHECKS_USE_PRINTK */
#endif /* SYSLOG_PRINT */

#define SYSLOG( fmt, ...) \
  SYSLOG_PRINT( "%s: " fmt, __func__, ##__VA_ARGS__)

#define SYSLOG_WARNING( fmt, ...) \
  SYSLOG_PRINT( "%s: Warning: " fmt, __func__, ##__VA_ARGS__)

#define SYSLOG_WARNING_SC( sc, fmt, ...) \
  if ((sc) != RTEMS_SUCCESSFUL) { \
    SYSLOG_PRINT( "%s: Warning: SC = %i: " fmt "\n", \
      __func__, sc, ##__VA_ARGS__); \
  }

#define SYSLOG_ERROR( fmt, ...) \
  SYSLOG_PRINT( "%s: Error: " fmt, __func__, ##__VA_ARGS__)

#define SYSLOG_ERROR_SC( sc, fmt, ...) \
  if ((sc) != RTEMS_SUCCESSFUL) { \
    SYSLOG_PRINT( "%s: Error: SC = %i: " fmt "\n", \
      __func__, sc, ##__VA_ARGS__); \
  }

#define CHECK_SC( sc, hint ) \
  if ((sc) != RTEMS_SUCCESSFUL) { \
    SYSLOG_ERROR( "SC = %i: %s\n", sc, hint ); \
    return sc; \
  } else { \
    DEBUG_PRINT( "Ok: %s\n", hint ); \
  }

#define CHECK_SCRV( sc, hint ) \
  if ((sc) != RTEMS_SUCCESSFUL) { \
    SYSLOG_ERROR( "SC = %i: %s\n", sc, hint ); \
    return -((int) (sc)); \
  } else { \
    DEBUG_PRINT( "Ok: %s\n", hint ); \
  }

#define CHECK_SC_VOID( sc, hint ) \
  if ((sc) != RTEMS_SUCCESSFUL) { \
    SYSLOG_ERROR( "SC = %i: %s\n", sc, hint ); \
    return; \
  } else { \
    DEBUG_PRINT( "Ok: %s\n", hint ); \
  }

#define CHECK_SC_TASK( sc, hint ) \
  if ((sc) != RTEMS_SUCCESSFUL) { \
    SYSLOG_ERROR( "SC = %i: %s\n", sc, hint ); \
    rtems_task_delete( RTEMS_SELF); \
    return; \
  } else { \
    DEBUG_PRINT( "Ok: %s\n", hint ); \
  }

#define CHECK_RV( rv, hint ) \
  if ((rv) < 0) { \
    SYSLOG_ERROR( "RV = %i: %s\n", rv, hint ); \
    return rv; \
  } else { \
    DEBUG_PRINT( "Ok: %s\n", hint ); \
  }

#define CHECK_RVSC( rv, hint ) \
  if ((rv) < 0) { \
    SYSLOG_ERROR( "RV = %i: %s\n", rv, hint ); \
    return RTEMS_IO_ERROR; \
  } else { \
    DEBUG_PRINT( "Ok: %s\n", hint ); \
  }

#define CHECK_RV_VOID( rv, hint ) \
  if ((rv) < 0) { \
    SYSLOG_ERROR( "RV = %i: %s\n", rv, hint ); \
    return; \
  } else { \
    DEBUG_PRINT( "Ok: %s\n", hint ); \
  }

#define CHECK_RV_TASK( rv, hint ) \
  if ((rv) < 0) { \
    SYSLOG_ERROR( "RV = %i: %s\n", rv, hint ); \
    rtems_task_delete( RTEMS_SELF); \
    return; \
  } else { \
    DEBUG_PRINT( "Ok: %s\n", hint ); \
  }

#define CLEANUP_SC( sc, label, hint ) \
  if ((sc) != RTEMS_SUCCESSFUL) { \
    SYSLOG_ERROR( "SC = %i: %s\n", sc, hint ); \
    goto label; \
  } else { \
    DEBUG_PRINT( "Ok: %s\n", hint ); \
  }

#define CLEANUP_SCRV( sc, rv, label, hint ) \
  if ((sc) != RTEMS_SUCCESSFUL) { \
    SYSLOG_ERROR( "SC = %i: %s\n", sc, hint ); \
    rv = -((int) (sc)); \
    goto label; \
  } else { \
    DEBUG_PRINT( "Ok: %s\n", hint ); \
  }

#define CLEANUP_RV( rv, label, hint ) \
  if ((rv) < 0) { \
    SYSLOG_ERROR( "RV = %i: %s\n", rv, hint ); \
    goto label; \
  } else { \
    DEBUG_PRINT( "Ok: %s\n", hint ); \
  }

#define CLEANUP_RVSC( rv, sc, label, hint ) \
  if ((rv) < 0) { \
    SYSLOG_ERROR( "RV = %i: %s\n", rv, hint ); \
    sc = RTEMS_IO_ERROR; \
    goto label; \
  } else { \
    DEBUG_PRINT( "Ok: %s\n", hint ); \
  }

#define DO_CLEANUP_SC( val, sc, label, hint ) \
  do { \
    sc = val; \
    SYSLOG_ERROR( "SC = %i: %s\n", sc, hint ); \
    goto label; \
  } while (0)

#define DO_CLEANUP_RV( val, rv, label, hint ) \
  do { \
    rv = val; \
    SYSLOG_ERROR( "RV = %i: %s\n", rv, hint ); \
    goto label; \
  } while (0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_STATUS_CHECKS_H */
