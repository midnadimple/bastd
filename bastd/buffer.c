#ifndef BASTD_BUFFER_C
#define BASTD_BUFFER_C

typedef struct Buffer Buffer;
struct Buffer {
	U8 *raw;
	ISize cap;
	ISize len;
	B8 error;
};

#define BUFFER(raw, cap) ((Buffer){raw, cap, 0, FALSE}) 

typedef enum BufferOutputType BufferOutputType;
enum BufferOutputType {
	BufferOutputType_string = 0,
	BufferOutputType_file,
};

typedef struct BufferOutput BufferOutput;
struct BufferOutput {
	BufferOutputType type;
	union {
		S8 str;
		int fd;
	} data;
};

FUNCTION void
Buffer_flush(Buffer *b, BufferOutput out)
{
	switch (out.type) {
	case BufferOutputType_string: {
		b->error |= out.data.str.len < b->len;
		m_memoryCopy(out.data.str.raw, b->raw, out.data.str.len);
	} break;
	case BufferOutputType_file: {
		b->error |= out.data.fd < 0;
		b->error |= !os_write(out.data.fd, b->raw, b->len);
	} break;
	}
	b->len = 0;
}

// Utility
FUNCTION S8
Buffer_buildS8(Buffer *b, m_Allocator *perm)
{
	S8 res = S8_alloc(b->len, perm);

	BufferOutput out = {0};
	out.type = BufferOutputType_string;
	out.data.str = res;

	Buffer_flush(b, out);
	return res;
}

FUNCTION void
Buffer_standardOutput(Buffer *b)
{
	BufferOutput output = {0};
	output.type = BufferOutputType_file;
	output.data.fd = 1;
	Buffer_flush(b, output);
}

FUNCTION void
Buffer_fileOutput(Buffer *b, S8 filename)
{
	BufferOutput output = {0};
	output.type = BufferOutputType_file;
	output.data.fd = os_openFile(filename.raw, TRUE);
	Buffer_flush(b, output);
	os_closeFile(output.data.fd);
}

FUNCTION void
Buffer_append(Buffer *b, U8 *src, ISize len)
{
	ISize available = b->cap - b->len;
	ISize amount = available < len ? available : len;
	for (ISize i = 0; i < amount; i++) {
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
Buffer_appendPtr(Buffer *b, void *p)
{
    Buffer_appendS8(b, S8("0x"));
    UPtr u = (UPtr)p;
    for (ISize i = 2*sizeof(u) - 1; i >= 0; i--) {
        Buffer_appendU8(b, "0123456789abcdef"[(u>>(4*i))&15]);
    }
}

FUNCTION void
Buffer_appendF64(Buffer *b, F64 x, ISize num_decimals)
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
	int fd = os_openFile(filename.raw, FALSE);
	U32 file_size = (U32)os_getFileSize(fd);

	U8 *data = m_MAKE(U8, file_size, perm);
	b->error |= os_read(fd, data, (int)file_size, &file_size);
	Buffer_append(b, data, file_size);

	os_closeFile(fd);
	m_RELEASE(data, file_size, perm);
}

FUNCTION void
Buffer_appendStandardInput(Buffer *b, ISize max_read_size, m_Allocator *perm)
{
	int stdin_fd = 0;
	U32 len = 0;

	U8 *data = m_MAKE(U8, max_read_size, perm);
	b->error |= os_read(stdin_fd, data, (int)max_read_size, &len);
	Buffer_append(b, data, len);

	// NOTE maybe we should switch to sl_U8 so theres no max length for stdin
	
	m_RELEASE(data, max_read_size, perm);
}

#endif//BASTD_BUFFER_C