#include "utils.h"
#include <string.h>
#include <errno.h>
#include "debug.h"
#define MAP_KEY(base, len) (map_key_t) {.key_base = base, .key_len = len}
#define MAP_VAL(base, len) (map_val_t) {.val_base = base, .val_len = len}
#define MAP_NODE(key_arg, val_arg, tombstone_arg) (map_node_t) {.key = key_arg, .val = val_arg, .tombstone = tombstone_arg}

bool is_same_key(map_key_t key1, map_key_t key2);

hashmap_t *create_map(uint32_t capacity, hash_func_f hash_function, destructor_f destroy_function) {
    hashmap_t *newmap = calloc(1, sizeof(hashmap_t));
    newmap->capacity = capacity;
    newmap->size = 0;
    newmap->nodes = calloc(capacity, sizeof(map_node_t));
    newmap->hash_function = hash_function;
    newmap->destroy_function = destroy_function;
    newmap->num_readers = 0;
    pthread_mutex_init(&newmap->write_lock, NULL);
    pthread_mutex_init(&newmap->fields_lock, NULL);
    newmap->invalid = false;

    return newmap;
}

bool put(hashmap_t *self, map_key_t key, map_val_t val, bool force) {
    if(self == NULL || key.key_base == NULL || key.key_len==0 || val.val_base==NULL || val.val_len == 0){
        errno = EINVAL;
        return false;
    }

    pthread_mutex_lock(&self->write_lock);

    if(self->invalid == true){
        errno = EINVAL;
        pthread_mutex_unlock(&self->write_lock);
        return false;
    }

    int index = get_index(self, key);

    map_node_t *first_map_node = self->nodes;
    map_node_t *cur_map_node = (first_map_node+index);

    if(self->size >= self->capacity && force == true){

        self->destroy_function(cur_map_node->key, cur_map_node->val);
        *cur_map_node = MAP_NODE(key, val, false);

        pthread_mutex_unlock(&self->write_lock);
        return true;
    }

    int probe_range = self->capacity;
    int i = 0;


    while((cur_map_node->key).key_base != NULL &&
        (!is_same_key(cur_map_node->key, key) || cur_map_node->tombstone==false)){
        i++;
        index++;
        if(i >= probe_range){
            pthread_mutex_unlock(&self->write_lock);
            return false;
        }
        if(index >= probe_range) index = 0;
        cur_map_node = first_map_node+index;
    }

    self->destroy_function(cur_map_node->key, cur_map_node->val);
    *cur_map_node = MAP_NODE(key, val, false);
    ++(self->size);

    pthread_mutex_unlock(&self->write_lock);
    return true;
}

map_val_t get(hashmap_t *self, map_key_t key) {

    if(self == NULL || key.key_base == NULL || key.key_len==0){
        errno = EINVAL;
        return MAP_VAL(NULL, 0);
    }

    pthread_mutex_lock(&self->fields_lock);
    self->num_readers++;
    if(self->num_readers==1){
        pthread_mutex_lock(&self->write_lock);
    }
    pthread_mutex_unlock(&self->fields_lock);

    if(self->invalid == true){
        errno = EINVAL;
        pthread_mutex_lock(&self->fields_lock);
        self->num_readers--;
        if(self->num_readers == 0){
            pthread_mutex_unlock(&self->write_lock);
        }
        pthread_mutex_unlock(&self->fields_lock);
        return MAP_VAL(NULL, 0);
    }

    int index = get_index(self, key);
    map_node_t *first_map_node = self->nodes;
    map_node_t *cur_map_node = (first_map_node+index);
    int probe_range = self->capacity;
    int i = 0;

    while(!is_same_key(cur_map_node->key, key) && cur_map_node->tombstone){
        i++;
        index++;
        if(i >= probe_range){
            pthread_mutex_lock(&self->fields_lock);
            self->num_readers--;
            if(self->num_readers == 0){
                pthread_mutex_unlock(&self->write_lock);
            }
            pthread_mutex_unlock(&self->fields_lock);
            return MAP_VAL(NULL, 0);
        }

        if(index >= probe_range) index = 0;
        cur_map_node = first_map_node+index;

    }
    debug("I %d index %d range %d", i, index, probe_range);

    pthread_mutex_lock(&self->fields_lock);
    self->num_readers--;
    if(self->num_readers == 0){
        pthread_mutex_unlock(&self->write_lock);
    }
    //pthread_mutex_unlock(&self->write_lock);
    pthread_mutex_unlock(&self->fields_lock);
    return cur_map_node->val;
}

map_node_t delete(hashmap_t *self, map_key_t key) {
    if(self == NULL || key.key_base == NULL || key.key_len==0){
        errno = EINVAL;
        return MAP_NODE(MAP_KEY(NULL, 0), MAP_VAL(NULL, 0), false);
    }

    pthread_mutex_lock(&self->write_lock);
    if(self->invalid == true){
        errno = EINVAL;
        pthread_mutex_unlock(&self->write_lock);
        return MAP_NODE(MAP_KEY(NULL, 0), MAP_VAL(NULL, 0), false);
    }

    int index = get_index(self, key);
    map_node_t *first_map_node = self->nodes;
    map_node_t *cur_map_node = (first_map_node+index);
    int probe_len = self->capacity;
    int i = 0;
    while(!is_same_key(cur_map_node->key, key) && cur_map_node->tombstone){
        i++;
        index++;
        if(i >= probe_len) {
            pthread_mutex_unlock(&self->write_lock);
            return MAP_NODE(MAP_KEY(NULL, 0), MAP_VAL(NULL, 0), false);
        }
        if(index >= probe_len) index = 0;
        cur_map_node = first_map_node+index;
    }
    cur_map_node->tombstone = true;
    --(self->size);
    pthread_mutex_unlock(&self->write_lock);
    return *cur_map_node;
}

bool clear_map(hashmap_t *self) {
    if(self == NULL){
        errno = EINVAL;
        return false;
    }

    pthread_mutex_lock(&self->write_lock);
    if(self->invalid == true){
        errno = EINVAL;
        pthread_mutex_unlock(&self->write_lock);
        return false;
    }
    self->size=0;
    map_node_t *first_map_node = self->nodes;
    for(int i=0; i<self->capacity; i++){
        *first_map_node = MAP_NODE(MAP_KEY(NULL, 0), MAP_VAL(NULL, 0), false);
    }

    pthread_mutex_unlock(&self->write_lock);
	return true;
}

bool invalidate_map(hashmap_t *self) {
    if(self == NULL){
        errno = EINVAL;
        return false;
    }

    pthread_mutex_lock(&self->write_lock);
    if(self->invalid == true){
        errno = EINVAL;
        pthread_mutex_unlock(&self->write_lock);
        return false;
    }

    self->invalid = true;
    map_node_t *first_map_node = self->nodes;
    for(int i=0; i<self->capacity; i++){
        self->destroy_function(first_map_node[i].key, first_map_node[i].val);
    }
    free(self->nodes);
    pthread_mutex_unlock(&self->write_lock);
    return true;
}

bool is_same_key(map_key_t key1, map_key_t key2){
    if( key1.key_len == key2.key_len && memcmp(key1.key_base, key2.key_base, key1.key_len) == 1){
        return true;
    }
    return false;
}

