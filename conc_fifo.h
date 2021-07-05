/* DESCRIZIONE CODA CONCORRENTE */

#ifndef conc_fifo_h
#define conc_fifo_h

#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#define ERR (-1)     // Definition of the error value
#define SUCCESS (0)     // Definition of the success value
#define TRUE 1
#define FALSE 0

typedef struct generic_node_t {
  void* data;
  struct generic_node_t* next;
  pthread_mutex_t node_mtx;
} generic_node_t;

typedef struct conc_queue {
    generic_node_t* head;
} conc_queue;

typedef generic_node_t* conc_node;

int conc_fifo_init(conc_queue*);     // Queue initialization
int conc_fifo_push(conc_queue*, void*);     // Inserts a generic node at the tail of the list
void* conc_fifo_pop(conc_queue*);     // Removes the generic node at the list's head
int conc_fifo_isEmpty(conc_queue*);     // Returns TRUE if the list is empty, ELSE otherwise
int ll_dealloc_full(conc_queue*);     // Deallocates each generic node of the list and all of their data

#endif // conc_fifo_h