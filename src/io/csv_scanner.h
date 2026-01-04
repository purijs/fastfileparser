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

        struct ParsedRow {
            const char* row_start;
            const char* row_end;
            const char* cell_start;
            const char* cell_end;
            bool found;
        };

        ParsedRow parse_next_row();

    public:
        void init(char* data, size_t size, int col_idx);
        std::optional<std::string_view> iterate_row_by_column();
        std::optional<std::string_view> filter_row_by_column(const std::string& filter);
};
