#include <criterion/criterion.h>
#include <criterion/logging.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <debug.h>
#include "hashmap.h"
#define NUM_THREADS 2500
#define MAP_KEY(kbase, klen) (map_key_t) {.key_base = kbase, .key_len = klen}
#define MAP_VAL(vbase, vlen) (map_val_t) {.val_base = vbase, .val_len = vlen}

hashmap_t *global_map;

typedef struct map_insert_t {
    void *key_ptr;
    void *val_ptr;
} map_insert_t;

/* Used in item destruction */
void map_free_function(map_key_t key, map_val_t val) {
    free(key.key_base);
    free(val.val_base);
}

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


void map_init(void) {
    global_map = create_map(NUM_THREADS, jenkins_hash, map_free_function);
}

void *thread_put(void *arg) {
    map_insert_t *insert = (map_insert_t *) arg;

    put(global_map, MAP_KEY(insert->key_ptr, sizeof(int)), MAP_VAL(insert->val_ptr, sizeof(int)), false);
    return NULL;
}

void *thread_delete(void *arg) {
    map_insert_t *insert = (map_insert_t *) arg;

    delete(global_map, MAP_KEY(insert->key_ptr, sizeof(int)));
    return NULL;
}

void *thread_get(void *arg) {
    map_insert_t *insert = (map_insert_t *) arg;

    get(global_map, MAP_KEY(insert->key_ptr, sizeof(int)));
    return NULL;
}

void map_fini(void) {
    invalidate_map(global_map);
}

Test(map_suite, 00_creation, .timeout = 2, .init = map_init, .fini = map_fini) {
    cr_assert_not_null(global_map, "Map returned was NULL");
    cr_assert_eq(global_map->capacity, NUM_THREADS, "Had a capacity of %d for the map. Expected %d", global_map->size, NUM_THREADS);
}

Test(map_suite, 02_multithreaded, .timeout = 2, .init = map_init, .fini = map_fini) {
    pthread_t thread_ids[NUM_THREADS];

    // spawn NUM_THREADS threads to put elements
    for(int index = 0; index < NUM_THREADS; index++) {
        int *key_ptr = malloc(sizeof(int));
        int *val_ptr = malloc(sizeof(int));
        *key_ptr = index;
        *val_ptr = index * 2;

        map_insert_t *insert = malloc(sizeof(map_insert_t));
        insert->key_ptr = key_ptr;
        insert->val_ptr = val_ptr;

        if(pthread_create(&thread_ids[index], NULL, thread_put, insert) != 0)
            exit(EXIT_FAILURE);
    }

    // wait for threads to die before checking queue
    for(int index = 0; index < NUM_THREADS; index++) {
        pthread_join(thread_ids[index], NULL);
    }

    int num_items = global_map->size;
    cr_assert_eq(num_items, NUM_THREADS, "Had %d items in map. Expected %d", num_items, NUM_THREADS);
}


Test(map_suite, 03_get, .timeout = 2, .init = map_init, .fini = map_fini) {
    pthread_t thread_ids[NUM_THREADS];

    for(int index = 0; index < NUM_THREADS; index++) {
        int *key_ptr = malloc(sizeof(int));
        int *val_ptr = malloc(sizeof(int));
        *key_ptr = index;
        *val_ptr = index * 2;

        map_insert_t *insert = malloc(sizeof(map_insert_t));
        insert->key_ptr = key_ptr;
        insert->val_ptr = val_ptr;

        if(pthread_create(&thread_ids[index], NULL, thread_put, insert) != 0){
            exit(EXIT_FAILURE);
        }

    }

    for(int index = 0; index < NUM_THREADS; index++) {
            pthread_join(thread_ids[index], NULL);
    }


    int index = 1;
    // Use the key to get the value from the hashmap
    int *key_ptr = malloc(sizeof(int));
    int *val_ptr = malloc(sizeof(int));
    *key_ptr = index;
    *val_ptr = index * 2;

    map_insert_t *insert = malloc(sizeof(map_insert_t));
    insert->key_ptr = key_ptr;
    insert->val_ptr = val_ptr;

    map_key_t key;
    key.key_len = sizeof(int);
    key.key_base = key_ptr;
    map_val_t retrieved_val = get(global_map,key);

    // The val we expect
    map_val_t val;
    val.val_len = sizeof(int);
    val.val_base = val_ptr;

    // Compare what we have to what we expect.
    int compare = memcmp(retrieved_val.val_base, val.val_base, val.val_len);
    cr_assert_eq(compare, 0, "Keys do not match");

}

Test(map_suite, 04_deletion, .timeout = 2, .init = map_init, .fini = map_fini) {

    pthread_t thread_ids[NUM_THREADS];

    // spawn NUM_THREADS threads to put elements
    for(int index = 0; index < NUM_THREADS; index++) {
        int *key_ptr = malloc(sizeof(int));
        int *val_ptr = malloc(sizeof(int));
        *key_ptr = index;
        *val_ptr = index * 2;

        map_insert_t *insert = malloc(sizeof(map_insert_t));
        insert->key_ptr = key_ptr;
        insert->val_ptr = val_ptr;

        if(pthread_create(&thread_ids[index], NULL, thread_put, insert) != 0)
            exit(EXIT_FAILURE);
    }

    for(int index = 0; index < NUM_THREADS; index++) {
        pthread_join(thread_ids[index], NULL);
    }
    printf("NUM: %d\n", global_map->size);

    for(int index = 0; index < NUM_THREADS; index++) {
        int *key_ptr = malloc(sizeof(int));
        *key_ptr = index;

        map_insert_t *insert = malloc(sizeof(map_insert_t));
        insert->key_ptr = key_ptr;

        if(pthread_create(&thread_ids[index], NULL, thread_delete, insert) != 0)
            exit(EXIT_FAILURE);
    }

    for(int index = 0; index < NUM_THREADS; index++) {
        pthread_join(thread_ids[index], NULL);
    }


    int num_items = global_map->size;
    cr_assert_eq(num_items, 0, "Had %d items in map. Expected %d", num_items, 0);

    cr_assert_not_null(global_map, "Map returned was NULL");
}

Test(map_suite, 05_put_get, .timeout = 2, .init = map_init, .fini = map_fini) {


    // put
    for(int index = 0; index < 5; index++){
        int *key_ptr = malloc(sizeof(int));
        int *val_ptr = malloc(sizeof(int));
        *key_ptr = index;
        *val_ptr = index * 2;
        map_insert_t *insert = malloc(sizeof(map_insert_t));
        insert->key_ptr = key_ptr;
        insert->val_ptr = val_ptr;
        put(global_map, MAP_KEY(insert->key_ptr, sizeof(int)), MAP_VAL(insert->val_ptr, sizeof(int)), false);
        map_val_t val;
        val = get(global_map,MAP_KEY(insert->key_ptr, sizeof(int)));

        int val_of_val;
        val_of_val = *(int *)(val.val_base);
        cr_assert_eq(val_of_val, index*2, "PUT GET i %d Expected %d and got %d.", index*2,val_of_val);
    }

    int num_items = global_map->size;
    cr_assert_eq(num_items, 5, "Had %d items in map that has a capacity of %d. Expected %d ", num_items, global_map->capacity, 5);

    // get
    for(int index = 0; index < 5; index++){
        int *key_ptr = malloc(sizeof(int));
        int *val_ptr = malloc(sizeof(int));
        *key_ptr = index;
        *val_ptr = index * 2;
        map_insert_t *insert = malloc(sizeof(map_insert_t));
        insert->key_ptr = key_ptr;
        insert->val_ptr = val_ptr;

        map_val_t val;
        val = get(global_map,MAP_KEY(insert->key_ptr, sizeof(int)));
        //if(val.val_len);

        int val_of_val;
        val_of_val = *(int *)(val.val_base);
        cr_assert_eq(val_of_val, index*2, "GET i %d Expected %d and got %d.", index, index*2,val_of_val);
    }

    num_items = global_map->size;
    cr_assert_eq(num_items, 5, "Had %d items in map that has a capacity of %d. Expected %d ", num_items, global_map->capacity, 5);

}

void *thread_clear(void *arg) {
    clear_map(global_map);
    return NULL;
}

Test(map_suite, 06_clear, .timeout = 2, .init = map_init, .fini = map_fini){
    pthread_t thread_ids[NUM_THREADS];

    // spawn NUM_THREADS threads to put elements
    for(int index = 0; index < NUM_THREADS; index++) {
        int *key_ptr = malloc(sizeof(int));
        int *val_ptr = malloc(sizeof(int));
        *key_ptr = index;
        *val_ptr = index * 2;

        map_insert_t *insert = malloc(sizeof(map_insert_t));
        insert->key_ptr = key_ptr;
        insert->val_ptr = val_ptr;

        if(pthread_create(&thread_ids[index], NULL, thread_put, insert) != 0)
            exit(EXIT_FAILURE);
    }

    // wait for threads to die before checking queue
    for(int index = 0; index < NUM_THREADS; index++) {
        pthread_join(thread_ids[index], NULL);
    }

    pthread_t clear_threads[2];

    for(int i = 0; i < 2; i++) {
        if(pthread_create(&clear_threads[i], NULL, thread_clear, NULL) != 0)
            exit(EXIT_FAILURE);
    }

    for(int i = 0; i < 2; i++)
        pthread_join(clear_threads[i], NULL);

    cr_assert_eq(global_map->size, 0, "Had %d. Expected 0", global_map->size);

    // int num_items = global_map->size;
    // cr_assert_eq(num_items, 0, "Had %d items in map. Expected %d", num_items, 0);
}


/*Test(map_suite, 07_put_delete_get, .timeout = 2, .init = map_init, .fini = map_fini) {


    int index = 5;
    int *key_ptr = malloc(sizeof(int));
    int *val_ptr = malloc(sizeof(int));
    *key_ptr = index;
    *val_ptr = index * 2;

    map_insert_t *insert = malloc(sizeof(map_insert_t));
    insert->key_ptr = key_ptr;
    insert->val_ptr = val_ptr;

    // PUT
    put(global_map, MAP_KEY(insert->key_ptr, sizeof(int)), MAP_VAL(insert->val_ptr, sizeof(int)), false);

    // DELETE
    //delete(global_map, MAP_KEY(insert->key_ptr, sizeof(int)));

    // GET
    map_val_t val = get(global_map, MAP_KEY(insert->key_ptr, sizeof(int)));
    int val_val = atoi(val.val_base);
    cr_assert_eq(val_val, 10, "Expected: %d\nGot: %d", 10, val_val);
}*/