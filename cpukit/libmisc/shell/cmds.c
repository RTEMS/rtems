/*
 * Author: Fernando RUIZ CASAS
 *
 *  Work: fernando.ruiz@ctv.es
 *  Home: correo@fernando-ruiz.com
 *
 * This file is inspired in rtems_monitor & Chris John MyRightBoot
 * 
 * But I want to make it more user friendly
 * A 'monitor' command is added to adapt the call rtems monitor commands
 * at my call procedure
 * 
 * TODO: A lot of improvements of course. 
 *      cp, mv, ...
 *      hexdump,
 *      
 *      More? Say me it, please...
 *      
 *      The BSP Specific are not welcome here.
 *      
 * C&S welcome...
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <sys/types.h>
#include <stddef.h>

#include <rtems.h>
#include <rtems/monitor.h>
#include <rtems/score/tod.h>

#include <imfs.h>
#include <rtems/shell.h>

/* ----------------------------------------------- *
  - str to int "0xaffe" "0b010010" "0123" "192939"
 * ----------------------------------------------- */
int str2int(char * s) {
 int sign=1;	
 int base=10;
 int value=0;
 int digit;
 if (!s) return 0;
 if (*s) {
  if (*s=='-') {
   sign=-1;
   s++;
   if (!*s) return 0;
  };
  if (*s=='0') {
   s++;
   switch(*s) {
    case 'x':
    case 'X':s++;
 	     base=16;
	     break;
    case 'b':
    case 'B':s++;
	     base=2;
	     break;
    default :base=8;
	     break;
   }
  }; 
  while (*s) {
   switch(*s) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':digit=*s-'0';
 	     break;
    case 'A':	   
    case 'B':	   
    case 'C':	   
    case 'D':	   
    case 'E':	   
    case 'F':digit=*s-'A'+10;
	     break;
    case 'a':	   
    case 'b':	   
    case 'c':	   
    case 'd':	   
    case 'e':	   
    case 'f':digit=*s-'a'+10;
	     break;
    default:return value*sign;	     
   };
   if (digit>base) return value*sign;
   value=value*base+digit;
   s++;
  };
 };
 return value*sign;	
}
/*----------------------------------------------------------------------------*
 * RAM MEMORY COMMANDS
 *----------------------------------------------------------------------------*/

#define mdump_adr (current_shell_env->mdump_adr)  /* static value */

int main_mdump(int argc,char * argv[]) {
 unsigned char n,m,max=0;
 int adr=mdump_adr;
 unsigned char * pb;
 if (argc>1) adr=str2int(argv[1]);
 if (argc>2) max=str2int(argv[2]);
 max/=16;
 if (!max) max=20;
 for (m=0;m<max;m++) {
  printf("0x%08X ",adr);
  pb=(unsigned char*) adr;
  for (n=0;n<16;n++)
   printf("%02X%c",pb[n],n==7?'-':' ');
  for (n=0;n<16;n++) {
   printf("%c",isprint(pb[n])?pb[n]:'.');
  };
  printf("\n");
  adr+=16;
 };
 mdump_adr=adr;
 return 0;
}
/*----------------------------------------------------------------------------*/
int main_mwdump(int argc,char * argv[]) {
 unsigned char n,m,max=0;
 int adr=mdump_adr;
 unsigned short * pw;
 if (argc>1) adr=str2int(argv[1]);
 if (argc>2) max=str2int(argv[2]);
 max/=16;
 if (!max) max=20;
 for (m=0;m<max;m++) {
  printf("0x%08X ",adr);
  pw=(unsigned short*) adr;
  for (n=0;n<8;n++)
   printf("%02X %02X%c",pw[n]/0x100,pw[n]%0x100,n==3?'-':' ');
  for (n=0;n<8;n++) {
   printf("%c",isprint(pw[n]/0x100)?pw[n]/0x100:'.');
   printf("%c",isprint(pw[n]%0x100)?pw[n]%0x100:'.');
  };
  printf("\n");
  adr+=16;
 };
 mdump_adr=adr;
 return 0;
}
/*----------------------------------------------------------------------------*/
int main_medit(int argc,char * argv[]) {
 unsigned char * pb;
 int n,i;
 if (argc<3) {
  printf("too few arguments\n");
  return 0;
 };
 pb=(unsigned char*)str2int(argv[1]);
 i=2;
 n=0;
 while (i<=argc) {
  pb[n++]=str2int(argv[i++])%0x100;
 }
 mdump_adr=(int)pb;
 return main_mdump(0,NULL);
}
/*----------------------------------------------------------------------------*/
int main_mfill(int argc,char * argv[]) {
 int  adr;
 int  size;
 unsigned char value;
 if (argc<4) {
  printf("too few arguments\n");
  return 0;
 };
 adr  =str2int(argv[1]);
 size =str2int(argv[2]);
 value=str2int(argv[3])%0x100;
 memset((unsigned char*)adr,size,value);
 mdump_adr=adr;
 return main_mdump(0,NULL);
}
/*----------------------------------------------------------------------------*/
int main_mmove(int argc,char * argv[]) {
 int  src;
 int  dst;
 int  size;
 if (argc<4) {
  printf("too few arguments\n");
  return 0;
 };
 dst  =str2int(argv[1]);
 src  =str2int(argv[2]);
 size =str2int(argv[3]);
 memcpy((unsigned char*)dst,(unsigned char*)src,size);
 mdump_adr=dst;
 return main_mdump(0,NULL);
}
/*----------------------------------------------------------------------------*/
#ifdef MALLOC_STATS  /* /rtems/s/src/lib/libc/malloc.c */
int main_malloc_dump(int argc,char * argv[]) {
 void malloc_dump(void); 
 malloc_dump();	
 return 0;
}
#endif
/*----------------------------------------------------------------------------
 * Reset. Assumes that the watchdog is present.
 *----------------------------------------------------------------------------*/
int main_reset (int argc, char **argv)
{
  rtems_interrupt_level level;
  printf ("Waiting for watchdog ... ");
  tcdrain(fileno(stdout));

  rtems_interrupt_disable (level);
  for (;;) 
      ;
  return 0;
}
/*----------------------------------------------------------------------------
 * Alias. make an alias
 *----------------------------------------------------------------------------*/
int main_alias (int argc, char **argv)
{
 if (argc<3) {
  printf("too few arguments\n");
  return 1;
 };
 if (!shell_alias_cmd(argv[1],argv[2])) {
  printf("unable to make an alias(%s,%s)\n",argv[1],argv[2]);
 };
 return 0;
}  
/*-----------------------------------------------------------*  	
 * Directory commands
 *-----------------------------------------------------------*/
int main_ls(int argc, char *argv[])
{
   char * fname;
   DIR                 *dirp;
   struct dirent       *dp;
   struct stat         stat_buf;
   struct passwd     * pwd;
   struct group      * grp;
   char * user;
   char * group;
   char   sbuf[256];
   char   nbuf[1024];
   int  n,size;

   fname=".";
   if (argc>1) fname=argv[1];

   if ((dirp = opendir(fname)) == NULL)
   {
      printf("%s: No such file or directory.\n", fname);
      return errno;
   }
   n=0;
   size=0;
   while ((dp = readdir(dirp)) != NULL)
   {
      strcpy(nbuf,fname);
      if (nbuf[strlen(nbuf)-1]!='/') strcat(nbuf,"/");
      strcat(nbuf,dp->d_name); /* always the fullpathname. Avoid ftpd problem.*/
      if (stat(nbuf, &stat_buf) == 0)
      { /* AWFUL buts works...*/
	 strftime(sbuf,sizeof(sbuf)-1,"%b %d %H:%M",gmtime(&stat_buf.st_mtime));     
	 pwd=getpwuid(stat_buf.st_uid);
	 user=pwd?pwd->pw_name:"nouser";
	 grp=getgrgid(stat_buf.st_gid);
	 group=grp?grp->gr_name:"nogrp";
         printf("%c%c%c%c%c%c%c%c%c%c %3d %6.6s %6.6s %11d %s %s%c\n",
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
                 (int)stat_buf.st_nlink,
	 	 user,group, 
                 (int)stat_buf.st_size,
		 sbuf,
                 dp->d_name,
                 S_ISDIR(stat_buf.st_mode)?'/':' ');
	 n++;
	 size+=stat_buf.st_size;
      }
   }
   printf("%d files %d bytes occupied\n",n,size);
   closedir(dirp);
   return 0;
}
/*-----------------------------------------------------------*/  	
int main_pwd (int argc, char *argv[]) {
   char dir[1024];
   getcwd(dir,1024);
   printf("%s\n",dir);
   return 0;
}
/*-----------------------------------------------------------*/  	
int main_chdir (int argc, char *argv[]) {
   char *dir;
   dir="/";
   if (argc>1) dir=argv[1];
   if (chdir(dir)) {
    printf("chdir to '%s' failed:%s\n",dir,strerror(errno));
    return errno;
   }; 
   return 0;
}
/*-----------------------------------------------------------*/  	
int main_mkdir (int argc, char *argv[]) {
   char *dir;
   int n;
   n=1;
   while (n<argc) {
    dir=argv[n++];
    if (mkdir(dir,S_IRWXU|S_IRWXG|S_IRWXO)) {
      printf("mkdir '%s' failed:%s\n",dir,strerror(errno));
    };  
   };  
   return errno;
}
/*-----------------------------------------------------------*/  	
int main_rmdir (int argc, char *argv[])
{
   char *dir;
   int n;
   n=1;
   while (n<argc) {
    dir=argv[n++];
    if (rmdir(dir)) printf("rmdir '%s' failed:%s\n",dir,strerror(errno));
   };
   return errno;
}
/*-----------------------------------------------------------*/  	
int main_chroot(int argc,char * argv[]) {
 char * new_root="/";
 if (argc==2) new_root=argv[1];
 if (chroot(new_root)<0) {
  printf("error %s:chroot(%s);\n",strerror(errno),new_root);
  return -1;
 };
 return 0;
}
/*-----------------------------------------------------------*/  	
int main_cat   (int argc, char *argv[])
{
   int n;
   n=1;
   while (n<argc) cat_file(stdout,argv[n++]);
   return 0;
}
/*-----------------------------------------------------------*/  	
int main_rm    (int argc, char *argv[])
{
   int n;
   n=1;
   while (n<argc) {
    if (unlink(argv[n])) {
     printf("error %s:rm %s\n",strerror(errno),argv[n]);
     return -1;
    };
    n++;
   };
   return 0;
}
/*-----------------------------------------------------------*/  	
/* date - print time and date */

int main_date(int argc,char *argv[]) 
{
  time_t t;
  time(&t);
  printf("%s", ctime(&t));
  return 0;
}
/*-----------------------------------------------------------*/
int main_logoff(int argc,char *argv[]) 
{
  printf("logoff from the system...");	
  current_shell_env->exit_shell=TRUE;	
  return 0;
}
/*-----------------------------------------------------------*/
int main_tty   (int argc,char *argv[]) 
{
  printf("%s\n",ttyname(fileno(stdin)));
  return 0;
}
/*-----------------------------------------------------------*/
int main_whoami(int argc,char *argv[]) 
{
   struct passwd     * pwd;
   pwd=getpwuid(getuid());
   printf("%s\n",pwd?pwd->pw_name:"nobody");
   return 0;
}
/*-----------------------------------------------------------*/
int main_id    (int argc,char *argv[]) 
{
   struct passwd     * pwd;
   struct group      * grp;
   pwd=getpwuid(getuid());
   grp=getgrgid(getgid());
   printf("uid=%d(%s),gid=%d(%s),",
		   getuid(),pwd?pwd->pw_name:"",
		   getgid(),grp?grp->gr_name:"");
   pwd=getpwuid(geteuid());
   grp=getgrgid(getegid());
   printf("euid=%d(%s),egid=%d(%s)\n",
		   geteuid(),pwd?pwd->pw_name:"",
		   getegid(),grp?grp->gr_name:"");
   return 0;
}
/*-----------------------------------------------------------*/
int main_umask(int argc,char *argv[]) 
{
   mode_t msk=umask(0);
   if (argc == 2) msk=str2int(argv[1]);
   umask(msk);
   msk=umask(0);
   printf("0%o\n",msk);
   umask(msk);
   return 0;
}
/*-----------------------------------------------------------*/
int main_chmod(int argc,char *argv[]) 
{
   int n;
   mode_t mode;
   if (argc > 2){
    mode=str2int(argv[1])&0777;
    n=2;
    while (n<argc) chmod(argv[n++],mode);
   };
   return 0;
}
/*-----------------------------------------------------------*  	
 * with this you can call at all the rtems monitor commands.
 * Not all work fine but you can show the rtems status and more.
 *-----------------------------------------------------------*/  	
int main_monitor(int argc,char * argv[]) {
 rtems_monitor_command_entry_t *command;
 extern rtems_monitor_command_entry_t rtems_monitor_commands[];
 rtems_task_ident(RTEMS_SELF,0,&rtems_monitor_task_id);
 rtems_monitor_node = rtems_get_node(rtems_monitor_task_id);
 rtems_monitor_default_node = rtems_monitor_node;
 if ((command=rtems_monitor_command_lookup(rtems_monitor_commands,argc,argv)))
  command->command_function(argc, argv, command->command_arg, 0);
 return 0;
}
/*-----------------------------------------------------------*/  	
void register_cmds(void) {
  rtems_monitor_command_entry_t *command;
  extern rtems_monitor_command_entry_t rtems_monitor_commands[];
  /* monitor topic */
  command=rtems_monitor_commands;
  while (command) {
   if (strcmp("exit",command->command)) /*Exclude EXIT (alias quit)*/
    shell_add_cmd(command->command,"monitor",
                  command->usage  ,main_monitor);
   command=command->next;
  };
  /* dir[ectories] topic */
  shell_add_cmd  ("ls"    ,"dir","ls [dir]     # list files in the directory" ,main_ls   );
  shell_add_cmd  ("chdir" ,"dir","chdir [dir]  # change the current directory",main_chdir);
  shell_add_cmd  ("rmdir" ,"dir","rmdir  dir   # remove directory"            ,main_rmdir);
  shell_add_cmd  ("mkdir" ,"dir","mkdir  dir   # make a directory"            ,main_mkdir);
  shell_add_cmd  ("pwd"   ,"dir","pwd          # print work directory"        ,main_pwd  );
  shell_add_cmd  ("chroot","dir","chroot [dir] # change the root directory"   ,main_chroot);
  shell_add_cmd  ("cat"   ,"dir","cat n1 [n2 [n3...]]# show the ascii contents",main_cat );
  shell_add_cmd  ("rm"    ,"dir","rm n1 [n2 [n3...]]# remove files"           ,main_rm   );
  shell_add_cmd  ("chmod" ,"dir","chmod 0777 n1 n2... #change filemode"       ,main_chmod);

  shell_alias_cmd("ls"    ,"dir");
  shell_alias_cmd("chdir" ,"cd");

  /* misc. topic */
  shell_add_cmd  ("logoff","misc","logoff from the system"                    ,main_logoff);
  shell_alias_cmd("logoff","exit"); 
  shell_add_cmd  ("date" ,"misc","date"                                       ,main_date);
  shell_add_cmd  ("reset","misc","reset the BSP"                              ,main_reset);
  shell_add_cmd  ("alias","misc","alias old new"                              ,main_alias);
  shell_add_cmd  ("tty"  ,"misc","show ttyname"                               ,main_tty  );
  shell_add_cmd  ("whoami","misc","show current user"                         ,main_whoami);
  shell_add_cmd  ("id"    ,"misc","show uid,gid,euid,egid"                    ,main_id    );
  shell_add_cmd  ("umask" ,"misc","umask [new_umask]"                         ,main_umask );


  /* memory topic */
  shell_add_cmd  ("mdump","mem"  ,"mdump [adr [size]]"           ,main_mdump);
  shell_add_cmd  ("wdump","mem"  ,"wdump [adr [size]]"           ,main_mwdump);
  shell_add_cmd  ("medit","mem"  ,"medit adr value [value ...]"  ,main_medit);
  shell_add_cmd  ("mfill","mem"  ,"mfill adr size value"         ,main_mfill);
  shell_add_cmd  ("mmove","mem"  ,"mmove dst src size"           ,main_mmove);
#ifdef MALLOC_STATS  /* /rtems/s/src/lib/libc/malloc.c */
  shell_add_cmd  ("malloc","mem","mem  show memory malloc'ed"                 ,main_mem);
#endif  
}
/*-----------------------------------------------------------*/  	
