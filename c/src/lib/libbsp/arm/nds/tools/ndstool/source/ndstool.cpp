/*
	Nintendo DS rom tool
	by Rafael Vuijk (aka DarkFader)
*/

#include <unistd.h>
#include "ndstool.h"
#include "sha1.h"
#include "ndscreate.h"
#include "ndsextract.h"
#include "passme.h"
#include "hook.h"
#include "encryption.h"

/*
 * Variables
 */
int verbose = 0;
Header header;
FILE *fNDS = 0;
char *romlistfilename = 0;
char *filemasks[MAX_FILEMASKS];
int filemask_num = 0;
char *ndsfilename = 0;
char *arm7filename = 0;
char *arm9filename = 0;
char *filerootdir = 0;
char *overlaydir = 0;
char *arm7ovltablefilename = 0;
char *arm9ovltablefilename = 0;
char *bannerfilename = 0;
char *bannertext = 0;
//bool compatibility = false;
char *headerfilename_or_size = 0;
//char *uniquefilename = 0;
char *logofilename = 0;
char *title = 0;
char *makercode = 0;
char *gamecode = 0;
char *vhdfilename = 0;
char *sramfilename = 0;
int latency1 = 0x1FFF;	//0x8F8;
int latency2 = 0x3F;	//0x18;
int romversion = 0;
char endecrypt_option = 0;

int bannertype;
unsigned int arm9RamAddress = 0;
unsigned int arm7RamAddress = 0;
unsigned int arm9Entry = 0;
unsigned int arm7Entry = 0;


/*
 * Title
 */
void Title()
{
	printf("Nintendo DS rom tool "VERSION" - %s\nby Rafael Vuijk, Dave Murphy, Alexei Karpenko\n",CompileDate);
}

/*
 * Help data
 */
struct HelpLine
{
	const char *option_char;
	const char *text;

	void Print()
	{
		char s[1024];
		strcpy(s, text);
		printf("%-22s ", strtok(s, "\n"));
		char *p = strtok(0, "\n"); if (p) printf("%-30s ", p);
		for (int i=0; ; i++)
		{
			char *p = strtok(0, "\n");
			if (!p) { printf("\n"); break; }
			if (i) printf("\n%54s", "");
			printf("%s", p);
		}
	}
};

HelpLine helplines[] =
{
	{"",	"Parameter\nSyntax\nComments"},
	{"",	"---------\n------\n--------"},
	{"?",	"Show this help:\n-?[option]\nAll or single help for an option."},
	{"i",	"Show information:\n-i [file.nds]\nHeader information."},
	{"v",	"  Show more info\n-v [roms_rc.dat]\nChecksums, warnings, release info"},
	{"p",	"PassMe:\n-p [file.nds] [passme.vhd] [passme.sav]\nGenerates PassMe2 files."},
	{"k",	"Hook ARM7 executable\n-k [file.nds]\nCurrently not tested."},
	{"f",	"Fix header CRC\n-f [file.nds]\nYou only need this after manual editing."},
	//{"T",	"Test\n-T [file.nds]"},
	{"s",	"En/decrypt secure area\n-s[e|E|d] [file.nds]\nEn/decrypt the secure area and\nput/remove card encryption tables and test patterns.\nOptionally add: d for decryption, e/E for encryption.\n(e: Nintendo offsets, E: others)"},
	//{"@",	"Hash file & compare:\n-@ [arm7.bin]"},		// used in buildscript
	{"1",	"List files:\n-l [file.nds]\nGive a list of contained files."},
	{"v",	"  Show offsets/sizes\n-v"},
	{"cx",	"Create/Extract\n-c/-x [file.nds]"},
	{"v",	"  Show more info\n-v[v...]\nShow filenames and more header info.\nUse multiple v's for more information."},
	{"9",	"  ARM9 executable\n-9 file.bin"},
	{"7",	"  ARM7 executable\n-7 file.bin"},
	{"y9",	"  ARM9 overlay table\n-y9 file.bin"},
	{"y7",	"  ARM7 overlay table\n-y7 file.bin"},
	{"d",	"  Data files\n-d directory"},
	{"y",	"  Overlay files\n-y directory"},
	{"b",	"  Banner bitmap/text\n-b file.bmp \"text;text;text\"\nThe three lines are shown at different sizes."},
	{"t",	"  Banner binary\n-t file.bin"},
	{"h",	"  Header template\n-h file.bin\nUse the header from another ROM as a template."},
	{"h",	"  Header size\n-h size\nA header size of 0x4000 is default for real cards, 0x200 for homebrew."},
	{"n",	"  Latency\n-n [L1] [L2]\ndefault=maximum"},
	{"o",	"  Logo bitmap/binary\n-o file.bmp/file.bin"},
	{"g",	"  Game info\n-g gamecode [makercode] [title] [rom ver]\nSets game-specific information.\nGame code is 4 characters. Maker code is 2 characters.\nTitle can be up to 12 characters."},
	{"r",	"  ARM9 RAM address\n-r9 address"},
	{"r",	"  ARM7 RAM address\n-r7 address"},
	{"e",	"  ARM9 RAM entry\n-e9 address"},
	{"e",	"  ARM7 RAM entry\n-e7 address"},
	{"w",	"  Wildcard filemask(s)\n-w [filemask]...\n* and ? are wildcard characters."},
};

/*
 * Help
 */
void Help(char *specificoption = 0)
{
	Title();
	printf("\n");

	if (specificoption)
	{
		bool found = false;
		for (unsigned int i=0; i<(sizeof(helplines) / sizeof(helplines[0])); i++)
		{
			for (const char *o = helplines[i].option_char; *o; o++)
			{
				if (*o == *specificoption) { helplines[i].Print(); found = true; }
			}
		}
		if (!found)
		{
			printf("Unknown option: %s\n\n", specificoption);
		}
	}
	else
	{
		for (unsigned int i=0; i<(sizeof(helplines) / sizeof(helplines[0])); i++)
		{
			helplines[i].Print();
		}
		printf("\n");
		printf("You only need to specify the NDS filename once if you want to perform multiple actions.\n");
		printf("Actions are performed in the specified order.\n");
		printf("Addresses can be prefixed with '0x' to use hexadecimal format.\n");
	}
}


#define REQUIRED(var)	var = ((argc > a+1) ? argv[++a] : 0)								// must precede OPTIONAL
#define OPTIONAL(var)	{ /*fprintf(stderr, "'%s'\n", argv[a]);*/ char *t = ((argc > a+1) && (argv[a+1][0] != '-') ? argv[++a] : 0); if (!var) var = t; else if (t) fprintf(stderr, "%s is already specified!\n", #var); }		// final paramter requirement checks are done when performing actions
#define OPTIONAL_INT(var)	{ char *t = ((argc > a+1) && (argv[a+1][0] != '-') ? argv[++a] : 0); if (t) var = strtoul(t,0,0); }		// like OPTIONAL, but for (positive) integers
#define MAX_ACTIONS		32
#define ADDACTION(a)	{ if (num_actions < MAX_ACTIONS) actions[num_actions++] = a; }

enum {
	ACTION_SHOWINFO,
	ACTION_FIXHEADERCRC,
	ACTION_ENCRYPTSECUREAREA,
	ACTION_PASSME,
	ACTION_LISTFILES,
	ACTION_EXTRACT,
	ACTION_CREATE,
	ACTION_HASHFILE,
	ACTION_HOOK,
};

/*
 * main
 */
int main(int argc, char *argv[])
{
	#ifdef _NDSTOOL_P_H
		if (sizeof(Header) != 0x200) { fprintf(stderr, "Header size %d != %d\n", sizeof(Header), 0x200); return 1; }
	#endif

	if (argc < 2) { Help(); return 0; }

	int num_actions = 0;
	int actions[MAX_ACTIONS];

	/*
	 * parse parameters to actions
	 */

	for (int a=1; a<argc; a++)
	{
		if (argv[a][0] == '-')
		{
			switch (argv[a][1])
			{
				case 'i':	// show information
				{
					ADDACTION(ACTION_SHOWINFO);
					OPTIONAL(ndsfilename);
					break;
				}

				case 'f':	// fix header CRC
				{
					ADDACTION(ACTION_FIXHEADERCRC);
					OPTIONAL(ndsfilename);
					break;
				}

				case 's':	// en-/decrypt secure area
				{
					ADDACTION(ACTION_ENCRYPTSECUREAREA);
					endecrypt_option = argv[a][2];
					OPTIONAL(ndsfilename);
					break;
				}

				case 'p':	// PassMe
				{
					ADDACTION(ACTION_PASSME);
					OPTIONAL(ndsfilename);
					OPTIONAL(vhdfilename);
					OPTIONAL(sramfilename);
					break;
				}

				case 'l':	// list files
				{
					ADDACTION(ACTION_LISTFILES);
					OPTIONAL(ndsfilename);
					break;
				}

				case 'x':	// extract
				{
					ADDACTION(ACTION_EXTRACT);
					OPTIONAL(ndsfilename);
					break;
				}

				case 'w':	// wildcard filemasks
				{
					while (1)
					{
						char *filemask = 0;
						OPTIONAL(filemask);
						if (!(filemasks[filemask_num] = filemask)) break;
						if (++filemask_num >= MAX_FILEMASKS) return 1;
					}
					break;
				}

				case 'c':	// create
				{
					ADDACTION(ACTION_CREATE);
					OPTIONAL(ndsfilename);
					break;
				}

				// file root directory
				case 'd': REQUIRED(filerootdir); break;

				// ARM7 filename
				case '7': REQUIRED(arm7filename); break;

				// ARM9 filename
				case '9': REQUIRED(arm9filename); break;

				// hash file
				case '@':
				{
					ADDACTION(ACTION_HASHFILE);
					OPTIONAL(arm7filename);
					break;
				}

				// hook ARM7 executable
				case 'k':
				{
					ADDACTION(ACTION_HOOK);
					OPTIONAL(ndsfilename);
					break;
				}

				case 't':
					REQUIRED(bannerfilename);
					bannertype = BANNER_BINARY;
					break;

				case 'b':
					bannertype = BANNER_IMAGE;
					REQUIRED(bannerfilename);
					REQUIRED(bannertext);
					break;

				case 'o':
					REQUIRED(logofilename);
					break;

				case 'h':	// load header or header size
					REQUIRED(headerfilename_or_size);
					break;

				/*case 'u':	// unique ID file
					REQUIRED(uniquefilename);
					break;*/

				case 'v':	// verbose
					for (char *p=argv[a]; *p; p++) if (*p == 'v') verbose++;
					OPTIONAL(romlistfilename);
					break;

				case 'n':	// latency
					//compatibility = true;
					OPTIONAL_INT(latency1);
					OPTIONAL_INT(latency2);
					break;

				case 'r':	// RAM address
					switch (argv[a][2])
					{
						case '7': arm7RamAddress = (argc > a) ? strtoul(argv[++a], 0, 0) : 0; break;
						case '9': arm9RamAddress = (argc > a) ? strtoul(argv[++a], 0, 0) : 0; break;
						default: Help(argv[a]); return 1;
					}
					break;

				case 'e':	// entry point
					switch (argv[a][2])
					{
						case '7': arm7Entry = (argc > a) ? strtoul(argv[++a], 0, 0) : 0; break;
						case '9': arm9Entry = (argc > a) ? strtoul(argv[++a], 0, 0) : 0; break;
						default: Help(argv[a]); return 1;
					}
					break;

				case 'm':	// maker code
					REQUIRED(makercode);
					break;

				case 'g':	// game code
					REQUIRED(gamecode);
					OPTIONAL(makercode);
					OPTIONAL(title);
					OPTIONAL_INT(romversion);
					break;

				case 'y':	// overlay table file / directory
					switch (argv[a][2])
					{
						case '7': REQUIRED(arm7ovltablefilename); break;
						case '9': REQUIRED(arm9ovltablefilename); break;
						case 0: REQUIRED(overlaydir); break;
						default: Help(argv[a]); return 1;
					}
					break;

				case '?':	// global or specific help
				{
					Help(argv[a][2] ? argv[a]+2 : 0);	// 0=global help
					return 0;	// do not perform any other actions
				}

				default:
				{
					Help(argv[a]);
					return 1;
				}
			}
		}
		else
		{
			//Help();
			if (ndsfilename)
			{
				fprintf(stderr, "NDS filename is already given!\n");
				return 1;
			}
			ndsfilename = argv[a];
			break;
		}
	}

	Title();

	/*
	 * sanity checks
	 */

	if (gamecode)
	{
		if (strlen(gamecode) != 4)
		{
			fprintf(stderr, "Game code must be 4 characters!\n");
			return 1;
		}
		for (int i=0; i<4; i++) if ((gamecode[i] >= 'a') && (gamecode[i] <= 'z'))
		{
			fprintf(stderr, "Warning: Gamecode contains lowercase characters.\n");
			break;
		}
		if (gamecode[0] == 'A')
		{
			fprintf(stderr, "Warning: Gamecode starts with 'A', which might be used for another commercial product.\n");
		}
	}
	if (makercode && (strlen(makercode) != 2))
	{
		fprintf(stderr, "Maker code must be 2 characters!\n");
		return 1;
	}
	if (title && (strlen(title) > 12))
	{
		fprintf(stderr, "Title can be no more than 12 characters!\n");
		return 1;
	}

	/*
	 * perform actions
	 */

	int status = 0;
	for (int i=0; i<num_actions; i++)
	{
//printf("action %d\n", actions[i]);
		switch (actions[i])
		{
			case ACTION_SHOWINFO:
				ShowInfo(ndsfilename);
				break;

			case ACTION_FIXHEADERCRC:
				FixHeaderCRC(ndsfilename);
				break;

			case ACTION_EXTRACT:
				if (arm9filename) Extract(arm9filename, true, 0x20, true, 0x2C, true);
				if (arm7filename) Extract(arm7filename, true, 0x30, true, 0x3C);
				if (bannerfilename) Extract(bannerfilename, true, 0x68, false, 0x840);
				if (headerfilename_or_size) Extract(headerfilename_or_size, false, 0x0, false, 0x200);
				if (logofilename) Extract(logofilename, false, 0xC0, false, 156);	// *** bin only
				if (arm9ovltablefilename) Extract(arm9ovltablefilename, true, 0x50, true, 0x54);
				if (arm7ovltablefilename) Extract(arm7ovltablefilename, true, 0x58, true, 0x5C);
				if (overlaydir) ExtractOverlayFiles();
				if (filerootdir) ExtractFiles(ndsfilename);
				break;

			case ACTION_CREATE:
				Create();
				break;

			case ACTION_PASSME:
				status = PassMe(ndsfilename, vhdfilename, sramfilename);
				break;

			case ACTION_LISTFILES:
				filerootdir = 0;
				/*status =*/ ExtractFiles(ndsfilename);
				break;

			case ACTION_HASHFILE:
			{
				char *filename = arm7filename;
				if (!filename) filename = ndsfilename;
				if (!filename) return 1;
				unsigned char sha1[SHA1_DIGEST_SIZE];
				int r = HashAndCompareWithList(filename, sha1);
				status = -1;
				if (r > 0)
				{
					for (int i=0; i<SHA1_DIGEST_SIZE; i++) printf("%02X", sha1[i]);
					printf("\n");
					status = 0;
				}
				break;
			}

			case ACTION_HOOK:
			{
				Hook(ndsfilename, arm7filename);
				break;
			}

			case ACTION_ENCRYPTSECUREAREA:
			{
				/*status =*/ EnDecryptSecureArea(ndsfilename, endecrypt_option);
				break;
			}
		}
	}

	return (status < 0) ? 1 : 0;
}
