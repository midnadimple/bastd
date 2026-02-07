U32
math_rand(U64 *seed)
{
    /* PCG32 PIE */
    /* taken from https://github.com/demetri/scribbles/blob/master/randomness/prngs.c */
    U64 old = 0;
    U32 xorshifted = 0, rot = 0;

    old = *seed ^ 0xc90fdaa2adf85459ULL;
    *seed = *seed * 6364136223846793005ULL + 0xc90fdaa2adf85459ULL;
    xorshifted = ((old >> 18u) ^ old) >> 27u;
    rot = old >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}