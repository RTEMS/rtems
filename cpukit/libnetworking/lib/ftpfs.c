/**
 * @file
 *
 * @brief File Transfer Protocol file system (FTP client).
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * (c) Copyright 2002
 * Thomas Doerfler
 * IMD Ingenieurbuero fuer Microcomputertechnik
 * Herbststr. 8
 * 82178 Puchheim, Germany
 * <Thomas.Doerfler@imd-systems.de>
 *
 * Modified by Sebastian Huber <sebastian.huber@embedded-brains.de>.
 *
 * This code has been created after closly inspecting "tftpdriver.c" from Eric
 * Norum.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 * $Id$
 */

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <malloc.h>
#include <netdb.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <rtems.h>
#include <rtems/ftpfs.h>
#include <rtems/imfs.h>
#include <rtems/libio.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/seterr.h>

#ifdef DEBUG
  #define DEBUG_PRINTF( ...) printf( __VA_ARGS__)
#else
  #define DEBUG_PRINTF( ...)
#endif

/**
 * @brief Connection entry for each open file stream.
 */
typedef struct {
  /**
   * @brief Control connection socket.
   */
  int ctrl_socket;

  /**
   * @brief Data transfer socket.
   */
  int data_socket;

  /**
   * @brief End of file flag.
   */
  bool eof;
} rtems_ftpfs_entry;

static bool rtems_ftpfs_verbose = false;

rtems_status_code rtems_ftpfs_mount( const char *mount_point)
{
  int rv = 0;

  if (mount_point == NULL) {
    mount_point = RTEMS_FTPFS_MOUNT_POINT_DEFAULT;
  }

  rv = mkdir( mount_point, S_IRWXU | S_IRWXG | S_IRWXO);
  if (rv != 0) {
    return RTEMS_IO_ERROR;
  }

  rv = mount(
    NULL,
    &rtems_ftpfs_ops,
    RTEMS_FILESYSTEM_READ_WRITE,
    NULL,
    mount_point
  );
  if (rv != 0) {
    return RTEMS_IO_ERROR;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_ftpfs_set_verbose( bool verbose)
{
  rtems_ftpfs_verbose = verbose;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_ftpfs_get_verbose( bool *verbose)
{
  if (verbose == NULL) {
    return RTEMS_INVALID_ADDRESS;
  }

  *verbose = rtems_ftpfs_verbose;

  return RTEMS_SUCCESSFUL;
}

int rtems_bsdnet_initialize_ftp_filesystem( void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;

  sc = rtems_ftpfs_mount( NULL);

  if (sc == RTEMS_SUCCESSFUL) {
    return 0;
  } else {
    return -1;
  }
}

typedef void (*rtems_ftpfs_reply_parser)(
  const char * /* reply fragment */,
  size_t /* reply fragment length */,
  void * /* parser argument */
);

typedef enum {
  RTEMS_FTPFS_REPLY_START,
  RTEMS_FTPFS_REPLY_SINGLE_LINE,
  RTEMS_FTPFS_REPLY_SINGLE_LINE_DONE,
  RTEMS_FTPFS_REPLY_MULTI_LINE,
  RTEMS_FTPFS_REPLY_NEW_LINE,
  RTEMS_FTPFS_REPLY_NEW_LINE_START
} rtems_ftpfs_reply_state;

typedef enum {
  RTEMS_FTPFS_REPLY_ERROR = 0,
  RTEMS_FTPFS_REPLY_1 = '1',
  RTEMS_FTPFS_REPLY_2 = '2',
  RTEMS_FTPFS_REPLY_3 = '3',
  RTEMS_FTPFS_REPLY_4 = '4',
  RTEMS_FTPFS_REPLY_5 = '5'
} rtems_ftpfs_reply;

#define RTEMS_FTPFS_REPLY_SIZE 3

static rtems_ftpfs_reply rtems_ftpfs_get_reply(
  int socket,
  rtems_ftpfs_reply_parser parser,
  void *parser_arg
)
{
  rtems_ftpfs_reply_state state = RTEMS_FTPFS_REPLY_START;
  bool verbose = rtems_ftpfs_verbose;
  char reply_first [RTEMS_FTPFS_REPLY_SIZE] = { 'a', 'a', 'a' };
  char reply_last [RTEMS_FTPFS_REPLY_SIZE] = { 'b', 'b', 'b' };
  size_t reply_first_index = 0;
  size_t reply_last_index = 0;
  char buf [128];

  while (true) {
    /* Receive reply fragment from socket */
    ssize_t i = 0;
    ssize_t rv = recv( socket, buf, sizeof( buf), 0);

    if (rv <= 0) {
      return RTEMS_FTPFS_REPLY_ERROR;
    }

    /* Be verbose if necessary */
    if (verbose) {
      write( STDERR_FILENO, buf, (size_t) rv);
    }

    /* Invoke parser if necessary */
    if (parser != NULL) {
      parser( buf, (size_t) rv, parser_arg);
    }

    /* Parse reply fragment */
    for (i = 0; i < rv; ++i) {
      char c = buf [i];

      switch (state) {
        case RTEMS_FTPFS_REPLY_START:
          if (reply_first_index < RTEMS_FTPFS_REPLY_SIZE) {
            reply_first [reply_first_index] = c;
            ++reply_first_index;
          } else if (c == '-') {
            state = RTEMS_FTPFS_REPLY_MULTI_LINE;
          } else {
            state = RTEMS_FTPFS_REPLY_SINGLE_LINE;
          }
          break;
        case RTEMS_FTPFS_REPLY_SINGLE_LINE:
          if (c == '\n') {
            state = RTEMS_FTPFS_REPLY_SINGLE_LINE_DONE;
          }
          break;
        case RTEMS_FTPFS_REPLY_MULTI_LINE:
          if (c == '\n') {
            state = RTEMS_FTPFS_REPLY_NEW_LINE_START;
            reply_last_index = 0;
          }
          break;
        case RTEMS_FTPFS_REPLY_NEW_LINE:
        case RTEMS_FTPFS_REPLY_NEW_LINE_START:
          if (reply_last_index < RTEMS_FTPFS_REPLY_SIZE) {
            state = RTEMS_FTPFS_REPLY_NEW_LINE;
            reply_last [reply_last_index] = c;
            ++reply_last_index;
          } else {
            state = RTEMS_FTPFS_REPLY_MULTI_LINE;
          }
          break;
        default:
          return RTEMS_FTPFS_REPLY_ERROR;
      }
    }

    /* Check reply */
    if (state == RTEMS_FTPFS_REPLY_SINGLE_LINE_DONE) {
      if (
        isdigit( reply_first [0])
          && isdigit( reply_first [1])
          && isdigit( reply_first [2])
      ) {
        break;
      } else {
        return RTEMS_FTPFS_REPLY_ERROR;
      }
    } else if (state == RTEMS_FTPFS_REPLY_NEW_LINE_START) {
      bool ok = true;

      for (i = 0; i < RTEMS_FTPFS_REPLY_SIZE; ++i) {
        ok = ok
          && reply_first [i] == reply_last [i]
          && isdigit( reply_first [i]);
      }

      if (ok) {
        break;
      }
    }
  }

  return reply_first [0];
}

static rtems_ftpfs_reply rtems_ftpfs_send_command_with_parser(
  int socket,
  const char *cmd,
  const char *arg,
  rtems_ftpfs_reply_parser parser,
  void *parser_arg
)
{
  const char *const eol = "\r\n";
  bool verbose = rtems_ftpfs_verbose;
  int rv = 0;

  /* Send command */
  rv = send( socket, cmd, strlen( cmd), 0);
  if (rv < 0) {
    return RTEMS_FTPFS_REPLY_ERROR;
  }
  if (verbose) {
    write( STDERR_FILENO, cmd, strlen( cmd));
  }

  /* Send command argument if necessary */
  if (arg != NULL) {
	  rv = send( socket, arg, strlen( arg), 0);
	  if (rv < 0) {
	    return RTEMS_FTPFS_REPLY_ERROR;
	  }
    if (verbose) {
      write( STDERR_FILENO, arg, strlen( arg));
    }
  }

  /* Send end of line */
  rv = send( socket, eol, 2, 0);
  if (rv < 0) {
    return RTEMS_FTPFS_REPLY_ERROR;
  }
  if (verbose) {
    write( STDERR_FILENO, &eol [1], 1);
  }

  /* Return reply */
  return rtems_ftpfs_get_reply( socket, parser, parser_arg);
}

static rtems_ftpfs_reply rtems_ftpfs_send_command(
  int socket,
  const char *cmd,
  const char *arg
)
{
  return rtems_ftpfs_send_command_with_parser( socket, cmd, arg, NULL, NULL);
}

typedef enum {
  STATE_USER_NAME,
  STATE_START_PASSWORD,
  STATE_START_HOST_NAME,
  STATE_START_HOST_NAME_OR_PATH,
  STATE_START_PATH,
  STATE_PASSWORD,
  STATE_HOST_NAME,
  STATE_DONE,
  STATE_INVALID
} split_state;

static bool rtems_ftpfs_split_names (
  char *s,
  const char **user,
  const char **password,
  const char **hostname,
  const char **path
)
{
  split_state state = STATE_USER_NAME;
  size_t len = strlen( s);
  size_t i = 0;

  *user = s;
  *password = NULL;
  *hostname = NULL;
  *path = NULL;

  for (i = 0; i < len; ++i) {
    char c = s [i];

    switch (state) {
      case STATE_USER_NAME:
        if (c == ':') {
          state = STATE_START_PASSWORD;
          s [i] = '\0';
        } else if (c == '@') {
          state = STATE_START_HOST_NAME;
          s [i] = '\0';
        } else if (c == '/') {
          state = STATE_START_HOST_NAME_OR_PATH;
          s [i] = '\0';
        }
        break;
      case STATE_START_PASSWORD:
        state = STATE_PASSWORD;
        *password = &s [i];
        --i;
        break;
      case STATE_START_HOST_NAME:
        state = STATE_HOST_NAME;
        *hostname = &s [i];
        --i;
        break;
      case STATE_START_HOST_NAME_OR_PATH:
        if (c == '@') {
          state = STATE_START_HOST_NAME;
        } else {
          state = STATE_DONE;
          *path = &s [i];
          goto done;
        }
        break;
      case STATE_START_PATH:
        state = STATE_DONE;
        *path = &s [i];
        goto done;
      case STATE_PASSWORD:
        if (c == '@') {
          state = STATE_START_HOST_NAME;
          s [i] = '\0';
        } else if (c == '/') {
          state = STATE_START_HOST_NAME_OR_PATH;
          s [i] = '\0';
        }
        break;
      case STATE_HOST_NAME:
        if (c == '/') {
          state = STATE_START_PATH;
          s [i] = '\0';
        }
        break;
      default:
        state = STATE_INVALID;
        goto done;
    }
  }

done:

  /* If we have no password use the user name */
  if (*password == NULL) {
    *password = *user;
  }

  return state == STATE_DONE;
}

static socklen_t rtems_ftpfs_create_address(
  struct sockaddr_in *sa,
  unsigned long address,
  unsigned short port
)
{
  memset( sa, sizeof( *sa), 0);

  sa->sin_family = AF_INET;
  sa->sin_addr.s_addr = address;
  sa->sin_port = port;
  sa->sin_len = sizeof( *sa);

  return sizeof( *sa);
}

static void rtems_ftpfs_terminate( rtems_ftpfs_entry *e, rtems_libio_t *iop)
{
  if (e != NULL) {
    /* Close data connection if necessary */
    if (e->data_socket >= 0) {
      close( e->data_socket);
    }

    /* Close control connection if necessary */
    if (e->ctrl_socket >= 0) {
      close( e->ctrl_socket);
    }

    /* Free connection entry */
    free( e);
  }

  /* Invalidate IO entry */
  iop->data1 = NULL;
}

static int rtems_ftpfs_open_ctrl_connection(
  rtems_ftpfs_entry *e,
  const char *user,
  const char *password,
  const char *hostname,
  uint32_t *client_address
)
{
  int rv = 0;
  rtems_ftpfs_reply reply = RTEMS_FTPFS_REPLY_ERROR;
  struct in_addr address = { .s_addr = 0 };
  struct sockaddr_in sa;
  socklen_t size = 0;

  /* Create the socket for the control connection */
  e->ctrl_socket = socket( AF_INET, SOCK_STREAM, 0);
  if (e->ctrl_socket < 0) {
    return ENOMEM;
  }

  /* Set up the server address from the hostname */
  if (hostname == NULL || strlen( hostname) == 0) {
    /* Default to BOOTP server address */
    address = rtems_bsdnet_bootp_server_address;
  } else if (inet_aton( hostname, &address) == 0) {
    /* Try to get the address by name */
    struct hostent *he = gethostbyname( hostname);

    if (he != NULL) {
      memcpy( &address, he->h_addr, sizeof( address));
    } else {
      return ENOENT;
    }
  }
  rtems_ftpfs_create_address( &sa, address.s_addr, htons( RTEMS_FTPFS_CTRL_PORT));
  DEBUG_PRINTF( "server = %s\n", inet_ntoa( sa.sin_addr));

  /* Open control connection */
  rv = connect(
    e->ctrl_socket,
    (struct sockaddr *) &sa,
    sizeof( sa)
  );
  if (rv != 0) {
    return ENOENT;
  }

  /* Get client address */
  size = rtems_ftpfs_create_address( &sa, INADDR_ANY, 0);
  rv = getsockname(
    e->ctrl_socket,
    (struct sockaddr *) &sa,
    &size
  );
  if (rv != 0) {
    return ENOMEM;
  }
  *client_address = ntohl( sa.sin_addr.s_addr);
  DEBUG_PRINTF( "client = %s\n", inet_ntoa( sa.sin_addr));

  /* Now we should get a welcome message from the server */
  reply = rtems_ftpfs_get_reply( e->ctrl_socket, NULL, NULL);
  if (reply != RTEMS_FTPFS_REPLY_2) {
    return ENOENT;
  }

  /* Send USER command */
  reply = rtems_ftpfs_send_command( e->ctrl_socket, "USER ", user);
  if (reply == RTEMS_FTPFS_REPLY_3) {
    /* Send PASS command */
    reply = rtems_ftpfs_send_command( e->ctrl_socket, "PASS ", password);
    if (reply != RTEMS_FTPFS_REPLY_2) {
      return EACCES;
    }

    /* TODO: Some server may require an account */
  } else if (reply != RTEMS_FTPFS_REPLY_2) {
    return EACCES;
  }

  /* Send TYPE command to set binary mode for all data transfers */
  reply = rtems_ftpfs_send_command( e->ctrl_socket, "TYPE I", NULL);
  if (reply != RTEMS_FTPFS_REPLY_2) {
    return EIO;
  }

  return 0;
}

static int rtems_ftpfs_open_data_connection_active(
  rtems_ftpfs_entry *e,
  uint32_t client_address,
  const char *file_command,
  const char *filename
)
{
  int rv = 0;
  int eno = 0;
  rtems_ftpfs_reply reply = RTEMS_FTPFS_REPLY_ERROR;
  struct in_addr address = { .s_addr = 0 };
  struct sockaddr_in sa;
  socklen_t size = 0;
  int port_socket = -1;
  char port_command [] = "PORT 000,000,000,000,000,000";
  uint16_t data_port = 0;

  /* Create port socket to establish a data data connection */
  port_socket = socket( AF_INET, SOCK_STREAM, 0);
  if (port_socket < 0) {
    eno = ENOMEM;
    goto cleanup;
  }

  /* Bind port socket */
  rtems_ftpfs_create_address( &sa, INADDR_ANY, 0);
  rv = bind(
    port_socket,
    (struct sockaddr *) &sa,
    sizeof( sa)
  );
  if (rv != 0) {
    eno = EBUSY;
    goto cleanup;
  }

  /* Get port number for data socket */
  size = rtems_ftpfs_create_address( &sa, INADDR_ANY, 0);
  rv = getsockname(
    port_socket,
    (struct sockaddr *) &sa,
    &size
  );
  if (rv != 0) {
    eno = ENOMEM;
    goto cleanup;
  }
  data_port = ntohs( sa.sin_port);

  /* Send PORT command to set data connection port for server */
  snprintf(
    port_command,
    sizeof( port_command),
    "PORT %" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu32 ",%" PRIu16 ",%" PRIu16,
    (client_address >> 24) & 0xff,
    (client_address >> 16) & 0xff,
    (client_address >> 8) & 0xff,
    (client_address >> 0) & 0xff,
    (data_port >> 8) & 0xff,
    (data_port >> 0) & 0xff
  );
  reply = rtems_ftpfs_send_command( e->ctrl_socket, port_command, NULL);
  if (reply != RTEMS_FTPFS_REPLY_2) {
    eno = ENOTSUP;
    goto cleanup;
  }

  /* Listen on port socket for incoming data connections */
  rv = listen( port_socket, 1);
  if (rv != 0) {
    eno = EBUSY;
    goto cleanup;
  }

  /* Send RETR or STOR command with filename */
  reply = rtems_ftpfs_send_command( e->ctrl_socket, file_command, filename);
  if (reply != RTEMS_FTPFS_REPLY_1) {
    eno = EIO;
    goto cleanup;
  }

  /*
   * Wait for connect on data connection.
   *
   * FIXME: This should become a select instead with a timeout.
   */
  size = sizeof( sa);
  e->data_socket = accept(
    port_socket,
    (struct sockaddr *) &sa,
    &size
  );
  if (e->data_socket < 0) {
    eno = EIO;
    goto cleanup;
  }

  /* FIXME: Check, that server data address is really from our server  */

cleanup:

  /* Close port socket if necessary */
  if (port_socket >= 0) {
    close( port_socket);
  }

  return eno;
}

typedef enum {
  RTEMS_FTPFS_PASV_START = 0,
  RTEMS_FTPFS_PASV_JUNK,
  RTEMS_FTPFS_PASV_DATA,
  RTEMS_FTPFS_PASV_DONE
} rtems_ftpfs_pasv_state;

typedef struct {
  rtems_ftpfs_pasv_state state;
  uint8_t data [6];
  size_t index;
} rtems_ftpfs_pasv_entry;

static void rtems_ftpfs_pasv_parser(
  const char* buf,
  size_t len,
  void *arg
)
{
  rtems_ftpfs_pasv_entry *e = (rtems_ftpfs_pasv_entry *) arg;
  size_t i = 0;

  for (i = 0; i < len; ++i) {
    char c = buf [i];

    switch (e->state) {
      case RTEMS_FTPFS_PASV_START:
        if (!isdigit( c)) {
          e->state = RTEMS_FTPFS_PASV_JUNK;
          e->index = 0;
        }
        break;
      case RTEMS_FTPFS_PASV_JUNK:
        if (isdigit( c)) {
          e->state = RTEMS_FTPFS_PASV_DATA;
          e->data [e->index] = (uint8_t) (c - '0');
        }
        break;
      case RTEMS_FTPFS_PASV_DATA:
        if (isdigit( c)) {
          e->data [e->index] = (uint8_t) (e->data [e->index] * 10 + c - '0');
        } else if (c == ',') {
          ++e->index;
          if (e->index < sizeof( e->data)) {
            e->data [e->index] = 0;
          } else {
            e->state = RTEMS_FTPFS_PASV_DONE;
          }
        } else {
          e->state = RTEMS_FTPFS_PASV_DONE;
        }
        break;
      default:
        return;
    }
  }
}

static int rtems_ftpfs_open_data_connection_passive(
  rtems_ftpfs_entry *e,
  uint32_t client_address,
  const char *file_command,
  const char *filename
)
{
  int rv = 0;
  rtems_ftpfs_reply reply = RTEMS_FTPFS_REPLY_ERROR;
  struct sockaddr_in sa;
  socklen_t size = 0;
  uint32_t data_address = 0;
  uint16_t data_port = 0;

  rtems_ftpfs_pasv_entry pe = {
    .state = RTEMS_FTPFS_PASV_START
  };

  /* Send PASV command */
  reply = rtems_ftpfs_send_command_with_parser(
    e->ctrl_socket,
    "PASV",
    NULL,
    rtems_ftpfs_pasv_parser,
    &pe
  );
  if (reply != RTEMS_FTPFS_REPLY_2) {
    return ENOTSUP;
  }
  data_address = (uint32_t) ((pe.data [0] << 24) + (pe.data [1] << 16)
    + (pe.data [2] << 8) + pe.data [3]);
  data_port = (uint16_t) ((pe.data [4] << 8) + pe.data [5]);
  rtems_ftpfs_create_address( &sa, htonl( data_address), htons( data_port));
  DEBUG_PRINTF(
    "server data = %s:%u\n",
    inet_ntoa( sa.sin_addr),
    (unsigned) ntohs( sa.sin_port)
  );

  /* Create data socket */
  e->data_socket = socket( AF_INET, SOCK_STREAM, 0);
  if (e->data_socket < 0) {
    return ENOMEM;
  }

  /* Open data connection */
  rv = connect(
    e->data_socket,
    (struct sockaddr *) &sa,
    sizeof( sa)
  );
  if (rv != 0) {
    return EIO;
  }

  /* Send RETR or STOR command with filename */
  reply = rtems_ftpfs_send_command( e->ctrl_socket, file_command, filename);
  if (reply != RTEMS_FTPFS_REPLY_1) {
    return EIO;
  }

  return 0;
}

static int rtems_ftpfs_open(
  rtems_libio_t *iop,
  const char *path,
  uint32_t flags,
  uint32_t mode
)
{
  int rv = 0;
  int eno = 0;
  bool ok = false;
  rtems_ftpfs_entry *e = NULL;
  const char *user = NULL;
  const char *password = NULL;
  const char *hostname = NULL;
  const char *filename = NULL;
  const char *file_command = (iop->flags & LIBIO_FLAGS_WRITE) != 0
    ? "STOR "
    : "RETR ";
  uint32_t client_address = 0;
  char *location = strdup( (const char *) iop->file_info);

  /* Check allocation */
  if (location == NULL) {
    return ENOMEM;
  }

  /* Check for either read-only or write-only flags */
  if (
    (iop->flags & LIBIO_FLAGS_WRITE) != 0
      && (iop->flags & LIBIO_FLAGS_READ) != 0
  ) {
    eno = ENOTSUP;
    goto cleanup;
  }

  /* Split location into parts */
  ok = rtems_ftpfs_split_names(
      location,
      &user,
      &password,
      &hostname,
      &filename
  );
  if (!ok) {
    eno = ENOENT;
    goto cleanup;
  }
  DEBUG_PRINTF(
    "user = '%s', password = '%s', filename = '%s'\n",
    user,
    password,
    filename
  );

  /* Allocate connection entry */
  e = malloc( sizeof( *e));
  if (e == NULL) {
    eno = ENOMEM;
    goto cleanup;
  }

  /* Initialize connection entry */
  e->ctrl_socket = -1;
  e->data_socket = -1;
  e->eof = false;

  /* Open control connection */
  eno = rtems_ftpfs_open_ctrl_connection(
    e,
    user,
    password,
    hostname,
    &client_address
  );
  if (eno != 0) {
    goto cleanup;
  }

  /* Open passive data connection */
  eno = rtems_ftpfs_open_data_connection_passive(
    e,
    client_address,
    file_command,
    filename
  );
  if (eno == ENOTSUP) {
    /* Open active data connection */
    eno = rtems_ftpfs_open_data_connection_active(
      e,
      client_address,
      file_command,
      filename
    );
  }

cleanup:

  /* Free location parts buffer */
  free( location);

  if (eno == 0) {
    /* Save connection state */
    iop->data1 = e;
  } else {
    /* Free all resources if an error occured */
    rtems_ftpfs_terminate( e, iop);
  }

  return eno;
}

static ssize_t rtems_ftpfs_read(
  rtems_libio_t *iop,
  void *buffer,
  size_t count
)
{
  rtems_ftpfs_entry *e = (rtems_ftpfs_entry *) iop->data1;
  char *in = (char *) buffer;
  size_t todo = count;

  if (e->eof) {
    return 0;
  }

  while (todo > 0) {
    ssize_t rv = recv( e->data_socket, in, todo, 0);

    if (rv <= 0) {
      if (rv == 0) {
        rtems_ftpfs_reply reply =
          rtems_ftpfs_get_reply( e->ctrl_socket, NULL, NULL);

        if (reply == RTEMS_FTPFS_REPLY_2) {
          e->eof = true;
          break;
        }
      }

      rtems_set_errno_and_return_minus_one( EIO);
    }

    in += rv;
    todo -= (size_t) rv;
  }

  return (ssize_t) (count - todo);
}

static ssize_t rtems_ftpfs_write(
  rtems_libio_t *iop,
  const void *buffer,
  size_t count
)
{
  rtems_ftpfs_entry *e = (rtems_ftpfs_entry *) iop->data1;
  const char *out = (const char *) buffer;
  size_t todo = count;

  if (e->eof) {
    return 0;
  }

  while (todo > 0) {
    ssize_t rv = send( e->data_socket, out, todo, 0);

    if (rv <= 0) {
      if (rv == 0) {
        rtems_ftpfs_reply reply =
          rtems_ftpfs_get_reply( e->ctrl_socket, NULL, NULL);

        if (reply == RTEMS_FTPFS_REPLY_2) {
          e->eof = true;
          break;
        }
      }

      rtems_set_errno_and_return_minus_one( EIO);
    }

    out += rv;
    todo -= (size_t) rv;
  }

  return (ssize_t) (count - todo);
}

static int rtems_ftpfs_close( rtems_libio_t *iop)
{
  rtems_ftpfs_entry *e = (rtems_ftpfs_entry *) iop->data1;

  rtems_ftpfs_terminate( e, iop);

  return 0;
}

/* Dummy version to let fopen( *,"w") work properly */
static int rtems_ftpfs_ftruncate( rtems_libio_t *iop, off_t count)
{
  return 0;
}

static int rtems_ftpfs_eval_path(
  const char *pathname,
  int flags,
  rtems_filesystem_location_info_t *pathloc
)
{
  /*
   * The caller of this routine has striped off the mount prefix from the path.
   * We need to store this path here or otherwise we would have to do this job
   * again.  It is not possible to allocate resources here since there is no
   * way to free them later in every case.  The path is used in
   * rtems_ftpfs_open() via iop->file_info.
   *
   * FIXME: Avoid to discard the const qualifier.
   */
  pathloc->node_access = (void *) pathname;

  return 0;
}

static int rtems_ftpfs_eval_for_make(
  const char *pathname,
  rtems_filesystem_location_info_t *pathloc,
  const char **name
)
{
  rtems_set_errno_and_return_minus_one( EIO);
}

static rtems_filesystem_node_types_t rtems_ftpfs_node_type(
  rtems_filesystem_location_info_t *pathloc
)
{
  return RTEMS_FILESYSTEM_MEMORY_FILE;
}

static const rtems_filesystem_file_handlers_r rtems_ftpfs_handlers;

static int rtems_ftpfs_mount_me(
  rtems_filesystem_mount_table_entry_t *e
)
{
  /* Set handler and oparations table */
  e->mt_fs_root.handlers = &rtems_ftpfs_handlers;
  e->mt_fs_root.ops = &rtems_ftpfs_ops;

  /* We have no FTP file system specific data to maintain */
  e->fs_info = NULL;

  /* We maintain no real file system nodes, so there is no real root */
  e->mt_fs_root.node_access = NULL;

  /* Just use the limits from IMFS */
  e->pathconf_limits_and_options = IMFS_LIMITS_AND_OPTIONS;

  return 0;
}

/*
 * The stat() support is intended only for the cp shell command.  Each request
 * will return that we have a regular file with read, write and execute
 * permissions for every one.  The node index uses a global counter to support
 * a remote to remote copy.  Is not a very sophisticated method.
 */
static int rtems_ftpfs_fstat(
  rtems_filesystem_location_info_t *loc,
  struct stat *st
)
{
  static unsigned ino = 0;

  memset( st, 0, sizeof( *st));

  /* FIXME */
  st->st_ino = ++ino;

  st->st_mode = S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO;

  return 0;
}

const rtems_filesystem_operations_table rtems_ftpfs_ops = {
  .evalpath_h = rtems_ftpfs_eval_path,
  .evalformake_h = rtems_ftpfs_eval_for_make,
  .link_h = NULL,
  .unlink_h = NULL,
  .node_type_h = rtems_ftpfs_node_type,
  .mknod_h = NULL,
  .chown_h = NULL,
  .freenod_h = NULL,
  .mount_h = NULL,
  .fsmount_me_h = rtems_ftpfs_mount_me,
  .unmount_h = NULL,
  .fsunmount_me_h = NULL,
  .utime_h = NULL,
  .eval_link_h = NULL,
  .symlink_h = NULL,
  .readlink_h = NULL
};

static const rtems_filesystem_file_handlers_r rtems_ftpfs_handlers = {
  .open_h = rtems_ftpfs_open,
  .close_h = rtems_ftpfs_close,
  .read_h = rtems_ftpfs_read,
  .write_h = rtems_ftpfs_write,
  .ioctl_h = NULL,
  .lseek_h = NULL,
  .fstat_h = rtems_ftpfs_fstat,
  .fchmod_h = NULL,
  .ftruncate_h = rtems_ftpfs_ftruncate,
  .fpathconf_h = NULL,
  .fsync_h = NULL,
  .fdatasync_h = NULL,
  .fcntl_h = NULL,
  .rmnod_h = NULL
};
