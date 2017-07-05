#ifndef RH_HASH_H
#define RH_HASH_H

#include "stdlib.h"
#include "string.h"

// Macros used in generic code
#define rh_hash_size(SIZE) (1 << SIZE)
#define RH_HASH_SLOT(HASH, SIZE) (HASH & ~(~((int64_t)0) << SIZE))
#define RH_SLOT_DIST(SLOT, POS, SIZE) (SLOT>POS?POS+rh_hash_size(SIZE)-SLOT:POS-SLOT)

// Helpful macros for generating maps with
#define rh_hash(KEY_T, VALUE_T) rh_hash_##KEY_T##_##VALUE_T
#define rh_make_hash(NAME, KEY_T, VALUE_T, HASH_F, EQ_F) 			\
	def_rh_hash(NAME, KEY_T, VALUE_T);					\
	impl_rh_hash(NAME, KEY_T, VALUE_T, HASH_F, EQ_F);			\

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

#define def_rh_hash(NAME, KEY_T, VALUE_T)					\
struct NAME##_bucket {								\
	int64_t hash;								\
										\
	KEY_T key;								\
	VALUE_T value;								\
};										\
										\
typedef struct {								\
	size_t size;								\
	struct rh_hash_bucket *items;						\
} NAME;										\

#define impl_rh_hash(NAME, KEY_T, VALUE_T, HASH_F, EQ_F)			\
static inline NAME NAME##_new(size_t size) {					\
	return (rh_hash) {							\
		.size = size,							\
		.items = calloc(sizeof(struct NAME##_bucket)			\
				, rh_hash_size(size)),				\
	};									\
}										\
										\
static inline struct NAME##_bucket *NAME##_find(NAME *map, KEY_T key) {		\
	if (!map || !map->items) {						\
		return NULL;							\
	}									\
										\
	int64_t hash = HASH_F(key);						\
	int64_t slot = RH_HASH_SLOT(hash, map->size);				\
										\
	int64_t i = slot;							\
	while (RH_SLOT_DIST(slot, i, map->size)					\
	<= RH_SLOT_DIST(RH_HASH_SLOT(map->items[i].hash, map->size) 		\
			, i, map->size)) {					\
		if (map->items[i].hash == hash					\
		&& EQ_F(map->items[i].key, key)) {				\
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
	struct rh_hash_bucket *found_at = NAME##_find(map, key);		\
	if (!found_at) {							\
		return (struct NAME##_bucket) {0};				\
	}									\
	struct NAME##_bucket ret = *found_at;					\
										\
	int64_t i = found_at - map->items;					\
	int64_t prev = i;							\
	i = (i+1) & ~(~((int64_t)0) << map->size);				\
	while (map->items[i].hash 						\
	&& RH_SLOT_DIST(RH_HASH_SLOT(map->items[i].hash, map->size)		\
			, i, map->size) > 0) {					\
		map->items[prev] = map->items[i];				\
		prev = i;							\
		i = (i+1) & ~(~((int64_t)0) << map->size);			\
	}									\
	map->items[prev] = (struct NAME##_bucket) {0};				\
										\
	return ret;								\
}										\
										\
										\
static inline struct NAME##_bucket 						\
		NAME##_set(NAME *map, KEY_T key, VALUE_T value) {		\
	int64_t hash = HASH_F(key);						\
										\
	struct NAME##_bucket ins = {						\
		.hash = hash,							\
		.key = key,							\
		.value = value,							\
	};									\
										\
	if (!map || !map->items) {						\
		return ins;							\
	}									\
										\
	int64_t i = RH_HASH_SLOT(ins.hash, map->size);				\
	while (ins.hash) {							\
		int64_t slot = RH_HASH_SLOT(ins.hash, map->size);		\
		while (map->items[i].hash					\
		&& RH_SLOT_DIST(slot, i, map->size) 				\
				<= RH_SLOT_DIST(RH_HASH_SLOT(map->items[i].hash, map->size)	\
						, i, map->size)) {		\
			if (map->items[i].hash == ins.hash			\
			&& EQ_F(map->items[i].key, ins.key)) {			\
				struct NAME##_bucket swap = map->items[i];	\
				map->items[i] = ins;				\
				ins = swap;					\
				return ins;					\
			}							\
			i = (i+1) & ~(~((int64_t)0) << map->size);		\
		}								\
		struct NAME##_bucket swap = map->items[i];			\
		map->items[i] = ins;						\
		ins = swap;							\
	}									\
										\
	return (struct NAME##_bucket) {0};					\
}
#endif
