#include "arg_parser.h"

ArgsParser::ArgsParser(int argc, char** argv, std::vector<Arg>& args) : argc_(argc), args_(args) {
    for (size_t i = 0; i < argc_; ++i) {
        argv_.push_back(argv[i]);
    }
}

void ArgsParser::ParsArgs() {
    for (auto& arg : args_) {
        auto command_iterator = std::find(argv_.begin(), argv_.end(), arg.flag);
        if (command_iterator == argv_.end()) {
            if (arg.type == ArgType::REQUIRED) {
                throw std::runtime_error("out of range");
                ;
            }
        } else {
            std::vector<std::string> current_command;
            command_iterator++;

            for (; command_iterator != argv_.end(); ++command_iterator) {
                if ((*command_iterator)[0] != '-') {
                    current_command.push_back(*command_iterator);
                } else {
                    break;
                }
            }

            switch (arg.multiple) {
                case ArgMultiple::ZERO:
                    if (!current_command.empty()) {
                        throw std::runtime_error("wrong arguments");
                    }
                    break;
                case ArgMultiple::ONE:
                    if (current_command.size() != 1) {
                        throw std::runtime_error("wrong arguments");
                    }
                    break;
                case ArgMultiple::MULTIPLE:
                    break;
            }

            arg.is_parsed = true;
            arg.parsed_args = current_command;
        }
    }
}