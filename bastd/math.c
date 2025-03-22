#ifndef BASTD_MATH_C
#define BASTD_MATH_C

// VECTORS
typedef union math_V2 math_V2;
union math_V2 {
	struct {
		F32 x, y;
	};

	struct {
		F32 u, v;
	};

	struct {
		F32 left, right;
	};

	struct {
		F32 width, height;
	};

	F32 elems[2];
};

typedef union math_V3 math_V3;
union math_V3 {
	struct {
		F32 x, y, z;
	};

	struct {
		F32 u, v, w;
	};

	struct {
		F32 r, g, b;
	};

	struct {
		math_V2 xy;
		F32 _ignored_0;
	};

	struct {
		F32 __ignored_1;
		math_V2 yz;
	};

	struct {
		math_V2 uv;
		F32 __ignored_2;
	};

	struct {
		F32 __ignored_3;
		math_V2 vw;
	};

	F32 elems[3];
};

typedef union math_V4 math_V4;
union math_V4 {
	struct {
		union {
			math_V3 xyz;
			struct {
				F32 x, y, z;
			}
		};

		F32 w;
	};

	struct {
		union {
			math_V3 rgb;
			struct {
				F32 r, g, b;
			};
		};

		F32 a;
	};

	struct {
		math_V2 xy;
		F32 __ignored_0;
		F32 __ignored_1;
	};

	struct {
		F32 __ignored_2;
		math_V2 yz;
		F32 __ignored_3;
	};

	struct {
		F32 __ignored_4;
		F32 __ignored_5;
		math_V2 zw;
	};

	F32 elems[4];

	#ifndef BASTD_MATH_NO_SSE
	__m128 sse;
	#endif
};

// MATRICES
typedef union math_M2 math_M2;
union math_M2 {
	F32 elems[2][2];
	math_V2 columns[2];
};

typedef union math_M4 math_M3;
union math_M3 {
	F32 elems[3][3];
	math_V3 columns[3];
};

typedef union math_M4 math_M4;
union math_M4 {
	F32 elems[4][4];
	math_V4 columns[4];
};

// QUATERNIONS
typedef union math_Quaternion;
union math_Quaternion {
	struct {
		union {
			math_V3 xyz;
			struct {
				F32 x, y, z;
			};
		};

		F32 w;
	}

	F32 elems[4];
	
#ifndef BASTD_MATH_NO_SSE
	__m128 sse;
#endif
};

// Rotation returned as turns primarily
typedef F32 math_Turn;

FUNCTION inline
F32 math_Turn_sine(math_Turn turns)
{
	// TODO
}

#endif//BASTD_MATH_C