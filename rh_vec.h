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
#ifndef RH_VEC_H
#define RH_VEC_H

#define RH_IMPL_SIZED_VEC(NAME, SIZE, TYPE)					\
static inline NAME NAME##_add(const NAME a, const NAME b) {			\
	NAME c;									\
	for (int i = 0;i < SIZE;++i) {						\
		c.array[i] = a->array[i] + b->array[i];				\
	}									\
	return c;								\
}										\
static inline NAME NAME##_sub(const NAME a, const NAME b) {			\
	NAME c;									\
	for (int i = 0;i < SIZE;++i) {						\
		c.array[i] = a->array[i] - b->array[i];				\
	}									\
	return c;								\
}										\
static inline int NAME##_eq(const NAME a, const NAME b) {			\
	for (int i = 0;i < SIZE;++i) {						\
		if (a.array[i] != b.array[i]) {					\
			return 0;						\
		}								\
	}									\
	return 1;								\
}										\
static inline NAME NAME##_scale(const NAME a, const TYPE factor) {		\
	NAME b;									\
	for (int i = 0;i < SIZE;++i) {						\
		b.array[i] = a->array[i] * factor;				\
	}									\
	return b;								\
}										\
static inline TYPE NAME##_dot(const NAME a, const NAME b) {			\
	TYPE dot = 0;								\
	for (int i = 0;i < SIZE;++i) {						\
		dot += a->array[i] * b->array[i];				\
	}									\
	return dot;								\
}										\
static inline TYPE NAME##_sum(const NAME a) {					\
	TYPE sum = 0;								\
	for (int i = 0;i < SIZE;++i) {						\
		sum += a->array[i];						\
	}									\
	return sum;								\
}										\

#define RH_IMPL_3_VEC(NAME, TYPE)						\
static inline NAME NAME##_cross(const NAME a, const NAME b) {			\
	TYPE x = a->y*b->z - a->z*b->y;						\
	TYPE y = a->z*b->x - a->x*b->z;						\
	TYPE z = a->x*b->y - a->y*b->x;						\
	return (NAME){{x, y, z}};						\
}										\

#define RH_VEC_DEF(TYPE)				\
	typedef union {					\
		TYPE array[2];				\
		struct {				\
			TYPE x;				\
			TYPE y;				\
		};					\
	} v2##TYPE;					\
							\
							\
	typedef union {					\
		TYPE array[3];				\
		struct {				\
			TYPE x;				\
			TYPE y;				\
			TYPE z;				\
		};					\
		struct {				\
			TYPE r;				\
			TYPE g;				\
			TYPE b;				\
		};					\
		struct {				\
			TYPE s;				\
			TYPE t;				\
			TYPE p;				\
		};					\
		struct {				\
			vec2(TYPE) vec2;		\
			TYPE _;				\
		};					\
	} v3##TYPE;					\
							\
	typedef union {					\
		TYPE array[4];				\
		struct {				\
			TYPE x;				\
			TYPE y;				\
			TYPE z;				\
			TYPE w;				\
		};					\
		struct {				\
			TYPE r;				\
			TYPE g;				\
			TYPE b;				\
			TYPE a;				\
		};					\
		struct {				\
			TYPE s;				\
			TYPE t;				\
			TYPE p;				\
			TYPE q;				\
		};					\
		struct {				\
			vec3(TYPE) vec3;		\
			TYPE _;				\
		};					\
	} v4##TYPE;					\

#define RH_VEC_IMPL(TYPE)				\
	RH_IMPL_SIZED_VEC(v2##TYPE, 2, TYPE);		\
	RH_IMPL_SIZED_VEC(v3##TYPE, 3, TYPE);		\
	RH_IMPL_SIZED_VEC(v4##TYPE, 4, TYPE);		\
	RH_IMPL_3_VEC(v3##TYPE, TYPE);

#define RH_VEC_MAKE(TYPE)					\
	RH_VEC_DEF(TYPE);					\
	RH_VEC_IMPL(TYPE);

#endif
