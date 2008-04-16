#ifndef __ENCRYPTION_H
#define __ENCRYPTION_H

#include "types.h"

extern void EnDecryptSecureArea(char *ndsfilename, char endecrypt_option);
extern void encrypt_arm9(u32 cardheader_gamecode, unsigned char *data);
extern void decrypt_arm9(u32 cardheader_gamecode, unsigned char *data);

#endif	// __ENCRYPTION_H
