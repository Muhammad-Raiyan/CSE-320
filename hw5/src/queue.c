#include "queue.h"
#include "errno.h"
#include "csapp.h"

queue_t *create_queue(void) {
    queue_t *new_q = calloc(1, sizeof(queue_t));
    sem_init(&new_q->items, 0, 0);
    pthread_mutex_init(&new_q->lock, NULL);
    new_q->front = NULL;
    new_q->rear = NULL;
    new_q->invalid = false;
    return new_q;
}

bool invalidate_queue(queue_t *self, item_destructor_f destroy_function) {
    if(self == NULL){
        errno = EINVAL;
        return false;
    }

    //P(&self->items);                // MOT SURE AT ALL
    pthread_mutex_lock(&self->lock);

    queue_node_t *temp = self->front;
    while(temp!=NULL){
        queue_node_t *next_temp = temp->next;
        destroy_function(temp);
        //free(temp);
        temp = next_temp;
    }
    self->invalid = true;

    pthread_mutex_unlock(&self->lock);

    return true;
}

bool enqueue(queue_t *self, void *item) {

    if(self == NULL){
        errno = EINVAL;
        return false;
    }

    pthread_mutex_lock(&self->lock);

    queue_node_t *node = calloc(1, sizeof(*node));
    node->item = item;
    node->next = NULL;
    if(self->front==NULL && self->rear==NULL){
        self->front = node;
        self->rear = node;
    } else {
        self->rear->next = node;
        self->rear = node;
    }

    pthread_mutex_unlock(&self->lock);
    V(&self->items);
    return true;
}

void *dequeue(queue_t *self) {

    if(self == NULL){
        errno = EINVAL;
        return NULL;
    }

    queue_node_t *temp;
    void *temp_item;

    P(&self->items);
    pthread_mutex_lock(&self->lock);

    temp = self->front;
    temp_item = temp->item;
    if(self->front == self->rear){
        self->front = self->rear = NULL;
    }
    else {
        self->front = self->front->next;
    }
    free(temp);
    pthread_mutex_unlock(&self->lock);
    return temp_item;
}
