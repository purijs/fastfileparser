#include "mmap_manager.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>

void* MMapManager::open_read(const char* filepath, size_t* out_size) {
    fd = open(filepath, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return nullptr;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Error getting file size");
        ::close(fd);
        return nullptr;
    }
    *out_size = sb.st_size;
    void* addr = mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        perror("Error mapping file");
        ::close(fd);
        return nullptr;
    }

    return addr;
};

void MMapManager::close(void* addr, size_t size) {
    if (munmap(addr, size) == -1) {
        perror("Error unmapping memory");
    }
    ::close(fd);
};

void MMapManager::advice_seq(void* addr, size_t size) {
    if (madvise(addr, size, MADV_SEQUENTIAL) == -1) {
        perror("Error giving advice for sequential access");
    }
}