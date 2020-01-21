/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: William Dang
 * Date: 12/4/2019
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    // FIXME: implement


    assert(map != NULL);
    for (int i = 0; i < map->capacity; i++)
    {
        if (map->table[i] != NULL) 
        {
            struct HashLink* currentLink = map->table[i];
            struct HashLink* nextLink;
            while (currentLink != NULL) 
            {
                nextLink = currentLink->next;
                hashLinkDelete(currentLink);
                currentLink = nextLink;
            }
        }
    }
    free(map->table);
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key  and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    // FIXME: implement
    assert(map != NULL);
    assert(key != NULL);
    
    int hashIndex = HASH_FUNCTION(key) % map->capacity;
    struct HashLink* currentLink;
    currentLink = map->table[hashIndex];

    while (currentLink != NULL)
    {
        if (strcmp(currentLink->key, key) == 0)
        {
            return &(currentLink->value);
        }
        currentLink = currentLink->next;
    }
    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given 
 * capacity (double of the old capacity). After allocating the new table, 
 * all of the links need to rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
    // FIXME: implement
    assert(map != NULL);
    assert(capacity > hashMapCapacity(map));
    int oldCapacity = hashMapCapacity(map);
    struct HashMap* temp = hashMapNew(capacity);

    for (int i = 0; i < oldCapacity; i++)
    {
        HashLink* currentLink = map->table[i];
        while (currentLink != NULL)
        {
            hashMapPut(temp, currentLink->key, currentLink->value);
            currentLink = currentLink->next;
        }
    }

    hashMapCleanUp(map);
    map->size = temp->size;
    map->table = temp->table;
    map->capacity = temp->capacity;

    temp->table = NULL;
    free(temp);
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    // FIXME: implement
    assert(map != NULL);
    assert(key != NULL);
    int mapCap = hashMapCapacity(map);
    int idx = HASH_FUNCTION(key) % hashMapCapacity(map);
    if (idx < 0) 
    {
        idx += hashMapCapacity(map);
    }

    if (hashMapContainsKey(map, key)) 
    {
        int* val = hashMapGet(map, key);
        (*val) += value;
    }
    else 
    {
        HashLink* new = hashLinkNew(key, value, NULL);
        assert(new != NULL);

        if (map->table[idx] == NULL) 
        {
            map->table[idx] = new;
        }
        else 
        {
            HashLink* cur = map->table[idx];
            while (cur->next) 
            {
                cur = cur->next;
            }
            cur->next = new;
        }
        map->size++;
    }

    float loadFact = hashMapTableLoad(map);
    if (loadFact >= MAX_TABLE_LOAD) 
    {
        resizeTable(map, 2 * mapCap);
    }
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    // FIXME: implement
    assert(map != NULL);
    assert(key != NULL);
    int idx = HASH_FUNCTION(key) % hashMapCapacity(map);
    HashLink* cur = map->table[idx];
    HashLink* prev = NULL;

    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            if (prev == NULL) 
            {
                map->table[idx] = cur->next;
            }
            else 
            {
                prev->next = cur->next;
            }
            hashLinkDelete(cur);
            map->size--;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    // FIXME: implement
    assert(map != NULL);
    assert(key != NULL);
    int cap = hashMapCapacity(map);
    int index = HASH_FUNCTION(key) % cap;

    if (index < 0) 
    {
        index += cap;
    }

    HashLink* cur = map->table[index];

    while (cur) 
    {
        if (strcmp(cur->key, key) == 0) 
        {
            return 1;
        }
        cur = cur->next;
    }
    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);
    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);
    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);
    int total = 0;
    int capacity = hashMapCapacity(map);
    for (int i = 0; i < capacity; i++) 
    {
        if (map->table[i] == NULL) 
        {
            total++;
        }
    }
    return total;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    // FIXME: implement
    assert(map != NULL);
    assert(hashMapCapacity(map) > 0);
    float size = (float)hashMapSize(map);
    float cap = (float)hashMapCapacity(map);
    return size / cap;
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
  // FIXME: implement
    for (int i = 0; i < map->capacity; i++)
    {
        HashLink* temp = map->table[i];
        if (temp != NULL)
        {
            printf("\nBucket %i ->", i);
            while (temp != NULL)
            {
                printf("(%s, %d) ->", temp->key, temp->value);
                temp = temp->next;
            }
        }
    }
    printf("\n");
}
