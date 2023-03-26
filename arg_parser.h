#include <string>
#include <string_view>
#include <vector>
#include <stdexcept>

struct ArgValues {
    std::string_view flag;
    std::vector<std::string> parsed_args;
    bool is_parsed = false;

    explicit ArgValues(std::string_view flag) : flag(flag) {
    }
};

enum class ArgType { REQUIRED, OPTIONAL };

enum class ArgMultiple { ZERO, ONE, MULTIPLE };

struct Arg {
    Arg(std::string_view flag, ArgType type, ArgMultiple multiple, std::vector<std::string>& parsed_args,
        bool& is_parsed)
        : flag(flag), type(type), multiple(multiple), parsed_args(parsed_args), is_parsed(is_parsed) {
    }

    std::string_view flag;
    ArgType type;
    ArgMultiple multiple;
    std::vector<std::string>& parsed_args;
    bool& is_parsed;
};

class ArgsParser {
public:
    ArgsParser(int argc, char** argv, std::vector<Arg>& args);
    void ParsArgs();

private:
    std::vector<std::string> argv_;
    int argc_;
    std::vector<Arg>& args_;
};