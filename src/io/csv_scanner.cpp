#include "csv_scanner.h"
#include <cstring>
#include <iostream>
#include <string_view>
#include <optional>

void CSVColumnScanner::init(char* data, size_t size, int col_idx) {
    data_start = data;
    data_end = data + size;
    target_column = col_idx;

    const char* header = (const char*)memchr(data, '\n', size);
    current_row = header ? header + 1 : data;

    drop_duplicates = false;
    duplicate_rows.clear();
}

std::double_t CSVColumnScanner::sum() {
    double sum = 0.0;
    while (auto value_opt = CSVColumnScanner::iterate_row_by_column()) {
        try {
            sum += std::stod(std::string(*value_opt));
        } catch (const std::invalid_argument& e) {
            continue;
        } catch (const std::out_of_range& e) {
            continue;
        }
    }
    return sum;
}

void CSVColumnScanner::enable_drop_duplicates(int dedup_column) {
    this->dedup_column = dedup_column;
    drop_duplicates = true;
    duplicate_rows.clear();
}

std::string_view CSVColumnScanner::extract_cell(const char* row_start, const char* row_end, int column_index) {
    const char* col_ptr = row_start;
    int current_col = 0;

    while (current_col < column_index && col_ptr < row_end) {
        const char* comma = (const char*)memchr(col_ptr, ',', row_end - col_ptr);
        if (!comma) return std::string_view();
        col_ptr = comma + 1;
        current_col++;
    }

    if (current_col == column_index) {
        const char* cell_end = (const char*)memchr(col_ptr, ',', row_end - col_ptr);
        if (!cell_end) cell_end = row_end;
        return std::string_view(col_ptr, cell_end - col_ptr);
    }

    return std::string_view();
}

CSVColumnScanner::ParsedRow CSVColumnScanner::parse_next_row() {
    ParsedRow result = {nullptr, nullptr, nullptr, nullptr, false};
    while (current_row < data_end) {
        result.row_start = current_row;
        result.row_end = (const char*)memchr(current_row, '\n', data_end - current_row);
        if (!result.row_end) result.row_end = data_end;
        current_row = result.row_end < data_end ? result.row_end + 1 : data_end;

        if (drop_duplicates) [[unlikely]] {
            std::string_view dedup_value = extract_cell(result.row_start, result.row_end, dedup_column);

            if (!dedup_value.empty()) {
                if (!duplicate_rows.insert(dedup_value).second) {
                    continue;
                }
            }
        }

        const char* col_ptr = result.row_start;
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
            return result;
        }
    }

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
            std::string_view cell_value(row.cell_start, row.cell_end - row.cell_start);

            if (cell_value.find(filter) != std::string_view::npos) {
                return std::string_view(row.row_start, row.row_end - row.row_start);
            }
        }
    }

    return std::nullopt;
}

std::double_t CSVColumnScanner::groupby_operation_sum(const std::string& filter, int target_column) {

    double sum = 0.0;
    while (auto row = filter_row_by_column(filter)) {
        if (row != std::nullopt) {
            const char* col_ptr = row->data();
            const char* row_end = col_ptr + row->size();
            int current_col = 0;

            // Navigate to target column
            while (current_col < target_column && col_ptr < row_end) {
                const char* comma = (const char*)memchr(col_ptr, ',', row_end - col_ptr);
                if (!comma) break;
                col_ptr = comma + 1;
                current_col++;
            }

            if (current_col == target_column) {
                const char* cell_end = (const char*)memchr(col_ptr, ',', row_end - col_ptr);
                if (!cell_end) cell_end = row_end;

                std::string_view numeric_cell(col_ptr, cell_end - col_ptr);
                try {
                    sum += std::stod(std::string(numeric_cell));
                } catch (...) {
                    continue;
                }
            }

        }
    }

    return sum;
}