/*
 *  FTP Server Daemon
 *
 *  Submitted by: Jake Janovetz <janovetz@tempest.ece.uiuc.edu>
 *
 *  $Id$
 */

/*
 * Current state:
 *   To untar, put as "untar"
 * CWD uses chdir
 *   This is bad due to global setting of chdir.
 *
 * Stored files come into RAM and are saved later.  This is an artifact
 *   of a previous implementation (no filesystem -- had to do stuff with
 *   the "files" later).  This can be eliminated once all of Jake's stuff
 *   is moved to devices/filesystems.
 *
 * CLOSE(S) doesn't seem to work.  This causes problems in 
 *          several areas.  It lets too many file descriptors pile up
 *          and it doesn't seem to flush the stream.
 *
 * Is 'recv' what I want to use to get commands from the control port?
 *
 */

/**************************************************************************
 *                                 ftpd.c                                 *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    This file contains the daemon which services requests that appear   *
 *    on the 'FTP' port.  This server is compatible with FTP, but it      *
 *    also provides services specific to the Erithacus system.            *
 *    This server is started at boot-time and runs forever.               *
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
 *    Implementation Notes:                                               *
 *                                                                        *
 *       The 'current working directory' implementation utilizes the      *
 *       RTEMS filesystem cwd.  This is no good since other processes     *
 *       inherit the same cwd.                                            *
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
 *                                                                        *
 * The public routines contained in this file are:                        *
 *                                                                        *
 *    FTPD_Start - Starts the server daemon, then returns to its caller.  *
 *                                                                        *
 *                                                                        *
 * The private routines contained in this file are:                       *
 *                                                                        *
 *    FTPD_SendReply    - Sends a reply code and text through the control *
 *                        port.                                           *
 *    FTPD_CommandStore - Performs the "STOR" command.                    *
 *    FTPD_CommandList  - Performs the "LIST" command.                    *
 *    FTPD_CommandPort  - Opens a data port (the "PORT" command).         *
 *    FTPD_ParseCommand - Parses an incoming command.                     *
 *    FTPD_Session      - Begins a service session.                       *
 *    FTPD_Daemon       - Listens on the FTP port for service requests.   *
 *                                                                        *
 *                                                                        *
 *------------------------------------------------------------------------*
 *                                                                        *
 * Jake Janovetz                                                          *
 * University of Illinois                                                 *
 * 1406 West Green Street                                                 *
 * Urbana IL  61801                                                       *
 *                                                                        *
 **************************************************************************
 * Change History:                                                        *
 *  12/01/97 - Creation (JWJ)                                             *
 *************************************************************************/

/* Revision Control Information:
 *
 * $Source$
 * $Id$
 * $Log$
 * Revision 1.3  1998/05/19 21:28:17  erithacus
 * Update control socket to file I/O.
 *
 * Revision 1.2  1998/05/19 20:13:50  erithacus
 * Remodeled to be entirely reentrant.
 *
 *
 */


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
#include "untar.h"


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
 * Maximum buffer size for use by the transfer protocol.
 *  This will be eliminated when the filesystem is complete enough that
 *  we don't have to store the received data until we have something to
 *  do with it.
 *************************************************************************/
#define FTPD_MAX_RECEIVESIZE  (512*1024)

/**************************************************************************
 * SessionInfo structure.
 *
 * The following structure is allocated for each session.  The pointer
 * to this structure is contained in the tasks notepad entry.
 *************************************************************************/
typedef struct
{
   struct sockaddr_in  data_addr;   /* Data address for PORT commands */
   int                 ctrl_sock;   /* Control connection socker */
   char                cwd[255];    /* Current working directory */
                                    /* Login -- future use -- */
   int                 xfer_mode;   /* Transfer mode (ASCII/binary) */
} FTPD_SessionInfo_t;


#define FTPD_WELCOME_MESSAGE \
   "Welcome to the RTEMS FTP server.\n" \
   "\n" \
   "Login accepted.\n"


/**************************************************************************
 * Function: FTPD_SendReply                                               *
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
FTPD_SendReply(int code, char *text)
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
   }
   else
   {
      sprintf(str, "%d\r\n", code);
   }
   send(info->ctrl_sock, str, strlen(str), 0);
}


/**************************************************************************
 * Function: FTPD_CommandRetrieve                                         *
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
FTPD_CommandRetrieve(char *filename)
{
   int                 s;
   int                 n;

   FILE                *fp;
   unsigned char       *bufr;
   rtems_status_code   sc;
   FTPD_SessionInfo_t  *info = NULL;


   sc = rtems_task_get_note(RTEMS_SELF, RTEMS_NOTEPAD_0,
                            (rtems_unsigned32 *)&info);


   if ((fp = fopen(filename, "r")) == NULL)
   {
      FTPD_SendReply(450, "Error opening file.");
      return(0);
   }

   bufr = (unsigned char *)malloc(BUFSIZ);
   if (bufr == NULL)
   {
      FTPD_SendReply(440, "Server error - malloc fail.");
      fclose(fp);
      return(0);
   }

   /***********************************************************************
    * Connect to the data connection (PORT made in an earlier PORT call).
    **********************************************************************/
   FTPD_SendReply(150, "BINARY data connection.");
   s = socket(AF_INET, SOCK_STREAM, 0);
   if (connect(s, (struct sockaddr *)&info->data_addr,
               sizeof(struct sockaddr)) < 0)
   {
      FTPD_SendReply(420, "Server error - could not connect socket.");
      free(bufr);
      fclose(fp);
      close(s);
      return(1);
   }

   /***********************************************************************
    * Send the data over the ether.
    **********************************************************************/
   while ((n = fread(bufr, 1, BUFSIZ, fp)) != 0)
   {
      send(s, bufr, n, 0);
   }

   if (feof(fp))
   {
      FTPD_SendReply(210, "File sent successfully.");
   }
   else
   {
      FTPD_SendReply(450, "Retrieve failed.");
   }

   if (close(s) != 0)
   {
      syslog(LOG_ERR, "ftpd: Error closing data socket");
   }

   free(bufr);
   fclose(fp);
   return(0);
}


/**************************************************************************
 * Function: FTPD_CommandStore                                            *
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
FTPD_CommandStore(char *filename)
{
   char                *bufr;
   char                *bigBufr;
   int                 s;
   int                 n;
   unsigned long       size = 0;
   rtems_status_code   sc;
   FTPD_SessionInfo_t  *info = NULL;


   sc = rtems_task_get_note(RTEMS_SELF, RTEMS_NOTEPAD_0,
                            (rtems_unsigned32 *)&info);

   bufr = (char *)malloc(BUFSIZ * sizeof(char));
   if (bufr == NULL)
   {
      FTPD_SendReply(440, "Server error - malloc fail.");
      return(1);
   }

   bigBufr = (char *)malloc(FTPD_MAX_RECEIVESIZE * sizeof(char));
   if (bigBufr == NULL)
   {
      FTPD_SendReply(440, "Server error - malloc fail.");
      free(bufr);
      return(1);
   }

   FTPD_SendReply(150, "BINARY data connection.");

   s = socket(AF_INET, SOCK_STREAM, 0);
   if (connect(s, (struct sockaddr *)&info->data_addr,
               sizeof(struct sockaddr)) < 0)
   {
      free(bufr);
      free(bigBufr);
      close(s);
      return(1);
   }


   /***********************************************************************
    * File: "/dev/null" just throws the data away.
    **********************************************************************/
   if (!strncmp("/dev/null", filename, 9))
   {
      while ((n = read(s, bufr, BUFSIZ)) > 0);
   }
   else
   {
      /***********************************************************************
       * Retrieve the file into our buffer space.
       **********************************************************************/
      size = 0;
      while ((n = read(s, bufr, BUFSIZ)) > 0)
      {
         if (size + n > FTPD_MAX_RECEIVESIZE)
         {
            FTPD_SendReply(440, "Server error - Buffer size exceeded.");
            free(bufr);
            free(bigBufr);
            close(s);
            return(1);
         }
         memcpy(&bigBufr[size], bufr, n);
         size += n;
      }
   }
   free(bufr);
   close(s);


   /***********************************************************************
    * Figure out what to do with the data we just received.
    **********************************************************************/
   if (!strncmp("untar", filename, 5))
   {
      Untar_FromMemory(bigBufr, size);
      FTPD_SendReply(210, "Untar successful.");
   }
   else
   {
      FILE   *fp;
      size_t len;
      size_t written;

      fp = fopen(filename, "w");
      if (fp == NULL)
      {
         FTPD_SendReply(440, "Could not open file.");
         free(bigBufr);
         return(1);
      }
      
      n = 0;
      written = 0;
      while (n<size)
      {
         len = ((size-n>BUFSIZ)?(BUFSIZ):(size-n));
         written = fwrite(&bigBufr[n], 1, len, fp);
         n += written;
         if (written != len)
         {
            break;
         }
      }
      fclose(fp);

      if (n == size)
      {
         FTPD_SendReply(226, "Transfer complete.");
      }
      else
      {
         FTPD_SendReply(440, "Error during write.");
         free(bigBufr);
         return(1);
      }
   }

   free(bigBufr);
   return(0);
}


/**************************************************************************
 * Function: FTPD_CommandList                                             *
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
FTPD_CommandList(char *fname)
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

   FTPD_SendReply(150, "ASCII data connection for LIST.");

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
      FTPD_SendReply(226, "Transfer complete.");
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
   FTPD_SendReply(226, "Transfer complete.");
}


/*
 * Cheesy way to change directories
 */
static void
FTPD_CWD(char *dir)
{
   rtems_status_code   sc;
   FTPD_SessionInfo_t  *info = NULL;


   sc = rtems_task_get_note(RTEMS_SELF, RTEMS_NOTEPAD_0,
                            (rtems_unsigned32 *)&info);

   if (chdir(dir) == 0)
   {
      FTPD_SendReply(250, "CWD command successful.");
   }
   else
   {
      FTPD_SendReply(550, "CWD command failed.");
   }
}


/**************************************************************************
 * Function: FTPD_CommandPort                                             *
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
FTPD_CommandPort(char *bufr)
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
 * Function: FTPD_ParseCommand                                            *
 **************************************************************************
 * Description:                                                           *
 *                                                                        *
 *    Here, we parse the commands that have come through the control      *
 *    connection.                                                         *
 *                                                                        *
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
FTPD_ParseCommand(char *bufr)
{
   char fname[255];
   rtems_status_code   sc;
   FTPD_SessionInfo_t  *info = NULL;


   sc = rtems_task_get_note(RTEMS_SELF, RTEMS_NOTEPAD_0,
                            (rtems_unsigned32 *)&info);

   if (!strncmp("PORT", bufr, 4))
   {
      FTPD_SendReply(200, "PORT command successful.");
      FTPD_CommandPort(&bufr[5]);
   }
   else if (!strncmp("RETR", bufr, 4))
   {
      sscanf(&bufr[5], "%254s", fname);
      FTPD_CommandRetrieve(fname);
   }
   else if (!strncmp("STOR", bufr, 4))
   {
      sscanf(&bufr[5], "%254s", fname);
      FTPD_CommandStore(fname);
   }
   else if (!strncmp("LIST", bufr, 4))
   {
      if (bufr[5] == '\n')
      {
         FTPD_CommandList(".");
      }
      else
      {
         sscanf(&bufr[5], "%254s", fname);
         FTPD_CommandList(fname);
      }
   }
   else if (!strncmp("USER", bufr, 4))
   {
      FTPD_SendReply(230, "User logged in.");
   }
   else if (!strncmp("SYST", bufr, 4))
   {
      FTPD_SendReply(240, "RTEMS");
   }
   else if (!strncmp("TYPE", bufr, 4))
   {
      if (bufr[5] == 'I')
      {
         info->xfer_mode = TYPE_I;
         FTPD_SendReply(200, "Type set to I.");
      }
      else if (bufr[5] == 'A')
      {
         info->xfer_mode = TYPE_A;
         FTPD_SendReply(200, "Type set to A.");
      } 
      else
      {
         info->xfer_mode = TYPE_I;
         FTPD_SendReply(504, "Type not implemented.  Set to I.");
      }
   }
   else if (!strncmp("PASS", bufr, 4))
   {
      FTPD_SendReply(230, "User logged in.");
   }
   else if (!strncmp("DELE", bufr, 4))
   {
      sscanf(&bufr[4], "%254s", fname);
      if (unlink(fname) == 0)
      {
         FTPD_SendReply(257, "DELE successful.");
      }
      else
      {
         FTPD_SendReply(550, "DELE failed.");
      }
   }
   else if (!strncmp("SITE CHMOD", bufr, 10))
   {
      int mask;

      sscanf(&bufr[11], "%o %254s", &mask, fname);
      if (chmod(fname, (mode_t)mask) == 0)
      {
         FTPD_SendReply(257, "CHMOD successful.");
      }
      else
      {
         FTPD_SendReply(550, "CHMOD failed.");
      }
   }
   else if (!strncmp("RMD", bufr, 3))
   {
      sscanf(&bufr[4], "%254s", fname);
      if (rmdir(fname) == 0)
      {
         FTPD_SendReply(257, "RMD successful.");
      }
      else
      {
         FTPD_SendReply(550, "RMD failed.");
      }
   }
   else if (!strncmp("MKD", bufr, 3))
   {
      sscanf(&bufr[4], "%254s", fname);
      if (mkdir(fname, S_IRWXU | S_IRWXG | S_IRWXO) == 0)
      {
         FTPD_SendReply(257, "MKD successful.");
      }
      else
      {
         FTPD_SendReply(550, "MKD failed.");
      }
   }
   else if (!strncmp("CWD", bufr, 3))
   {
      sscanf(&bufr[4], "%254s", fname);
      FTPD_CWD(fname);
   }
   else if (!strncmp("PWD", bufr, 3))
   {
      char *cwd = getcwd(0, 0);
      sprintf(bufr, "\"%s\" is the current directory.", cwd);
      FTPD_SendReply(250, bufr);
      free(cwd);
   }
   else
   {
      FTPD_SendReply(500, "Unrecognized/unsupported command.");
   }
}

/**************************************************************************
 * Function: FTPD_Session                                                 *
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
 *    rtems_task_argument arg - The FTPD_Daemon task passes the socket    *
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
FTPD_Session(rtems_task_argument arg)
{
   char                cmd[256];
   rtems_status_code   sc;
   FTPD_SessionInfo_t  *info = NULL;


   sc = rtems_task_get_note(RTEMS_SELF, RTEMS_NOTEPAD_0,
                            (rtems_unsigned32 *)&info);

   FTPD_SendReply(220, "Erithacus FTP server (Version 1.0) ready.");

   /***********************************************************************
    * Set initial directory to "/".
    **********************************************************************/
   strcpy(info->cwd, "/");
   info->xfer_mode = TYPE_A;
   while (1)
   {
      if (recv(info->ctrl_sock, cmd, 256, 0) == -1)
      {
         syslog(LOG_INFO, "ftpd: Connection aborted.");
         break;
      }

      if (!strncmp("QUIT", cmd, 4))
      {
         FTPD_SendReply(221, "Goodbye.");
         break;
      }
      else
      {
         FTPD_ParseCommand(cmd);
      }
   }

   if (close(info->ctrl_sock) < 0)
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
 * Function: FTPD_Daemon                                                  *
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
FTPD_Daemon()
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
   localAddr.sin_port        = FTPD_CONTROL_PORT;
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
      info->ctrl_sock = s1;
      sc = rtems_task_set_note(tid, RTEMS_NOTEPAD_0,
                               (rtems_unsigned32)info);
      sc = rtems_task_start(tid, FTPD_Session, 0);
      if (sc != RTEMS_SUCCESSFUL)
      {
         syslog(LOG_ERR, "ftpd: Could not start FTPD session: %s",
                rtems_status_text(sc));
      }
   }
}


/**************************************************************************
 * Function: FTPD_Start                                                   *
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
 *    none                                                                *
 *                                                                        *
 **************************************************************************
 * Change History:                                                        *
 *  12/01/97 - Creation (JWJ)                                             *
 *************************************************************************/
void
FTPD_Start(rtems_task_priority priority)
{
   rtems_status_code   sc;
   rtems_id            tid;


   sc = rtems_task_create(rtems_build_name('F', 'T', 'P', 'D'),
                          priority, 8*1024,
                          RTEMS_PREEMPT | RTEMS_NO_TIMESLICE | RTEMS_NO_ASR |
                          RTEMS_INTERRUPT_LEVEL(0),
                          RTEMS_NO_FLOATING_POINT | RTEMS_LOCAL,
                          &tid);
   if (sc != RTEMS_SUCCESSFUL)
   {
      syslog(LOG_ERR, "ftpd: Could not create FTP daemon: %s",
             rtems_status_text(sc));
   }

   sc = rtems_task_start(tid, FTPD_Daemon, 0);
   if (sc != RTEMS_SUCCESSFUL)
   {
      syslog(LOG_ERR, "ftpd: Could not start FTP daemon: %s",
             rtems_status_text(sc));
   }   

   syslog(LOG_INFO, "ftpd: FTP daemon started.");
}
