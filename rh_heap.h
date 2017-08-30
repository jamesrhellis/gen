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


#ifndef RH_HEAP_H
#define RH_HEAP_H

#include <stdlib.h>

#define RH_HEAP_MAKE(NAME, TYPE, CMP)				 		\
	RH_HEAP_DEF(NAME, TYPE);						\
	RH_HEAP_IMPL(NAME, TYPE, CMP);

#define RH_HEAP_DEF(NAME, TYPE) 						\
typedef struct {								\
	size_t size;								\
	size_t top;								\
										\
	TYPE *items;								\
} NAME;										\

#define RH_HEAP_IMPL(NAME, TYPE, CMP)						\
static inline size_t NAME##_resize(NAME *hp, size_t to) {			\
	if (hp->top > to) {							\
		return 0;							\
	}									\
										\
	TYPE *new = realloc(hp->items, to * sizeof(TYPE));			\
	if (!new) {								\
		return 0;							\
	}									\
										\
	hp->items = new;							\
	hp->size = to;								\
										\
	return hp->size;							\
}										\
										\
static inline NAME NAME##_new(size_t size) {					\
	NAME ret = {0};								\
	NAME##_resize(&ret, size);						\
	return ret;								\
}										\
										\
static inline TYPE NAME##_peek(NAME *hp) {					\
	return hp->top ? hp->items[0] : (TYPE) {0};				\
}										\
										\
static inline int NAME##_ins(NAME *hp, TYPE ins) {				\
	if (hp->top == hp->size							\
	&& !NAME##_resize(hp, (hp->size?:1) * 2)) {				\
		return 0;							\
	}									\
										\
	size_t i = hp->top++;							\
	while (i && CMP(ins, hp->items[(i-1) >> 1]) < 0) {			\
		hp->items[i] = hp->items[(i - 1) >> 1];				\
		i = (i - 1) >> 1;						\
	}									\
	hp->items[i] = ins;							\
										\
	return 1;								\
}										\
										\
static inline TYPE NAME##_rem(NAME *hp) {					\
	if (!hp->top) {								\
		return (TYPE) {0};						\
	}									\
										\
	TYPE ret = hp->items[0];						\
	TYPE rep = hp->items[hp->top--];					\
	size_t i = 0;								\
										\
	while (1) {								\
		size_t n;							\
		if ((n = (i << 1) + 1) <= hp->top 				\
				&& CMP(rep, hp->items[n]) > 0) {		\
			hp->items[i] = hp->items[n];				\
			i = n;							\
			continue;						\
		}								\
		if ((n = (i << 1) + 2) <= hp->top 				\
				&& CMP(rep, hp->items[n]) > 0) {		\
			hp->items[i] = hp->items[n];				\
			i = n;							\
			continue;						\
		}								\
		break;								\
	}									\
	hp->items[i] = rep;							\
										\
	return 1;								\
}

#endif
