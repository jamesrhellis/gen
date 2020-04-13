/*******************************************************************************
* Copyright 2020 James RH Ellis
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

#ifndef RH_DEQ_H
#define RH_DEQ_H

#include <stdlib.h>
#include <string.h>

#define RH_DEQ_MAKE(NAME, TYPE)					 		\
	RH_DEQ_DEF(NAME, TYPE);							\
	RH_DEQ_IMPL(NAME, TYPE);

// Useful iteration macro
#define rh_deq_for(iter, deq)							\
if (deq.items)									\
	for (size_t _i = deq.end;						\
		iter = deq.items[_i], _i != deq.start;				\
		_i = (_i - 1) % deq.size)	 				\

#define RH_DEQ_DEF(NAME, TYPE) 							\
typedef struct {								\
	size_t size;								\
	size_t start, end;							\
										\
	TYPE *items;								\
} NAME;										\

#define RH_DEQ_IMPL(NAME, TYPE)							\
static inline size_t NAME##_resize(NAME *deq, size_t to) {			\
	if (!to || (deq->size && (deq->start - deq->end) % deq->size > to)) {	\
		return 0;							\
	}									\
										\
	TYPE *new = realloc(deq->items, to * sizeof(TYPE));			\
	if (!new) {								\
		return 0;							\
	}									\
										\
	if (deq->start < deq->end) {						\
		size_t end_size = deq->size - deq->end;				\
		memmove(&new[deq->end],						\
			&new[to - end_size],					\
			end_size * sizeof(TYPE));				\
		deq->end = end_size;						\
	}									\
										\
	deq->items = new;							\
	deq->size = to;								\
										\
	return deq->size;							\
}										\
										\
static inline void NAME##_free(NAME *deq) {					\
	free(deq->items);							\
}										\
										\
static inline NAME NAME##_new(size_t size) {					\
	NAME ret = {0};								\
	NAME##_resize(&ret, size);						\
	memset(ret.items, 0, size);						\
	return ret;								\
}										\
										\
static inline NAME NAME##_clone(NAME *deq) {					\
	if (!deq->items || deq->start == deq->end) {				\
		return (NAME) {0};						\
	}									\
										\
	NAME ret = {0};								\
	NAME##_resize(&ret, deq->size);						\
	memcpy(ret.items, deq->items, deq->size * sizeof(TYPE));		\
	return ret;								\
}										\
										\
static inline TYPE *NAME##_peek(NAME *deq) {					\
	if (deq->start == deq->end) {						\
		return NULL;							\
	}									\
	return &deq->items[deq->end -1];					\
}										\
										\
static inline TYPE *NAME##_rpeek(NAME *deq) {					\
	if (deq->start == deq->end) {						\
		return NULL;							\
	}									\
	return &deq->items[(deq->start - 1) % deq->size];			\
}										\
										\
static inline int NAME##_push(NAME *deq, TYPE push) {				\
	if ((!deq->size || (deq->start + 1) % deq->size == deq->end)		\
	&& !NAME##_resize(deq, (deq->size?:8) * 2)) {				\
		return 0;							\
	}									\
										\
	deq->items[deq->start] = push;						\
	deq->start = (deq->start + 1) % deq->size;				\
	return 1;								\
}										\
										\
static inline TYPE NAME##_pop(NAME *deq) {					\
	if (deq->start == deq->end) {						\
		return (TYPE) {0};						\
	}									\
										\
	deq->start = (deq->start - 1) % deq->size;				\
	return deq->items[deq->start];						\
}										\
										\
static inline int NAME##_rpush(NAME *deq, TYPE push) {				\
	if ((!deq->size || (deq->end - 1) % deq->size == deq->start)		\
	&& !NAME##_resize(deq, (deq->size?:1) * 2)) {				\
		return 0;							\
	}									\
										\
	deq->end = (deq->end - 1) % deq->size;					\
	deq->items[deq->end] = push;						\
	return 1;								\
}										\
										\
static inline TYPE NAME##_rpop(NAME *deq) {					\
	if (deq->start == deq->end) {						\
		return (TYPE) {0};						\
	}									\
										\
	TYPE pop = deq->items[deq->end];					\
	deq->end = (deq->end + 1) % deq->size;					\
	return pop;								\
}										\
										\
static inline TYPE NAME##_view(NAME *deq, size_t pos) {				\
	if (!deq->size || pos >= (deq->start - deq->end) % deq->size) {		\
		return (TYPE) {0};						\
	}									\
										\
	return deq->items[(deq->start + pos) % deq->size];			\
}										\

#endif
