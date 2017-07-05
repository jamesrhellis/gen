#ifndef RH_HASH_H
#define RH_HASH_H

#include "stdlib.h"
#include "string.h"

// This is returned by functions as the hashmap does not take ownership of any resources
// So both the key and value must be returned for management
struct rh_hash_bucket {							
	// Hash should be size_t, but this would require Arch dependent hashing algorithms
	// 0 is reserved for no-item case -- This is only useful if the key type cannot be
	// optional, e.g. int, char etc
	int64_t hash;							
									
	const char *key;							
	const char *value;							
};									
									
typedef struct {							
	size_t size;							
	//size_t no_items;		// Re-sizeable hash tables NYI
	struct rh_hash_bucket *items;					
} rh_hash;						

static inline int64_t rh_string_hash(const char *string) {
	// This is a 64 bit FNV-1a hash
	int64_t hash = 14695981039346656037LU;

	while (*string) {
		hash ^= *string++;
		hash *= 1099511628211;
	}

	// Hash must not return 0
	return hash?hash:1;
}

static inline int rh_string_eq(const char *a, const char *b) {
	return !strcmp(a, b);
}
		
#define rh_hash_size(SIZE) (1 << SIZE)
#define rh_hash_slot(HASH, SIZE) (HASH & ~(~((int64_t)0) << SIZE))
#define RH_SLOT_DIST(SLOT, POS, SIZE) (SLOT>POS?POS+rh_hash_size(SIZE)-SLOT:POS-SLOT)

static inline rh_hash rh_hash_new(size_t size) {
	return (rh_hash) {
		.size = size,
		//.no_items = 0,
		.items = calloc(sizeof(struct rh_hash_bucket), rh_hash_size(size)),
	};
}

static inline struct rh_hash_bucket *rh_hash_find(rh_hash *map, const char *key) {
	if (!map || !map->items || !key) {
		return NULL;
	}

	int64_t hash = rh_string_hash(key);
	int64_t slot = rh_hash_slot(hash, map->size);

	int64_t i = slot;
	while (RH_SLOT_DIST(slot, i, map->size) <= RH_SLOT_DIST(rh_hash_slot(map->items[i].hash, map->size), i, map->size)) {
		if (map->items[i].hash == hash
		&& rh_string_eq(map->items[i].key, key)) {
			return &map->items[i];
		}
		i = (i+1) & ~(~((int64_t)0) << map->size);
	}

	return NULL;
}

static inline struct rh_hash_bucket rh_hash_remove(rh_hash *map, const char *key) {
	if (!map || !map->items || !key) {
		return (struct rh_hash_bucket) {0};
	}

	struct rh_hash_bucket *found_at = rh_hash_find(map, key);
	if (!found_at) {
		return (struct rh_hash_bucket) {0};
	}
	struct rh_hash_bucket ret = *found_at;

	int64_t i = found_at - map->items;
	int64_t prev = i;
	i = (i+1) & ~(~((int64_t)0) << map->size);
	while (map->items[i].hash && RH_SLOT_DIST(rh_hash_slot(map->items[i].hash, map->size), i, map->size) > 0) {
		map->items[prev] = map->items[i];
		prev = i;
		i = (i+1) & ~(~((int64_t)0) << map->size);
	}
	map->items[prev] = (struct rh_hash_bucket) {0};

	return ret;
}


static inline struct rh_hash_bucket rh_hash_set(rh_hash *map, const char *key, const char *value) {
	int64_t hash = rh_string_hash(key);

	struct rh_hash_bucket ins = {
		.hash = hash,
		.key = key,
		.value = value,
	};

	if (!map || !map->items || !key) {
		return ins;
	}

	int64_t i = rh_hash_slot(ins.hash, map->size);
	while (ins.hash) {
		int64_t slot = rh_hash_slot(ins.hash, map->size);
		while (map->items[i].hash && RH_SLOT_DIST(slot, i, map->size) <= RH_SLOT_DIST(rh_hash_slot(map->items[i].hash, map->size), i, map->size)) {
			if (map->items[i].hash == ins.hash
			&& rh_string_eq(map->items[i].key, ins.key)) {
				struct rh_hash_bucket swap = map->items[i];
				map->items[i] = ins;
				ins = swap;
				return ins;
			}
			i = (i+1) & ~(~((int64_t)0) << map->size);
		}
		struct rh_hash_bucket swap = map->items[i];
		map->items[i] = ins;
		ins = swap;
	}

	return (struct rh_hash_bucket) {0};
}
#endif
