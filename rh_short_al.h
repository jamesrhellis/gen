/*******************************************************************************
* Copyright 2017-2020 James RH Ellis
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

#ifndef RH_SHORT_AL_H
#define RH_SHORT_AL_H

#include <stdlib.h>
#include <string.h>

#define RH_SHORT_AL_MAKE(NAME, TYPE)					 	\
	RH_SHORT_AL_DEF(NAME, TYPE);						\
	RH_SHORT_AL_IMPL(NAME, TYPE);

// Useful iteration macro
#define rh_al_for(iter, al)							\
if (al.items)									\
	for (size_t _i = 0;iter = al.items[_i], _i < al.top;++_i)	 	\

#define RH_SHORT_AL_DEF(NAME, TYPE)						\
typedef struct {								\
	size_t top;								\
	union {									\
		TYPE *short_items[(sizeof(size_t) + sizeof(TYPE *)) / sizeof(TYPE)];\
		struct {							\
			size_t size;						\
			TYPE *items;						\
		};								\
	}									\
} NAME;										\

#define RH_SHORT_AL_IMPL(NAME, TYPE)						\
const size_t NAME##_short_no = (sizeof(size_t) + sizeof(TYPE *));		\
static inline size_t NAME##_resize(NAME *al, size_t to) {			\
	if (al->top <= NAME##_short_no) {					\
		if (to <= NAME##_short_no) {					\
			return 1;						\
		}								\
		TYPE *new = malloc(to * sizeof(TYPE));				\
		if (!new) {							\
			return 0;						\
		}								\
										\
		memcpy(new, al->short_items, al->top * sizeof(TYPE));		\
		al->items = new;						\
		al->size = to;							\
										\
		return al->size;						\
	}									\
										\
	if (to <= NAME##_short_no) {						\
		TYPE *old = al->items;						\
		memcpy(al->short_items, old, to * sizeof(TYPE));		\
		free(old);							\
		al->top = (al->top > to) ? to : al->top;			\
										\
		return to;							\
	}									\
										\
	TYPE *new = realloc(al->items, to * sizeof(TYPE));			\
	if (!new) {								\
		return 0;							\
	}									\
										\
	al->items = new;							\
	al->size = to;								\
										\
	return al->size;							\
}										\
										\
static inline void NAME##_free(NAME *al) {					\
	free(al->items);							\
}										\
										\
static inline NAME NAME##_new(size_t size) {					\
	NAME ret = {0};								\
	NAME##_resize(&ret, size);						\
	memset(ret.items, 0, size * sizeof(TYPE));				\
	return ret;								\
}										\
										\
static inline NAME NAME##_clone(NAME *al) {					\
	if (!al->items || !al->top) {						\
		return (NAME) {0};						\
	}									\
										\
	NAME ret = {0};								\
	NAME##_resize(&ret, al->size);						\
	ret.top = al->top;							\
	if (al->top >= NAME##_short_no) {					\
		memcpy(ret.short_items, al->short_items, al->top * sizeof(TYPE));\
	} else {								\
		memcpy(ret.items, al->items, al->top * sizeof(TYPE));		\
	}									\
	return ret;								\
}										\
										\
static inline TYPE *NAME##_peek(NAME *al) {					\
	if (!al->top) {								\
		return NULL;							\
	}									\
	if (al->top >= NAME##_short_no) {					\
		return &al->short_items[al->top - 1];				\
	} else {								\
		return &al->items[al->top - 1];					\
	}									\
}										\
										\
static inline int NAME##_push(NAME *al, TYPE push) {				\
	if (al->size < NAME##_short_no) {					\
		al->short_items[al->top++] = push;				\
		return 1;							\
	}									\
										\
	if (al->top != NAME##_short_no && al->top != al->size) {		\
		al->items[al->top++] = push;					\
		return 1;							\
	}									\
										\
	if (!NAME##_resize(al, al->top * 2)) {					\
		return 0;							\
	}									\
										\
	al->items[al->top++] = push;						\
	return 1;								\
										\
}										\
										\
static inline TYPE NAME##_pop(NAME *al) {					\
	if (!al->top) {								\
		return (TYPE) {0};						\
	}									\
										\
	if (al->top <= NAME##_short_no) {					\
		return al->short_items[--al->top];				\
	}									\
										\
	if (al->top > NAME##_short_no + 1) {					\
		return al->items[--al->top];					\
	}									\
										\
	TYPE ret = al->items[--al->top];					\
	NAME##_resize(al, al->top);						\
	return ret;								\
}										\
										\
static inline TYPE NAME##_view(NAME *al, size_t pos) {				\
	if (pos >= al->top) {							\
		return (TYPE) {0};						\
	}									\
										\
	if (al->top <= NAME##_short_no) {					\
		return al->short_items[pos];					\
	} else {								\
		return al->items[pos];						\
	}									\
}										\
										\
static inline TYPE NAME##_pick(NAME *al, size_t pos) {				\
	if (pos >= al->top) {							\
		return (TYPE) {0};						\
	}									\
										\
	if (al->top <= NAME##_short_no) {					\
		return al->short_items[(al->top - 1) - pos];			\
	} else {								\
		return al->items[(al->top - 1) - pos];				\
	}									\
}										\

#endif
