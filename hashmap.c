#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "./json.h"

static void freeHashMapEntrySingle(JSONValue *, bool);
static void freeHashMapEntryList(JSONValue *, bool);
static void freeHashMapEntries(JSONValue **, u_int32_t, bool, bool);
static void printHashMapEntry(JSONValue *, u_int32_t);
static u_int32_t defaultHashFunction(char *, u_int32_t);

static bool hashMapEntriesInsert(JSONValue **, u_int32_t, JSONValue *);

static JSONValue **hashMapEntriesInit(u_int32_t);

static inline bool isMapFull(HashMap *);
static void hashMapResize(HashMap *map);

// Jenkins's one_at_a_time
static u_int32_t defaultHashFunction(char *key, u_int32_t capacity)
{
    u_int32_t len = strlen(key);
    u_int32_t hash = 0;

    for (u_int32_t i = 0; i < len; ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash % capacity;
}

extern HashMap *DefaultHashMapInit(void)
{
    return HashMapInit(DEFAULT_MAP_SIZE, NULL);
}

static JSONValue **hashMapEntriesInit(u_int32_t capacity)
{
    JSONValue **entries = malloc(sizeof(JSONValue *) * capacity);
    if (entries == NULL)
    {
        return NULL;
    }

    for (u_int32_t i = 0; i < capacity; i++)
    {
        entries[i] = NULL;
    }

    return entries;
}

extern HashMap *HashMapInit(u_int32_t initial_capacity, HashFunction *hashFunction)
{
    HashMap *map = malloc(sizeof(HashMap));
    if (map == NULL)
    {
        printf("[ERROR]: could not allocate memory for HashMap\n");
        return NULL;
    }
    map->size = 0;
    map->collision_count = 0;
    map->capacity = initial_capacity;
    map->entries = hashMapEntriesInit(initial_capacity);

    if (map->entries == NULL)
    {
        FreeHashMap(map);
        return NULL;
    }

    if (hashFunction == NULL)
    {
        map->hashFunction = defaultHashFunction;
    }
    return map;
}

static inline bool isMapFull(HashMap *map)
{
    return map->capacity == map->size;
}

extern void HashMapInsert(HashMap *map, JSONValue *entry)
{
    if (map == NULL || entry->key == NULL || (entry->value == NULL && entry->value_type != NULL_t))
    {
        return;
    }
    if (isMapFull(map))
    {
        hashMapResize(map);
    }
    u_int32_t index = map->hashFunction(entry->key, map->capacity);
    bool collision = hashMapEntriesInsert(map->entries, index, entry);
    if (collision)
    {
        map->collision_count++;
    }
    else
    {
        map->size++;
    }
}

static bool hashMapEntriesInsert(JSONValue **entries, u_int32_t index, JSONValue *entry)
{
    JSONValue *collision = entries[index];
    if (collision == NULL)
    {
        entries[index] = entry;
        return false;
    }
    // If duplicate key, update
    if (strcmp(collision->key, entry->key) == 0)
    {
        entry->next = collision->next;
        collision->next = NULL;
        freeHashMapEntrySingle(collision, true);
        entries[index] = entry;
        return true;
    }

    JSONValue *iterator_prev = collision;
    JSONValue *iterator = collision->next;
    while (iterator != NULL)
    {
        if (strcmp(iterator->key, entry->key) == 0)
        {
            iterator_prev->next = entry;
            entry->next = iterator->next;
            iterator->next = NULL;
            freeHashMapEntrySingle(iterator, true);
            return true;
        }
        iterator = iterator->next;
        iterator_prev = iterator_prev->next;
    }
    iterator_prev->next = entry;
    return true;
}

extern JSONValue *HashMapGet(HashMap *map, char *key)
{
    if (map == NULL || key == NULL)
    {
        return NULL;
    }
    u_int32_t index = map->hashFunction(key, map->capacity);
    JSONValue *entry = map->entries[index];
    if (entry == NULL)
    {
        return NULL;
    }

    JSONValue *iterator = entry;
    while (iterator != NULL)
    {
        if (strcmp(key, iterator->key) == 0)
        {
            return iterator;
        }
        iterator = iterator->next;
    }
    return NULL;
}

static void freeHashMapEntryList(JSONValue *entry, bool deep)
{
    JSONValue *temp = NULL;
    while (entry != NULL)
    {
        temp = entry;
        entry = entry->next;
        if (temp->key != NULL)
        {
            free(temp->key);
            temp->key = NULL;
        }
        FreeJSONValue(temp, deep);
    }
}

static void freeHashMapEntrySingle(JSONValue *entry, bool deep)
{
    if (entry != NULL)
    {
        if (entry->key != NULL)
        {
            free(entry->key);
            entry->key = NULL;
        }
        FreeJSONValue(entry, deep);
    }
}

static void freeHashMapEntries(JSONValue **entries, u_int32_t size, bool deep, bool entry_values)
{
    if (entries == NULL)
    {
        return;
    }
    if (deep)
    {
        for (u_int32_t i = 0; i < size; i++)
        {
            freeHashMapEntryList(entries[i], entry_values);
        }
    }
    free(entries);
}

extern void FreeHashMap(HashMap *map)
{
    if (map == NULL)
    {
        return;
    }
    if (map->entries != NULL)
    {
        freeHashMapEntries(map->entries, map->capacity, true, true);
    }
    free(map);
}

extern void HashMapRemove(HashMap *map, char *key)
{
    if (map == NULL)
    {
        return;
    }
    u_int32_t index = map->hashFunction(key, map->capacity);
    JSONValue *entry = map->entries[index];
    if (entry == NULL)
    {
        return;
    }
    if (entry->next == NULL)
    {
        freeHashMapEntrySingle(entry, true);
        map->entries[index] = NULL;
        map->size--;
        return;
    }

    if (strcmp(key, entry->key) == 0)
    {
        JSONValue *temp = entry;
        map->entries[index] = entry->next;
        entry->next = NULL;
        freeHashMapEntrySingle(temp, true);
        map->collision_count--;
        return;
    }

    JSONValue *iterator_prev = entry;
    JSONValue *iterator = entry->next;
    while (iterator != NULL)
    {
        if (strcmp(key, iterator->key) == 0)
        {
            iterator_prev->next = iterator->next;
            map->entries[index] = iterator_prev;
            freeHashMapEntrySingle(iterator, true);
            map->collision_count--;
            break;
        }
        iterator_prev = iterator_prev->next;
        iterator = iterator->next;
    }
}

extern void PrintHashMap(HashMap *map)
{
    if (map == NULL)
    {
        return;
    }
    printf("{");
    u_int32_t entry_count = 0;
    for (u_int32_t i = 0; i < map->capacity; i++)
    {
        JSONValue *entry = map->entries[i];
        if (entry != NULL)
        {
            printHashMapEntry(entry, i);
            if (entry_count < map->size - 1)
            {
                printf(", ");
            }
            entry_count++;
        }
    }
    printf("}");
}

static void printHashMapEntry(JSONValue *entry, u_int32_t index)
{
    if (entry == NULL)
    {
        return;
    }
    if (index)
    {
        pass;
    }
    JSONValue *iterator = entry;
    while (iterator != NULL)
    {
        printf("\"%s\": ", iterator->key);
        PrintJSONValue(iterator);

        iterator = iterator->next;
        if (iterator != NULL)
        {
            printf(", ");
        }
    }
}

// JOSH
static void hashMapResize(HashMap *map)
{
    if (map == NULL)
    {
        return;
    }

    u_int32_t new_capacity = map->capacity * DEFAULT_MAP_RESIZE_MULTIPLE;
    u_int32_t new_size = 0;
    u_int32_t new_collision_count = 0;

    JSONValue **new_entries = hashMapEntriesInit(new_capacity);

    if (new_entries == NULL)
    {
        return;
    }

    for (u_int32_t i = 0; i < map->capacity; i++)
    {
        JSONValue *entry = map->entries[i];
        JSONValue *iterator = entry;

        while (iterator != NULL)
        {
            JSONValue *new_entry = JSONValueInit(iterator->value_type, iterator->value, iterator->key);
            u_int32_t new_index = map->hashFunction(iterator->key, new_capacity);
            bool collision = hashMapEntriesInsert(new_entries, new_index, new_entry);
            if (!collision)
            {
                new_size++;
            }
            else
            {
                new_collision_count++;
            }
            JSONValue *temp = iterator;
            iterator = iterator->next;
            FreeJSONValue(temp, false);
        }
    }
    freeHashMapEntries(map->entries, map->capacity, false, false);

    map->size = new_size;
    map->collision_count = new_collision_count;
    map->capacity = new_capacity;
    map->entries = new_entries;
}

extern HashMap *HashMapReplicate(HashMap *map)
{
    if (map == NULL)
    {
        return NULL;
    }
    HashMap *deep_clone = HashMapInit(map->capacity, map->hashFunction);
    deep_clone->collision_count = map->collision_count;
    deep_clone->size = map->collision_count;
    for (u_int32_t i = 0; i < map->capacity; i++)
    {
        deep_clone->entries[i] = JSONValueReplicate(map->entries[i]);
    }
    return deep_clone;
}
