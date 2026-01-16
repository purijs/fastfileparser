#pragma once

#include <cstddef>
#include <optional>
#include <string_view>
#include <unordered_set>

class CSVColumnScanner {
    private:
        const char* data_start;
        const char* data_end;
        int target_column;
        const char* current_row;
        bool drop_duplicates = false;
        int dedup_column;
        std::unordered_set<std::string_view> duplicate_rows;

        struct ParsedRow {
            const char* row_start;
            const char* row_end;
            const char* cell_start;
            const char* cell_end;
            bool found;
        };

        ParsedRow parse_next_row();
        std::string_view extract_cell(const char* row_start, const char* row_end, int column);

    public:
        void init(char* data, size_t size, int col_idx);
        std::optional<std::string_view> iterate_row_by_column();
        std::optional<std::string_view> filter_row_by_column(const std::string& filter);
        std::double_t sum();
        std::double_t groupby_operation_sum(const std::string& filter, int target_column);
        void enable_drop_duplicates(int dedup_column);
};
