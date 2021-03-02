#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int has_initialized = 0;

// our memory area we can allocate from, here 64 kB
#define MEM_SIZE (64*1024)
uint8_t heap[MEM_SIZE];

// start and end of our own heap memory area
void *managed_memory_start; 

// this block is stored at the start of each free and used block
struct mem_control_block {
  int size;
  struct mem_control_block *next;
};

// pointer to start of our free list
struct mem_control_block *free_list_start;      

void mymalloc_init() { 

  // our memory starts at the start of the heap array
  managed_memory_start = &heap;

  // allocate and initialize our memory control block 
  // for the first (and at the moment only) free block
  struct mem_control_block *m = (struct mem_control_block *)managed_memory_start;
  m->size = MEM_SIZE - sizeof(struct mem_control_block);

  // no next free block
  m->next = (struct mem_control_block *)0;

  // initialize the start of the free list
  free_list_start = m;

  // We're initialized and ready to go
  has_initialized = 1;
  printf("Free list start: %p\n", free_list_start);
}

void *mymalloc(long numbytes) {
    if (has_initialized == 0) {
        mymalloc_init();
    }
    /* add your code here! */

    int free_block_size = free_list_start->size;
    struct mem_control_block *block_to_fill = free_list_start;
    struct mem_control_block *prev_block = free_list_start; // Last free block too small to hold numbytes

    // Set block_to_fill to address to be filled
    while (free_block_size < numbytes + sizeof(struct mem_control_block)){
        // Exit loop if there is no space and we are at the last free block
        if (block_to_fill->next == (struct mem_control_block *)0){
            return (void *)0;
        }
        prev_block = block_to_fill;
        block_to_fill = block_to_fill->next;
        free_block_size = block_to_fill->size;
        printf("Inne i while prev %p og next %p \n", prev_block, block_to_fill);
    }
    
    // New address of next free block
    void *p = (void *)prev_block;
    int allocated_bytes = sizeof(struct mem_control_block) + numbytes + (8-(sizeof(struct mem_control_block) + numbytes)%8);
    void *new_free_address = p + allocated_bytes;
    
    if (block_to_fill == free_list_start) {
        free_list_start = new_free_address; // New address 
        printf("Inserting into first free space. Free list start is now: %p\n",  free_list_start);
    }
    else {
        prev_block->next = new_free_address;
        printf("Not inserting into first free space. It was too small.\n");
    }

    // New mem_control_block for the free space left in the filled free block
    struct mem_control_block *new_free_block = (struct mem_control_block *)new_free_address;
    new_free_block->size = block_to_fill->size - allocated_bytes;
    new_free_block->next = block_to_fill->next;
    printf("New free block size: %d, next%p\n", new_free_block->size, new_free_block->next);

    // Update block_to_fill now that it is metadata to allocated memory
    block_to_fill->size = numbytes;
    block_to_fill->next = (struct mem_control_block *)0;

    return block_to_fill;
}

void myfree(void *firstbyte) {

    /* add your code here! */

}

int main(int argc, char **argv) {

    /* add your test cases here! */
    struct mem_control_block *a = mymalloc(3);
    struct mem_control_block *b = mymalloc(5);
    struct mem_control_block *c = mymalloc(9);
    struct mem_control_block *d = mymalloc(65500);
    printf("a:%p, b:%p, c:%p\n", a, b, c);
    printf("Not space:  %p\n", d);
    printf("Iterating through list of free blocks:\n");
    struct mem_control_block *n = free_list_start;
    do {
        printf("Block addr: %p, size: %d, next block addr: %p \n", n, n->size, n->next);
        struct mem_control_block *temp = n->next;
        n->next = temp;
    } while(n->next != (struct mem_control_block *)0);
}