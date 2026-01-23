#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

B32
os_writeFileDesc(int fd, void *buf, ISize len, ISize offset)
{
	ISize written;

	if (offset > 0) {
		len -= offset;
	}

	while (len > 0) {
		written = write(fd, buf, len);
		if (written < 0) {
			printToStderr(S8("Failed to write to file descriptor %d (errno %d)"), fd, errno);
			return FALSE;
		}
	}

	return TRUE;
}

S8
os_readFile(S8 file_name, mem_Arena *arena, ISize start_offset, ISize end_offset)
{
	ISize read_size;
	FILE *file_ptr;
	S8 file = {0};

	assert(!S8_isNil(file_name), "file_name cannot be empty");
	assert(arena != NIL, "Arena cannot be NIL");
	
	/* Open file */
	file_ptr = fopen(file_name, "r");
	if (file_ptr == NULL) {
		printToStderr(S8("[%s] Failed to open source file (errno %d)"), file_name, errno);
		return file;
	}

	/* Get file size, if no end offset given, else only read until the offset */
	if (end_offset < 0) {
		fseek(file_ptr, 0, SEEK_END);
		read_size = ftell(file_ptr);
	} else {
		read_size = end_offset;
	}

	/* Move forward to start_offset and adjust read_size, if start offset given */
	if (start_offset > 0) {
		fseek(file_ptr, start_offset, SEEK_SET);
		read_size -= start_offset;
	} else {
		fseek(file_ptr, 0, SEEK_SET);
	}

	/* Read file into buffer, then close it */
	file = S8_alloc(read_size, arena);
	fread(file.data, sizeof(U8), read_size, file_ptr);
	fclose(file_ptr);

	return file;
}

int
main(int argc, char **argv)
{
	mem_Arena arg_arena = mem_Arena_create(1 << 16);
	S8Slice args = {0};
	int i;
	S8 *ap;
	int res;

	/* making the args slice */
	for (i = 0; i < argc; i++) {
		ap = mem_Slice_push(args, arg_arena);
		ap->data = argv[i];
		/* read until NULL terminator to get length */
		while (*(ap->data + ap->len) != 0) {
			ap->len++;
		}
	}

	/* TODO make stdout and stderr buffers */

	res = entry(args);

	return res;
}