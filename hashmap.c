#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include <standardloop/util.h>

#include "./json.h"

static void freeHashMapEntrySingle(JSONValue *, bool);
static void freeHashMapEntryList(JSONValue *, bool);
static void freeHashMapEntries(JSONValue **, u_int32_t, bool, bool);
static void printHashMapEntry(JSONValue *);
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
    return HashMapInit(DEFAULT_MAP_SIZE, NULL, false);
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

extern HashMap *HashMapInit(u_int32_t initial_capacity, HashFunction *hashFunction, bool force_lowercase)
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
    map->force_lowercase = force_lowercase;
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
    if (map == NULL || entry->key == NULL || (entry->value == NULL && entry->value_type != JSONNULL_t))
    {
        return;
    }
    if (isMapFull(map))
    {
        hashMapResize(map);
    }
    if (map->force_lowercase)
    {
        StringToLower(entry->key);
    }
    u_int32_t index = map->hashFunction(entry->key, map->capacity);
    // printf("[JOSH]: %d\n", (int)index);
    assert(index < map->capacity);
    bool collision = hashMapEntriesInsert(map->entries, index, entry);
    if (collision)
    {
        // printf("COLLISION!\n");
        // fflush(stdout);
        map->collision_count++;
    }
    else
    {
        map->size++;
    }
}

static bool hashMapEntriesInsert(JSONValue **entries, u_int32_t index, JSONValue *entry)
{
    // FIXME: may have to use enum for return values
    // collision, no collision, or error
    if (entries == NULL || entry == NULL || entry->key == NULL)
    {
        return false;
    }
    // printf("[JOSH]: %s\n", entry->key);
    // fflush(stdout);
    JSONValue *collision = entries[index];
    if (collision == NULL)
    {
        entry->next = NULL;
        entries[index] = entry;
        return false;
    }
    // printf("%s -> %s\n", collision->key, entry->key);
    // If duplicate key, update (in future could maybe make this a feature flag for the init function)
    if (collision->key != NULL)
    {
        size_t collision_key_len = strlen(collision->key);
        size_t entry_key_len = strlen(entry->key);
        if (collision_key_len == entry_key_len)
        {
            if (strncmp(collision->key, entry->key, entry_key_len) == 0)
            {
                entry->next = collision->next;
                collision->next = NULL;
                freeHashMapEntrySingle(collision, true);
                entries[index] = entry;
                return true;
            }
        }
    }
    JSONValue *iterator_prev = collision;
    JSONValue *iterator = collision->next;
    while (iterator != NULL)
    {
        if (iterator->key != NULL && entry->key != NULL)
        {
            size_t collision_key_len = strlen(collision->key);
            size_t entry_key_len = strlen(entry->key);
            if (collision_key_len == entry_key_len)
            {
                if (strncmp(iterator->key, entry->key, collision_key_len) == 0)
                {
                    iterator_prev->next = entry;
                    entry->next = iterator->next;
                    iterator->next = NULL;
                    freeHashMapEntrySingle(iterator, true);
                    return true;
                }
            }
        }

        iterator_prev = iterator;
        iterator = iterator->next;
    }
    entry->next = NULL;
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

extern void *HashMapGetValueDirect(HashMap *map, char *key)
{
    JSONValue *value_obj = HashMapGet(map, key);
    if (value_obj == NULL || value_obj->value == NULL)
    {
        return NULL;
    }
    return value_obj->value;
}

static void freeHashMapEntryList(JSONValue *entry, bool deep)
{
    JSONValue *temp = NULL;
    while (entry != NULL)
    {
        temp = entry;
        entry = entry->next;
        if (temp != NULL && temp->key != NULL)
        {
            free(temp->key);
            temp->key = NULL;
        }
        if (temp != NULL)
        {
            FreeJSONValue(temp, deep);
        }
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
            if (entries[i] != NULL)
            {
                freeHashMapEntryList(entries[i], entry_values);
                entries[i] = NULL;
            }
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
        map->entries = NULL;
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
            printHashMapEntry(entry);
            if (entry_count < map->size - 1)
            {
                printf(", ");
            }
            entry_count++;
        }
    }
    printf("}");
}

static void printHashMapEntry(JSONValue *entry)
{
    if (entry == NULL || entry->value == NULL || entry->key == NULL)
    {
        return;
    }
    JSONValue *iterator = entry;
    while (iterator != NULL)
    {
        if (iterator->key != NULL && iterator->value != NULL)
        {
            printf("\"%s\": ", iterator->key);
            PrintJSONValue(iterator);
        }
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
    // printf("hashMapResize\n");
    // fflush(stdout);
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

// FIXME: not complete
extern HashMap *HashMapReplicate(HashMap *map)
{
    if (map == NULL)
    {
        return NULL;
    }
    HashMap *deep_clone = HashMapInit(map->capacity, map->hashFunction, map->force_lowercase);
    deep_clone->collision_count = map->collision_count;
    deep_clone->size = map->collision_count;
    for (u_int32_t i = 0; i < map->capacity; i++)
    {
        deep_clone->entries[i] = JSONValueReplicate(map->entries[i]);
    }
    return deep_clone;
}

extern char *ObjToString(HashMap *map)
{
    // FIXME map->entries == NULL ?
    if (map == NULL)
    {
        return NULL;
    }
    size_t obj_as_string_size = 3; // "{}\0"
    char *obj_as_string = malloc(sizeof(char) * obj_as_string_size);
    if (obj_as_string == NULL)
    {
        printf("[FATAL]: cannot allocate memory for obj as string\n");
        return NULL;
    }
    obj_as_string[0] = CURLY_OPEN_CHAR;
    obj_as_string[1] = NULL_CHAR;

    size_t chars_written = 2 - 1;

    u_int64_t entry_count = 0;
    for (u_int64_t i = 0; i < map->capacity; i++)
    {
        JSONValue *map_entry = map->entries[i];
        bool needs_comma = false;

        while (map_entry != NULL)
        {
            char *entry_key = map_entry->key;
            size_t duplicated_key_size = strlen(entry_key);
            char *duplicated_key = PutQuotesAroundString(entry_key, false);
            duplicated_key_size += 2;

            char *entry_value = JSONValueToString(map_entry);
            size_t entry_value_len = strlen(entry_value);
            if ((map_entry->next == NULL && entry_count < map->size - 1) || map_entry->next != NULL)
            {
                needs_comma = true;
            }

            obj_as_string_size += duplicated_key_size;
            obj_as_string_size++; // ':'
            obj_as_string_size += entry_value_len;
            obj_as_string_size += needs_comma;

            obj_as_string = realloc(obj_as_string, obj_as_string_size);

            CopyStringCanary(obj_as_string, duplicated_key, chars_written);
            chars_written += duplicated_key_size;
            CopyStringCanary(obj_as_string, ":", chars_written);
            chars_written++;
            CopyStringCanary(obj_as_string, entry_value, chars_written);
            chars_written += entry_value_len;
            if (needs_comma)
            {
                CopyStringCanary(obj_as_string, ",", chars_written);
                chars_written++;
            }
            free(duplicated_key);
            free(entry_value);
            if (map_entry->next == NULL)
            {
                entry_count++;
            }
            needs_comma = false;
            map_entry = map_entry->next;
        }
    }

    obj_as_string[obj_as_string_size - 2] = CURLY_CLOSE_CHAR;
    obj_as_string[obj_as_string_size - 1] = NULL_CHAR;
    return obj_as_string;
}
