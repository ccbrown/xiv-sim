#include "applications.h"

#include <stdio.h>
#include <cstring>

int main(int argc, const char* argv[]) {
	if (argc < 2) {
		printf("Usage: %s [single-html|multi-iteration]\n", argv[0]);
		return 1;
	}
	
	if (!strcmp(argv[1], "single-html")) {
		return applications::SingleHTML(argc - 2, argv + 2);
	} else if (!strcmp(argv[1], "multi-iteration")) {
		return applications::MultiIteration(argc - 2, argv + 2);
	}

	return 0;
}