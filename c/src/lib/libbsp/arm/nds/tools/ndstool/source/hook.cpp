// tests... don't mind these
// make && cp ASNE.nds 1.nds && ndstool -v -i -k -i 1.nds -7 c:\work\ds\dummyhook\dummyhook.bin | grep CRC32
// C:\work\DS\buildscripts\tools\nds\ndstool>make && cp submarine_tech_demo_e3_2005.nds 1.nds && ndstool -v -i -T 1.nds -i -7 c:\work\ds\dummyhook\dummyhook.bin

#include <ndstool.h>
#include "crc.h"

/*
 * Hook
 * Append own ARM7 code without affecting CRC32 of the file and patchability
 * This could be used for trainers
 */
void Hook(char *ndsfilename, char *arm7filename)
{
	fNDS = fopen(ndsfilename, "r+b");
	if (!fNDS) { fprintf(stderr, "Cannot open file '%s'.\n", ndsfilename); exit(1); }
	fseek(fNDS, 0, SEEK_SET);
	fread(&header, 512, 1, fNDS);

	// load additional ARM7 code
	FILE *fARM7 = fopen(arm7filename, "rb");
	if (!fARM7) { fprintf(stderr, "Cannot open file '%s'.\n", arm7filename); exit(1); }
	fseek(fARM7, 0, SEEK_END);
	unsigned int add_arm7_size = (ftell(fARM7) + 3) &~ 3;
	unsigned char *add_arm7 = new unsigned char [add_arm7_size];
	fseek(fARM7, 0, SEEK_SET);
	fread(add_arm7, 1, add_arm7_size, fARM7);
	fclose(fARM7);

	// restore backup of original header if found
	if (header.offset_0x160)
	{
		fseek(fNDS, header.offset_0x78, SEEK_SET);
		Header originalHeader;
		fread(&originalHeader, 512, 1, fNDS);
		if (*(unsigned int *)header.gamecode == *(unsigned int *)originalHeader.gamecode) header = originalHeader;
	}

	// calculate new offsets
	unsigned int new_arm7_offset = (header.application_end_offset + 0x100 + 0x1FF) &~ 0x1FF;
	unsigned int add_arm7_offset = new_arm7_offset + header.arm7_size;
	unsigned int header_backup_offset = add_arm7_offset + add_arm7_size;
	unsigned int new_application_end_offset = header_backup_offset + 0x200;

	// read original ARM7 code
	unsigned char *arm7 = new unsigned char [header.arm7_size];
	fseek(fNDS, header.arm7_rom_offset, SEEK_SET);
	fread(arm7, 1, header.arm7_size, fNDS);

	// write original header, original ARM7 code and append own ARM7 code
	//fseek(fNDS, new_arm7_offset, SEEK_SET);
	//fwrite(arm7, 1, header.arm7_size, fNDS);
	FFixCrc32(fNDS, new_arm7_offset, arm7, header.arm7_size);
	//fseek(fNDS, add_arm7_offset, SEEK_SET);
	//fwrite(add_arm7, 1, add_arm7_size, fNDS);
	FFixCrc32(fNDS, add_arm7_offset, add_arm7, add_arm7_size);
	//fseek(fNDS, header_backup_offset, SEEK_SET);
	//fwrite(&header, 1, 0x200, fNDS);
	FFixCrc32(fNDS, header_backup_offset, (unsigned char *)&header, 0x200);

	// write new header information
	header.offset_0x78 = header_backup_offset;		// ROM offset of header backup
	header.offset_0x7C = header.arm7_ram_address + header.arm7_size + add_arm7_size;		// RAM location of header backup
	header.arm7_entry_address = header.arm7_entry_address + header.arm7_size;
	header.arm9_entry_address = 0x027FFE18;
	*(unsigned_int *)(header.reserved1 + 3) = 0xE59FF004;
	header.arm7_rom_offset = new_arm7_offset;
	header.arm7_size = header.arm7_size + add_arm7_size + 0x200;		// also load our code and the original header into memory
	header.application_end_offset = new_application_end_offset;
	header.header_crc = CalcHeaderCRC(header);
	//fseek(fNDS, 0, SEEK_SET);
	//fwrite(&header, 1, 0x200, fNDS);
	FFixCrc32(fNDS, 0, (unsigned char *)&header, 0x200, header.application_end_offset);

	fclose(fNDS);
}
