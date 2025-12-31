#include <cstddef>

class MMapManager {
    public:
        void* open_read(const char* filepath, size_t* out_size);
        void close(void* addr, size_t size);

        void advice_seq(void* addr, size_t size);
    
    private:
        int fd;
};
