/*******************************************************************************
* Copyright 2017 James RH Ellis
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal 
* in the Software without restriction, including without limitation the rights 
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
* copies of the Software, and to permit persons to whom the Software is 
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all 
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
* SOFTWARE.
*******************************************************************************/

#ifndef RH_POOL_H
#define RH_POOL_H

#include <stdlib.h>

typedef struct rh_mem_link {
	struct rh_mem_link *link;
	unsigned char rest[];
} rh_mem_link;

static inline void *rh_pool_alloc(rh_mem_link **pool, void *alloc(size_t), size_t size) {
	if (!*pool) {
		return alloc(size);
	}

	rh_mem_link *mem = *pool;
	*pool = (*pool)->link;

	return (void *) mem;
}

static inline void rh_pool_free(rh_mem_link **pool, void *mem) {
	rh_mem_link *l = mem;
	l->link = *pool;
	*pool = l;
}

static inline void rh_pool_freeall(rh_mem_link **pool, void free(void *)) {
	while (*pool) {
		rh_mem_link *mem = *pool;
		*pool = (*pool)->link;
		free(mem);
	}
}

#define RH_SIZEOF(X) sizeof(X)

#define RH_SIZED_POOL_MAKE(NAME, SIZE, ALLOC, FREE)				\
	RH_SIZED_POOL_DEF(NAME);						\
	RH_SIZED_POOL_IMPL(NAME, SIZE, ALLOC, FREE);

#define RH_SIZED_POOL_DEF(NAME)							\
typedef rh_mem_link NAME##_pool;						\
extern NAME##_pool *NAME;

#define RH_SIZED_POOL_IMPL(NAME, SIZE, ALLOC, FREE)				\
NAME##_pool *NAME = NULL;							\
										\
static inline void *NAME##_alloc(void) {					\
	return rh_pool_alloc(&NAME, ALLOC, SIZE);				\
}										\
										\
static inline void NAME##_free(void *to_free) {					\
	if (!to_free) { return; }						\
	rh_pool_free(&NAME, to_free);						\
}										\
										\
static inline void NAME##_freeall(void) {					\
	rh_pool_freeall(&NAME, FREE);						\
}										\

#endif
