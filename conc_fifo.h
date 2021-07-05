/* DESCRIZIONE CODA CONCORRENTE */

#ifndef conc_fifo_h
#define conc_fifo_h

#include "conc_elem.h"
#define SUCCESS 0
#define ERR -1
#define TRUE 1
#define FALSE 0

typedef struct conc_queue {
    conc_node head;
} conc_queue;

int conc_fifo_init(conc_queue*);     // Queue initialization
int conc_fifo_push(conc_queue*, void*);     // Inserts a generic node at the tail of the list
void* conc_fifo_pop(conc_queue*);     // Removes the generic node at the list's head
int conc_fifo_isEmpty(conc_queue*);     // Returns TRUE if the list is empty, ELSE otherwise
int ll_dealloc_full(conc_queue*);     // Deallocates each generic node of the list and all of their data

#endif // conc_fifo_h