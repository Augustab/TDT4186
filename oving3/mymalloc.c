#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

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
  printf("FOR DEBUG: Free list start: %p\n", free_list_start);
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
    }
    // Update numbytes so all allocated and free addresses will be multiples of 8
    numbytes = numbytes + (8-(sizeof(struct mem_control_block) + numbytes)%8)%8;
    // New address of next free block
    void *p = (void *)prev_block;
    int allocated_bytes = sizeof(struct mem_control_block) + numbytes;
    void *new_free_address = p + allocated_bytes;
    
    if (block_to_fill == free_list_start) {
        free_list_start = new_free_address; // New address 
        // New mem_control_block for the free space left in the filled free block
        struct mem_control_block *new_free_block = (struct mem_control_block *)new_free_address;
        new_free_block->size = block_to_fill->size - allocated_bytes;
        new_free_block->next = block_to_fill->next;
        // Update block_to_fill now that it is metadata to allocated memory
        block_to_fill->size = numbytes;
        block_to_fill->next = (struct mem_control_block *)0;
        return block_to_fill;
    }
    else {
        // prev_block->next = new_free_address;
        prev_block->next = (void *)prev_block->next + allocated_bytes;
        // New mem_control_block for the free space left in the filled free block
        struct mem_control_block *new_free_block = (struct mem_control_block *)prev_block->next;
        new_free_block->size = block_to_fill->size - allocated_bytes;
        new_free_block->next = block_to_fill->next;
        // Update block_to_fill now that it is metadata to allocated memory
        block_to_fill->size = numbytes;
        block_to_fill->next = (struct mem_control_block *)0;
        return block_to_fill;
    }
}

void myfree(void *firstbyte) {

    /* add your code here! */
    struct mem_control_block *block_to_free = (struct mem_control_block *)firstbyte;
    
    struct mem_control_block *free_before = free_list_start;
    struct mem_control_block *free_after = free_list_start;
    while (free_after < block_to_free) {
        free_before = free_after;
        free_after = free_after->next;
    }

    if (free_after == free_list_start){ // Freeing mem before first free block
        free_list_start = block_to_free;
        free_list_start->next = free_after;
    }
    else{ // Freeing mem after first free block
        free_before->next = block_to_free;
    }
    block_to_free->next = free_after;
    
    // Combine blocks
    // freed block and after
    if ((void *)block_to_free + block_to_free->size + sizeof(struct mem_control_block) == (void *)free_after) {
        block_to_free->next = free_after->next;
        block_to_free->size = block_to_free->size + sizeof(struct mem_control_block) + free_after->size;
    }
    // Before and freed block
    if ((void *)free_before + free_before->size + sizeof(struct mem_control_block) == (void *)block_to_free) {
        free_before->next = block_to_free->next;
        free_before->size = free_before->size + sizeof(struct mem_control_block) + block_to_free->size;
    }
    
}

int main(int argc, char **argv) {

    /* add your test cases here! */
    
    struct mem_control_block *a = mymalloc(5);
    struct mem_control_block *b = mymalloc(15);
    struct mem_control_block *c = mymalloc(30);
    struct mem_control_block *d = mymalloc(40);
    struct mem_control_block *e = mymalloc(69);


    printf("\nFIRST TEST: MANUALLY CHECK THAT ALL ALLOCATED BLOCKS ARE AT CORRECT ADDRESS\n");
    printf("\tBlock addr a: %p Size: %d\n", a, a->size);
    printf("\tBlock addr b: %p Size: %d\n", b, b->size);
    printf("\tBlock addr c: %p Size: %d\n", c, c->size);
    printf("\tBlock addr d: %p Size: %d\n", d, d->size);
    printf("\tBlock addr e: %p Size: %d\n", e, e->size);
    
    //Some tests to check that addresses are correct
    assert((void *)a + a->size + sizeof(struct mem_control_block)==b);
    assert((void *)b + b->size + sizeof(struct mem_control_block)==c);
    assert((void *)c + c->size + sizeof(struct mem_control_block)==d);
    assert((void *)d + d->size + sizeof(struct mem_control_block)==e);
    printf("WEE SEE THAT ADDRESS DIFFERENCE IS CORRECT WITH RESPECT TO BLOCK SIZE. EACH META DATA BLOCK ADDS 16 BYTES\n");
    printf("ASSERT TESTS ALSO SUCCESFUL\n\n");


    printf("SECOND TEST: MANUALLY CHECK THAT FREE BLOCKS LIST START AT FIRST FREE BLOCK (ONE FREE BLOCK)\n");
    printf("\tFirst free Block addr: %p Size: %d\n",free_list_start, free_list_start->size);
    assert((void *)e + e->size + sizeof(struct mem_control_block)==free_list_start);
    printf("WEE SEE THAT ADDRESS OF FREE LIST START IS CORRECT\n");
    printf("ASSERT TESTS ALSO SUCCESFUL\n\n");


    //Freeing two blocks 
    myfree(b);
    myfree(d);

    printf("THIRD TEST: TWO BLOCKS FREED - CHECK THAT FREED BLOCKS ARE ADDED TO FREE LIST AT CORRECT ADDRESSES AND CORRECT SIZE\n");
    assert((void *)a + a->size + sizeof(struct mem_control_block)==free_list_start);
    assert(b->size ==free_list_start->size);
    assert((void *)c + c->size + sizeof(struct mem_control_block)==free_list_start->next);
    assert(d->size == free_list_start->next->size);
    printf("ASSERT TESTS SUCCESS - ALL FREED BLOCKS ARE ADDED TO FREE LIST AT CORRECT ADRESSES WITH CORRECT SIZE\n\n");


    //Freeing mid to check that merging on both sides of block works
    myfree(c);

    printf("FOURTH TEST: FREEING MID-BLOCK - CHECK WETHER FREE BLOCKS MERGED TOGETHER\n");
    assert(40+32+16+2*sizeof(struct mem_control_block) == free_list_start->size);
    assert((void *)e + e->size + sizeof(struct mem_control_block)==free_list_start->next);
    printf("ASSERT TEST SUCCESS - ALL FREE BLOCKS NEXT TO EACH OTHER ARE MERGED TOGETHER\n\n");


    printf("FIFTH TEST: ADDING BLOCK THAT FITS IN FREE BLOCK GAP AND BLOCK THAT DOES NOT FIT / ALSO CHECKS THAT NEW FREE BLOCK IS ADDED AT CORRECT ADDRESS WITH CORRECT SIZE\n");
    struct mem_control_block *f = mymalloc(96);
    assert((void *)a + a->size + sizeof(struct mem_control_block)==f);

    struct mem_control_block *g = mymalloc(96);
    assert((void *)e + e->size + sizeof(struct mem_control_block)==g);

    assert((void *)e + 16 + 72 + 16 + 96 == (void *)g+ sizeof(struct mem_control_block) +96);
    assert(g->size ==96);
    assert(free_list_start->next->size == MEM_SIZE - 376);
    printf("ASSERT TEST SUCCESS - ALL BLOCKS ADDED AT CORRECT ADDRESS AND WITH CORRECT SIZE\n\n");



    //Iteration through free-list at the end
    printf("\nIterating through list of free blocks:\n");
    struct mem_control_block *n = free_list_start;
    int i = 0;
    do {
        printf("Block addr: %p, size: %d, next block addr: %p \n", n, n->size, n->next);
        struct mem_control_block *temp = n->next;
        n = temp;
        i++;
    } while(n != (struct mem_control_block *)0 && i<10);
}