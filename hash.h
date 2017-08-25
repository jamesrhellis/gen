#ifndef RH_HASH_H
#define RH_HASH_H

#include "stdlib.h"
#include "string.h"

// Macros used in generic code
#define RH_SIZE(SIZE) ((size_t)1 << SIZE)
#define RH_SLOT(HASH, SIZE) (HASH & ~(~((int64_t)0) << SIZE))
#define RH_SLOT_DIST(SLOT, POS, SIZE) (SLOT>POS?POS+RH_SIZE(SIZE)-SLOT:POS-SLOT)

// Helpful macros for generating maps with
#define RH_MAKE_HASH(NAME, KEY_T, VALUE_T, HASH_F, EQ_F, LOAD)	 		\
	RH_DEF_HASH(NAME, KEY_T, VALUE_T);					\
	RH_IMPL_HASH(NAME, KEY_T, VALUE_T, HASH_F, EQ_F, LOAD);			\

// Useful functions for making hashmaps with strings
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


// NAME##bucket is returned by functions as the hashmap does not take ownership of 	
// any resources so both the key and value must be returned for management	

// Hash should be size_t, but this would require Arch 
// dependent hashing algorithms	
// 0 is reserved for no-item case -- This is only useful if
// the key type cannot be optional, e.g. int, char etc

// mo_items is the number of items which can be added before a re-size

#define RH_DEF_HASH(NAME, KEY_T, VALUE_T)					\
typedef struct NAME##_bucket {							\
	KEY_T key;								\
	VALUE_T value;								\
} NAME##_bucket;								\
										\
typedef struct {								\
	size_t size;								\
	size_t no_items;							\
										\
	int64_t *hash;								\
	struct NAME##_bucket *items;						\
} NAME;										\

#define RH_IMPL_HASH(NAME, KEY_T, VALUE_T, HASH_F, EQ_F, LOAD)			\
static inline struct NAME##_bucket 						\
		NAME##_uset(NAME *map, int64_t hash, NAME##_bucket item) {	\
	int64_t i = RH_SLOT(hash, map->size);					\
	while (hash) {								\
		int64_t slot = RH_SLOT(hash, map->size);			\
		while (map->hash[i] && RH_SLOT_DIST(slot, i, map->size) 	\
				<= RH_SLOT_DIST(RH_SLOT(map->hash[i], map->size)\
						, i, map->size)) {		\
			/* Return old if item already in table */		\
			if (map->hash[i] == hash				\
			&& EQ_F(map->items[i].key, item.key)) {			\
				struct NAME##_bucket swap = map->items[i];	\
				map->items[i] = item;				\
				item = swap;					\
				return item;					\
			}							\
			i = (i+1) & ~(~((int64_t)0) << map->size);		\
		}								\
		struct NAME##_bucket swap = map->items[i];			\
		map->items[i] = item;						\
		item = swap;							\
		int64_t h_swap = map->hash[i];					\
		map->hash[i] = hash;						\
		hash = h_swap;							\
	}									\
	--map->no_items;							\
										\
	return (struct NAME##_bucket) {0};					\
}										\
										\
static inline int NAME##_resize(NAME *map, size_t to) {				\
	if (to <= map->size) {							\
		return 0;							\
	}									\
										\
	int64_t *hash = calloc(sizeof *hash, RH_SIZE(to));			\
	if (!hash) {								\
		return 0;							\
	}									\
										\
	NAME##_bucket *items = calloc(sizeof *items, RH_SIZE(to));		\
	if (!items) {								\
		free(hash);							\
		return 0;							\
	}									\
										\
	/* New map is used to avoid swapping later */				\
	NAME temp = {								\
		.size = to,							\
		/* Excess items will be removed while inserting */		\
		.no_items = (size_t)((RH_SIZE(to)) * LOAD),			\
		.hash = hash,							\
		.items = items,							\
	};									\
										\
	/* Allow for empty starting map */					\
	if (map->hash && map->items) {						\
		for (int i = 0;i < RH_SIZE(map->size);++i) {			\
			if (map->hash[i]) {					\
				/* Return can be ignored as impossible for */	\
				/* same item to be in old hash twice */		\
				NAME##_uset(&temp, map->hash[i], map->items[i]);\
			}							\
		}								\
	}									\
	free(map->hash);							\
	free(map->items);							\
										\
	*map = temp;								\
	return 1;								\
}										\
										\
static inline NAME NAME##_new(size_t size) {					\
	NAME ret = {0};								\
	NAME##_resize(&ret, size);						\
	return ret;								\
}										\
										\
static inline struct NAME##_bucket *NAME##_find(NAME *map, KEY_T key) {		\
	if (!map || !map->items) {						\
		return NULL;							\
	}									\
										\
	int64_t hash = HASH_F(key);						\
	int64_t slot = RH_SLOT(hash, map->size);				\
										\
	int64_t i = slot;							\
	while (RH_SLOT_DIST(slot, i, map->size)					\
	<= RH_SLOT_DIST(RH_SLOT(map->hash[i], map->size) 			\
			, i, map->size)) {					\
		if (map->hash[i] == hash && EQ_F(map->items[i].key, key)) {	\
			return &map->items[i];					\
		}								\
		i = (i+1) & ~(~((int64_t)0) << map->size);			\
	}									\
										\
	return NULL;								\
}										\
										\
static inline struct NAME##_bucket NAME##_remove(NAME *map, KEY_T key) {	\
	if (!map || !map->items) {						\
		return (struct NAME##_bucket) {0};				\
	}									\
										\
	struct NAME##_bucket *found_at = NAME##_find(map, key);			\
	if (!found_at) {							\
		return (struct NAME##_bucket) {0};				\
	}									\
	struct NAME##_bucket ret = *found_at;					\
										\
	int64_t i = found_at - map->items;					\
	int64_t prev = i;							\
	i = (i+1) & ~(~((int64_t)0) << map->size);				\
	while (map->hash[i]	 						\
	&& RH_SLOT_DIST(RH_SLOT(map->hash[i], map->size)			\
			, i, map->size) > 0) {					\
		map->hash[prev] = map->hash[i];					\
		map->items[prev] = map->items[i];				\
		prev = i;							\
		i = (i+1) & ~(~((int64_t)0) << map->size);			\
	}									\
	map->hash[prev] = 0;							\
	map->items[prev] = (struct NAME##_bucket) {0};				\
	++map->no_items;							\
										\
	return ret;								\
}										\
										\
static inline struct NAME##_bucket 						\
		NAME##_set(NAME *map, KEY_T key, VALUE_T value) {		\
	struct NAME##_bucket ins = {						\
		.key = key,							\
		.value = value,							\
	};									\
										\
	if (!map->no_items && !NAME##_resize(map, map->size+1)) {		\
		return ins;							\
	}									\
	if (!map || !map->items) {						\
		return ins;							\
	}									\
	int64_t hash = HASH_F(key);						\
										\
	return NAME##_uset(map, hash, ins);					\
}
#endif
