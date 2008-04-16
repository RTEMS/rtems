struct BigInt
{
	unsigned char data[129*2];	// 128*2 doesn't seem to be enough for 1024 bit keys

	int Sub(BigInt &a, BigInt &b, int b_shift);
	void MulMod(BigInt &a, BigInt &b, BigInt &m);
	void PowMod(BigInt &n, BigInt &m);
	void print();
	void Set(unsigned char *data, unsigned int length);
	void Get(unsigned char *data, unsigned int length);
};
