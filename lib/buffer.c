#include <stdarg.h>

void
Buffer_append(Buffer *b, U8 *src, ISize len)
{
	U8 *end = src + len;
	ISize left;
	ISize available;
	ISize amount;
	ISize i;

	while (!b->error && src < end) {
		left = end - src;
		available = b->cap - b->len;
		amount = available < left ? available : left;

		for (i = 0; i < amount; i++) {
			b->buf[b->len + i] = src[i];
		}
		b->len += amount;
		src += amount;

		if (amount < left) {
			Buffer_flush(b);
		}
	}
}

void
Buffer_appendS8(Buffer *b, S8 s)
{
	Buffer_append(b, s.data, s.len);
}

void
Buffer_appendU8(Buffer *b, U8 c)
{
	Buffer_append(b, &c, 1);
}

void
Buffer_appendI64(Buffer *b, I64 x)
{
	U8 tmp[64];
	U8 *end = tmp + isizeOf(tmp);
	U8 *beg = end;
	I64 t = x > 0 ? -x : x;
	do {
		*--beg = '0' - t % 10;
	} while (t /= 10);
	if (x < 0) {
		*--beg = '-';
	}
	Buffer_append(b, beg, end - beg);
}

void
Buffer_appendPtr(Buffer *b, void *p)
{
	UPtr u = (UPtr)p;
	ISize i;

	Buffer_appendS8(b, S8("0x"));
	for (i = 2 * isizeOf(u) - 1; i >= 0; i--) {
		Buffer_appendU8(b, "0123456789abcdef"[(u >> (4 * i)) & 15]);
	}
}

void
Buffer_appendF64(Buffer *b, F64 x)
{
	I64 integral;
	I64 fractional;
	ISize i;
	/* 6 d.p. */
	ISize prec = 1000000;

	if (x < 0) {
		Buffer_appendU8(b, '-');
		x = -x;
	}

	/* round last decimal */
	x += 0.5 / prec;

	/* is the float out of long range (i.e. too large to display) */
	if (x >= (F64)(-1UL>>1)) {
		Buffer_appendS8(b, S8("inf"));
	} else {
		integral = (I64)x;
		fractional = ((I64)x - integral) * prec;
		Buffer_appendI64(b, integral);
		Buffer_appendU8(b, '.');
		for (i = prec / 10; i > 1; i /= 10) {
			if (i > fractional) {
				Buffer_appendU8(b, '0');
			}
		}
		Buffer_appendI64(b, fractional);
	}
}

void
Buffer_flush(Buffer *b)
{
	S8 str;
	mem_ArenaTemp scratch = tctx_getScratchArena(0, NIL);
	
	b->error |= b->file.raw == NIL;
	if (!b->error && b->len) {
		str = Buffer_buildS8(b, scratch.arena);
		b->error |= !os_File_write(b->file, str, 0);
		b->len = 0;
	}

	mem_ArenaTemp_end(scratch);
}

S8
Buffer_buildS8(Buffer *b, mem_Arena *a)
{
	S8 s = S8_alloc(b->len, a);
	mem_copy(s.data, b->buf, b->len);
	return s;
}

static void
__printVaList(Buffer *b, S8 fmt, va_list args)
{
	U8 *c = fmt.data;
	while (*c != 0 && c < fmt.data + fmt.len) {
		/* Format specifier or normal text? */
		if (*c == '%') {
			c++;

			switch (*c) {
			/* just a percent symbol */
			case '%':
				Buffer_appendU8(b, '%');
				break;
			/* single character */
			case 'c':
				Buffer_appendU8(b, va_arg(args, U8));
				break;
			/* string */
			case 's':
				Buffer_appendS8(b, va_arg(args, S8));
				break;
			/* signed integer */
			case 'd':
				Buffer_appendI64(b, va_arg(args, I64));
				break;
			/* floating point */
			case 'f':
				Buffer_appendF64(b, va_arg(args, F64));
				break;
			/* pointer */
			case 'p':
				Buffer_appendPtr(b, va_arg(args, void *));
				break;
			}
		} else {
			Buffer_appendU8(b, *c);
		}

		c++;
	}

	Buffer_flush(b);
}

void
print(S8 fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	__printVaList(&tctx.console_out, fmt, args);

	va_end(args);
}

void
printError(S8 fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	__printVaList(&tctx.console_error, fmt, args);

	va_end(args);
}

void
printBuffer(Buffer *b, S8 fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	__printVaList(b, fmt, args);

	va_end(args);
}


void
printAndAbort(S8 fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	__printVaList(&tctx.console_error, fmt, args);
	os_exit(1);

	va_end(args);
}