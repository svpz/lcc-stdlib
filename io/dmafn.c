// dmafn.c — Dynamic Memory Allocator for LCC / LUB
// Implements malloc() and free() using a custom heap built on VirtualAlloc.
// Features: linked-list heap blocks, best-fit search, block splitting, merging,
// and automatic heap extension when needed.

extern void* VirtualAlloc(void* lpAddress, unsigned long dwSize, unsigned long flAllocationType, unsigned long flProtect);
extern void  VirtualFree(void* lpAddress, unsigned long dwSize, unsigned long dwFreeType);

typedef unsigned long long size_t;

struct Block {
    unsigned long long size;     // 8 bytes - explicit!
    struct Block* next;          // 8 bytes at offset 8
    int is_free;                 // 4 bytes at offset 16
};

struct Block *heap_head = 0;
size_t PAGE_SIZE = 1048576; // 1 MB default

// Internal helper: allocate a new heap region using VirtualAlloc
struct Block* extend_heap(size_t size) {
    void* mem = VirtualAlloc(0, size, 0x00001000 | 0x00002000, 0x04); // MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE
    if (!mem) return 0;

    struct Block* new_block = (struct Block*) mem;
    new_block->size = size - sizeof(struct Block);
    new_block->is_free = 1;
    new_block->next = 0;

    // Append to linked list
    if (!heap_head) {
        heap_head = new_block;
    } else {
        struct Block* curr = heap_head;
        while (curr->next) curr = curr->next;
        curr->next = new_block;
    }

    return new_block;
}

// Initialize the heap (optional manual call)
void init_heap(size_t size) {
    heap_head = extend_heap(size);
}

void* malloc(size_t size) {
    if (size == 0) return 0;

    // Auto-init heap if not yet initialized
    if (!heap_head) {
        if (!extend_heap(PAGE_SIZE))
            return 0; // allocation failed
    }

    struct Block* best = 0;
    struct Block* curr = heap_head;

    // 1. Find best fit block
    while (curr) {
        if (curr->is_free && curr->size >= size) {
            if (!best || curr->size < best->size)
                best = curr;
        }
        curr = curr->next;
    }

    // 2. No free block found — extend heap
    if (!best) {
        best = extend_heap(PAGE_SIZE);  // Use the returned block directly!
        if (!best) return 0;
        // Don't call malloc recursively - use 'best' we just got
    }

    // 3. Split block if large enough
    if (best->size > size + sizeof(struct Block)) {
        struct Block* new_block = (struct Block*)((char*)best + sizeof(struct Block) + size);
        new_block->size = best->size - size - sizeof(struct Block);
        new_block->is_free = 1;
        new_block->next = best->next;
        best->next = new_block;
        best->size = size;
    }

    // 4. Mark used
    best->is_free = 0;
    return (char*)best + sizeof(struct Block);
}

// free() — mark a block free and merge consecutive free ones
void free(void* ptr) {
    if (!ptr) return;
    struct Block* block = (struct Block*)((char*)ptr - sizeof(struct Block));
    block->is_free = 1;

    // Merge consecutive free blocks
    struct Block* curr = heap_head;
    while (curr && curr->next) {
        if (curr->is_free && curr->next->is_free) {
            curr->size = curr->size + sizeof(struct Block) + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}
