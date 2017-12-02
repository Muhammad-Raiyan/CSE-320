#include "cream.h"
#include "utils.h"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "queue.h"
#include "hashmap.h"
#include "csapp.h"
#include "debug.h"

void *thread(void *vargp);
bool is_valid_header(size_t key_size, size_t val_size);
bool is_valid_key(size_t key_size);
bool is_valid_value(size_t value_size);

void put_request(int connfd, request_header_t *request_header, response_header_t *response_header);
void get_request(int connfd, request_header_t *request_header, response_header_t *response_header);
void evict_request(int connfd, request_header_t *request_header, response_header_t *response_header);
void clear_request(int connfd, request_header_t *request_header, response_header_t *response_header);
void invalid_request(int connfd, request_header_t *request_header, response_header_t *response_header);

ssize_t My_Write(int fd, const void *buf, size_t count);

struct sockaddr_storage clientaddr;
queue_t *global_queue;
hashmap_t *global_map;


void map_free_function(map_key_t key, map_val_t val) {
    free(key.key_base);
    free(val.val_base);
}

int main(int argc, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);
    if(argc == 1) {
        fprintf (stderr, "usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    } else {
        if(strcmp(argv[1], "-h")==0) {
            printf("-h                 Displays this help menu and returns EXIT_SUCCESS.\n"
                    "NUM_WORKERS       The number of worker threads used to service requests.\n"
                    "PORT_NUMBER       Port number to listen on for incoming connections.\n"
                    "MAX_ENTRIES       The maximum number of entries that can be stored in `cream` underlying data store.\n");
            exit(EXIT_SUCCESS);
        }
        else if(argc > 4){
            fprintf (stderr, "usage: %s <port>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    int NUM_WORKERS = atoi(argv[1]);
    char* PORT_NUMBER = (argv[2]);
    uint32_t MAX_ENTRIES = atoi(argv[3]);
    int listenfd;
    socklen_t clientlen;

    pthread_t *tid = calloc(NUM_WORKERS, sizeof(pthread_t));

    listenfd = Open_listenfd(PORT_NUMBER);

    global_queue = create_queue();
    global_map = create_map(MAX_ENTRIES, jenkins_one_at_a_time_hash, map_free_function);

    for(int i = 0; i < NUM_WORKERS; i++) /* Create worker threads */
        pthread_create (&tid[i], NULL, thread, NULL) ;

    while (1) {
        int *connfd = Calloc(1, sizeof(int));
        clientlen = sizeof(struct sockaddr_storage);
        *connfd = Accept(listenfd, (SA*) &clientaddr, &clientlen);

        enqueue(global_queue, connfd);
        debug("Before enqueue %d", *connfd);
    }

    exit(0);
}

void *thread(void *vargp){
    while (1) {

        int *connfd = (int *)dequeue(global_queue); /* Remove connfd from buffer */
        debug("After dequeue %d", *connfd);

        request_header_t *req_header = calloc(1, sizeof(request_header_t));
        response_header_t *response_header = calloc(1, sizeof(response_header_t));

        read(*connfd, req_header, sizeof(request_header_t));
        debug("KEY SIZE %u", req_header->key_size);

        switch(req_header->request_code){
            case PUT: put_request(*connfd, req_header, response_header); break;
            case GET: get_request(*connfd, req_header, response_header); break;
            case EVICT: evict_request(*connfd, req_header, response_header); break;
            case CLEAR: clear_request(*connfd, req_header, response_header); break;
            default: invalid_request(*connfd, req_header, response_header); break;
        }
        //free(req_header);
        //free(response_header);
        debug("Fd %d", *connfd);
        close (*connfd);
    }

}

void put_request(int connfd, request_header_t *request_header, response_header_t *response_header){
    void *keys = calloc(1, request_header->key_size);
    read(connfd, keys, request_header->key_size);

    void *vals = calloc(1, request_header->value_size);
    read(connfd, vals, request_header->value_size);
    debug("KEY SIZE %u", request_header->key_size);
    map_key_t key = MAP_KEY(keys, request_header->key_size);
    map_val_t val = MAP_VAL(vals,request_header->value_size);

    if(!is_valid_header(key.key_len, val.val_len)){
        response_header->response_code = BAD_REQUEST;
        response_header->value_size = 0;
        My_Write(connfd, response_header, sizeof(response_header_t));
        return;
    }

    bool overwrite = global_map->size >= global_map->capacity ? true : false;
    bool success = put(global_map, key, val, overwrite);
    debug("success %d", success);
    response_header->response_code = success ?
    OK : BAD_REQUEST;
    if(!success) response_header->value_size = 0;

    My_Write(connfd, response_header, sizeof(response_header_t));
}

void get_request(int connfd, request_header_t *request_header, response_header_t *response_header){

    void *keys = calloc(1, request_header->key_size);
    read(connfd, keys, request_header->key_size);
    map_key_t key = MAP_KEY(keys, request_header->key_size);

    if(!is_valid_key(key.key_len)){
        response_header->response_code = BAD_REQUEST;
        response_header->value_size = 0;
        My_Write(connfd, response_header, sizeof(response_header_t));
        return;
    }

    map_val_t map_val = get(global_map, key);
    if(map_val.val_base == NULL){
        response_header->response_code = NOT_FOUND;
        response_header->value_size = 0;
    } else {
        response_header->response_code = OK;
        response_header->value_size = map_val.val_len;
    }
    My_Write(connfd, response_header, sizeof(response_header_t));
    My_Write(connfd, map_val.val_base, map_val.val_len);

}

void evict_request(int connfd, request_header_t *request_header, response_header_t *response_header){
    void *keys = calloc(1, request_header->key_size);
    read(connfd, keys, request_header->key_size);

    map_key_t key = MAP_KEY(keys, request_header->key_size);

    if(!is_valid_key(key.key_len)){
        response_header->response_code = BAD_REQUEST;
        response_header->value_size = 0;
        My_Write(connfd, response_header, sizeof(response_header_t));
        return;
    }

    map_node_t map_node = delete(global_map, key);
    map_free_function(map_node.key, map_node.val);

    response_header->response_code = OK;
    response_header->value_size = 0;
    My_Write(connfd, response_header, sizeof(response_header_t));
}

void clear_request(int connfd, request_header_t *request_header, response_header_t *response_header){

    clear_map(global_map);
    response_header->response_code = OK;
    response_header->value_size = 0;
    My_Write(connfd, response_header, sizeof(response_header_t));
}

void invalid_request(int connfd, request_header_t *request_header, response_header_t *response_header){
    response_header->response_code = UNSUPPORTED;
    response_header->value_size = 0;
    My_Write(connfd, response_header, sizeof(response_header_t));
}

bool is_valid_header(size_t key_size, size_t val_size){
    return is_valid_key(key_size) && is_valid_value(val_size);
}

bool is_valid_key(size_t key_size){
    if(key_size < MIN_KEY_SIZE || key_size > MAX_KEY_SIZE) {
        return false;
    }
    return true;
}
bool is_valid_value(size_t val_size){
    if(val_size < MIN_VALUE_SIZE || val_size > MAX_VALUE_SIZE) {
        return false;
    }
    return true;
}

ssize_t My_Write(int fd, const void *buf, size_t count){
    size_t nleft = count;
    ssize_t nwritten;
    const char *bufp = buf;

    while (nleft > 0) {
        if ((nwritten = write(fd, bufp, nleft)) <= 0) {
            if (errno == EINTR) // Interrupted by sig handler return
                nwritten = 0;   /* and call write() again */
            else if(errno == EPIPE){
                debug("EPIPE");
                close(fd);
            }
            else
                return -1; /* errno set by write() */
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    return count;
}