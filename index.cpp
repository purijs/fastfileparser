#include <iostream>
#include <string_view>
#include <string>
#include <vector>
#include "src/storage/mmap_manager.h"
#include "src/io/csv_scanner.h"
#include <unordered_map>
#include <charconv>

enum ColumnType {
    COLUMN_TYPE_INT,
    COLUMN_TYPE_FLOAT,
    COLUMN_TYPE_STRING
};

ColumnType infer_type(std::string_view sample) {
    if (sample.empty()) {
        return COLUMN_TYPE_STRING;
    }

    bool has_dot = false;
    for (char c:sample) {
        if (c == '.') has_dot = true;
        else if (!isdigit(c) && c!='-') {
            return COLUMN_TYPE_STRING;
        }
    }

    return has_dot ? COLUMN_TYPE_FLOAT : COLUMN_TYPE_INT;
}

std::unordered_map<std::string, ColumnType> infer_csv_schema(const char* data, size_t file_size) {
   
    const char* line_start = data;
    const char* line_end = strchr(line_start, '\n');
    size_t header_len = line_end ? (line_end - line_start) : file_size;
    std::string_view header(line_start, header_len);
    
    std::vector<std::string> column_names;
    size_t start = 0;
    size_t end = header.find(",");

    while(end!=std::string_view::npos) {
        column_names.emplace_back(header.substr(start, end - start));
        start = end + 1;
        end = header.find(",", start);
    }
    column_names.emplace_back(header.substr(start));

    std::unordered_map<std::string, ColumnType> type_map;
    for (const auto& name:column_names) {
        type_map[name] = ColumnType::COLUMN_TYPE_INT;
    }

    const char* row_ptr = line_end ? (line_end + 1) : nullptr;
    for (int r=0; r<5 && row_ptr && row_ptr < data + file_size; ++r) {
        const char* next_line = strchr(row_ptr, '\n');
        std::string_view row(row_ptr, next_line ? (next_line - row_ptr) : (data + file_size - row_ptr));

        size_t c_start = 0, c_idx = 0;
        for (size_t c_end = row.find(","); c_idx < column_names.size(); ) {
            size_t len = (c_end == std::string_view::npos) ? (row.size() - c_start) : (c_end - c_start);
            std::string_view cell = row.substr(c_start, len);
            
            // Infer type and "update"
            ColumnType detected = infer_type(cell);
            if (detected > type_map[column_names[c_idx]]) {
                type_map[column_names[c_idx]] = detected;
            }

            if (c_end == std::string_view::npos) break;
            c_start = c_end + 1;
            c_end = row.find(",", c_start);
            c_idx++;
        }
        row_ptr = next_line ? next_line + 1 : nullptr;
    }

    return type_map;

}

int main() {
    const char* filepath = "/Users/jaskaransinghpuri/Documents/c++/fastfileparser/test.csv";

    MMapManager mmap_manager;
    size_t file_size;
    char* data = (char*)mmap_manager.open_read(filepath, &file_size);
    mmap_manager.advice_seq(data, file_size);

    std::unordered_map<std::string, ColumnType> schema = infer_csv_schema(data, file_size);
    std::cout << "Inferred Schema:\n";
    for (const auto& [col, type]: schema) {
        std::cout << "Column: " << col << "Type: ";
        switch(type) {
            case COLUMN_TYPE_INT:
                std::cout << "INT";
                break;
            case COLUMN_TYPE_FLOAT:
                std::cout << "FLOAT";
                break;
            case COLUMN_TYPE_STRING:
                std::cout << "STRING";
                break;
        }
        std::cout << "\n";
    }

    CSVColumnScanner scanner;
    scanner.init(data, file_size, 1);

    std::cout << "\nValues in column index 2:\n";

    double sum = 0.0;
    while (auto value_opt = scanner.iterate_row_by_column()) {
        try {
            sum += std::stod(std::string(*value_opt));
        } catch (const std::invalid_argument& e) {
            continue;
        } catch (const std::out_of_range& e) {
            continue;
        }
    }

    std::cout << "Sum of column 2: " << sum << "\n";
    scanner.init(data, file_size, 2);  // Reset scanner to column 2
    std::string filter = "jas";

    std::cout << "\nRows matching '" << filter << "':\n";
    while (auto row = scanner.filter_row_by_column(&filter)) {
        std::cout << *row << "\n";
    }

    mmap_manager.close(data, file_size);
    return 0;
}