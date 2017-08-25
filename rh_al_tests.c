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

#include "rh_al.h"
#include <stdio.h>

RH_AL_MAKE(test_al, const char *);

void print_al(test_al *al) {
	puts("Array List Stats:");
	fprintf(stderr, "Size: %lu, Top: %lu, Allocted block: %s\n"
			, al->size, al->top, al->items?"True":"False");

	if (!al->items) {
		return;
	}
	puts("Array List items:");
	for (int i = 0;i < al->size;++i) {
		fprintf(stderr, "value:%s\n", al->items[i]?:"Null");
	}
}

#define DEBUG_PRINT(HASH) print_h(&HASH);
#define ERROR_MSG(MSG, ...) fprintf(stderr, MSG "\nAt line: %d\n", ##__VA_ARGS__, __LINE__); print_al(&al)

int push(void) {
	test_al al = test_al_new(2);
	char *val = "Success";
	test_al_push(&al, val);
	if (al.items[0] != val || al.top != 1) {
		ERROR_MSG("Push test FAILED!");
		return 1;
	}

	return 0;
}

int push_resize(void) {
	test_al al = test_al_new(1);
	char *val = "Success";
	test_al_push(&al, "Fail");
	test_al_push(&al, val);
	if (al.items[1] != val || al.top != 2) {
		ERROR_MSG("Push resize test FAILED!");
		return 1;
	}

	return 0;
}

int push_zero(void) {
	test_al al = {0};
	char *val = "Success";
	test_al_push(&al, val);
	if (al.items[0] != val || al.top != 1) {
		ERROR_MSG("Push resize test FAILED!");
		return 1;
	}

	return 0;
}

int main() {
	int no_errors = 0;

	// Push tests
	no_errors += push();
	no_errors += push_resize();
	no_errors += push_zero();

	if (no_errors) {
		fprintf(stderr, "\n\n\tTotal number of TESTS FAILED: %d\n", no_errors);
	}

	return no_errors;
}
