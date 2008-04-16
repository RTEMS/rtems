#pragma pack(1)

struct OverlayEntry
{
	unsigned int id;
	unsigned int ram_address;
	unsigned int ram_size;
	unsigned int bss_size;
	unsigned int sinit_init;
	unsigned int sinit_init_end;
	unsigned int file_id;
	unsigned int reserved;
};

#pragma pack()

#define OVERLAY_FMT		"overlay_%04u.bin"
