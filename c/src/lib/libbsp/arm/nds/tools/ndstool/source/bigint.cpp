/*
	big integer class for RSA calculation
	Numbers stored in big endian order.
*/

/*
 * Includes
 */
#include <stdio.h>
#include <string.h>
#include "bigint.h"

/*
 * Sub
 * Subtract (shifted) b from a.
 */
int BigInt::Sub(BigInt &a, BigInt &b, int b_shift)
{
	unsigned int b_byte = sizeof(data) - 1 + (b_shift / 8);
	int b_bit = (b_shift % 8);
	int carry = 0;
	for (int ia=sizeof(data)-1; ia>=0; ia--)
	{
		unsigned char b_data_shifted = 0;
		if (b_byte + 1 < sizeof(b.data)) b_data_shifted |= b.data[b_byte + 1] >> (8 - b_bit);
		if (b_byte + 0 < sizeof(b.data)) b_data_shifted |= b.data[b_byte + 0] << b_bit;
		b_byte--;
		int r = a.data[ia] - b_data_shifted + carry;
		data[ia] = r;
		carry = r>>8;
	}
	return carry;
}

/*
 * MulMod
 * Multiply two numbers and perform modulo.
 */
void BigInt::MulMod(BigInt &a, BigInt &b, BigInt &m)
{
	memset(this, 0, sizeof(*this));
	for (unsigned int ia=sizeof(data) - 1; ia>=sizeof(data)/2; ia--)
	{
		int carry = 0;
		for (unsigned int ib=sizeof(data) - 1; ib>=sizeof(data)/2; ib--)
		{
			int iab = ia + ib - sizeof(data) + 1;
			unsigned int r = a.data[ia] * b.data[ib] + carry + data[iab];
			data[iab] = r;
			carry = r>>8;
		}
	}

	for (int shift=sizeof(data)/2*8; shift>=0; shift--)
	{
		BigInt subbed;
		if (!subbed.Sub(*this, m, shift))	// positive?
		{
			memcpy(this, &subbed, sizeof(*this));
		}
	}
}

/*
 * PowMod
 * Raise to power 65537 and perform modulo.
 */
void BigInt::PowMod(BigInt &n, BigInt &m)
{
	BigInt n65536;
	memcpy(&n65536, &n, sizeof(n));
	for (int i=0; i<16; i++)	// 16 times n^2 = n^65536
	{
		BigInt tmp;
		tmp.MulMod(n65536, n65536, m);
		memcpy(&n65536, &tmp, sizeof(n));
	}
	MulMod(n, n65536, m);	// final result: n^1 * n^65536
}

/*
 * print
 */
void BigInt::print()
{
	printf("0x");
	bool show = false;
	for (unsigned int i=0; i<sizeof(data); i++)
	{
		if (data[i]) show = true;
		if (show) printf("%02X", data[i]);
	}
	printf(show ? "\n" : "0\n");
}

/*
 * Set
 */
void BigInt::Set(unsigned char *data, unsigned int length)
{
	memset(this, 0, sizeof(*this));
	memcpy(this->data + sizeof(this->data) - length, data, length);
}

/*
 * Get
 */
void BigInt::Get(unsigned char *data, unsigned int length)
{
	memcpy(data, this->data + sizeof(this->data) - length, length);
}
