#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "common.h"

size_t get_file_size(const char *filename)
{
	struct stat file_info;

	if (stat(filename, &file_info) < 0) {
		fprintf(stderr, "Could not stat %s: %s\n", filename, strerror(errno));
		exit(1);
	}
	return (size_t) file_info.st_size;
}

void map_file_region(const char *filename, size_t size, struct mmap_region *region)
{
	region->size = size;

	// Open the file
	region->fd = open(filename, O_RDONLY);
	if (region->fd < 0) {
		fprintf(stderr, "Could not open %s: %s\n", filename, strerror(errno));
		exit(1);
	}

	// Map the file into memory.
	region->addr = mmap(NULL, (size_t) size, PROT_READ, MAP_PRIVATE, region->fd, (off_t) 0);
	if (region->addr == MAP_FAILED) {
		fprintf(stderr, "Could not map %s: %s\n", filename, strerror(errno));
		exit(1);
	}
}

void unmap_file_region(struct mmap_region *region)
{
	munmap(region->addr, region->size);
	close(region->fd);
}
