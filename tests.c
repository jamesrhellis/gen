#include "hash.h"
#include <stdio.h>

static inline int64_t fake_hash(const char *hash) {
	return *hash - '0';
}
	

RH_MAKE_HASH(test_map, const char *, const char *, fake_hash, rh_string_eq);

void print_h(test_map *h) {
	size_t s = RH_HASH_SIZE(h->size);
	puts("Hash table items:");
	for (int i = 0;i < s;++i) {
		fprintf(stderr, "hash:%lu slot:%lu key:%s value:%s\n", h->items[i].hash
				, RH_HASH_SLOT(h->items[i].hash, h->size)
				, h->items[i].key?:"Null", h->items[i].value?:"Null");
	}
}

#define DEBUG_PRINT(HASH) print_h(&HASH);
#define ERROR_MSG(MSG, ...) fprintf(stderr, MSG "\nAt line: %d\n", ##__VA_ARGS__, __LINE__); print_h(&h)

int insert(void) {
	test_map h = test_map_new(2);
	test_map_set(&h, "1", "Success");
	if (!rh_string_eq(h.items[1].value, "Success")) {
		ERROR_MSG("Insert test FAILED!");
		return 1;
	}

	return 0;
}

int ins_after_other(void) {
	test_map h = test_map_new(2);
	h.items[2] = (struct test_map_bucket) {
		.hash = 2,
		.key = "Fake",
		.value = "FAIL",
	};
	test_map_set(&h, "2", "Success");
	if (!rh_string_eq(h.items[3].value, "Success")) {
		ERROR_MSG("Insert after other FAILED!");
		return 1;
	}

	return 0;
}

int ins_after_smaller_other(void) {
	test_map h = test_map_new(2);
	h.items[2] = (struct test_map_bucket) {
		.hash = 1,
		.key = "Fake",
		.value = "FAIL",
	};
	test_map_set(&h, "2", "Success");
	if (!rh_string_eq(h.items[3].value, "Success")) {
		ERROR_MSG("Insert after smaller other FAILED!");
		return 1;
	};

	return 0;
}

int ins_after_other_over_boundry(void) {
	test_map h = test_map_new(2);
	h.items[3] = (struct test_map_bucket) {
		.hash = 3,
		.key = "Fake",
		.value = "FAIL",
	};
	test_map_set(&h, "3", "Success");
	if (!rh_string_eq(h.items[0].value, "Success")) {
		ERROR_MSG("Insert over boundry test FAILED!");
		return 1;
	}

	return 0;
}

int ins_already_set(void) {
	test_map h = test_map_new(2);
	h.items[2] = (struct test_map_bucket) {
		.hash = 2,
		.key = "2",
		.value = "FAIL",
	};
	struct test_map_bucket removed = test_map_set(&h, "2", "Success");
	if (!rh_string_eq(h.items[2].value, "Success")
	|| !rh_string_eq(removed.value, "FAIL")) {
		ERROR_MSG("Modify already present value FAILED!");
		return 1;
	}

	return 0;
}

int find(void) {
	test_map h = test_map_new(2);
	h.items[2] = (struct test_map_bucket) {
		.hash = 2,
		.key = "2",
		.value = "Success",
	};
	struct test_map_bucket *found = test_map_find(&h, "2");
	if (!found || !rh_string_eq(found->value, "Success")) {
		ERROR_MSG("Find test FAILED!");
		return 1;
	}

	return 0;
}

int find_after_other(void) {
	test_map h = test_map_new(2);
	h.items[2] = (struct test_map_bucket) {
		.hash = 2,
		.key = "22",
		.value = "FAIL",
	};
	h.items[3] = (struct test_map_bucket) {
		.hash = 2,
		.key = "2",
		.value = "Success",
	};
	struct test_map_bucket *found = test_map_find(&h, "2");
	if (!found || !rh_string_eq(found->value, "Success")) {
		ERROR_MSG("Find after other FAILED!");
		return 1;
	}

	return 0;
}

int find_after_smaller_other(void) {
	test_map h = test_map_new(2);
	h.items[2] = (struct test_map_bucket) {
		.hash = 1,
		.key = "1",
		.value = "FAIL",
	};
	h.items[3] = (struct test_map_bucket) {
		.hash = 2,
		.key = "2",
		.value = "Success",
	};
	struct test_map_bucket *found = test_map_find(&h, "2");
	if (!found || !rh_string_eq(found->value, "Success")) {
		ERROR_MSG("Find after smaller other FAILED!");
		return 1;
	}

	return 0;
}

int find_over_boundry(void) {
	test_map h = test_map_new(2);
	h.items[3] = (struct test_map_bucket) {
		.hash = 3,
		.key = "33",
		.value = "FAIL",
	};
	h.items[0] = (struct test_map_bucket) {
		.hash = 3,
		.key = "3",
		.value = "Success",
	};
	struct test_map_bucket *found = test_map_find(&h, "3");
	if (!found || !rh_string_eq(found->value, "Success")) {
		ERROR_MSG("Find over boundry FAILED!");
		return 1;
	}

	return 0;
}

int remove_test(void) {
	test_map h = test_map_new(2);
	h.items[2] = (struct test_map_bucket) {
		.hash = 2,
		.key = "2",
		.value = "Success",
	};
	struct test_map_bucket removed = test_map_remove(&h, "2");
	if (!rh_string_eq(removed.value, "Success") || h.items[2].hash) {
		ERROR_MSG("Remove test FAILED!");
		return 1;
	}

	return 0;
}

int remove_after_other(void) {
	test_map h = test_map_new(2);
	h.items[2] = (struct test_map_bucket) {
		.hash = 2,
		.key = "22",
		.value = "FAIL",
	};
	h.items[3] = (struct test_map_bucket) {
		.hash = 2,
		.key = "2",
		.value = "Success",
	};
	struct test_map_bucket removed = test_map_remove(&h, "2");
	if (!rh_string_eq(removed.value, "Success") 
			|| h.items[3].hash || !rh_string_eq(h.items[2].value, "FAIL")) {
		ERROR_MSG("Remove after other test FAILED!");
		return 1;
	}

	return 0;
}

int remove_after_smaller_other(void) {
	test_map h = test_map_new(2);
	h.items[2] = (struct test_map_bucket) {
		.hash = 1,
		.key = "11",
		.value = "FAIL",
	};
	h.items[3] = (struct test_map_bucket) {
		.hash = 2,
		.key = "2",
		.value = "Success",
	};
	struct test_map_bucket removed = test_map_remove(&h, "2");
	if (!rh_string_eq(removed.value, "Success") 
			|| h.items[3].hash || !rh_string_eq(h.items[2].value, "FAIL")) {
		ERROR_MSG("Remove after other test FAILED!");
		return 1;
	}

	return 0;
}

int remove_over_boundry(void) {
	test_map h = test_map_new(2);
	h.items[3] = (struct test_map_bucket) {
		.hash = 3,
		.key = "33",
		.value = "FAIL",
	};
	h.items[0] = (struct test_map_bucket) {
		.hash = 3,
		.key = "3",
		.value = "Success",
	};
	struct test_map_bucket removed = test_map_remove(&h, "3");
	if (!rh_string_eq(removed.value, "Success") 
			|| h.items[0].hash || !rh_string_eq(h.items[3].value, "FAIL")) {
		ERROR_MSG("Remove over boundry FAILED!");
		return 1;
	}

	return 0;
}

int remove_backtrack_other(void) {
	test_map h = test_map_new(2);
	h.items[2] = (struct test_map_bucket) {
		.hash = 2,
		.key = "22",
		.value = "FAIL",
	};
	h.items[3] = (struct test_map_bucket) {
		.hash = 2,
		.key = "2",
		.value = "Success",
	};
	test_map_remove(&h, "22");
	if (!rh_string_eq(h.items[2].value, "Success") || h.items[3].hash) {
		ERROR_MSG("Remove backtrack test FAILED!");
		return 1;
	}

	return 0;
}

int remove_backtrack_over_boundry(void) {
	test_map h = test_map_new(2);
	h.items[3] = (struct test_map_bucket) {
		.hash = 3,
		.key = "33",
		.value = "FAIL",
	};
	h.items[0] = (struct test_map_bucket) {
		.hash = 3,
		.key = "3",
		.value = "Success",
	};
	test_map_remove(&h, "33");
	if (!rh_string_eq(h.items[3].value, "Success") || h.items[0].hash) {
		ERROR_MSG("Remove backtrack over boundry FAILED!");
		return 1;
	}

	return 0;
}

int remove_backtrack_optimal(void) {
	test_map h = test_map_new(2);
	h.items[2] = (struct test_map_bucket) {
		.hash = 2,
		.key = "22",
		.value = "FAIL",
	};
	h.items[3] = (struct test_map_bucket) {
		.hash = 3,
		.key = "3",
		.value = "Success",
	};
	test_map_remove(&h, "22");
	if (h.items[2].hash || !h.items[3].hash 
			|| !rh_string_eq(h.items[3].value, "Success")) {
		ERROR_MSG("Remove backtrack over boundry FAILED!");
		return 1;
	}

	return 0;
}

int remove_backtrack_over_boundry_optimal(void) {
	test_map h = test_map_new(2);
	h.items[3] = (struct test_map_bucket) {
		.hash = 3,
		.key = "33",
		.value = "FAIL",
	};
	h.items[0] = (struct test_map_bucket) {
		.hash = 4,
		.key = "4",
		.value = "Success",
	};
	test_map_remove(&h, "33");
	if (h.items[3].hash || !h.items[0].hash 
			|| !rh_string_eq(h.items[0].value, "Success")) {
		ERROR_MSG("Remove backtrack over boundry FAILED!");
		return 1;
	}

	return 0;
}

int remove_backtrack_2(void) {
	test_map h = test_map_new(2);
	h.items[1] = (struct test_map_bucket) {
		.hash = 4,
		.key = "44",
		.value = "FAIL4",
	};
	h.items[2] = (struct test_map_bucket) {
		.hash = 1,
		.key = "11",
		.value = "FAIL1",
	};
	h.items[3] = (struct test_map_bucket) {
		.hash = 2,
		.key = "2",
		.value = "Success",
	};
	test_map_remove(&h, "44");
	if (h.items[3].hash || !h.items[2].hash 
			|| !rh_string_eq(h.items[2].value, "Success")) {
		ERROR_MSG("Remove backtrack over boundry FAILED!");
		return 1;
	}

	return 0;
}

int main() {
	int no_errors = 0;

	// Insertion tests
	no_errors += insert();
	no_errors += ins_after_other();
	no_errors += ins_after_smaller_other();
	no_errors += ins_after_other_over_boundry();
	no_errors += ins_already_set();

	// Find tests
	no_errors += find();
	no_errors += find_after_other();
	no_errors += find_after_smaller_other();
	no_errors += find_over_boundry();

	// Remove tests
	no_errors += remove_test();
	no_errors += remove_after_other();
	no_errors += remove_after_smaller_other();
	no_errors += remove_over_boundry();
	no_errors += remove_backtrack_other();
	no_errors += remove_backtrack_over_boundry();
	no_errors += remove_backtrack_optimal();
	no_errors += remove_backtrack_over_boundry_optimal();
	no_errors += remove_backtrack_2();

	if (no_errors) {
		fprintf(stderr, "\n\n\tTotal number of TESTS FAILED: %d\n", no_errors);
	}

	return no_errors;
}
