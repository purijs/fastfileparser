#pragma once

#include <cstddef>

class CSVColumnScanner {
    private:
        const char* data_start;
        const char* data_end;
        int target_column;
        const char* current_row;
    public:
        void init(char* data, size_t size, int col_idx);
        bool next(const char** out_value, size_t* out_length);
};