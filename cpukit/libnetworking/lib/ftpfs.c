/*
 * File Transfer Protocol client
 *
 * Transfer file to/from remote host
 * 
 * This driver can be added to the RTEMS file system with a call to 
 * "rtems_bsdnet_initialize_ftp_filesystem () ".
 * From then on, you can open, read and close files on a remote FTP server 
 * using the following syntax:
 * To open a file "myfile.txt" in the directory "mydir" (relative to home 
 * directory) on a server named "myserver" using the user id
 * "myuserid" and the password "my_very_secret_password" you must 
 * specify the following path:
 * 
 * /FTP/myuserid:my_very_secret_password/@myserver/mydirectory/myfile.txt
 * 
 * If the server is the default server specified in BOOTP, it can be ommitted:
 * 
 * /FTP/myuserid:my_very_secret_password/mydirectory/myfile.txt
 *
 * WARNING: write accesses have not yet been tested.
 * 
 *
 * (c) Copyright 2002
 * Thomas Doerfler
 * IMD Ingenieurbuero fuer Microcomputertechnik
 * Herbststr. 8
 * 82178 Puchheim, Germany 
 * <Thomas.Doerfler@imd-systems.de>
 *
 * This code has been created after closly inspecting 
 * "tftpdriver.c" from Eric Norum.
 * 
 *  $Id$
 */

#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/rtems_bsdnet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <rtems/ftpfs.h>


#ifndef set_errno_and_return_minus_one
#define set_errno_and_return_minus_one( _error ) \
  do { errno = (_error); return -1; } while(0)
#endif

/* #define DEBUG_OUT */

/*
 * Well-known port for FTP
 */
#define FTP_PORT_NUM	21

/*
 * Pathname prefix
 */
#define FTP_PATHNAME_PREFIX	"/FTP/"
/*
 * reply codes
 */
#define FTP_REPLY_CONNECT 220  /* Connection established       */
#define FTP_REPLY_PASSREQ 331  /* user ok, password required   */
#define FTP_REPLY_LOGIN   230  /* login finished               */
#define FTP_REPLY_SUCCESS 200  /* xxx successful               */
#define FTP_REPLY_OPENCONN 150 /* opening connection for tfer  */
#define FTP_REPLY_TFERCMPL 226 /* transfer complete            */

extern rtems_filesystem_operations_table  rtems_ftp_ops;  
extern rtems_filesystem_file_handlers_r rtems_ftp_handlers;

/*
 * FTP command strings
 */
#define FTP_USER_CMD   "USER "
#define FTP_PASS_CMD   "PASS "
#define FTP_BINARY_CMD "TYPE I"
#define FTP_PORT_CMD   "PORT "
#define FTP_STOR_CMD   "STOR "
#define FTP_RETR_CMD   "RETR "
#define FTP_QUIT_CMD   "QUIT"

/*
 * State of each FTP stream
 */
struct ftpStream {
  /*
   * Control connection socket
   */
  int ctrl_socket;
  struct sockaddr_in	myCtrlAddress;
  struct sockaddr_in	farCtrlAddress;
  /*
   * Data transfer socket
   */
  int port_socket;
  int data_socket;
  struct sockaddr_in	myDataAddress;
  struct sockaddr_in	farDataAddress;
  /*
   * other stuff to remember
   */
  boolean eof_reached;
};

/*
 * Number of streams open at the same time
 */
static rtems_id ftp_mutex;
static int nStreams;
static struct ftpStream ** volatile ftpStreams;

extern rtems_filesystem_operations_table  rtems_tftp_ops;
extern rtems_filesystem_file_handlers_r   rtems_tftp_handlers;

/*
 *  Direct copy from the IMFS/TFTP.  Look at this.
 */

rtems_filesystem_limits_and_options_t rtems_ftp_limits_and_options = {
   5,   /* link_max */
   6,   /* max_canon */
   7,   /* max_input */
   255, /* name_max */
   255, /* path_max */
   2,   /* pipe_buf */
   1,   /* posix_async_io */
   2,   /* posix_chown_restrictions */
   3,   /* posix_no_trunc */
   4,   /* posix_prio_io */
   5,   /* posix_sync_io */
   6    /* posix_vdisable */
};

int rtems_ftp_mount_me(
  rtems_filesystem_mount_table_entry_t *temp_mt_entry
)
{
  rtems_status_code  sc;

  temp_mt_entry->mt_fs_root.handlers = &rtems_ftp_handlers;
  temp_mt_entry->mt_fs_root.ops      = &rtems_ftp_ops;

  /*
   *   We have no ftp filesystem specific data to maintain.  This
   *   filesystem may only be mounted ONCE.
   *
   *   And we maintain no real filesystem nodes, so there is no real root.
   */

  temp_mt_entry->fs_info                = NULL;
  temp_mt_entry->mt_fs_root.node_access = NULL;

  /*
   *  These need to be looked at for full POSIX semantics.
   */

  temp_mt_entry->pathconf_limits_and_options = rtems_ftp_limits_and_options; 


  /*
   *  Now allocate a semaphore for mutual exclusion.
   *
   *  NOTE:  This could be in an fsinfo for this filesystem type.
   */
  
  sc = rtems_semaphore_create (rtems_build_name('F','T','P',' '),
			       1,
			       RTEMS_FIFO |
			       RTEMS_BINARY_SEMAPHORE |
			       RTEMS_NO_INHERIT_PRIORITY |
			       RTEMS_NO_PRIORITY_CEILING |
			       RTEMS_LOCAL,
			       0,
			       &ftp_mutex);

  if (sc != RTEMS_SUCCESSFUL)
    set_errno_and_return_minus_one( ENOMEM ); 

  return 0;
}

/*
 * Initialize the FTP driver
 */

int rtems_bsdnet_initialize_ftp_filesystem () 
{
 int                                   status;
 rtems_filesystem_mount_table_entry_t *entry;

 status = mkdir( FTP_PATHNAME_PREFIX, S_IRWXU | S_IRWXG | S_IRWXO );
 if ( status == -1 )
   return status; 

  status = mount( 
     &entry,
     &rtems_ftp_ops,
     RTEMS_FILESYSTEM_READ_ONLY, 
     NULL,
     FTP_PATHNAME_PREFIX
  );

  if ( status )
    perror( "FTP mount failed" );

  return status;
}

/*
 * read and return message code from ftp control connection
 */
int rtems_ftp_get_message
(
  const struct ftpStream *fsp,  /* ptr to ftp control structure */
  int *msg_code                 /* ptr to return message code   */
)
{
  char rd_buffer[4];
  size_t rd_size;
  size_t tmp_size;
  int eno = 0;
  rtems_boolean finished = FALSE;
  do {
    /*
     * fetch (at least) 4 characters from control connection
     * FIXME: how about a timeout?
     */    
    rd_size = 0;
    while ((eno == 0) &&
	   (rd_size < sizeof(rd_buffer))) {
      tmp_size = read(fsp->ctrl_socket,
		      (&rd_buffer)+rd_size,
		      sizeof(rd_buffer)-rd_size);
      if (tmp_size < 0) {
	eno = EIO;
      }
      else {
#ifdef DEBUG_OUT
	write(1,(&rd_buffer)+rd_size,tmp_size);
#endif
	rd_size += tmp_size;
      }
    }
    /*
     * check for 3 digits and space, otherwise not finished
     */    
    if ((eno == 0) &&
	(isdigit((unsigned int)rd_buffer[0])) &&
	(isdigit((unsigned int)rd_buffer[1])) &&
	(isdigit((unsigned int)rd_buffer[2])) &&
	(rd_buffer[3] == ' ')) {
      finished = TRUE;
      rd_buffer[3] = '\0';
      *msg_code = atol(rd_buffer);
    }
    /*
     * skip rest until end-of-line
     */
    do {
      tmp_size = read(fsp->ctrl_socket,
		      &rd_buffer,
		      1);
      if (tmp_size < 0) {
	eno = EIO;
      }
#ifdef DEBUG_OUT
      else {
	write(1,(&rd_buffer),tmp_size);
      }
#endif
    } while ((eno == 0) &&
	     (rd_buffer[0] != '\n'));
  } while ((eno == 0) && !finished);
  return eno;
}

/*
 * split a pseudo file name into host, user, password, filename
 * NOTE: this function will allocate space for these strings,
 * the calling function should free the space, when no longer needed
 * exception: when we return any error, we will also cleanup
 * the strings
 * valid forms:
 * /FTP/user:pass/filepath
 * /FTP/user:pass@hostname/filepath

 * /FTP/user:pass/filepath
 * /FTP/user:pass/@hostname/filepath
 * NOTE: /FTP is already stripped from the name
 */
int rtems_ftp_split_names
( const char *pathname,         /* total path name (including prefix)     */
  char **usernamep,             /* ptr to ptr to user name                */
  char **passwordp,             /* ptr to ptr to password                 */
  char **hostnamep,             /* ptr to ptr to host name                */
  char **filenamep)             /* ptr to ptr to hostremaining file name  */
{
  const char  *chunk_start;
  const char  *chunk_end;
  size_t chunk_len;
  int rc = 0;

  /*
   * ensure, that result pointers are NULL...
   */
  *usernamep = NULL;
  *passwordp = NULL;
  *hostnamep = NULL;
  *filenamep = NULL;

#if 1
  chunk_start = pathname;
#else /* no longer needed with IMFS */
  /*
   * check, that total path is long enough, skip prefix
   */
  if (rc == 0) {
    if (strlen (pathname) <= strlen (FTP_PATHNAME_PREFIX)) {
      rc = ENOENT;
    }
    else {
      chunk_start = pathname + strlen (FTP_PATHNAME_PREFIX);
    }
  }
#endif
  /*
   * fetch user name: terminated with ":"
   */
  if (rc == 0) {
    chunk_end = strchr(chunk_start,':');
    if ((chunk_end == NULL) ||         /* No ':' found or                  */
	(chunk_end == chunk_start)) {  /* ':' is first character-> no name */
      rc = ENOENT;
    }
    else {
      chunk_len = chunk_end-chunk_start;
      *usernamep = malloc(chunk_len);
      if (*usernamep == NULL) {
	rc = ENOMEM;
      }
      else {
	memcpy(*usernamep,chunk_start,chunk_len);
	(*usernamep)[chunk_len] = '\0';
      }
    }
  }
  /*
   * fetch password: terminated with "/" or "@"
   */
  if (rc == 0) {
    chunk_start = chunk_end + 1; /* skip ":" after user name */
    chunk_end = strchr(chunk_start,'/');
    if ((chunk_end == NULL) ||         /* No '/' found or                  */
	(chunk_end == chunk_start)) {  /* '/' is first character-> no pwd  */
      rc = ENOENT;
    }
    else {
      /*
       * we have found a proper '/'
       * this is the end of the password
       */
      chunk_len = chunk_end-chunk_start;
      *passwordp = malloc(chunk_len);
      if (*passwordp == NULL) {
	rc = ENOMEM;
      }
      else {
	memcpy(*passwordp,chunk_start,chunk_len);
	(*passwordp)[chunk_len] = '\0';
      }
    }
  }
  /*
   * if first char after '/' is '@', then this is the hostname
   * fetch hostname terminated with "/"
   * if exists at all. otherwise take default server from bootp
   */
  if (rc == 0) {
    chunk_start = chunk_end+1;
    if (*chunk_start == '@') {
      /* 
       * hostname follows
       */
      chunk_start = chunk_start + 1; /* skip "@" after password */
      chunk_end = strchr(chunk_start,'/');
      if ((chunk_end == NULL) ||         /* No '/' found or                  */
	  (chunk_end == chunk_start)) {  /* '/' is first character-> no host */
	rc = ENOENT;
      }
      else {
	/*
	 * we have found a proper '/'
	 */
	chunk_len = chunk_end-chunk_start;
	*hostnamep = malloc(chunk_len);
	if (*hostnamep == NULL) {
	  rc = ENOMEM;
	}
	else {
	  memcpy(*hostnamep,chunk_start,chunk_len);
	  (*hostnamep)[chunk_len] = '\0';
	}
      }
    }
    else { /* chunk_start != '@' */
      /*
       * no host name given, keep string empty
       */
      *hostnamep = malloc(1);
      if (*hostnamep == NULL) {
	rc = ENOMEM;
      }
      else {
	(*hostnamep)[0] = '\0';
      }
    }      
  }
  /*
   * fetch filename. This is all the rest...
   */
  if (rc == 0) {
    chunk_start = chunk_end+1;
    if (*chunk_start == '\0') {  /* nothing left for filename */
      rc = ENOENT;
    }
    else {
      chunk_len = strlen(chunk_start);
      *filenamep = malloc(chunk_len);
      if (*filenamep == NULL) {
	rc = ENOMEM;
      }
      else {
	memcpy(*filenamep,chunk_start,chunk_len);
	(*filenamep)[chunk_len] = '\0';
      }
    }
  }
  
  /*
   * cleanup anything, if error occured
   */
  if (rc != 0) {
    if (*hostnamep != NULL) {
      free(*hostnamep);
      *hostnamep = NULL;
    }
    if (*usernamep != NULL) {
      free(*usernamep);
      *usernamep = NULL;
    }
    if (*passwordp != NULL) {
      free(*passwordp);
      *passwordp = NULL;
    }
    if (*filenamep != NULL) {
      free(*filenamep);
      *filenamep = NULL;
    }
  }
  return rc;
}
				       
int rtems_ftp_evaluate_for_make(
   const char                         *path,       /* IN     */
   rtems_filesystem_location_info_t   *pathloc,    /* IN/OUT */
   const char                        **name        /* OUT    */
)
{  
  set_errno_and_return_minus_one( EIO );    
}

/*
 * XXX - Fix return values.
 */

int rtems_ftp_eval_path(  
  const char                        *pathname,     /* IN     */
  int                                flags,        /* IN     */
  rtems_filesystem_location_info_t  *pathloc       /* IN/OUT */
)
{

  /*
   * Read-only for now
   */
   
  if ( ((flags & O_RDONLY) != O_RDONLY ) && 
       ((flags & O_WRONLY) != O_WRONLY )) {
    set_errno_and_return_minus_one( ENOENT );
  }
  /*
   * The File system is mounted at FTP_PATHNAME_PREFIX
   * the caller of this routine has striped off this part of the
   * name. Save the remainder of the name for use by the open routine.
   */

  pathloc->node_access = (void * ) pathname;
  pathloc->handlers    = &rtems_ftp_handlers;

  return 0;
}

/*
 * Open a FTP stream
 */
int rtems_ftp_open(
  rtems_libio_t *iop,
  const char    *new_name,
  unsigned32     flag,
  unsigned32     mode
)
{
  int s = 0;
  char *filename  = NULL;
  char *uname     = NULL;
  char *upass     = NULL;
  char *hostname  = NULL;
  char port_buffer[sizeof(FTP_PORT_CMD)+6*4+1+1];
  rtems_unsigned32 my_ip;
  rtems_unsigned16 my_port;
  int eno = 0;
  rtems_status_code rc;
  rtems_boolean is_write = FALSE;
  rtems_boolean sema_obtained = FALSE;
  struct ftpStream *fsp = NULL;
  int msg_tmp;
  int sockaddr_size;
  /*
   * check for R/O or W/O flags
   */
  if (eno == 0) {
    if ((0 != (iop->flags & LIBIO_FLAGS_WRITE)) && 
	(0 != (iop->flags & LIBIO_FLAGS_READ))) {
      eno = ENOTSUP;
    }
    else {
      is_write = (0 != (iop->flags & LIBIO_FLAGS_WRITE));
    }
  }
  /*
   * split pathname into parts
   */
  if (eno == 0) {
    eno = rtems_ftp_split_names(iop->file_info,
				&uname,
				&upass,
				&hostname,
				&filename);
  }
  
  /*
   * Find a free stream
   */
  if (eno == 0) {
    rc = rtems_semaphore_obtain (ftp_mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
    if (rc == RTEMS_SUCCESSFUL) {
      sema_obtained = TRUE;
    }
    else {
      eno = EBUSY;
    }
  }
  if (eno == 0) {
    for (s = 0 ; s < nStreams ; s++) {
      if (ftpStreams[s] == NULL)
	break;
    }
    if (s == nStreams) {
      /*
       * Reallocate stream pointers
       * Guard against the case where realloc() returns NULL.
       */
      struct ftpStream **np;
      
      np = realloc (ftpStreams, ++nStreams * sizeof *ftpStreams);
      if (np == NULL) {
	eno = ENOMEM;
      }
      else {
	ftpStreams = np;
      }
    }
  }
  if (eno == 0) {
    fsp = ftpStreams[s] = malloc (sizeof (struct ftpStream));
    rtems_semaphore_release (ftp_mutex);
    sema_obtained = FALSE;
    if (fsp == NULL) {
      eno = ENOMEM;
    }
    else {
      iop->data0 = s;
      iop->data1 = fsp;
      fsp->ctrl_socket = -1; /* mark, that sockets not yet created */
      fsp->port_socket = -1;
      fsp->data_socket = -1;
      fsp->eof_reached = FALSE;
    }
  }
  if (eno == 0) {  
  /*
   * Create the socket for control connection
   */
    if ((fsp->ctrl_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
      eno = ENOMEM;
    }
  }

  if (eno == 0) {
    /*
     * Set the destination to the FTP server
     * port on the remote machine.
     */
    memset(&(fsp->farCtrlAddress),sizeof(fsp->farCtrlAddress),0);
    fsp->farCtrlAddress.sin_family = AF_INET;
    if (*hostname == '\0') {
      fsp->farCtrlAddress.sin_addr.s_addr = rtems_bsdnet_bootp_server_address.s_addr;
    }
    else if (1 != inet_aton(hostname,&(fsp->farCtrlAddress.sin_addr))) {
      struct hostent *hent;
      struct hostent *gethostbyname(const char *name);
      
      hent = gethostbyname(hostname);
      if (hent != NULL) {
	bcopy(hent->h_addr, 
	      (char *)(&(fsp->farCtrlAddress.sin_addr)), 
	      sizeof(fsp->farCtrlAddress.sin_addr));
      }
      else {
	eno = ENOENT;
      }
    }
    if (eno == 0) {
      fsp->farCtrlAddress.sin_port = htons (FTP_PORT_NUM);  
      fsp->farCtrlAddress.sin_len  = sizeof(fsp->farCtrlAddress);    
      if (0 > connect(fsp->ctrl_socket,
		      (struct sockaddr *)&(fsp->farCtrlAddress),
		      sizeof(fsp->farCtrlAddress))) {
	eno = ENOENT;
      }
    }
    if (eno == 0) {
      /*
       * fetch IP address of interface used
       */
      memset(&(fsp->myCtrlAddress),sizeof(fsp->myCtrlAddress),0);
      fsp->myCtrlAddress.sin_family = AF_INET;
      fsp->myCtrlAddress.sin_addr.s_addr = INADDR_ANY;
      fsp->myCtrlAddress.sin_port   = 0; 
      fsp->myCtrlAddress.sin_len  = sizeof(fsp->myDataAddress);
      sockaddr_size = sizeof(fsp->myCtrlAddress);
      if (0 > getsockname(fsp->ctrl_socket,
			  (struct sockaddr *)&(fsp->myCtrlAddress),
			  &sockaddr_size)) {
	eno = ENOMEM;
      }
    }
  }
  if (eno == 0) {
    /*
     * now we should get a connect message from the FTP server
     */
    eno = rtems_ftp_get_message(fsp,&msg_tmp);
    if ((eno == 0) &&
	(msg_tmp != FTP_REPLY_CONNECT)) {
      eno = ENOENT;
    }
  }
  if (eno == 0) {
    /*
     * send user ID to server
     * NOTE: the following lines will be executed in order
     * and will be aborted whenever an error occures... (see your ANSI C book)
     */
    if ((0 > send(fsp->ctrl_socket,FTP_USER_CMD,strlen(FTP_USER_CMD),0)) ||
	(0 > send(fsp->ctrl_socket,uname,       strlen(uname),       0)) ||
	(0 > send(fsp->ctrl_socket,"\n",        1,                   0))) {
      eno = EIO;
    }
  }
  if (eno == 0) {
    /*
     * now we should get a request for the password or a login...
     */
    eno = rtems_ftp_get_message(fsp,&msg_tmp);
    if (eno == 0) {
      if (msg_tmp == FTP_REPLY_PASSREQ) {
	/*
	 * send password to server
	 */
#ifdef DEBUG_OUT
	write(1,FTP_PASS_CMD,strlen(FTP_PASS_CMD));
	write(1,upass,       strlen(upass)       );
	write(1,"\n",        1                   );
#endif    
	if ((0 > send(fsp->ctrl_socket,FTP_PASS_CMD,strlen(FTP_PASS_CMD),0)) ||
	    (0 > send(fsp->ctrl_socket,upass,       strlen(upass),       0)) ||
	    (0 > send(fsp->ctrl_socket,"\n",        1,                   0))) {
	  eno = EIO;
	}
	/*
	 * at least now a login reply should come up...
	 * this is checked some lines downwards the code
	 */
	if (eno == 0) {
	  eno = rtems_ftp_get_message(fsp,&msg_tmp);
	}
      }
    }
  }
  if (eno == 0) {
    /*
     * check for a login reply. this should be present now...
     */
    if (msg_tmp != FTP_REPLY_LOGIN) {
      eno = EACCES; /* pseudo for wrong user/pass */
    }
  }
  if (eno == 0) {
    /*
     * set binary mode for all transfers
     */
#ifdef DEBUG_OUT
    write(1,FTP_BINARY_CMD,strlen(FTP_BINARY_CMD));
    write(1,"\n",        1                   );
#endif    
    if ((0 > send(fsp->ctrl_socket,FTP_BINARY_CMD,strlen(FTP_BINARY_CMD),0)) ||
	(0 > send(fsp->ctrl_socket,"\n",          1,                     0))) {
      eno = EIO;
    }
    else {
      eno = rtems_ftp_get_message(fsp,&msg_tmp);
    }
  }
  if (eno == 0) {
    /*
     * check for a "BINARY TYPE command successful" reply
     */
    if (msg_tmp != FTP_REPLY_SUCCESS) {
      eno = EIO;
    }
  }
  if (eno == 0) {
    /*
     * create and bind socket for data connection
     */
    if ((fsp->port_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
      eno = ENOMEM;
    }
    else {
      memset(&(fsp->myDataAddress),sizeof(fsp->myDataAddress),0);
      fsp->myDataAddress.sin_family = AF_INET;
      fsp->myDataAddress.sin_addr.s_addr = INADDR_ANY;
      fsp->myDataAddress.sin_port   = 0; /* unique port will be assigned */
      fsp->myDataAddress.sin_len  = sizeof(fsp->myDataAddress);
      if (0 > bind(fsp->port_socket,
		   (struct sockaddr *)&(fsp->myDataAddress),
		   sizeof(fsp->myDataAddress))) {
	eno = EBUSY;
      }
      else {
	/*
	 * fetch port number of data socket
	 */
	memset(&(fsp->myDataAddress),sizeof(fsp->myDataAddress),0);
	fsp->myDataAddress.sin_family = AF_INET;
	fsp->myDataAddress.sin_addr.s_addr = INADDR_ANY;
	fsp->myDataAddress.sin_port   = 0; 
	fsp->myDataAddress.sin_len  = sizeof(fsp->myDataAddress);
	sockaddr_size = sizeof(fsp->myDataAddress);
	if (0 > getsockname(fsp->port_socket,
			    (struct sockaddr *)&(fsp->myDataAddress),
			    &sockaddr_size)) {
	  eno = ENOMEM;
	}
      }
    }
  }
  if (eno == 0) {
    /*
     * propagate data connection port to server
     */
    my_ip   = ntohl(fsp->myCtrlAddress.sin_addr.s_addr);
    my_port = ntohs(fsp->myDataAddress.sin_port);
    sprintf(port_buffer,"%s%u,%u,%u,%u,%u,%u\n",
	    FTP_PORT_CMD,
	    (my_ip >> 24) & 0x00ff,
	    (my_ip >> 16) & 0x00ff,
	    (my_ip >>  8) & 0x00ff,
	    (my_ip >>  0) & 0x00ff,
	    (my_port>> 8) & 0x00ff,
	    (my_port>> 0) & 0x00ff);
#ifdef DEBUG_OUT
    write(1,port_buffer,strlen(port_buffer));
#endif
    if (0 > send(fsp->ctrl_socket,port_buffer,strlen(port_buffer),0)) {
      eno = EIO;
    }    
    else {
      eno = rtems_ftp_get_message(fsp,&msg_tmp);
    }
  }
  if (eno == 0) {
    /*
     * check for a "PORT command successful" reply
     */
    if (msg_tmp != FTP_REPLY_SUCCESS) {
      eno = EBUSY;
    }
  }
  /*
   * prepare port socket to listen for incoming connections 
   */
  if (eno == 0) {
    if (0 > listen(fsp->port_socket,1)) {
      eno = EBUSY;
    }
  }
  if (eno == 0) {
    /*
     * send retrive/store command with filename
     */
    if (is_write) {
#ifdef DEBUG_OUT
    write(1,FTP_STOR_CMD,strlen(FTP_STOR_CMD));
    write(1,filename    ,strlen(filename)    );
    write(1,"\n",1);
#endif
      if ((0 > send(fsp->ctrl_socket,FTP_STOR_CMD,strlen(FTP_STOR_CMD),0)) ||
	  (0 > send(fsp->ctrl_socket,filename,    strlen(filename),    0)) ||
	  (0 > send(fsp->ctrl_socket,"\n",        1,                   0))) {
	eno = EIO;
      }
    }
    else {
#ifdef DEBUG_OUT
    write(1,FTP_RETR_CMD,strlen(FTP_RETR_CMD));
    write(1,filename    ,strlen(filename)    );
    write(1,"\n",1);
#endif
      if ((0 > send(fsp->ctrl_socket,FTP_RETR_CMD,strlen(FTP_RETR_CMD),0)) ||
	  (0 > send(fsp->ctrl_socket,filename,    strlen(filename),    0)) ||
	  (0 > send(fsp->ctrl_socket,"\n",        1,                   0))) {
	eno = EIO;
      }
    }      
  }
#if 1
  if (eno == 0) {
    eno = rtems_ftp_get_message(fsp,&msg_tmp);
  }
  if (eno == 0) {
    /*
     * check for a "OPENING binary connection" reply
     */
    if (msg_tmp != FTP_REPLY_OPENCONN) {
      eno = EACCES;
    }
  }
#endif
  /*
   * wait for connect on data connection
   * FIXME: this should become a select instead with a timeout
   */
  if (eno == 0) {
    sockaddr_size = sizeof(fsp->farDataAddress);
    fsp->data_socket = accept(fsp->port_socket,
			      (struct sockaddr *)&(fsp->farDataAddress),
			      &sockaddr_size);
    if (0 > fsp->data_socket) {
      eno = EIO;
    }
  }
  /*
   * FIXME: check, that fardataAddr is really from our ftp server
   */
  
  /* 
   * clean up temp strings...
   */
  if (uname != NULL) {
    free(uname);
    uname = NULL;
  }
  if (upass != NULL) {
    free(upass);
    upass = NULL;
  }
  if (hostname != NULL) {
    free(hostname);
    hostname = NULL;
  }
  if (filename != NULL) {
    free(filename);
    filename = NULL;
  }
  /*
   * close part socket, no longer needed
   */
  if (fsp->port_socket != -1) {
    close(fsp->port_socket);
    fsp->port_socket = -1;
  }
  /*
   * if error, clean up everything
   */
  if (eno != 0) {
    if (fsp != NULL) {
      /*
       * check and close ctrl/data connection
       */
      if (fsp->data_socket != -1) {
	close(fsp->data_socket);
	fsp->data_socket = -1;
      }
      if (fsp->ctrl_socket != -1) {
	close(fsp->ctrl_socket);
	fsp->ctrl_socket = -1;
      }
      /*
       * free ftpStream structure 
       */
      ftpStreams[s] = NULL;
      free(fsp);
      fsp = NULL;
    }
  }
  /*
   * return sema, if still occupied
   */
  if (sema_obtained) {
    rtems_semaphore_release (ftp_mutex);
    sema_obtained = FALSE;
  }
#if 0
  if (eno != 0) {
    set_errno_and_return_minus_one(eno);
  }
  return 0;
#else
  return eno;
#endif
}

/*
 * Read from a FTP stream
 */
int rtems_ftp_read(
  rtems_libio_t *iop,
  void          *buffer,
  unsigned32     count
)
{
  int eno = 0;
  struct ftpStream *fsp;
  size_t want_cnt;
  ssize_t rd_cnt;
  int msg_tmp;

  fsp = iop->data1;
  want_cnt = count;
  /*
   * check, that data connection present
   */
  if (eno == 0) {
    if ((fsp == NULL) || 
	(fsp->data_socket < 0)) {
      eno = EBADF;
    }
  }  
   
  /*
   * perform read from data socket
   * read multiple junks, if smaller than wanted
   */
  while ((eno == 0) && 
	 (want_cnt > 0) &&
	 !(fsp->eof_reached) ) {
    rd_cnt = read(fsp->data_socket,buffer,want_cnt);
    if (rd_cnt > 0) {
      buffer += rd_cnt;
      want_cnt -= rd_cnt;
    }
    else {
      eno = rtems_ftp_get_message(fsp,&msg_tmp);
      fsp->eof_reached = TRUE;
      if ((eno == 0) &&
	  (msg_tmp != FTP_REPLY_TFERCMPL)) {
	eno = EIO;
      }
      if (rd_cnt < 0) {
	eno = EIO;
      }
    }
  }
  if (eno != 0) {
    set_errno_and_return_minus_one(eno);
  }
  return count - want_cnt;
}

int rtems_ftp_write(
  rtems_libio_t *iop,
  const void    *buffer,
  unsigned32     count
)
{
  int eno = EIO;
  struct ftpStream *fsp;
  size_t want_cnt;
  ssize_t wr_cnt;
  int msg_tmp;

  fsp = iop->data1;
  want_cnt = count;
  /*
   * check, that data connection present
   */
  if (eno == 0) {
    if ((fsp == NULL) || 
	(fsp->data_socket < 0)) {
      eno = EBADF;
    }
  }  
   
  /*
   * perform write to data socket
   */
  if (eno == 0) {
    wr_cnt = write(fsp->data_socket,buffer,want_cnt);
    if (wr_cnt > 0) {
      buffer += wr_cnt;
      want_cnt -= wr_cnt;
    }
    else {
      eno = rtems_ftp_get_message(fsp,&msg_tmp);
      if ((eno == 0) &&
	  (msg_tmp != FTP_REPLY_TFERCMPL)) {
	eno = EIO;
      }
      if (wr_cnt < 0) {
	eno = EIO;
      }
    }
  }
  if (eno != 0) {
    set_errno_and_return_minus_one(eno);
  }
  return count - want_cnt;
}

/*
 * Close a FTP stream
 */
int rtems_ftp_close(
  rtems_libio_t *iop
)
{
  int s = iop->data0;
  struct ftpStream *fsp = iop->data1;

  /*
   * close ctrl/data connection, if needed
   */
  if (fsp->data_socket >= 0) {
    close(fsp->data_socket);
    fsp->data_socket = -1;
  }
  if (fsp->ctrl_socket >= 0) {
    close(fsp->ctrl_socket);
    fsp->ctrl_socket = -1;
  }
  /*
   * free any used space...
   */
  rtems_semaphore_obtain (ftp_mutex, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
  free (ftpStreams[s]);
  ftpStreams[s] = NULL;
  rtems_semaphore_release (ftp_mutex);

  return 0;
}

rtems_device_driver rtems_ftp_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void *pargp
)
{
  return RTEMS_NOT_CONFIGURED;
}

/*
 * Dummy version to let fopen(xxxx,"w") work properly.
 */
static int rtems_ftp_ftruncate(
    rtems_libio_t   *iop,
    off_t           count
)
{
    return 0;
}

rtems_filesystem_node_types_t rtems_ftp_node_type(
     rtems_filesystem_location_info_t        *pathloc                 /* IN */
)
{
    return RTEMS_FILESYSTEM_MEMORY_FILE;
}

rtems_filesystem_operations_table  rtems_ftp_ops = {
    rtems_ftp_eval_path,             /* eval_path */
    rtems_ftp_evaluate_for_make,     /* evaluate_for_make */
    NULL,                            /* link */
    NULL,                            /* unlink */
    rtems_ftp_node_type,             /* node_type */
    NULL,                            /* mknod */
    NULL,                            /* chown */
    NULL,                            /* freenodinfo */
    NULL,                            /* mount */
    rtems_ftp_mount_me,              /* initialize */
    NULL,                            /* unmount */
    NULL,                            /* fsunmount */
    NULL,                            /* utime, */
    NULL,                            /* evaluate_link */
    NULL,                            /* symlink */
    NULL,                            /* readlin */
};
  
rtems_filesystem_file_handlers_r rtems_ftp_handlers = {
    rtems_ftp_open,      /* open */     
    rtems_ftp_close,     /* close */    
    rtems_ftp_read,      /* read */     
    rtems_ftp_write,     /* write */    
    NULL,                /* ioctl */    
    NULL,                /* lseek */    
    NULL,                /* fstat */    
    NULL,                /* fchmod */   
    rtems_ftp_ftruncate, /* ftruncate */
    NULL,                /* fpathconf */
    NULL,                /* fsync */    
    NULL,                /* fdatasync */
    NULL,                /* fcntl */
    NULL                 /* rmnod */
};
