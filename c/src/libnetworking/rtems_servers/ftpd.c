/* FIXME: 1. Parse command is a hack.  We can do better.
 *        2. chdir is a hack.  We can do better.
 *        3. PWD doesn't work.
 *        4. Some sort of access control?
 *
 *  FTP Server Daemon 
 *
 *  Submitted by: Jake Janovetz <janovetz@tempest.ece.uiuc.edu>
 *
 *  $Id$
 */

/**************************************************************************
 *                                 ftpd.c                                 *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This file contains the daemon which services requests that appear   *
 *    on the FTP port.  This server is compatible with FTP, but it        *
 *    also provides 'hooks' to make it usable in situations where files   *
 *    are not used/necessary.  Once the server is started, it runs        *
 *    forever.                                                            *
 *                                                                        *
 *                                                                        *
 *    Organization:                                                       *
 *                                                                        *
 *       The FTP daemon is started upon boot.  It runs all the time       *
 *       and waits for connections on the known FTP port (21).  When      *
 *       a connection is made, it starts a 'session' task.  That          *
 *       session then interacts with the remote host.  When the session   *
 *       is complete, the session task deletes itself.  The daemon still  *
 *       runs, however.                                                   *
 *                                                                        *
 *                                                                        *
 * Supported commands are:                                                *
 *                                                                        *
 * RETR xxx     - Sends a file from the client.                           *
 * STOR xxx     - Receives a file from the client.  xxx = filename.       *
 * LIST xxx     - Sends a file list to the client.                        *
 *                (LIST xxx isn't working yet...)                         *
 * USER         - Does nothing.                                           *
 * PASS         - Does nothing.                                           *
 * SYST         - Replies with the system type (`RTEMS').                 *
 * DELE xxx     - Delete file xxx.                                        *
 * MKD xxx      - Create directory xxx.                                   *
 * RMD xxx      - Remove directory xxx.                                   *
 * PWD          - Print working directory.                                *
 * CWD xxx      - Change working directory.                               *
 * SITE CHMOD xxx yyy - Change permissions on file yyy to xxx.            *
 * PORT a,b,c,d,x,y   - Setup for a data port to IP address a.b.c.d       *
 *                      and port (x*256 + y).                             *
 *                                                                        *
 *                                                                        *
 * The public routines contained in this file are:                        *
 *                                                                        *
 *    rtems_initialize_ftpd_start - Starts the server daemon, then        *
 *                                  returns to its caller.                *
 *                                                                        *
 *                                                                        *
 * The private routines contained in this file are:                       *
 *                                                                        *
 *    rtems_ftpd_send_reply    - Sends a reply code and text through the  *
 *                               control port.                            *
 *    rtems_ftpd_command_retrieve - Performs to "RETR" command.           *
 *    rtems_ftpd_command_store - Performs the "STOR" command.             *
 *    rtems_ftpd_command_list  - Performs the "LIST" command.             *
 *    rtems_ftpd_command_port  - Opens a data port (the "PORT" command).  *
 *    rtems_ftpd_parse_command - Parses an incoming command.              *
 *    rtmes_ftpd_session       - Begins a service session.                *
 *    rtems_ftpd_daemon        - Listens on the FTP port for service      *
 *                               requests.                                *
 *------------------------------------------------------------------------*
 * Jake Janovetz                                                          *
 * University of Illinois                                                 *
 * 1406 West Green Street                                                 *
 * Urbana IL  61801                                                       *
 **************************************************************************
 * Change History:                                                        *
 *  12/01/97 - Creation (JWJ)                                             *
 *************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <rtems.h>
#include <rtems/rtems_bsdnet.h>
#include <rtems/error.h>
#include <syslog.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/ftp.h>
#include <netinet/in.h>

#include "ftpd.h"


extern struct rtems_ftpd_configuration rtems_ftpd_configuration;

/**************************************************************************
 * Meanings of first and second digits of reply codes:
 *
 * Reply:  Description:
 *-------- --------------
 *  1yz    Positive preliminary reply.  The action is being started but
 *         expect another reply before sending another command.
 *  2yz    Positive completion reply.  A new command can be sent.
 *  3yz    Positive intermediate reply.  The command has been accpeted
 *         but another command must be sent.
 *  4yz    Transient negative completion reply.  The requested action did
 *         not take place, but the error condition is temporary so the
 *         command can be reissued later.
 *  5yz    Permanent negative completion reply.  The command was not  
 *         accepted and should not be retried.
 *-------------------------------------------------------------------------
 *  x0z    Syntax errors.
 *  x1z    Information.
 *  x2z    Connections.  Replies referring to the control or data
 *         connections.
 *  x3z    Authentication and accounting.  Replies for the login or
 *         accounting commands.
 *  x4z    Unspecified.
 *  x5z    Filesystem status.
 *************************************************************************/


/**************************************************************************
 * SessionInfo structure.
 *
 * The following structure is allocated for each session.  The pointer
 * to this structure is contained in the tasks notepad entry.
 *************************************************************************/
typedef struct
{
   struct sockaddr_in  data_addr;   /* Data address for PORT commands */
   FILE                *ctrl_fp;    /* File pointer for control connection */
   char                cwd[255];    /* Current working directory */
                                    /* Login -- future use -- */
   int                 xfer_mode;   /* Transfer mode (ASCII/binary) */
} FTPD_SessionInfo_t;


#define FTPD_SERVER_MESSAGE  "RTEMS FTP server (Version 1.0-JWJ) ready."
#define FTPD_WELCOME_MESSAGE \
   "Welcome to the RTEMS FTP server.\n" \
   "\n" \
   "Login accepted.\n"


/**************************************************************************
 * Function: rtems_ftpd_send_reply                                        *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This procedure sends a reply to the client via the control          *
 *    connection.                                                         *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    int  code  - The 3-digit reply code.                                *
 *    char *text - Reply text.                                            *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    none                                                                *
 *                                                                        *
 **************************************************************************
 * Change History:                                                        *
 *  12/01/97 - Creation (JWJ)                                             *
 *************************************************************************/
static void
rtems_ftpd_send_reply(int code, char *text)
{
   rtems_status_code   sc;
   FTPD_SessionInfo_t  *info = NULL;
   char                str[80];


   sc = rtems_task_get_note(RTEMS_SELF, RTEMS_NOTEPAD_0,
                            (rtems_unsigned32 *)&info);

   /***********************************************************************
    * code must be a 3-digit number.
    **********************************************************************/
   if ((code < 100) || (code > 999))
   {
      syslog(LOG_ERR, "ftpd: Code not 3-digits.");
      return;
   }

   /***********************************************************************
    * If a text reply exists, add it to the reply data.
    **********************************************************************/
   if (text != NULL)
   {
      sprintf(str, "%d %.70s\r\n", code, text);
      fprintf(info->ctrl_fp, "%d %.70s\r\n", code, text);
   }
   else
   {
      sprintf(str, "%d\r\n", code);
      fprintf(info->ctrl_fp, "%d\r\n", code);
   }
   fflush(info->ctrl_fp);
}


/**************************************************************************
 * Function: rtems_ftpd_command_retrieve                                  *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This performs the "RETR" command.  A data connection must already   *
 *    be open (via the "PORT" command.)  Here, we send the data to the    *
 *    connection.                                                         *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    char *filename   - Source filename.                                 *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    int  - 0 for reply sent.                                            *
 *           1 for no reply sent.                                         *
 *                                                                        *
 **************************************************************************
 * Change History:                                                        *
 *  04/29/98 - Creation (JWJ)                                             *
 *************************************************************************/
static int
rtems_ftpd_command_retrieve(char *filename)
{
   int                 s;
   int                 n;
   int                 fd;
   unsigned char       *bufr;
   rtems_status_code   sc;
   FTPD_SessionInfo_t  *info = NULL;


   sc = rtems_task_get_note(RTEMS_SELF, RTEMS_NOTEPAD_0,
                            (rtems_unsigned32 *)&info);

   if ((fd = open(filename, O_RDONLY)) == -1)
   {
      rtems_ftpd_send_reply(450, "Error opening file.");
      return(0);
   }

   bufr = (unsigned char *)malloc(BUFSIZ);
   if (bufr == NULL)
   {
      rtems_ftpd_send_reply(440, "Server error - malloc fail.");
      close(fd);
      return(0);
   }

   /***********************************************************************
    * Connect to the data connection (PORT made in an earlier PORT call).
    **********************************************************************/
   rtems_ftpd_send_reply(150, "BINARY data connection.");
   s = socket(AF_INET, SOCK_STREAM, 0);
   if (connect(s, (struct sockaddr *)&info->data_addr,
               sizeof(struct sockaddr)) < 0)
   {
      rtems_ftpd_send_reply(420, "Server error - could not connect socket.");
      free(bufr);
      close(fd);
      close(s);
      return(1);
   }

   /***********************************************************************
    * Send the data over the ether.
    **********************************************************************/
   while ((n = read(fd, bufr, BUFSIZ)) > 0)
   {
      send(s, bufr, n, 0);
      bufr[n-1] = '\0';
   }

   if (n == 0)
   {
      rtems_ftpd_send_reply(210, "File sent successfully.");
   }
   else
   {
      rtems_ftpd_send_reply(450, "Retrieve failed.");
   }

   if (close(s) != 0)
   {
      syslog(LOG_ERR, "ftpd: Error closing data socket");
   }

   free(bufr);
   close(fd);
   return(0);
}


/**************************************************************************
 * Function: rtems_ftpd_command_store                                     *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This performs the "STOR" command.  A data connection must already   *
 *    be open (via the "PORT" command.)  Here, we get the data from the   *
 *    connection and figure out what to do with it.                       *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    char *filename   - Destination filename.                            *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    int  - 0 for success.                                               *
 *           1 for failure.                                               *
 *                                                                        *
 **************************************************************************
 * Change History:                                                        *
 *  12/01/97 - Creation (JWJ)                                             *
 *************************************************************************/
static int
rtems_ftpd_command_store(char *filename)
{
   char                   *bufr;
   int                    s;
   int                    n;
   unsigned long          size = 0;
   rtems_status_code      sc;
   FTPD_SessionInfo_t     *info = NULL;
   struct rtems_ftpd_hook *usehook = NULL;


   sc = rtems_task_get_note(RTEMS_SELF, RTEMS_NOTEPAD_0,
                            (rtems_unsigned32 *)&info);

   bufr = (char *)malloc(BUFSIZ * sizeof(char));
   if (bufr == NULL)
   {
      rtems_ftpd_send_reply(440, "Server error - malloc fail.");
      return(1);
   }

   rtems_ftpd_send_reply(150, "BINARY data connection.");

   s = socket(AF_INET, SOCK_STREAM, 0);
   if (connect(s, (struct sockaddr *)&info->data_addr,
               sizeof(struct sockaddr)) < 0)
   {
      free(bufr);
      close(s);
      return(1);
   }


   /***********************************************************************
    * File: "/dev/null" just throws the data away.
    * Otherwise, search our list of hooks to see if we need to do something
    *   special.
    **********************************************************************/
   if (!strncmp("/dev/null", filename, 9))
   {
      while ((n = read(s, bufr, BUFSIZ)) > 0);
   }
   else if (rtems_ftpd_configuration.hooks != NULL)
   {
      struct rtems_ftpd_hook *hook;
      int i;

      i = 0;
      hook = &rtems_ftpd_configuration.hooks[i++];
      while (hook->filename != NULL)
      {
         if (!strcmp(hook->filename, filename))
         {
            usehook = hook;
            break;
         }
         hook = &rtems_ftpd_configuration.hooks[i++];
      }
   }

   if (usehook != NULL)
   {
      char                *bigBufr;

      /***********************************************************************
       * Allocate space for our "file".
       **********************************************************************/
      bigBufr = (char *)malloc(
                  rtems_ftpd_configuration.max_hook_filesize * sizeof(char));
      if (bigBufr == NULL)
      {
         rtems_ftpd_send_reply(440, "Server error - malloc fail.");
         free(bufr);
         return(1);
      }

      /***********************************************************************
       * Retrieve the file into our buffer space.
       **********************************************************************/
      size = 0;
      while ((n = read(s, bufr, BUFSIZ)) > 0)
      {
         if (size + n >
               rtems_ftpd_configuration.max_hook_filesize * sizeof(char))
         {
            rtems_ftpd_send_reply(440, "Server error - Buffer size exceeded.");
            free(bufr);
            free(bigBufr);
            close(s);
            return(1);
         }
         memcpy(&bigBufr[size], bufr, n);
         size += n;
      }
      close(s);

      /***********************************************************************
       * Call our hook.
       **********************************************************************/
      if ((usehook->hook_function)(bigBufr, size) == 0)
      {
         rtems_ftpd_send_reply(210, "File transferred successfully.");
      }
      else
      {
         rtems_ftpd_send_reply(440, "File transfer failed.");
      }
      free(bigBufr);
   }
   else
   {
      int    fd;
      size_t written;

      fd = creat(filename, S_IRUSR | S_IWUSR |
                           S_IRGRP | S_IWGRP |
                           S_IROTH | S_IWOTH);
      if (fd == -1)
      {
         rtems_ftpd_send_reply(450, "Could not open file.");
         close(s);
         free(bufr);
         return(1);
      }
      while ((n = read(s, bufr, BUFSIZ)) > 0)
      {
         written = write(fd, bufr, n);
         if (written == -1)
         {
            rtems_ftpd_send_reply(450, "Error during write.");
            close(fd);
            close(s);
            free(bufr);
            return(1);
         }
      }
      close(fd);
      close(s);
      rtems_ftpd_send_reply(226, "Transfer complete.");
   }

   free(bufr);
   return(0);
}


/**************************************************************************
 * Function: rtems_ftpd_command_list                                      *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    Sends a file list through a data connection.  The data              *
 *    connection must have already been opened with the "PORT" command.   *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    char *fname  - File (or directory) to list.                         *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    none                                                                *
 *                                                                        *
 **************************************************************************
 * Change History:                                                        *
 *  12/01/97 - Creation (JWJ)                                             *
 *************************************************************************/
static void
rtems_ftpd_command_list(char *fname)
{
   int                 s;
   rtems_status_code   sc;
   FTPD_SessionInfo_t  *info = NULL;
   DIR                 *dirp;
   struct dirent       *dp;
   char                dirline[255];
   struct stat         stat_buf;


   sc = rtems_task_get_note(RTEMS_SELF, RTEMS_NOTEPAD_0,
                            (rtems_unsigned32 *)&info);

   rtems_ftpd_send_reply(150, "ASCII data connection for LIST.");

   s = socket(AF_INET, SOCK_STREAM, 0);
   if (connect(s, (struct sockaddr *)&info->data_addr,
               sizeof(struct sockaddr)) < 0)
   {
      syslog(LOG_ERR, "ftpd: Error connecting to data socket.");
      return;
   }

   if ((dirp = opendir(fname)) == NULL)
   {
      sprintf(dirline, "%s: No such file or directory.%s\n",
              fname, (info->xfer_mode==TYPE_A)?("\r"):(""));
      send(s, dirline, strlen(dirline), 0);
      close(s);
      rtems_ftpd_send_reply(226, "Transfer complete.");
      return;
   }
   while ((dp = readdir(dirp)) != NULL)
   {
      if (stat(dp->d_name, &stat_buf) == 0)
      {
         sprintf(dirline, "%c%c%c%c%c%c%c%c%c%c  %5d %5d %11d  %s%s\n",
                 (S_ISLNK(stat_buf.st_mode)?('l'):
                    (S_ISDIR(stat_buf.st_mode)?('d'):('-'))),
                 (stat_buf.st_mode & S_IRUSR)?('r'):('-'),
                 (stat_buf.st_mode & S_IWUSR)?('w'):('-'),
                 (stat_buf.st_mode & S_IXUSR)?('x'):('-'),
                 (stat_buf.st_mode & S_IRGRP)?('r'):('-'),
                 (stat_buf.st_mode & S_IWGRP)?('w'):('-'),
                 (stat_buf.st_mode & S_IXGRP)?('x'):('-'),
                 (stat_buf.st_mode & S_IROTH)?('r'):('-'),
                 (stat_buf.st_mode & S_IWOTH)?('w'):('-'),
                 (stat_buf.st_mode & S_IXOTH)?('x'):('-'),
                 (int)stat_buf.st_uid,
                 (int)stat_buf.st_gid,
                 (int)stat_buf.st_size,
                 dp->d_name,
                 (info->xfer_mode==TYPE_A)?("\r"):(""));
         send(s, dirline, strlen(dirline), 0);
      }
   }
   closedir(dirp);

   close(s);
   rtems_ftpd_send_reply(226, "Transfer complete.");
}


/*
 * Cheesy way to change directories
 */
static void
rtems_ftpd_CWD(char *dir)
{
   rtems_status_code   sc;
   FTPD_SessionInfo_t  *info = NULL;


   sc = rtems_task_get_note(RTEMS_SELF, RTEMS_NOTEPAD_0,
                            (rtems_unsigned32 *)&info);

   if (chdir(dir) == 0)
   {
      rtems_ftpd_send_reply(250, "CWD command successful.");
   }
   else
   {
      rtems_ftpd_send_reply(550, "CWD command failed.");
   }
}


/**************************************************************************
 * Function: rtems_ftpd_command_port                                      *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This procedure opens up a data port given the IP address of the     *
 *    remote machine and the port on the remote machine.  This connection *
 *    will then be used to transfer data between the hosts.               *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    char *bufr - Arguments to the "PORT" command.                       *
 *                                                                        *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    none                                                                *
 *                                                                        *
 **************************************************************************
 * Change History:                                                        *
 *  12/01/97 - Creation (JWJ)                                             *
 *************************************************************************/
static void
rtems_ftpd_command_port(char *bufr)
{
   char                *ip;
   char                *port;
   int                 ip0, ip1, ip2, ip3, port0, port1;
   rtems_status_code   sc;
   FTPD_SessionInfo_t  *info = NULL;


   sc = rtems_task_get_note(RTEMS_SELF, RTEMS_NOTEPAD_0,
                            (rtems_unsigned32 *)&info);

   sscanf(bufr, "%d,%d,%d,%d,%d,%d", &ip0, &ip1, &ip2, &ip3, &port0, &port1);
   ip = (char *)&(info->data_addr.sin_addr);
   ip[0] = ip0 & 0xff;
   ip[1] = ip1 & 0xff;
   ip[2] = ip2 & 0xff;
   ip[3] = ip3 & 0xff;
   port = (char *)&(info->data_addr.sin_port);
   port[0] = port0 & 0xff;
   port[1] = port1 & 0xff;
   info->data_addr.sin_family = AF_INET;
}


/**************************************************************************
 * Function: rtems_ftpd_parse_command                                     *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    Here, we parse the commands that have come through the control      *
 *    connection.                                                         *
 *                                                                        *
 * FIXME: This section is somewhat of a hack.  We should have a better    *
 *        way to parse commands.                                          *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    char *bufr     - Pointer to the buffer which contains the command   *
 *                     text.                                              *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    none                                                                *
 *                                                                        *
 **************************************************************************
 * Change History:                                                        *
 *  12/01/97 - Creation (JWJ)                                             *
 *************************************************************************/
static void
rtems_ftpd_parse_command(char *bufr)
{
   char fname[255];
   rtems_status_code   sc;
   FTPD_SessionInfo_t  *info = NULL;


   sc = rtems_task_get_note(RTEMS_SELF, RTEMS_NOTEPAD_0,
                            (rtems_unsigned32 *)&info);

   if (!strncmp("PORT", bufr, 4))
   {
      rtems_ftpd_send_reply(200, "PORT command successful.");
      rtems_ftpd_command_port(&bufr[5]);
   }
   else if (!strncmp("RETR", bufr, 4))
   {
      sscanf(&bufr[5], "%254s", fname);
      rtems_ftpd_command_retrieve(fname);
   }
   else if (!strncmp("STOR", bufr, 4))
   {
      sscanf(&bufr[5], "%254s", fname);
      rtems_ftpd_command_store(fname);
   }
   else if (!strncmp("LIST", bufr, 4))
   {
      if (bufr[5] == '\n')
      {
         rtems_ftpd_command_list(".");
      }
      else
      {
         sscanf(&bufr[5], "%254s", fname);
         rtems_ftpd_command_list(fname);
      }
   }
   else if (!strncmp("USER", bufr, 4))
   {
      rtems_ftpd_send_reply(230, "User logged in.");
   }
   else if (!strncmp("SYST", bufr, 4))
   {
      rtems_ftpd_send_reply(240, "RTEMS");
   }
   else if (!strncmp("TYPE", bufr, 4))
   {
      if (bufr[5] == 'I')
      {
         info->xfer_mode = TYPE_I;
         rtems_ftpd_send_reply(200, "Type set to I.");
      }
      else if (bufr[5] == 'A')
      {
         info->xfer_mode = TYPE_A;
         rtems_ftpd_send_reply(200, "Type set to A.");
      } 
      else
      {
         info->xfer_mode = TYPE_I;
         rtems_ftpd_send_reply(504, "Type not implemented.  Set to I.");
      }
   }
   else if (!strncmp("PASS", bufr, 4))
   {
      rtems_ftpd_send_reply(230, "User logged in.");
   }
   else if (!strncmp("DELE", bufr, 4))
   {
      sscanf(&bufr[4], "%254s", fname);
      if (unlink(fname) == 0)
      {
         rtems_ftpd_send_reply(257, "DELE successful.");
      }
      else
      {
         rtems_ftpd_send_reply(550, "DELE failed.");
      }
   }
   else if (!strncmp("SITE CHMOD", bufr, 10))
   {
      int mask;

      sscanf(&bufr[11], "%o %254s", &mask, fname);
      if (chmod(fname, (mode_t)mask) == 0)
      {
         rtems_ftpd_send_reply(257, "CHMOD successful.");
      }
      else
      {
         rtems_ftpd_send_reply(550, "CHMOD failed.");
      }
   }
   else if (!strncmp("RMD", bufr, 3))
   {
      sscanf(&bufr[4], "%254s", fname);
      if (rmdir(fname) == 0)
      {
         rtems_ftpd_send_reply(257, "RMD successful.");
      }
      else
      {
         rtems_ftpd_send_reply(550, "RMD failed.");
      }
   }
   else if (!strncmp("MKD", bufr, 3))
   {
      sscanf(&bufr[4], "%254s", fname);
      if (mkdir(fname, S_IRWXU | S_IRWXG | S_IRWXO) == 0)
      {
         rtems_ftpd_send_reply(257, "MKD successful.");
      }
      else
      {
         rtems_ftpd_send_reply(550, "MKD failed.");
      }
   }
   else if (!strncmp("CWD", bufr, 3))
   {
      sscanf(&bufr[4], "%254s", fname);
      rtems_ftpd_CWD(fname);
   }
   else if (!strncmp("PWD", bufr, 3))
   {
      char *cwd = getcwd(0, 0);
      sprintf(bufr, "\"%s\" is the current directory.", cwd);
      rtems_ftpd_send_reply(250, bufr);
      free(cwd);
   }
   else
   {
      rtems_ftpd_send_reply(500, "Unrecognized/unsupported command.");
   }
}


/**************************************************************************
 * Function: rtems_ftpd_session                                           *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This task is started when the FTP daemon gets a service request     *
 *    from a remote machine.  Here, we watch for commands that will       *
 *    come through the "control" connection.  These commands are then     *
 *    parsed and executed until the connection is closed, either          *
 *    unintentionally or intentionally with the "QUIT" command.           *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    rtems_task_argument arg - The daemon task passes the socket         *
 *                              which serves as the control connection.   *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    none                                                                *
 *                                                                        *
 **************************************************************************
 * Change History:                                                        *
 *  12/01/97 - Creation (JWJ)                                             *
 *************************************************************************/
static void
rtems_ftpd_session(rtems_task_argument arg)
{
   char                cmd[256];
   rtems_status_code   sc;
   FTPD_SessionInfo_t  *info = NULL;


   sc = rtems_task_get_note(RTEMS_SELF, RTEMS_NOTEPAD_0,
                            (rtems_unsigned32 *)&info);

   rtems_ftpd_send_reply(220, FTPD_SERVER_MESSAGE);

   /***********************************************************************
    * Set initial directory to "/".
    **********************************************************************/
   strcpy(info->cwd, "/");
   info->xfer_mode = TYPE_A;
   while (1)
   {
      if (fgets(cmd, 256, info->ctrl_fp) == NULL)
      {
         syslog(LOG_INFO, "ftpd: Connection aborted.");
         break;
      }

      if (!strncmp("QUIT", cmd, 4))
      {
         rtems_ftpd_send_reply(221, "Goodbye.");
         break;
      }
      else
      {
         rtems_ftpd_parse_command(cmd);
      }
   }

   if (fclose(info->ctrl_fp) != 0)
   {
      syslog(LOG_ERR, "ftpd: Could not close session.");
   }


   /* Least we can do is put the CWD back to /. */
   chdir("/");

   /***********************************************************************
    * Free up the allocated SessionInfo struct and exit.
    **********************************************************************/
   free(info);
   sc = rtems_task_delete(RTEMS_SELF);
   syslog(LOG_ERR, "ftpd: Task deletion failed: %s",
          rtems_status_text(sc));
}


/**************************************************************************
 * Function: rtems_ftpd_daemon                                            *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This task runs in the background forever.  It waits for service     *
 *    requests on the FTP port (port 21).  When a request is received,    *
 *    it opens a new session to handle those requests until the           *
 *    connection is closed.                                               *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    none                                                                *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    none                                                                *
 *                                                                        *
 **************************************************************************
 * Change History:                                                        *
 *  12/01/97 - Creation (JWJ)                                             *
 *************************************************************************/
static void
rtems_ftpd_daemon()
{
   int                 s;
   int                 s1;
   int                 addrLen;
   struct sockaddr_in  remoteAddr;
   struct sockaddr_in  localAddr;
   char                sessionID;
   rtems_task_priority priority;
   rtems_status_code   sc;
   rtems_id            tid;
   FTPD_SessionInfo_t  *info = NULL;


   sessionID = 'a';

   s = socket(AF_INET, SOCK_STREAM, 0);
   if (s < 0)
   {
      perror("Creating socket");
   }

   localAddr.sin_family      = AF_INET;
   localAddr.sin_port        = htons(rtems_ftpd_configuration.port);
   localAddr.sin_addr.s_addr = INADDR_ANY;
   memset(localAddr.sin_zero, '\0', sizeof(localAddr.sin_zero));
   if (bind(s, (struct sockaddr *)&localAddr,
            sizeof(localAddr)) < 0)
   {
      perror("Binding control socket");
   }

   if (listen(s, 2) < 0)
   {
      perror("Listening on control socket");
   }

   while (1)
   {
      /********************************************************************
       * Allocate a SessionInfo structure for the session task.
       *******************************************************************/
      info = (FTPD_SessionInfo_t *)malloc(sizeof(FTPD_SessionInfo_t));
      if (info == NULL)
      {
         syslog(LOG_ERR, "ftpd: Could not allocate session info struct.");
         rtems_panic("Malloc fail.");
      }

      /********************************************************************
       * Accept on the socket and start the session task.
       *******************************************************************/
      addrLen = sizeof(remoteAddr);
      s1 = accept(s, (struct sockaddr *)&remoteAddr, &addrLen);
      if (s1 < 0)
      {
         perror("Accepting control connection");
      }

      rtems_task_set_priority(RTEMS_SELF, RTEMS_CURRENT_PRIORITY, &priority);
      sc = rtems_task_create(rtems_build_name('F', 'T', 'P', sessionID),
                             priority, 8*1024,
                             RTEMS_PREEMPT | RTEMS_NO_TIMESLICE |
                             RTEMS_NO_ASR | RTEMS_INTERRUPT_LEVEL(0),
                             RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL,
                             &tid);
      if (sc != RTEMS_SUCCESSFUL)
      {
         syslog(LOG_ERR, "ftpd: Could not create FTPD session: %s", 
                rtems_status_text(sc));
      }

      if (sessionID == 'z')
      {
         sessionID = 'a';
      }
      else
      {
         sessionID++;
      }

      /********************************************************************
       * Send the socket on to the new session.
       *******************************************************************/
      if ((info->ctrl_fp = fdopen(s1, "r+")) == NULL)
      {
         syslog(LOG_ERR, "ftpd: fdopen() on socket failed.");
         close(s1);
      }
      else
      {
         sc = rtems_task_set_note(tid, RTEMS_NOTEPAD_0,
                                  (rtems_unsigned32)info);
         sc = rtems_task_start(tid, rtems_ftpd_session, 0);
         if (sc != RTEMS_SUCCESSFUL)
         {
            syslog(LOG_ERR, "ftpd: Could not start FTPD session: %s",
                   rtems_status_text(sc));
         }
      }
   }
}


/**************************************************************************
 * Function: rtems_ftpd_start                                             *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    Here, we start the FTPD task which waits for FTP requests and       *
 *    services them.  This procedure returns to its caller once the       *
 *    task is started.                                                    *
 *                                                                        *
 *                                                                        *
 * Inputs:                                                                *
 *                                                                        *
 *    rtems_task_priority priority - Priority to assign to this task.     *
 *                                                                        *
 * Output:                                                                *
 *                                                                        *
 *    int - RTEMS_SUCCESSFUL on successful start of the daemon.           *
 *                                                                        *
 **************************************************************************
 * Change History:                                                        *
 *  12/01/97 - Creation (JWJ)                                             *
 *************************************************************************/
int
rtems_initialize_ftpd()
{
   rtems_status_code   sc;
   rtems_id            tid;


   if (rtems_ftpd_configuration.port == 0)
   {
      rtems_ftpd_configuration.port = FTPD_CONTROL_PORT;
   }

   /***********************************************************************
    * Default FTPD priority.
    **********************************************************************/
   if (rtems_ftpd_configuration.priority == 0)
   { 
      rtems_ftpd_configuration.priority = 40;
   }
   sc = rtems_task_create(rtems_build_name('F', 'T', 'P', 'D'),
                          rtems_ftpd_configuration.priority, 8*1024,
                          RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_NO_ASR |
                          RTEMS_INTERRUPT_LEVEL(0),
                          RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL,
                          &tid);
   if (sc != RTEMS_SUCCESSFUL)
   {
      syslog(LOG_ERR, "ftpd: Could not create FTP daemon: %s",
             rtems_status_text(sc));
      return(RTEMS_UNSATISFIED);
   }

   sc = rtems_task_start(tid, rtems_ftpd_daemon, 0);
   if (sc != RTEMS_SUCCESSFUL)
   {
      syslog(LOG_ERR, "ftpd: Could not start FTP daemon: %s",
             rtems_status_text(sc));
      return(RTEMS_UNSATISFIED);
   }   

   syslog(LOG_INFO, "ftpd: FTP daemon started.");
   return(RTEMS_SUCCESSFUL);
}

