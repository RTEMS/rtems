/*
 * Includes
 */
#include "ndstool.h"
#include "crc.h"
#include "passme_sram.h"

/*
 *
 */
#define DS_RAM			0x02000000
#define RAM_SIZE		(4*1024*1024)
unsigned char *pc_ram;

/*
 * Find
 */
template <typename T> unsigned long Find(FILE *f, const char *fmt, unsigned long begin, unsigned long end, T data, T mask, int thumb, int armnr)
{
	for (unsigned long i=begin; i<end; i+=sizeof(T))
	{
		T w = *(T *)(pc_ram + i - DS_RAM);
		w &= mask;
		if (w == data)
		{
			if (armnr == 7)
				header.arm7_entry_address = i + thumb;		// ARM7 entry
			else
				header.arm9_entry_address = i + thumb;		// ARM9 entry
			if (f) fprintf(f, fmt, i + thumb);
			printf(fmt, i + thumb);
			return i;
		}
	}
	return 0;
}

/*
 * PassMe
 */
int PassMe(char *ndsfilename, char *vhdfilename, char *sramfilename)
{
	fNDS = fopen(ndsfilename, "rb");
	if (!fNDS) { fprintf(stderr, "Cannot open file '%s'.\n", ndsfilename); exit(1); }

	FILE *fVHD = fopen(vhdfilename, "wt");
	if (vhdfilename && !fVHD) { fprintf(stderr, "Cannot open file '%s'.\n", vhdfilename); exit(1); }

	fread(&header, 512, 1, fNDS);

	if (fVHD)
	{
		fprintf(fVHD, "-- PassMe generated CPLD code\n");
		fprintf(fVHD, "-- Game code: \t"); for (unsigned int i=0; i<sizeof(header.gamecode); i++) fprintf(fVHD, "%c", header.gamecode[i]); fprintf(fVHD, "\n");
		fprintf(fVHD, "-- ROM version: \t"); fprintf(fVHD, "%d\n", header.romversion);
		fprintf(fVHD, "-- Game title: \t"); for (unsigned int i=0; i<sizeof(header.title); i++) if (header.title[i]) fprintf(fVHD, "%c", header.title[i]);
		fprintf(fVHD, "\n\n");
	}

	pc_ram = new unsigned char[RAM_SIZE];

	// load ARM7 binary
	fseek(fNDS, header.arm7_rom_offset, SEEK_SET);
	fread(pc_ram + header.arm7_ram_address - DS_RAM, 1, header.arm7_size, fNDS);

	// load ARM9 binary
	fseek(fNDS, header.arm9_rom_offset, SEEK_SET);
	fread(pc_ram + header.arm9_ram_address - DS_RAM, 1, header.arm9_size, fNDS);


	header.arm7_entry_address = 0x55555555;
	header.arm9_entry_address = 0x55555555;


	unsigned char old_header[512];
	memcpy(old_header, &header, 512);

	bool bError = false;
	
	unsigned int begin9 = 0x02000000, end9 = 0x02280000;
	//unsigned int begin9 = header.arm9_ram_address, end9 = header.arm9_ram_address + header.arm9_size;
	if (!Find<unsigned long>(fVHD, "-- BX LR @ 0x%08X\n", begin9, end9, 0xE120001E, 0xFFF000FF, 0, 9))		// BX LR
//	if (!Find<unsigned short>(fVHD, "-- BX LR @ 0x%08X\n", begin9, end9, 0x4770, 0xFFFF, 1, 9))				// BX LR
	{ printf("BX LR instruction not found!\n"); bError = true; }

	unsigned int begin7 = 0x02000000, end7 = 0x023FE000;
	//unsigned int begin7 = header.arm7_ram_address, end7 = header.arm7_ram_address + header.arm7_size;
//	if (!Find<unsigned long>(fVHD, "-- SWI 0xAF @ 0x%08X\n", begin7, end7, 0xEFAF0000, 0xFFFF0000, 0, 7))		// SWI
//	if (!Find<unsigned long>(fVHD, "-- SWI 0xFF @ 0x%08X\n", begin7, end7, 0xEFFF0000, 0xFFFF0000, 0, 7))		// SWI
//	if (!Find<unsigned long>(fVHD, "-- SWI 0xA4 @ 0x%08X\n", begin7, end7, 0xEFA40000, 0xFFFF0000, 0, 7))		// SWI
//	if (!Find<unsigned long>(fVHD, "-- SWI 0xEA @ 0x%08X\n", begin7, end7, 0xEFEA0000, 0xFFFF0000, 0, 7))		// SWI
	if (!Find<unsigned short>(fVHD, "-- SWI 0xAF @ 0x%08X\n", begin7, end7, 0xDFAF, 0xFFFF, 1, 7))				// SWI
	if (!Find<unsigned short>(fVHD, "-- SWI 0xFF @ 0x%08X\n", begin7, end7, 0xDFFF, 0xFFFF, 1, 7))				// SWI
	if (!Find<unsigned short>(fVHD, "-- SWI 0xA4 @ 0x%08X\n", begin7, end7, 0xDFA4, 0xFFFF, 1, 7))				// SWI
	if (!Find<unsigned short>(fVHD, "-- SWI 0xEA @ 0x%08X\n", begin7, end7, 0xDFEA, 0xFFFF, 1, 7))				// SWI
	{ printf("SWI instruction not found!\n"); bError = true; }

	//Find<unsigned long>("%08X\n", 0x037F8000, 0x0380F000, 0xEFEA0000, 0xFFFF0000, 0);		// SWI
	//...

	//Find<unsigned short>("%08X\n", 0x037F8000, 0x0380F000, 0xDFA4, 0xFFFF, 1);				// SWI
	//...

	if (bError)
	{
		printf("Cannot patch.\n");
		return -1;
	}
	else
	{
		/*
		 * VHD
		 */

		header.reserved2 |= 0x04;	// set autostart bit

		bool forcepatch[512];
		memset(forcepatch, 0, sizeof(forcepatch));

		// ldr pc, 0x027FFE24
		((unsigned char *)&header)[0x4] = 0x18;
		((unsigned char *)&header)[0x5] = 0xF0;
		((unsigned char *)&header)[0x6] = 0x9F;
		((unsigned char *)&header)[0x7] = 0xE5;
	
		header.header_crc = CalcCrc16((unsigned char *)&header, 0x15E);
	
		if (fVHD) fputs(
			#include "passme_vhd1.h"
			, fVHD
		);

		for (int i=0; i<512; i++)
		{
			if (((unsigned char *)&header)[i] != old_header[i])
			{
				printf("Patch:\t0x%03X\t0x%02X\n", i, ((unsigned char *)&header)[i]);
				if (fVHD) fprintf(fVHD, "\t\t\twhen 16#%03X# => patched_data <= X\"%02X\";\n", i, ((unsigned char *)&header)[i]);
			}
		}

		if (fVHD) fputs(
			#include "passme_vhd2.h"
			, fVHD
		);
		
		if (fVHD) fclose(fVHD);
		
		/*
		 * SRAM
		 */

		printf("ARM9 patched entry address: \t0x%08X\n", (unsigned int)header.arm9_entry_address);

		FILE *fSRAM = fopen(sramfilename, "wb");
		if (sramfilename && !fSRAM) { fprintf(stderr, "Cannot open file '%s'.\n", sramfilename); exit(1); }
		if (fSRAM)
		{
			for (int i=0; i<passme_sram_size; i++)
			{
				unsigned char c = passme_sram[i];
				// patch with address of ARM9 loop so it can be redirected by the code in SRAM.
				if ((i &~ 1) == 0x764) c = header.arm9_entry_address >> 0 & 0xFF;
				if ((i &~ 1) == 0x796) c = header.arm9_entry_address >> 8 & 0xFF;
				if ((i &~ 1) == 0x7C8) c = header.arm9_entry_address >> 16 & 0xFF;
				if ((i &~ 1) == 0x7FA) c = header.arm9_entry_address >> 24 & 0xFF;
				fputc(c, fSRAM);
			}
			fclose(fSRAM);
		}
	}

	fclose(fNDS);
	
	return 0;
}
