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
#ifndef RH_MAT_H
#define RH_MAT_H

#define RH_IMPL_SIZED_MAT(NAME, SIZE, TYPE)					\
static inline NAME NAME##_add(const NAME a, const NAME b) {			\
	NAME c;									\
	for (int i = 0;i < SIZE * SIZE;++i) {						\
		c.flat[i] = a.flat[i] + b.flat[i];				\
	}									\
	return c;								\
}										\
static inline NAME NAME##_sub(const NAME a, const NAME b) {			\
	NAME c;									\
	for (int i = 0;i < SIZE * SIZE;++i) {					\
		c.flat[i] = a.flat[i] - b.flat[i];				\
	}									\
	return c;								\
}										\
static inline int NAME##_eq(const NAME a, const NAME b) {			\
	for (int i = 0;i < SIZE * SIZE;++i) {					\
		if (a.flat[i] != b.flat[i]) {					\
			return 0;						\
		}								\
	}									\
	return 1;								\
}										\
static inline NAME NAME##_scale(const NAME a, const TYPE factor) {		\
	NAME b;									\
	for (int i = 0;i < SIZE * SIZE;++i) {					\
		b.flat[i] = a.flat[i] * factor;				\
	}									\
	return b;								\
}										\
static inline NAME NAME##_mul(const NAME a, const NAME b) {			\
	NAME c;									\
	for (int x = 0;x < SIZE;++x) {						\
	for (int y = 0;y < SIZE;++y) {						\
		TYPE acc = 0;							\
		for (int i = 0;i < SIZE;++i) {					\
			acc += a.array[y][i] * b.array[i][x];			\
		}								\
		c.array[x][y] = acc;						\
	}									\
	}									\
	return c;								\
}										\
static inline TYPE NAME##_sum(const NAME a) {					\
	TYPE sum = 0;								\
	for (int i = 0;i < SIZE * SIZE;++i) {					\
		sum += a.flat[i];						\
	}									\
	return sum;								\
}										\

#define RH_MAT_DEF(TYPE)				\
	typedef union {					\
		TYPE array[2][2];			\
		TYPE flat[4];				\
	} m2##TYPE;					\
							\
	typedef union {					\
		TYPE array[3][3];			\
		TYPE flat[9];				\
	} m3##TYPE;					\
							\
	typedef union {					\
		TYPE array[4][4];			\
		TYPE flat[16];				\
	} m4##TYPE;

#define RH_MAT_IMPL(TYPE)				\
	RH_IMPL_SIZED_MAT(m2##TYPE, 2, TYPE);		\
	RH_IMPL_SIZED_MAT(m3##TYPE, 3, TYPE);		\
	RH_IMPL_SIZED_MAT(m4##TYPE, 4, TYPE);

#define RH_MAT_MAKE(TYPE)					\
	RH_MAT_DEF(TYPE);					\
	RH_MAT_IMPL(TYPE);

#endif
