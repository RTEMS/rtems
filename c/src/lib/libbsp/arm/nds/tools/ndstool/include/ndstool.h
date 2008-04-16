#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
//#include <libelf.h>
#include "little.h"
#include "header.h"

#define ROMTYPE_HOMEBREW	0
#define ROMTYPE_MULTIBOOT	1
#define ROMTYPE_NDSDUMPED	2	// decrypted secure area
#define ROMTYPE_ENCRSECURE	3
#define ROMTYPE_MASKROM		4	// unknown layout

#define MAX_FILEMASKS		16

enum { BANNER_BINARY, BANNER_IMAGE };

extern unsigned int free_file_id;
extern unsigned int file_end;

extern unsigned int free_dir_id;
extern unsigned int directory_count;
extern unsigned int file_count;
extern unsigned int total_name_size;

extern unsigned int free_file_id;
extern unsigned int _entry_start;
extern unsigned int file_top;

extern int verbose;
extern Header header;
extern FILE *fNDS;
extern char *romlistfilename;
extern char *filemasks[MAX_FILEMASKS];
extern int filemask_num;
extern char *ndsfilename;
extern char *arm7filename;
extern char *arm9filename;
extern char *filerootdir;
extern char *overlaydir;
extern char *arm7ovltablefilename;
extern char *arm9ovltablefilename;
extern char *bannerfilename;
extern char *bannertext;
extern int bannertype;
//extern bool compatibility;
extern char *headerfilename_or_size;
extern char *uniquefilename;
extern char *logofilename;
extern unsigned int arm9RamAddress;
extern unsigned int arm7RamAddress;
extern unsigned int arm9Entry;
extern unsigned int arm7Entry;
extern char *title;
extern char *makercode;
extern char *gamecode;
extern int latency1;
extern int latency2;
extern int romversion;

extern const char CompileDate[];
