#include "applications.h"

#include <stdio.h>
#include <cstring>

int main(int argc, const char* argv[]) {
	if (argc < 2) {
		printf("Usage: %s [single-json|thorough-json]\n", argv[0]);
		return 1;
	}
	
	if (!strcmp(argv[1], "single-json")) {
		return applications::SingleJSON(argc - 2, argv + 2);
	} else if (!strcmp(argv[1], "thorough-json")) {
		return applications::ThoroughJSON(argc - 2, argv + 2);
	}

	return 0;
}