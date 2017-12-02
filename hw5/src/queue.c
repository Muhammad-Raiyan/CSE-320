#include "queue.h"
#include "errno.h"
#include "csapp.h"

queue_t *create_queue(void) {
    queue_t *my_queue = calloc(1, sizeof(queue_t));

    if (my_queue == NULL
        || pthread_mutex_init(&my_queue->lock, 0) != 0
        || sem_init(&my_queue->items, 0, 0) != 0)
    {
        return NULL;
    }

    return my_queue;
}

bool invalidate_queue(queue_t *self, item_destructor_f destroy_function) {
    if(self == NULL){
        errno = EINVAL;
        return false;
    }

    //P(&self->items);                // MOT SURE AT ALL
    pthread_mutex_lock(&self->lock);

    if(self->invalid == true){
        errno = EINVAL;
        return false;
    }
    queue_node_t *temp = self->front;
    while(temp!=NULL){

        queue_node_t *next_temp = temp->next;
        destroy_function(temp->item);
        free(temp);
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
    if(self->invalid == true){
        errno = EINVAL;
        pthread_mutex_unlock(&self->lock);
        return false;
    }
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

    if(self->invalid == true){
        errno = EINVAL;
        V(&self->items);
        pthread_mutex_unlock(&self->lock);
        return NULL;
    }

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