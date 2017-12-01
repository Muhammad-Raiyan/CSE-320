#include "cream.h"
#include "utils.h"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "queue.h"
#include "hashmap.h"
#include "csapp.h"


void *thread(void *vargp);

struct sockaddr_storage clientaddr;
queue_t *global_queue;
hashmap_t *global_map;


void map_free_function(map_key_t key, map_val_t val) {
    free(key.key_base);
    free(val.val_base);
}

int main(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf (stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    int NUM_WORKERS = atoi(argv[1]);
    char* PORT_NUMBER = (argv[2]);
    uint32_t MAX_ENTRIES = atoi(argv[3]);
    int listenfd, connfd;
    socklen_t clientlen;

    pthread_t *tid = calloc(NUM_WORKERS, sizeof(pthread_t));

    listenfd = Open_listenfd(PORT_NUMBER);

    global_queue = create_queue();
    global_map = create_map(MAX_ENTRIES, jenkins_one_at_a_time_hash, map_free_function);

    for(int i = 0; i < NUM_WORKERS; i++) /* Create worker threads */
        Pthread_create (&tid[i], NULL, thread, NULL) ;

    while (1) {
        clientlen = sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd, (SA*) &clientaddr, &clientlen);
        enqueue(global_queue, &connfd);
    }
    exit(0);
}

void *thread(void *vargp){
    Pthread_detach(pthread_self());
    while (1) {
        int connfd = *(int *)dequeue(global_queue); /* Remove connfd from buffer */
        request_header_t *req_header = calloc(1, sizeof(request_header_t));
        read(connfd, req_header, sizeof(request_header_t));

        void *keys = Calloc(1, req_header->key_size);
        read(connfd, keys, req_header->key_size);

        void *vals = Calloc(1, req_header->value_size);
        read(connfd, vals, req_header->value_size);

        map_key_t key = MAP_KEY(keys, req_header->key_size);
        map_val_t val = MAP_VAL(vals,req_header->value_size);

        response_header_t *response_header = Calloc(1, sizeof(response_header_t));
        switch(req_header->request_code){
            case PUT:{
                bool overwrite =
                    global_map->size >= global_map->capacity ? true : false;
                bool success = put(global_map, key, val, overwrite);
                response_header->response_code = success ?
                     OK : BAD_REQUEST;
                if(!success) response_header->value_size = 0;
                write(connfd, response_header, sizeof(response_header_t));
                break;
            }
            case GET:{
                map_val_t map_val = get(global_map, key);
                if(map_val.val_base == NULL){
                    response_header->response_code = BAD_REQUEST;
                    response_header->value_size = 0;
                } else {
                    response_header->response_code = OK;
                    response_header->value_size = map_val.val_len;
                }
                write(connfd, response_header, sizeof(response_header_t));
                write(connfd, map_val.val_base, map_val.val_len);
                break;
            }
            case EVICT: {
                delete(global_map, key);
                response_header->response_code = OK;
                response_header->value_size = 0;
                write(connfd, response_header, sizeof(response_header_t));
                break;
            }
            case CLEAR:{
                clear_map(global_map);
                response_header->response_code = OK;
                response_header->value_size = 0;
                write(connfd, response_header, sizeof(response_header_t));
                break;
            }
        }

        Close (connfd);
    }
}
