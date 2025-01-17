#ifndef BASTD_STRING_C
#define BASTD_STRING_C

typedef struct S8 S8;
struct S8 {
	ISize length;
	U8 *raw;
};

#define S8(s) (S8){(U8 *)s, LENGTH_OF(s)}

#endif//BASTD_STRING_C