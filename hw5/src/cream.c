#include "cream.h"
#include "utils.h"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include "hashmap.h"


uint32_t jenkins_hash(map_key_t map_key) {
    const uint8_t *key = map_key.key_base;
    size_t length = map_key.key_len;
    size_t i = 0;
    uint32_t hash = 0;

    while (i != length) {
        hash += key[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }

    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

void map_free_function(map_key_t key, map_val_t val) {
    free(key.key_base);
    free(val.val_base);
}

int main(int argc, char *argv[]) {
    hashmap_t *map = create_map(100, jenkins_hash, map_free_function);
    int key_ptr = 10;
    int val_ptr = 5;

    put(map, MAP_KEY(&key_ptr, sizeof(int)), MAP_VAL(&val_ptr, sizeof(int)), false);
    printf("Size of map: %d\n", map->size);
    map_val_t val =get(map, MAP_KEY(&key_ptr, sizeof(int)));
    printf("%d\n", *(int *)val.val_base);
    exit(0);
}
