#include "csv_scanner.h"
#include <cstring>

void CSVColumnScanner::init(char* data, size_t size, int col_idx) {
    data_start = data;
    data_end = data + size;
    target_column = col_idx;
    current_row = data_start;
}

bool CSVColumnScanner::next(const char** out_value, size_t* out_length) {
    if (current_row >= data_end) return false;
    const char* row_end = (const char*)memchr(current_row, '\n', data_end - current_row);

    if (!row_end) row_end = data_end;

    const char* col_ptr = current_row;
    int current_col = 0;

    while(current_col < target_column && col_ptr < row_end) {
        const char* next_comma = (const char*)memchr(col_ptr, ',', row_end - col_ptr);
        if (!next_comma) break;
        col_ptr = next_comma + 1;
        current_col++;
    }

    if (current_col == target_column) {
        const char* next_comma = (const char*)memchr(col_ptr, ',', row_end - col_ptr);
        const char* val_end = next_comma ? next_comma : row_end;

        if (val_end > col_ptr && *(val_end - 1) == '\r') {
            *out_length = (val_end - 1) - col_ptr;
        } else {
            *out_length = val_end - col_ptr;
        }

        *out_value = col_ptr;
        current_row = (row_end < data_end) ? row_end + 1 : data_end;
        return true;
    }

    // Move to the next row
    current_row = (row_end < data_end) ? row_end + 1 : data_end;
    return next(out_value, out_length);
}
