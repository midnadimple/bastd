#include <windows.h>

#pragma comment(lib, "user32.lib")

static S8
__win32_GetLastErrorS8(mem_Arena *a)
{
	S8 res = {0};
	res = S8_alloc(256, a);
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, (LPSTR)res.data, 1024, NULL);
	return res;
}

os_File 
os_File_open(S8 filename, os_FileOpenMode mode)
{
	os_File res = {0};
	U32 attribs = GetFileAttributes((LPCSTR)filename.data);
	U32 open_style = 0;
	LARGE_INTEGER file_size = {0};
	mem_ArenaTemp scratch;

	scratch = tctx_getScratchArena(NIL, 0);

	res.name = filename;
	res.already_exists = attribs != INVALID_FILE_ATTRIBUTES && !(attribs & FILE_ATTRIBUTE_DIRECTORY);
	switch (mode) {
	case os_FileOpenExisting:
		open_style = OPEN_EXISTING;
		break;
	case os_FileCreate:
		open_style = CREATE_ALWAYS;
		break;
	case os_FileCreateIfNotExists:
		open_style = CREATE_NEW;
		break;
	default: break;
	}
	
	res.raw = (void*)CreateFileA((LPCSTR)filename.data, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, 0, open_style, FILE_ATTRIBUTE_NORMAL, 0);

	if (res.raw == NIL) {
		tctx_logAppend(tctx_MsgError, S8("Failed to open file `%s`. Win32 Error: %s"), filename, __win32_GetLastErrorS8(scratch.arena));
		return (os_File){0};
	}

	GetFileSizeEx(res.raw, &file_size);
	res.size = (ISize)file_size.QuadPart;

	return res;
}

B32
os_File_delete(os_File file)
{
	return DeleteFileA((LPCSTR)file.name.data);	
}

void
os_File_close(os_File file)
{
	CloseHandle((HANDLE)file.raw);
}

B32
os_File_write(os_File file, S8 str, ISize offset)
{
	DWORD dummy;
	B32 res;
	mem_ArenaTemp scratch;

	scratch = tctx_getScratchArena(NIL, 0);

	if (offset < 0) {
		SetFilePointer((HANDLE)file.raw, 0, NULL, FILE_END);
	} else {
		SetFilePointer((HANDLE)file.raw, (long)offset, NULL, FILE_BEGIN);
	}

	res = WriteFile((HANDLE)file.raw, str.data, (I32)str.len, &dummy, 0);

	SetFilePointer((HANDLE)file.raw, 0, NULL, FILE_BEGIN);

	if (!res) {
		tctx_logAppend(tctx_MsgError, S8("Failed to write file `%s`. Win32 Error: %s"), file.name, __win32_GetLastErrorS8(scratch.arena));
	}

	mem_ArenaTemp_end(scratch);

	return res;
}

S8
os_File_read(os_File file, mem_Arena *arena, ISize start_offset, ISize end_offset)
{
	ISize read_size;
	S8 res = {0};
	B32 ok = FALSE;
	mem_ArenaTemp scratch;

	if (arena == NIL) {
		tctx_logAppend(tctx_MsgError, S8("No arena given for reading file %d"), file.name);
		return (S8){0};
	}
	scratch = tctx_getScratchArena(&arena, 1);
	
	/* Read whole size, if no end offset given, else only read until the offset */
	read_size = (end_offset > 0) ? end_offset : file.size;

	/* Move forward to start_offset and adjust read_size, if start offset given */
	if (start_offset > 0) {
		SetFilePointer((HANDLE)file.raw, (long)start_offset, NULL, FILE_BEGIN);
		read_size -= start_offset;
	} else {
		SetFilePointer((HANDLE)file.raw, 0, NULL, FILE_BEGIN);
	}

	/* Read file into buffer, then close it */
	res = S8_alloc(read_size, arena);
	ok = ReadFile((HANDLE)file.raw, res.data, (DWORD)read_size, &(DWORD)res.len, 0);

	/* Reset file pointer to start after reading */
	SetFilePointer((HANDLE)file.raw, 0, NULL, FILE_BEGIN);

	if (!ok) {
		tctx_logAppend(tctx_MsgError, S8("Failed to read file `%s`. Win32 Error: %s"), file.name, __win32_GetLastErrorS8(scratch.arena));
		return (S8){0};
	}

	return res;
}

U64
os_wallclock(void)
{
	LARGE_INTEGER count, freq;
	U64 res = 0;
    
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    
    res = (count.QuadPart * 1000000) / freq.QuadPart;
    return res;
}

ISize
os_getPageSize(void)
{
    SYSTEM_INFO sysinfo = {0};
	GetSystemInfo(&sysinfo);

	return (ISize)sysinfo.dwPageSize;
}

void *
os_memReserve(ISize size)
{
	return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
}

B32
os_memCommit(void *p, ISize size)
{
	void *ret = VirtualAlloc(p, size, MEM_COMMIT, PAGE_READWRITE);
	assert(ret != NULL);
	return ret != NULL;
}

B32
os_memDecommit(void *p, ISize size)
{
	B32 res = VirtualFree(p, size, MEM_DECOMMIT);
	assert(res);
	return res;
}

B32
os_memRelease(void *p, ISize size)
{
	B32 res = VirtualFree(p, size, MEM_RELEASE);
	assert(res);
	return res;
}

void
os_exit(U32 err_code)
{
	ExitProcess(err_code);
}

/* TODO CRT free */
int
main(int argc, char **argv)
{
	mem_Arena *arg_arena = mem_Arena_create(MiB(1), KiB(64));
	S8Slice args = {0};
	os_File stdfile = {0};
	int i;
	S8 *ap;
	int res;

	/* making the args slice */
	for (i = 0; i < argc; i++) {
		ap = mem_Slice_push(&args, arg_arena);
		ap->data = (U8 *)argv[i];
		/* read until NULL terminator to get length */
		while (*(ap->data + ap->len) != 0) {
			ap->len++;
		}
	}

	/* creating stdout and stderr */
    stdfile.already_exists = TRUE;
    stdfile.size = KiB(1);

    stdfile.name = S8("stdout");
    stdfile.raw = (void*)GetStdHandle((U32)(-11));
	tctx.console_out = Buffer_FILE(mem_make(arg_arena, U8, KiB(1)), KiB(1), stdfile);

	stdfile.name = S8("stderr");
	/* STDERR DONT EXIST ON WINDOWS FOR SOME REASON SO JUST OUTPUT TO STDOUT I CANT WITH THIS OS I SWEAR TO GOD */
    stdfile.raw = (void*)GetStdHandle((U32)(-11));
	tctx.console_error = Buffer_FILE(mem_make(arg_arena, U8, KiB(1)), KiB(1), stdfile);

	tctx.log_arena = mem_Arena_create(MiB(1), KiB(64));
	tctx_logFrameBegin(S8("bastd"), TRUE);

	res = entry(args);

	tctx_logFrameEnd(0, NIL);

	return res;
}