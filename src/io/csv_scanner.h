#pragma once

#include <cstddef>
#include <optional>
#include <string_view>

class CSVColumnScanner {
    private:
        const char* data_start;
        const char* data_end;
        int target_column;
        const char* current_row;
    public:
        void init(char* data, size_t size, int col_idx);
        std::optional<std::string_view> iterate_row_by_column();
        std::optional<std::string_view> filter_row_by_column(std::string* filter);
};
