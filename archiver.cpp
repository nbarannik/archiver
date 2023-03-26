#include "arg_parser.h"
#include "encode.h"
#include "decode.h"

void RunEncoding(std::string_view archive_name, const std::vector<std::string>& file_names) {
    OutBinaryStream out(archive_name);
    size_t compressed_count = 0;

    for (const auto& file : file_names) {
        InBinaryStream in(file);
        Encode(in, out, (compressed_count + 1 == file_names.size()));
        ++compressed_count;
    }
}

void RunDecoding(std::string_view archive_name) {
    InBinaryStream in(archive_name);
    Decode(in);
}

void RunHelp() {
    std::cout << "Available commands:\n"
              << "archiver -c archive_name file1 [file2 ...]\n"
              << "archiver -d archive_name\n"
              << "archiver -h\n";
}

int main(int argc, char** argv) {
    ArgValues value_compress("-c");
    ArgValues value_decompress("-d");
    ArgValues value_help("-h");

    std::vector<Arg> args = {
        {value_compress.flag, ArgType::OPTIONAL, ArgMultiple::MULTIPLE, value_compress.parsed_args,
         value_compress.is_parsed},
        {value_decompress.flag, ArgType::OPTIONAL, ArgMultiple::ONE, value_decompress.parsed_args,
         value_decompress.is_parsed},
        {value_help.flag, ArgType::OPTIONAL, ArgMultiple::ZERO, value_help.parsed_args, value_help.is_parsed}};

    try {
        ArgsParser parser(argc, argv, args);
        parser.ParsArgs();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 111;
    }

    if (value_compress.is_parsed) {
        if (value_compress.parsed_args.size() < 2) {
            std::cout << "No required arguments in command" << std::endl;
            return 111;
        }

        std::string archive_name = value_compress.parsed_args[0];
        value_compress.parsed_args.erase(value_compress.parsed_args.begin());
        try {
            RunEncoding(archive_name, value_compress.parsed_args);
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
            return 111;
        }
    } else if (value_decompress.is_parsed) {
        if (value_decompress.parsed_args.empty()) {
            std::cout << "No required arguments in command";
            return 111;
        }

        try {
            RunDecoding(value_decompress.parsed_args[0]);
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
            return 111;
        }
    } else if (value_help.is_parsed) {
        RunHelp();
    }

    return 0;
}
