#include "csv_scanner.h"
#include <cstring>
#include <iostream>
#include <string_view>
#include <optional>

void CSVColumnScanner::init(char* data, size_t size, int col_idx) {
    data_start = data;
    data_end = data + size;
    target_column = col_idx;
    current_row = data_start;
}

std::optional<std::string_view> CSVColumnScanner::iterate_row_by_column() {
    while (current_row < data_end) {
        const char* row_end = (const char*)memchr(current_row, '\n', data_end - current_row);
        if (!row_end) row_end = data_end;

        const char* col_ptr = current_row;
        int current_col = 0;

        while (current_col < target_column && col_ptr < row_end) {
            const char* comma = (const char*)memchr(col_ptr, ',', row_end - col_ptr);
            if (!comma) break;
            col_ptr = comma + 1;
            current_col++;
        }

        if (current_col == target_column) {
            const char* val_end = (const char*)memchr(col_ptr, ',', row_end - col_ptr);
            if (!val_end) val_end = row_end;

            current_row = (row_end < data_end) ? row_end + 1 : data_end;

            return std::string_view(col_ptr, val_end - col_ptr);
        }

        current_row = (row_end < data_end) ? row_end + 1 : data_end;
    }

    return std::nullopt;
}

std::optional<std::string_view> CSVColumnScanner::filter_row_by_column(std::string* filter) {

    while (current_row < data_end) {
        const char* row_start = current_row;  // Save row start for return
        const char* row_end = (const char*)memchr(current_row, '\n', data_end - current_row);
        if (!row_end) row_end = data_end;

        const char* col_ptr = current_row;
        int current_col = 0;

        // Navigate to target column
        while (current_col < target_column && col_ptr < row_end) {
            const char* comma = (const char*)memchr(col_ptr, ',', row_end - col_ptr);
            if (!comma) break;
            col_ptr = comma + 1;
            current_col++;
        }

        if (current_col == target_column) {
            const char* val_end = (const char*)memchr(col_ptr, ',', row_end - col_ptr);
            if (!val_end) val_end = row_end;

            std::string_view cell_value(col_ptr, val_end - col_ptr);

            if (cell_value == *filter) {
                current_row = (row_end < data_end) ? row_end + 1 : data_end;
                return std::string_view(row_start, row_end - row_start);
            }
        }

        current_row = (row_end < data_end) ? row_end + 1 : data_end;
    }

    return std::nullopt;
}
