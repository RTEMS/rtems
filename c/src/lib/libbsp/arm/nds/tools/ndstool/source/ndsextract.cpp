#include "ndstool.h"
#include "overlay.h"
#include <errno.h>

/*
 * MkDir
 */
void MkDir(char *name)
{
#ifdef __MINGW32__
	if (mkdir(name))
#else
	if (mkdir(name, S_IRWXU))
#endif
	{
		if (errno != EEXIST)
		{
			fprintf(stderr, "Cannot create directory '%s'.\n", name);
			exit(1);
		}
	}
}

/*
 * ExtractFile
 * if rootdir==0 nothing will be written
 */
void ExtractFile(char *rootdir, const char *prefix, char *entry_name, unsigned int file_id)
{
	unsigned int save_filepos = ftell(fNDS);

	// read FAT data
	fseek(fNDS, header.fat_offset + 8*file_id, SEEK_SET);
	unsigned_int top;
	fread(&top, 1, sizeof(top), fNDS);
	unsigned_int bottom;
	fread(&bottom, 1, sizeof(bottom), fNDS);
	unsigned int size = bottom - top;
	if (size > (1U << (17 + header.devicecap))) { fprintf(stderr, "File %u: Size is too big. FAT offset 0x%X contains invalid data.\n", file_id, header.fat_offset + 8*file_id); exit(1); }

	// print file info
	if (!rootdir || verbose)
	{
		printf("%5u 0x%08X 0x%08X %9u %s%s\n", file_id, (int)top, (int)bottom, size, prefix, entry_name);
	}

	// extract file
	if (rootdir)
	{
		// make filename
		char filename[MAXPATHLEN];
		strcpy(filename, rootdir);
		strcat(filename, prefix);
		strcat(filename, entry_name);

		fseek(fNDS, top, SEEK_SET);
		FILE *fo = fopen(filename, "wb");
		if (!fo) { fprintf(stderr, "Cannot create file '%s'.\n", filename); exit(1); }
		while (size > 0)
		{
			unsigned char copybuf[1024];
			unsigned int size2 = (size >= sizeof(copybuf)) ? sizeof(copybuf) : size;
			fread(copybuf, 1, size2, fNDS);
			fwrite(copybuf, 1, size2, fo);
			size -= size2;
		}
		fclose(fo);
	}

	fseek(fNDS, save_filepos, SEEK_SET);
}

/*
 * MatchName
 */
bool MatchName(char *name, char *mask, int level=0)
{
	char *a = name;
	char *b = mask;
	//for (int i=0; i<level; i++) printf("  "); printf("matching a='%s' b='%s'\n", a, b);
	while (1)
	{
		//for (int i=0; i<level; i++) printf("a=%s b=%s\n", a, b);
		if (*b == '*')
		{
			while (*b == '*') b++;
	//for (int i=0; i<level; i++) printf("  "); printf("* '%s' '%s'\n", a, b);
			bool match = false;
			char *a2 = a;
			do
			{
				if (MatchName(a2, b, level+1)) { a = a2; match = true; break; }
			} while (*a2++);
			if (!match) return false;
			//for (int i=0; i<level; i++) printf("  "); printf("matched a='%s' b='%s'\n", a, b);
		}
		//for (int i=0; i<level; i++) printf("  "); printf("a=%s b=%s\n", a, b);
		if (!*a && !*b) return true;
		if (*a && !*b) return false;
		if (!*a && *b) return false;
		if (*b != '?' && *a != *b) return false;
		a++; b++;
	}
}

/*
 * ExtractDirectory
 * filerootdir can be 0 for just listing files
 */
void ExtractDirectory(const char *prefix, unsigned int dir_id)
{
	char strbuf[MAXPATHLEN];
	unsigned int save_filepos = ftell(fNDS);

	fseek(fNDS, header.fnt_offset + 8*(dir_id & 0xFFF), SEEK_SET);
	unsigned_int entry_start;	// reference location of entry name
	fread(&entry_start, 1, sizeof(entry_start), fNDS);
	unsigned_short top_file_id;	// file ID of top entry
	fread(&top_file_id, 1, sizeof(top_file_id), fNDS);
	unsigned_short parent_id;	// ID of parent directory or directory count (root)
	fread(&parent_id, 1, sizeof(parent_id), fNDS);

	fseek(fNDS, header.fnt_offset + entry_start, SEEK_SET);

	// print directory name
	//printf("%04X ", dir_id);
	if (!filerootdir || verbose)
	{
		printf("%s\n", prefix);
	}

	for (unsigned int file_id=top_file_id; ; file_id++)
	{
		unsigned char entry_type_name_length;
		fread(&entry_type_name_length, 1, sizeof(entry_type_name_length), fNDS);
		unsigned int name_length = entry_type_name_length & 127;
		bool entry_type_directory = (entry_type_name_length & 128) ? true : false;
		if (name_length == 0) break;

		char entry_name[128];
		memset(entry_name, 0, 128);
		fread(entry_name, 1, entry_type_name_length & 127, fNDS);
		if (entry_type_directory)
		{
			unsigned_short dir_id;
			fread(&dir_id, 1, sizeof(dir_id), fNDS);

			if (filerootdir)
			{
				strcpy(strbuf, filerootdir);
				strcat(strbuf, prefix);
				strcat(strbuf, entry_name);
				MkDir(strbuf);
			}

			strcpy(strbuf, prefix);
			strcat(strbuf, entry_name);
			strcat(strbuf, "/");
			ExtractDirectory(strbuf, dir_id);
		}
		else
		{
			if (1)
			{
				bool match = (filemask_num == 0);
				for (int i=0; i<filemask_num; i++)
				{
					//printf("%s %s\n", entry_name, filemasks[i]);
					if (MatchName(entry_name, filemasks[i])) { match = true; break; }
				}

				//if (!directorycreated)
				//{
				//}

				//printf("%d\n", match);

				if (match) ExtractFile(filerootdir, prefix, entry_name, file_id);
			}
		}
	}

	fseek(fNDS, save_filepos, SEEK_SET);
}

/*
 * ExtractFiles
 */
void ExtractFiles(char *ndsfilename)
{
//printf("%d\n", MatchName("hello", "h*el*lo")); exit(0);		// TEST
	fNDS = fopen(ndsfilename, "rb");
	if (!fNDS) { fprintf(stderr, "Cannot open file '%s'.\n", ndsfilename); exit(1); }
	fread(&header, 512, 1, fNDS);

	if (filerootdir)
	{
		MkDir(filerootdir);
	}

	ExtractDirectory("/", 0xF000);		// list or extract

	fclose(fNDS);
}

/*
 * ExtractOverlayFiles2
 */
 void ExtractOverlayFiles2(unsigned int overlay_offset, unsigned int overlay_size)
 {
 	OverlayEntry overlayEntry;

	if (overlay_size)
	{
		fseek(fNDS, overlay_offset, SEEK_SET);
		for (unsigned int i=0; i<overlay_size; i+=sizeof(OverlayEntry))
		{
			fread(&overlayEntry, 1, sizeof(overlayEntry), fNDS);
			char s[32]; sprintf(s, OVERLAY_FMT, overlayEntry.file_id);
			ExtractFile(overlaydir, "/", s, overlayEntry.file_id);
		}
	}
}

/*
 * ExtractOverlayFiles
 */
void ExtractOverlayFiles()
{
	fNDS = fopen(ndsfilename, "rb");
	if (!fNDS) { fprintf(stderr, "Cannot open file '%s'.\n", ndsfilename); exit(1); }
	fread(&header, 512, 1, fNDS);

	if (overlaydir)
	{
		MkDir(overlaydir);
	}

	ExtractOverlayFiles2(header.arm9_overlay_offset, header.arm9_overlay_size);
	ExtractOverlayFiles2(header.arm7_overlay_offset, header.arm7_overlay_size);

	fclose(fNDS);
}

/*
 * Extract
 */
void Extract(char *outfilename, bool indirect_offset, unsigned int offset, bool indirect_size, unsigned size, bool with_footer)
{
	fNDS = fopen(ndsfilename, "rb");
	if (!fNDS) { fprintf(stderr, "Cannot open file '%s'.\n", ndsfilename); exit(1); }
	fread(&header, 512, 1, fNDS);

	if (indirect_offset) offset = *((unsigned int *)&header + offset/4);
	if (indirect_size) size = *((unsigned int *)&header + size/4);

	fseek(fNDS, offset, SEEK_SET);

	FILE *fo = fopen(outfilename, "wb");
	if (!fo) { fprintf(stderr, "Cannot create file '%s'.\n", outfilename); exit(1); }

	unsigned char copybuf[1024];
	while (size > 0)
	{
		unsigned int size2 = (size >= sizeof(copybuf)) ? sizeof(copybuf) : size;
		fread(copybuf, 1, size2, fNDS);
		fwrite(copybuf, 1, size2, fo);
		size -= size2;
	}

	if (with_footer)
	{
		unsigned_int nitrocode;
		fread(&nitrocode, sizeof(nitrocode), 1, fNDS);
		if (nitrocode == 0xDEC00621)
		{
			// 0x2106C0DE, version info, reserved?
			for (int i=0; i<3; i++)		// write additional 3 words
			{
				fwrite(&nitrocode, sizeof(nitrocode), 1, fo);
				fread(&nitrocode, sizeof(nitrocode), 1, fNDS);	// next field
			}
		}
	}

	fclose(fo);
	fclose(fNDS);
}
