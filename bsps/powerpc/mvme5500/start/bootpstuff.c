#define FLAG_MAND	1
#define FLAG_NOUSE	2	/* dont put into the commandline at all */
#define FLAG_CLRBP  4	/* field needs to be cleared for bootp  */

typedef struct ParmRec_ {
	char	*name;
	char	**pval;
	int		flags;
} ParmRec, *Parm;


static char *boot_filename=0;
static char *boot_srvname=0;
static char *boot_use_bootp=0;
static char	*boot_my_ip=0;
static char	*boot_my_netmask=0;

#define boot_cmdline BSP_commandline_string

static ParmRec parmList[]={
	{ "BP_FILE=",  &boot_filename,
			FLAG_MAND,
	},
	{ "BP_PARM=",  &boot_cmdline,
			0,
	},
	{ "BP_SRVR=",  &boot_srvname,
			FLAG_MAND,
	},
	{ "BP_GTWY=",  &net_config.gateway,
			FLAG_CLRBP,
	},
	{ "BP_MYIP=",  &boot_my_ip,
			FLAG_MAND | FLAG_CLRBP,
	},
	{ "BP_MYMK=",  &boot_my_netmask,
			FLAG_MAND | FLAG_CLRBP,
	},
	{ "BP_MYNM=",  &net_config.hostname,
			FLAG_CLRBP,
	},
	{ "BP_MYDN=",  &net_config.domainname,
			FLAG_CLRBP,
	},
	{ "BP_LOGH=",  &net_config.log_host,
			FLAG_CLRBP,
	},
	{ "BP_DNS1=",  &net_config.name_server[0],
			FLAG_CLRBP,
	},
	{ "BP_DNS2=",  &net_config.name_server[1],
			FLAG_CLRBP,
	},
	{ "BP_DNS3=",  &net_config.name_server[2],
			FLAG_CLRBP,
	},
	{ "BP_NTP1=",  &net_config.ntp_server[0],
			FLAG_CLRBP,
	},
	{ "BP_NTP2=",  &net_config.ntp_server[1],
			FLAG_CLRBP,
	},
	{ "BP_NTP3=",  &net_config.ntp_server[2],
			FLAG_CLRBP,
	},
	{ "BP_ENBL=",  &boot_use_bootp,
			0,
	},
	{ 0, }
};
