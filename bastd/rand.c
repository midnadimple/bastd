#ifndef BASTD_RAND_C
#define BASTD_RAND_C

// standard xoshiro256**, idk what any of ts means but it works :P

FUNCTION inline U64
rand_rotl(U64 x, int k)
{
	return (x << k) | (x >> (64 - k));
}

GLOBAL_VAR U64 rand_seed[4];

FUNCTION U64
rand_generateSeed(U64 x)
{
	// SplitMix64
	U64 z = (x += 0x9e3779b97f4a7c15);
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
	z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
	return z ^ (z >> 31);
}

FUNCTION U64
rand_next(void)
{
	U64 wallclock = os_wallclock();
	for (int i = 0; i < COUNT_OF(rand_seed); i++) {
		if (rand_seed[i] == 0) {
			rand_seed[i] = rand_generateSeed(wallclock);
		}
	}

	U64 res = rand_rotl(rand_seed[1] * 5, 7) * 9;

	U64 t = rand_seed[1] << 17;

	rand_seed[2] ^= rand_seed[0];
	rand_seed[3] ^= rand_seed[1];
	rand_seed[1] ^= rand_seed[2];
	rand_seed[0] ^= rand_seed[3];

	rand_seed[2] ^= t;

	rand_seed[3] = rand_rotl(rand_seed[3], 45);

	return res;
}

FUNCTION void
rand_jump(void) {
	LOCAL_PERSIST const U64 JUMP[] = { 0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c };

	U64 s0 = 0;
	U64 s1 = 0;
	U64 s2 = 0;
	U64 s3 = 0;
	for(int i = 0; i < COUNT_OF(JUMP); i++) {
		for(int b = 0; b < 64; b++) {
			if (JUMP[i] & UINT64_C(1) << b) {
				s0 ^= rand_seed[0];
				s1 ^= rand_seed[1];
				s2 ^= rand_seed[2];
				s3 ^= rand_seed[3];
			}
			rand_next();	
		}
	}
		
	rand_seed[0] = s0;
	rand_seed[1] = s1;
	rand_seed[2] = s2;
	rand_seed[3] = s3;
}

FUNCTION void
rand_longJump(void) {
	LOCAL_PERSIST const U64 LONG_JUMP[] = { 0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635 };

	U64 s0 = 0;
	U64 s1 = 0;
	U64 s2 = 0;
	U64 s3 = 0;
	for(int i = 0; i < COUNT_OF(LONG_JUMP); i++) {
		for(int b = 0; b < 64; b++) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				s0 ^= rand_seed[0];
				s1 ^= rand_seed[1];
				s2 ^= rand_seed[2];
				s3 ^= rand_seed[3];
			}
			rand_next();	
		}
	}
		
	rand_seed[0] = s0;
	rand_seed[1] = s1;
	rand_seed[2] = s2;
	rand_seed[3] = s3;
}

#endif