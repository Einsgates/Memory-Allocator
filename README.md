# Memory Allocator
The provided code is a part of a memory management system written in C. It's a custom implementation of dynamic memory allocation, similar to how malloc and free work in C.

The code starts with the definition of a struct called mem_block. This structure represents a block of memory in this custom memory management system. Each mem_block has a size (the size of the memory block), a free flag (indicating whether the block is free or not), a ptr (a pointer to the memory block itself), and next and prev pointers (pointing to the next and previous memory blocks respectively).

The head pointer is a static variable that points to the start of the memory list. The requested_size and allocated_size are static variables that keep track of the total size of memory requested and allocated respectively.

The code also includes the declaration of several helper functions. The get_addr function is expected to return the address of the mem_block structure given a pointer to the memory block. The coalesce_prev and coalesce_next functions are used to combine free memory blocks, either with the previous block or the next block respectively. This is a common technique used in memory management to reduce fragmentation and make better use of available memory.

The comment at the end mentions "First-Fit", which is a strategy for memory allocation where the allocator searches for the first block that is large enough to satisfy the memory request. The comment suggests that there might be a disadvantage to this approach in the context of this code, possibly related to inefficient use of memory.

- Developed a custom dynamic memory allocation system in C, implementing a linked list data structure to manage memory blocks. This involved creating a mem_block structure to represent each memory block, with properties for size, a flag to indicate if it's free, and pointers to the next and previous memory blocks.

- Implemented helper functions to manage memory allocation and deallocation. This included a function to retrieve the address of a memory block, and functions to coalesce memory blocks, combining adjacent free blocks to reduce fragmentation and optimize memory usage.

- Addressed the inefficiencies of the First-Fit memory allocation strategy, which can lead to large spaces being occupied unnecessarily. This involved careful analysis and optimization to ensure efficient use of memory.