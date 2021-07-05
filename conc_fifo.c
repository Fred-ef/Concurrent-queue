#include "conc_fifo.h"


int conc_fifo_init(conc_queue* queue) {
    if(!queue) {errno=EINVAL; return ERR;}

    queue->head=conc_node_create(NULL);
    if(!(queue->head)) return ERR;

    return SUCCESS;
}


int conc_fifo_push(conc_queue* queue, void* data) {
    if(!queue) {errno=EINVAL; return ERR;}     // Uninitialized list
    if(!(queue->head)) {errno=EINVAL; return ERR;}     // Uninitialized list

    int temperr;
    conc_node newelement=conc_node_create(data);
    if(!newelement) return ERR;     // errno already set

    temperr=pthread_mutex_lock(&((queue->head)->node_mtx));
    if(temperr) {errno=temperr; free(newelement); return ERR;}

    if(!((queue->head)->next)) {         // If list is empty, append element and return
        (queue->head)->next=newelement;
        temperr=pthread_mutex_unlock(&((queue->head)->node_mtx));
        if(temperr) {errno=temperr; free(newelement); return ERR;}
        return SUCCESS;
    }     
    else {
        conc_node aux1=queue->head, aux2;
        while(aux1->next!=NULL) {
            aux2=aux1;
            aux1=aux1->next;
            temperr=pthread_mutex_lock(&(aux1->node_mtx));
            if(temperr) {errno=temperr; free(newelement); return ERR;}
            temperr=pthread_mutex_unlock(&(aux2->node_mtx));
        }
        aux1->next=newelement;
        temperr=pthread_mutex_unlock(&(aux1->node_mtx));
        if(temperr) {errno=temperr; free(newelement); return ERR;}
    }
    return SUCCESS;
}


// Removes the generic node at the list's head
void* conc_fifo_pop(conc_queue* queue) {
    if(!queue) {errno=EINVAL; return (void*)NULL;}     // Uninitialized list
    if(!(queue->head)) {errno=EINVAL; return (void*)NULL;}     // Uninitialized list

    int temperr;
    temperr=pthread_mutex_lock(&((queue->head)->node_mtx));
    if(temperr) {errno=temperr; return (void*)NULL;}

    if(!((queue->head)->next)) {
        temperr=pthread_mutex_unlock(&((queue->head)->node_mtx));
        if(temperr) {errno=temperr; return (void*)NULL;}
        errno=EINVAL;
        return (void*)NULL;
    }

    conc_node aux=(queue->head)->next;
    temperr=pthread_mutex_lock(&(aux->node_mtx));
    if(temperr) {errno=temperr; return (void*)NULL;}

    (queue->head)->next=aux->next;

    temperr=pthread_mutex_unlock(&((queue->head)->node_mtx));
    if(temperr) {errno=temperr; return (void*)NULL;}
    temperr=pthread_mutex_unlock(&(aux->node_mtx));
    if(temperr) {errno=temperr; return (void*)NULL;}

    return (conc_node_destroy(aux));
}

// Returns TRUE if the list is empty, ELSE otherwise
int conc_fifo_isEmpty(conc_queue* queue) {
  if(!queue) {errno=EINVAL; return ERR;}     // Uninitialized list
  if(!(queue->head)) {errno=EINVAL; return ERR;}     // Uninitialized list

  int temperr;
  temperr=pthread_mutex_lock(&((queue->head)->node_mtx));
  if(temperr) {errno=temperr; return ERR;}

  if(!((queue->head)->next)) {      // If empty, return TRUE
    temperr=pthread_mutex_unlock(&((queue->head)->node_mtx));
    if(temperr) {errno=temperr; return ERR;}
    return TRUE;
  }
  
  // else, unlock mutex and return FALSE
  temperr=pthread_mutex_unlock(&((queue->head)->node_mtx));
  if(temperr) {errno=temperr; return ERR;}

  return FALSE;
}


// Deallocates each generic node of the list and all of their data; WARNING: NON CONCURRENT
int fifo_dealloc_full(conc_queue* queue) {
    if(!queue) {errno=EINVAL; return ERR;}     // Uninitialized list
    if(!(queue->head)) {errno=EINVAL; return ERR;}     // Uninitialized list

    int temperr;
    
    if(!((queue->head)->next)) {        // For an empty list, deallocating its head node and pointer suffices
        temperr=pthread_mutex_destroy(&((queue->head)->node_mtx));
        if(temperr) {errno=temperr; return ERR;}
        if(queue->head) free(queue->head);
        if(queue) free(queue);
        return SUCCESS;
    }

    conc_node aux1=queue->head, aux2;
    while(aux1!=NULL) {
        aux2=aux1;
        aux1=aux1->next;
        temperr=pthread_mutex_destroy(&((aux2)->node_mtx));
        if(temperr) {errno=temperr; return ERR;}
        if((aux2)->data) free((aux2)->data);
        if(aux2) free(aux2);
    }

    if(queue) free(queue);
    return SUCCESS;
}







void* t_fun_pari(void* arg) {
  int i;
  pid_t tid=gettid();
  printf("Thread %d: let's begin\n", tid);
  conc_queue* list=(conc_queue*)arg;
  for(i=0; i<100; i++) {
    int* i_p=(int*)malloc(sizeof(int));
    (*i_p)=i;
    if(conc_fifo_push(list, (void*)i_p)) return (void*)1;
  }

  return (void*)NULL;
}

void* t_fun_dispari(void* arg) {
  int i;
  int* temp;
  conc_queue* list=(conc_queue*)arg;
  for(i=0; i<100; i++) {
    temp=(int*)conc_fifo_pop(list);
    if(temp) printf("%d\n", (*temp));
  }

  return (void*)NULL;
}

int main() {
  int i;
  int temperr;

  conc_queue* list=(conc_queue*)malloc(sizeof(conc_queue));
  conc_fifo_init(list);

  pthread_t* t_array = (pthread_t*)malloc(10*sizeof(pthread_t));
  for(i=0; i<10; i++) {
    
    if((i%2)==0) pthread_create(&(t_array[i]), NULL, t_fun_pari, (void*)list);
    else pthread_create(&(t_array[i]), NULL, t_fun_dispari, (void*)list);
    
    // pthread_create(&(t_array[i]), NULL, t_fun_pari, (void*)list);
    printf("Spawned thread %d\n", i);
  }

  for(i=0; i<10; i++) {
    pthread_join(t_array[i], NULL);
    printf("Joined thread %d\n", i);
  }
}