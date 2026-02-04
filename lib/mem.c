#include <stdlib.h>

void *
mem_copy(void *dst, void *src, ISize n)
{
	/* we need a specific size so that we can step through a byte at a time */
    U8 *csrc = (U8 *)src;
    U8 *cdst = (U8 *)dst;
    int i;

    /* simply copy from src to dst */
    for (i = 0; i < n; i++)
        cdst[i] = csrc[i];

    return dst;
}

ISize
mem_compare(void *a, void *b, ISize n)
{
    U8 *ca = a;
    U8 *cb = b;
    ISize count = 0;

    /* if they are the same base pointer, they are the same memory */
    if (a == b)
        return 0;

    while (n > 0) {
        if (*ca != *cb) {
            count = (*ca > *cb) ? 1 : -1;
            break;
        }
        n--;
        ca++;
        cb++;
    }
    return count;
}

void *
mem_move(void *dst, void *src, ISize n)
{
    U8 *csrc = (U8 *)src;
    U8 *cdst = (U8 *)dst;

    /* flip direction based on whether cdst comes after csrc in memory */
    if (cdst > csrc && cdst < csrc + n) {
        cdst += n;
        csrc += n;
        while (n--) {
            *cdst-- = *csrc--;
        }
    } else {
        while (n--) {
            *cdst++ = *csrc++;
        }
    }
    return dst;
}

void *
mem_set(void *buf, int val, ISize size)
{
    U8 *p = buf;
    
    while (size > 0) {
        *p = (U8) val;
        p++;
        size--;
    }

    return buf;
}

#define __mem_Arena_BASE_POS (isizeOf(mem_Arena))
#define __mem_AlignUpPow2(n, p) (((ISize)(n) + ((ISize)(p) - 1)) & ~((ISize)(p) - 1))

mem_Arena *
mem_Arena_create(ISize reserve_size, ISize commit_size)
{
    mem_Arena *arena;
    ISize page_size = os_getPageSize();

    reserve_size = __mem_AlignUpPow2(reserve_size, page_size);
    commit_size = __mem_AlignUpPow2(commit_size, page_size);

    arena = os_memReserve(reserve_size);
    os_memCommit(arena, commit_size);

    arena->reserve_size = reserve_size;
    arena->commit_size = commit_size;
    arena->pos = __mem_Arena_BASE_POS;
    arena->commit_pos = commit_size;
        
    return arena;
}

void
mem_Arena_destroy(mem_Arena *a)
{
    os_memRelease(a, a->reserve_size);
}

#define __mem_Arena_ALIGN (isizeOf(void *))

void *
mem_Arena_push(mem_Arena *a, ISize size)
{
    ISize pos_aligned;
    ISize new_pos;
    ISize new_commit_pos;
    ISize new_commit_size;
    U8 *out;

    if (a == NIL) {
        tctx_logAppend(tctx_MsgDebug, S8("Failed to allocate in a NIL arena."));
        return NIL;
    }

    pos_aligned = __mem_AlignUpPow2(a->pos, __mem_Arena_ALIGN);
    new_pos = pos_aligned + size;

    if (new_pos > a->reserve_size) {
        /* out of memory */
        tctx_logAppend(tctx_MsgError, S8("Out of memory in Arena (pointer %p)."), a);
        return NIL;
    }

    if (new_pos > a->commit_pos) {
        /* grow the arena */
        new_commit_pos = new_pos;
        new_commit_pos += a->commit_size - 1;
        new_commit_pos -= new_commit_pos % a->commit_size;
        new_commit_pos = min(new_commit_pos, a->reserve_size);

        out = (U8 *)a + a->commit_pos;
        new_commit_size = new_commit_pos - a->commit_pos;
        os_memCommit(out, new_commit_size);

        a->commit_pos = new_commit_pos;
    }

    a->pos = new_pos;
    out = (U8 *)a + pos_aligned;
    mem_set(out, 0, size);

    return out;
}

void
mem_Arena_pop(mem_Arena *a, ISize size)
{
    if (a == NIL) {
        tctx_logAppend(tctx_MsgDebug, S8("passed a NIL arena."));
        return;
    }
    size = min(size, a->pos - __mem_Arena_BASE_POS);
    a->pos -= size;
}

void
mem_Arena_popTo(mem_Arena *a, ISize pos)
{
    mem_Arena_pop(a, a->pos - pos);
}

void
mem_Arena_clear(mem_Arena *a)
{
    mem_Arena_popTo(a, __mem_Arena_BASE_POS);
}

mem_ArenaTemp
mem_ArenaTemp_begin(mem_Arena *a)
{
    mem_ArenaTemp res = {0};
    res.arena = a;
    res.pos = a->pos;
    return res;
}

void
mem_ArenaTemp_end(mem_ArenaTemp at)
{
    mem_Arena_popTo(at.arena, at.pos);
}

void
mem_Slice_grow(void *slice, ISize size, mem_Arena *a)
{
    struct {
        ISize len;
        ISize cap;
        U8  *data;
    } replica;
    void *data;

    /* We don't know the specific type of the slice struct, so copy its general shape */
    mem_copy(&replica, slice, isizeOf(replica));

    if (replica.data == NIL) {
        /* Empty Slice, so Zero-Initialization */
        replica.cap = 1;
        replica.data = mem_Arena_push(a, 2 * size * replica.cap);
    } else {
        /* We have to copy the whole slice to a new region of memory */
        data = mem_Arena_push(a, 2 * size * replica.cap);
        mem_copy(data, replica.data, size * replica.len);
        replica.data = data;
    }

    replica.cap *= 2;
    mem_copy(slice, &replica, isizeOf(replica));
}
