#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

struct mmap_region {
	int fd;
	void *addr;
	size_t size;
};

size_t get_file_size(const char *filename);
void map_file_region(const char *filename, size_t size, struct mmap_region *region);
void unmap_file_region(struct mmap_region *region);

#endif // COMMON_H
