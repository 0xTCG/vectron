#include <cstring> // for strlen

const char* to_byte_arr(std::vector<std::pair<std::string, std::string>> pairs) {
    std::string flattened_pairs;
    
    for (const auto& pair : pairs)
        flattened_pairs += pair.first + pair.second;
    
    // Dynamically allocate memory for the flattened data
    char* result = new char[flattened_pairs.length() + 1]; // +1 for null terminator
    std::strcpy(result, flattened_pairs.c_str()); // Copy data to the allocated buffer
    
    return result;
}
