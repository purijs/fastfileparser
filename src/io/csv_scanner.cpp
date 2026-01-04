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

CSVColumnScanner::ParsedRow CSVColumnScanner::parse_next_row() {
    ParsedRow result = {nullptr, nullptr, nullptr, nullptr, false};
    if (current_row >= data_end) return result;

    result.row_start = current_row;
    result.row_end = (const char*)memchr(current_row, '\n', data_end - current_row);
    if (!result.row_end) result.row_end = data_end;

    const char* col_ptr = current_row;
    int current_col = 0;

    // Navigate to target column
    while (current_col < target_column && col_ptr < result.row_end) {
        const char* comma = (const char*)memchr(col_ptr, ',', result.row_end - col_ptr);
        if (!comma) break;
        col_ptr = comma + 1;
        current_col++;
    }

    if (current_col == target_column) {
        result.cell_start = col_ptr;
        result.cell_end = (const char*)memchr(col_ptr, ',', result.row_end - col_ptr);
        if (!result.cell_end) result.cell_end = result.row_end;
        result.found = true;
    }
    current_row = (result.row_end < data_end) ? result.row_end + 1 : data_end;
    return result;
}

std::optional<std::string_view> CSVColumnScanner::iterate_row_by_column() {
    while (current_row < data_end) {
        ParsedRow row = parse_next_row();
        if (row.found) {
            return std::string_view(row.cell_start, row.cell_end - row.cell_start);
        }
    }

    return std::nullopt;
}

std::optional<std::string_view> CSVColumnScanner::filter_row_by_column(const std::string& filter) {

    while (current_row < data_end) {
        ParsedRow row = parse_next_row();
        if (row.found) {
            std::string cell_value(row.cell_start, row.cell_end - row.cell_start);

            if (cell_value.find(filter) != std::string_view::npos) {
                return std::string_view(row.row_start, row.row_end - row.row_start);
            }
        }
    }

    return std::nullopt;
}
