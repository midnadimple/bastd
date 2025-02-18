#ifndef BASTD_BUFFER_C
#define BASTD_BUFFER_C

typedef struct Buffer Buffer;
struct Buffer {
	U8 *raw;
	U64 cap;
	U64 len;
	B8 error;
};

#define BUFFER(raw, cap) ((Buffer){raw, cap, 0, FALSE}) 

// Utility
FUNCTION S8
Buffer_buildS8(Buffer *b, m_Allocator *perm)
{
	S8 res = S8_alloc(b->len, perm);

	b->error |= res.len < b->len;
	m_memoryCopy(res.raw, b->raw, res.len);

	b->len = 0;
	return res;
}

FUNCTION void
Buffer_standardOutput(Buffer *b)
{
	os_File stdout = os_getStdout();
	b->error |= !os_writeFile(stdout, b->raw, b->len);
	b->len = 0;
}

FUNCTION void
Buffer_fileOutput(Buffer *b, S8 filename)
{
	os_File file = os_openFile(filename.raw);
	b->error |= !os_writeFile(file, b->raw, b->len);
	os_closeFile(file);
	b->len = 0;
}

FUNCTION void
Buffer_append(Buffer *b, U8 *src, U64 len)
{
	U64 available = b->cap - b->len;
	U64 amount = available < len ? available : len;
	for (U64 i = 0; i < amount; i++) {
		b->raw[b->len + i] = src[i];
	}
	b->len += amount;
	b->error |= amount < len;
}

FUNCTION void
Buffer_appendS8(Buffer *b, S8 s)
{
	Buffer_append(b, s.raw, s.len);
}

FUNCTION void
Buffer_appendU8(Buffer *b, U8 c)
{
	Buffer_append(b, &c, 1);
}

FUNCTION void
Buffer_appendI64(Buffer *b, I64 x)
{
	U8 tmp[64];
    U8 *end = tmp + sizeof(tmp);
    U8 *beg = end;
    I64 t = x > 0 ? -x : x;
    do {
        *--beg = '0' - t%10;
    } while (t /= 10);
    if (x < 0) {
        *--beg = '-';
    }
    Buffer_append(b, beg, end-beg);
}

FUNCTION void
Buffer_appendU64Hex(Buffer *b, U64 x)
{
	U8 tmp[64];
    U8 *end = tmp + sizeof(tmp);
    U8 *beg = end;
    do {
		char remainder = x % 16;
		char c = 0;
		if (remainder > 9) {
			c = remainder +55;
		} else {
			c = remainder + '0';
		}
        *--beg = c;
    } while (x /= 16);
	Buffer_appendS8(b, S8("0x"));
    Buffer_append(b, beg, end-beg);
}

FUNCTION void 
Buffer_appendPtr(Buffer *b, void *p)
{
    Buffer_appendS8(b, S8("0x"));
    UPtr u = (UPtr)p;
    for (U64 i = 2*sizeof(u) - 1; i >= 0; i--) {
        Buffer_appendU8(b, "0123456789abcdef"[(u>>(4*i))&15]);
    }
}

FUNCTION void
Buffer_appendF64(Buffer *b, F64 x, U64 num_decimals)
{
	F64 prec = 10 ^ num_decimals;

    if (x < 0) {
        Buffer_appendU8(b, '-');
        x = -x;
    }

    x += 0.5 / prec;  // round last decimal
    if (x >= (F64)(-1UL>>1)) {  // out of I64 range?
        Buffer_appendS8(b, S8("inf"));
    } else {
        I64 integral = (I64)x;
        I64 fractional = (x - integral)*prec;
        Buffer_appendI64(b, integral);
        Buffer_appendU8(b, '.');
        for (I64 i = prec/10; i > 1; i /= 10) {
            if (i > fractional) {
                Buffer_appendU8(b, '0');
            }
        }
        Buffer_appendI64(b, fractional);
    }
}

FUNCTION void
Buffer_appendFile(Buffer *b, S8 filename, m_Allocator *perm)
{
	os_File file = os_openFile(filename.raw);

	U8 *data = m_MAKE(U8, file.size, perm);
	b->error |= os_readFile(file, data, (int)file.size, &file.size);
	Buffer_append(b, data, file.size);

	os_closeFile(file);
	if (!file.already_exists) {
		os_deleteFile(file);
	}
	m_RELEASE(data, file.size, perm);
}

FUNCTION void
Buffer_appendStandardInput(Buffer *b, U64 max_read_size, m_Allocator *perm)
{
	int len = 0;
	U8 *data = m_MAKE(U8, max_read_size, perm);
	os_File stdin = os_getStdin();
	b->error |= os_readFile(stdin, data, (int)max_read_size, &len);
	Buffer_append(b, data, len);

	// NOTE maybe we should switch to sl_U8 so theres no max length for stdin
	
	m_RELEASE(data, max_read_size, perm);
}

#endif//BASTD_BUFFER_C