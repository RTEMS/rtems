/*
 *
 *  Instantatiate a new terminal shell.
 *
 *  Author: 
 *
 *   WORK: fernando.ruiz@ctv.es 
 *   HOME: correo@fernando-ruiz.com
 *
 *   Thanks at:
 *    Chris John
 *
 *  $Id$
 */

#include <stdio.h>

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/libio.h>
#include <rtems/libio_.h> 

#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

#include <rtems/shell.h>
/* ----------------------------------------------- *
 * This is a stupidity but is cute.
 * ----------------------------------------------- */
rtems_unsigned32 new_rtems_name(char * rtems_name) {
	static char b[5];
	sprintf(b,"%-4.4s",rtems_name);
	return rtems_build_name(b[0],b[1],b[2],b[3]);
}
/* **************************************************************
 * common linked list of shell commands.
 * Because the help report is very long
 * I have a topic for each command.
 * Help list the topics
 * help [topic] list the commands for the topic
 * help [command] help for the command
 * Can you see help rtems monitor report?
 * ************************************************************** */

struct shell_topic_tt;
typedef struct shell_topic_tt shell_topic_t;

struct shell_topic_tt {
	char * topic;
	shell_topic_t * next;
};


static shell_cmd_t   * shell_first_cmd;
static shell_topic_t * shell_first_topic;
/* ----------------------------------------------- *
 * Using Chain I can reuse the rtems code. 
 * I am more comfortable with this, sorry.
 * ----------------------------------------------- */
shell_topic_t * shell_lookup_topic(char * topic) {
  shell_topic_t * shell_topic;
  shell_topic=shell_first_topic;
  while (shell_topic) {
   if (!strcmp(shell_topic->topic,topic)) return shell_topic;
   shell_topic=shell_topic->next;
  };
  return (shell_topic_t *) NULL;
}
/* ----------------------------------------------- */
shell_topic_t * shell_add_topic(char * topic) {
 shell_topic_t * current,*aux;
 if (!shell_first_topic) {
  aux=malloc(sizeof(shell_topic_t));	
  aux->topic=topic;
  aux->next=(shell_topic_t*)NULL;
  return shell_first_topic=aux;
 } else {
  current=shell_first_topic;
  if (!strcmp(topic,current->topic)) return current;
  while (current->next) {
   if (!strcmp(topic,current->next->topic)) return current->next;
   current=current->next;
  }; 
  aux=malloc(sizeof(shell_topic_t));	
  aux->topic=topic;
  aux->next=(shell_topic_t*)NULL;
  current->next=aux;
  return aux;
 };
}
/* ----------------------------------------------- */
shell_cmd_t * shell_lookup_cmd(char * cmd) {
  shell_cmd_t * shell_cmd;
  shell_cmd=shell_first_cmd;
  while (shell_cmd) {
   if (!strcmp(shell_cmd->name,cmd)) return shell_cmd;
   shell_cmd=shell_cmd->next;
  };
  return (shell_cmd_t *) NULL;
}
/* ----------------------------------------------- */
shell_cmd_t * shell_add_cmd(char * cmd,
		      char * topic,
		      char * usage,
                      shell_command_t command) {
  int shell_help(int argc,char * argv[]);
  shell_cmd_t * shell_cmd,*shell_pvt;
  if (!shell_first_cmd) {
   shell_first_cmd=(shell_cmd_t *) malloc(sizeof(shell_cmd_t));
   shell_first_cmd->name   ="help";
   shell_first_cmd->topic  ="help";
   shell_first_cmd->usage  ="help [topic] # list of usage of commands";
   shell_first_cmd->command=shell_help;
   shell_first_cmd->alias  =(shell_cmd_t *) NULL;
   shell_first_cmd->next   =(shell_cmd_t *) NULL;
   shell_add_topic(shell_first_cmd->topic);
   register_cmds();
  };
  if (!cmd)     return (shell_cmd_t *) NULL;
  if (!command) return (shell_cmd_t *) NULL;
  shell_cmd=(shell_cmd_t *) malloc(sizeof(shell_cmd_t));
  shell_cmd->name   =cmd;
  shell_cmd->topic  =topic;
  shell_cmd->usage  =usage;
  shell_cmd->command=command;
  shell_cmd->alias  =(shell_cmd_t *) NULL;
  shell_cmd->next   =(shell_cmd_t *) NULL;
  shell_add_topic(shell_cmd->topic);
  shell_pvt=shell_first_cmd;
  while (shell_pvt->next) shell_pvt=shell_pvt->next;
  return shell_pvt->next=shell_cmd;
}
/* ----------------------------------------------- *
 * you can make an alias for every command.
 * ----------------------------------------------- */
shell_cmd_t * shell_alias_cmd(char * cmd, char * alias) {
  shell_cmd_t * shell_cmd,* shell_aux;
  shell_aux=(shell_cmd_t *) NULL;
  if (alias) {
   if ((shell_aux=shell_lookup_cmd(alias))!=NULL) {
    return NULL;
   };
   if ((shell_cmd=shell_lookup_cmd(cmd))!=NULL) {
    shell_aux=shell_add_cmd(alias,shell_cmd->topic,
		            shell_cmd->usage,shell_cmd->command);
    if (shell_aux) shell_aux->alias=shell_cmd;
   };
  };
  return shell_aux;
}
/* ----------------------------------------------- *
 * Poor but enough..
 * TODO: Redirection capture. "" evaluate, ... C&S welcome.
 * ----------------------------------------------- */
int shell_make_args(char * cmd,
		 int  * pargc,
		 char * argv[]) {
  int argc=0;
  while ((cmd=strtok(cmd," \t\r\n"))!=NULL) {
    argv[argc++]=cmd;
    cmd=(char*)NULL;
   };
  argv[argc]=(char*)NULL;
  return *pargc=argc;
}
/* ----------------------------------------------- *
 * show the help for one command.
 * ----------------------------------------------- */
int shell_help_cmd(shell_cmd_t * shell_cmd) {
  char * pc;
  int    col,line;
  printf("%-10.10s -",shell_cmd->name);
  col=12;
  line=1;
  if (shell_cmd->alias) {
   printf("is an <alias> for command '%s'",shell_cmd->alias->name);
  } else 
  if (shell_cmd->usage) {
   pc=shell_cmd->usage;
   while (*pc) {
    switch(*pc) {
     case '\r':break;
     case '\n':putchar('\n');
               col=0;
               break;
     default  :putchar(*pc);
               col++;
               break;
    };
    pc++;	    
    if(col>78) { /* What daring... 78?*/
     if (*pc) {
      putchar('\n');
      col=0;
     };
    };
    if (!col && *pc) {
      printf("            ");
      col=12;line++;
    };  
   };
  };
  puts("");
  return line;
}
/* ----------------------------------------------- *
 * show the help. The first command implemented.
 * Can you see the header of routine? Known?
 * The same with all the commands....
 * ----------------------------------------------- */
int shell_help(int argc,char * argv[]) {
  int col,line,arg;	
  shell_topic_t *topic;
  shell_cmd_t * shell_cmd=shell_first_cmd;
  if (argc<2) {
   printf("help: TOPIC? The topics are\n");	  
   topic=shell_first_topic;
   col=0;
   while (topic) {
    if (!col){
     col=printf("   %s",topic->topic);
    } else {
     if ((col+strlen(topic->topic)+2)>78){
      printf("\n");
      col=printf("   %s",topic->topic);
     } else {
      col+=printf(", %s",topic->topic);
     }; 
    };
    topic=topic->next;
   };
   printf("\n");
   return 1;
  };
  line=0;
  for (arg=1;arg<argc;arg++) {
   if (line>16) {
    printf("Press any key to continue...");getchar();
    printf("\n");
    line=0;
   };
   topic=shell_lookup_topic(argv[arg]);
   if (!topic){
    if ((shell_cmd=shell_lookup_cmd(argv[arg]))==NULL) {
     printf("help: topic or cmd '%s' not found. Try <help> alone for a list\n",argv[arg]);
     line++;
    } else {
     line+=shell_help_cmd(shell_cmd);
    }
    continue;
   };
   printf("help: list for the topic '%s'\n",argv[arg]);
   line++;
   while (shell_cmd) {
    if (!strcmp(topic->topic,shell_cmd->topic)) 
     line+=shell_help_cmd(shell_cmd);
    if (line>16) {
     printf("Press any key to continue...");getchar();
     printf("\n");
     line=0;
    };
    shell_cmd=shell_cmd->next;
   };
  }; 
  puts("");
  return 0;
}
/* ----------------------------------------------- *
 * TODO:Change to bash readline() better.
 * ----------------------------------------------- */
int shell_scanline(char * line,int size,FILE * in,FILE * out) {
  int c,col;
  col=0;
  for (;;) {
   line[col]=0;	  
   c=fgetc(in);
   switch (c) {
    case EOF :return 0;
    case '\r':if (out) fputc('\n',out);
	      return 1;
    case '\b':if (col) {
	       if (out) {
	        fputc('\b',out);
	        fputc(' ',out);
	        fputc('\b',out);
	       };
	       col--;
	      } else {
	       if (out) fputc('\a',out);
	      };
	      break;
    default  :if (!iscntrl(c)) {
		if (col<size-1) {
		 line[col++]=c;	
	         if (out) fputc(c,out);
		} else {
	          if (out) fputc('\a',out);
		};
	      } else {
	        if (out) fputc('\a',out);
	      };
	      break;
   };
  };
}
/* ----------------------------------------------- *
 * - The shell TASK                               
 * Poor but enough..
 * TODO: Redirection. Tty Signals. ENVVARs. Shell language.
 * ----------------------------------------------- */
shell_env_t global_shell_env ,
          * current_shell_env=&global_shell_env; 

extern char **environ;	  

rtems_task shell_shell(rtems_task_argument task_argument) {

  shell_env_t * shell_env =(shell_env_t*) task_argument;
  shell_cmd_t * shell_cmd;

  rtems_status_code sc;

  struct termios term;	
  char * devname;
  char * curdir;

  char cmd[256];
  char last_cmd[256]; /* to repeat 'r' */
  int  argc;
  char * argv[128];

  sc=rtems_libio_set_private_env();
  if (sc!=RTEMS_SUCCESSFUL) {
   rtems_error(sc,"rtems_libio_set_private_env():");	 
   printk("rtems_libio_set_private_env():%d",sc);	 
   rtems_task_delete(RTEMS_SELF);
  }; 

  sc=rtems_task_variable_add(RTEMS_SELF,(void*)&current_shell_env,free);
  if (sc!=RTEMS_SUCCESSFUL) {
   rtems_error(sc,"rtems_task_variable_add():");	 
   printk("rtems_task_variable_add():%d",sc);	 
   rtems_task_delete(RTEMS_SELF);
  }; 

  current_shell_env=shell_env; /* Set the task var */

  devname=shell_env->devname;

  stdin =fopen(devname,"r+");
    
  if (!stdin) {
   fprintf(stderr,"shell:unable to open stdin.%s:%s\n",devname,strerror(errno));
   printk("shell:unable to open stdin.(%s)",strerror(errno));	 
   rtems_task_delete(RTEMS_SELF);
  };
  setvbuf(stdin,NULL,_IONBF,0); /* Not buffered*/
  /* make a raw terminal,Linux MANuals */
  if (tcgetattr (fileno(stdin), &term)>=0) {
   term.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
   term.c_oflag &= ~OPOST;
   term.c_oflag |= (OPOST|ONLCR);
   term.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
   term.c_cflag  = CLOCAL | CREAD |(shell_env->tcflag);
   term.c_cc[VMIN]  = 1;
   term.c_cc[VTIME] = 0;
   if (tcsetattr (fileno(stdin), TCSADRAIN, &term) < 0) {
     fprintf(stderr,"shell:cannot set terminal attributes(%s)\n",devname);
   };
   stdout=fopen(devname,"r+");
   if (!stdout) {
    fprintf(stderr,"shell:unable to open stdout.%s:%s\n",devname,strerror(errno));
   };
   stderr=fopen(devname,"r+");
   if (!stderr) {
    printf("shell:unable to open stderr.%s:%s\n",devname,strerror(errno));
   };
   /* when the future user environment runs ok
    * a freopen() reopens the terminals. Now this don't work
    * (sorry but you can't use because FILENO_STDIN!=0. Better fileno(stdin))
    */
  };
  shell_add_cmd(NULL,NULL,NULL,NULL); /* init the chain list*/
  strcpy(cmd,"");
  printf("\n"
         "RTEMS-SHELL:%s. "__DATE__". 'help' to list commands.\n",devname);
  curdir=malloc(1024); 
  chdir("/");
  for (;;) {
   /* Prompt section */	  
   /* XXX: show_prompt user adjustable */
   getcwd(curdir,1024);
   printf("%s [%s] # ",devname,curdir);
   /* getcmd section */	  
   if (!shell_scanline(cmd,sizeof(cmd),stdin,stdout)) {
    printf("shell:unable scanline(%s)\n",devname);	
    break;
   };
   /* evaluate cmd section */	  
   if (!strcmp(cmd,"r")) {  /* repeat last command, forced, not automatic */
    strcpy(cmd,last_cmd);
   } else 
   if (strcmp(cmd,"")) { /* only for get a new prompt */
    strcpy(last_cmd,cmd);
   }; 
   /* exec cmd section */	  
   /* TODO: 
    *  To avoid user crash catch the signals. 
    *  Open a new stdio files with posibility of redirection *
    *  Run in a new shell task background. (unix &)
    *  Resuming. A little bash.
    */	  
   if (shell_make_args(cmd,&argc,argv)) {
    if ((shell_cmd=shell_lookup_cmd(argv[0]))!=NULL) {
     current_shell_env->errorlevel=shell_cmd->command(argc,argv);
    } else {
     printf("shell:%s command not found\n",argv[0]);
     current_shell_env->errorlevel=-1;
    };
   };
   /* end exec cmd section */	  
  };
  free(curdir);
  rtems_task_delete(RTEMS_SELF);
}
/* ----------------------------------------------- */
rtems_status_code   shell_init (char * task_name,
				rtems_unsigned32    task_stacksize,
				rtems_task_priority task_priority,
				char * devname,
				tcflag_t tcflag) {
 rtems_id task_id;
 rtems_status_code sc;
 shell_env_t * shell_env;
 sc=rtems_task_create(new_rtems_name(task_name),
 		     task_priority,
		     task_stacksize?task_stacksize:RTEMS_MINIMUM_STACK_SIZE,
		     (RTEMS_DEFAULT_MODES&~RTEMS_ASR_MASK)|RTEMS_ASR,
		     RTEMS_DEFAULT_ATTRIBUTES,
		     &task_id);
 if (sc!=RTEMS_SUCCESSFUL) {
  rtems_error(sc,"creating task %s in shell_init()",task_name);	 
  return sc;
 }; 
 shell_env=malloc(sizeof(shell_env_t));
 if (!shell_env) {
  rtems_task_delete(task_id);
  sc=RTEMS_NO_MEMORY;	 
  rtems_error(sc,"allocating shell_env %s in shell_init()",task_name);	 
  return sc;
 };
 if (global_shell_env.magic!=new_rtems_name("SENV")) {
  global_shell_env.magic   =new_rtems_name("SENV");
  global_shell_env.devname ="/dev/console";
  global_shell_env.taskname="GLOBAL";
  global_shell_env.tcflag   =0;
 }; 
 shell_env->magic   =global_shell_env.magic; 
 shell_env->devname =devname;
 shell_env->taskname=task_name;
 shell_env->tcflag  =tcflag;
 return rtems_task_start(task_id,shell_shell,(rtems_task_argument) shell_env);
}
/* ----------------------------------------------- */

