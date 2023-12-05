#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

/**
 * Struct with information about a mem_block
 * 
 * size     size of the block
 * free     0 is used and 1 is available
 * ptr      pointer to the memeory of mem_block
 * next     pointer point to the next mem_block
 * prev     pointer point to the prev mem_block
 */
typedef struct mem_block{
    size_t size;
    int free;
    void *ptr;
    struct mem_block *next;
    struct mem_block *prev;
} mem_block;

static mem_block *head = NULL;
static size_t requested_size = 0;
static size_t allocated_size = 0;
/********************************* HELPER FUNCTION HERE *********************************/

/**
 * Get the address of the struct
 */
mem_block *get_addr(void *ptr);

/**
 * Combine the space:
 * combine the prev pointer memory to current memory
 */
void coalesce_prev(mem_block *block);

/**
 * Combine the space:
 * combine the current pointer memory to next memory
 */
void coalesce_next(mem_block *block);

/**
 * Because First-Fit has a deadly disadvantage, it can occupy very large space,
 * it is necessary to split space into many small spaces.
 */
void split_mem(mem_block *block, size_t size);
/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
    size_t totalSize = num * size;
    void *ptr = malloc(totalSize);
    if (!ptr) return NULL;
    memset(ptr, 0, totalSize);
    return ptr;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
void *malloc(size_t size) {
    if (!size) return NULL;
    mem_block *p = head;
    mem_block *chosen = NULL;
    if (allocated_size - requested_size >= size) {
        while (p) {
            if (p->free && p->size >= size) {
                //First-Fit
                chosen = p;
                if (p->size >= 2*size && (p->size-size) >= 1024) {
                    split_mem(p, size);
                    requested_size += sizeof(mem_block);
                }
                break;
            }
            p = p->next;
        }
    }
    if (chosen) {
        chosen->free = 0;
        requested_size += chosen->size;
        return chosen->ptr;
    }
    chosen = sbrk(sizeof(mem_block) + size);
    if (chosen == (void *) - 1) return NULL;
    chosen->ptr = chosen + 1;
    chosen->size = size;
    chosen->free = 0;
    chosen->next = head;
    if (head) {
        chosen->prev = head->prev;
        head->prev = chosen;
    } else {
        chosen->prev = NULL;
    }
    head = chosen;
    allocated_size += size + sizeof(mem_block);
    requested_size += size + sizeof(mem_block);
    return chosen->ptr;
}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) {
    if (!ptr) return;
    mem_block *block_ptr = get_addr(ptr);
    if (block_ptr->free) return;
    block_ptr->free = 1;
    requested_size -= block_ptr->size;
    coalesce_prev(block_ptr);
    coalesce_next(block_ptr);
}

/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
    if (size <= 0) free(ptr);
    if (!ptr) return malloc(size);
    mem_block *block = get_addr(ptr);
    assert(block->ptr == ptr);
    assert(block->free == 0);
    size_t oldSize = block->size;
    if (oldSize >= size) {
        return ptr;
    } else if (block->prev && block->prev->free) {
        if ((oldSize + block->prev->size + sizeof(mem_block)) >= size) {
            requested_size += block->prev->size;
            coalesce_prev(block);
            return block->ptr;
        }
    }
    void *result = malloc(size);
    size_t minSize = size <= oldSize ? size : oldSize;
    memcpy(result, ptr, minSize);
    free(ptr);
    return result;
}

/********************************* HELPER FUNCTION IMPLEMENTATION *********************************/

mem_block *get_addr(void *ptr) {
    return (mem_block *)ptr - 1;
}

void coalesce_prev(mem_block *block) {
    if (block->prev && block->prev->free) {
        block->size += block->prev->size + sizeof(mem_block);
        block->prev = block->prev->prev;
        if (block->prev) {
            block->prev->next = block;
        } else {
            head = block;
        }
        requested_size -= sizeof(mem_block);
    }
}

void coalesce_next(mem_block *block) {
    if (block->next && block->next->free) {
        block->next->size += block->size + sizeof(mem_block);
        block->next->prev = block->prev;
        if (block->prev) {
            block->prev->next = block->next;
        } else {
            head = block->next;
        }
        requested_size -= sizeof(mem_block);
    }
}

void split_mem(mem_block *block, size_t size) {
    mem_block *new_block = block->ptr + size;
    new_block->size = block->size - size - sizeof(mem_block);
    new_block->free = 1;
    new_block->ptr = new_block + 1;
    new_block->next = block;
    if (block->prev) {
        block->prev->next = new_block;
    } else {
        head = new_block;
    }
    new_block->prev = block->prev;
    block->size = size;
    block->prev = new_block;
}