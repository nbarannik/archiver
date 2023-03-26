#include <string>

std::string_view GetFileName(std::string_view name) {
    for (int i = static_cast<int>(name.size()) - 1; i >= 0; --i) {
        if (name[i] == '/') {
            name = name.substr(i + 1, name.size() - i - 1);
            break;
        }
    }

    return name;
}