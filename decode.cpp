#include "decode.h"
#include <vector>

std::vector<std::pair<BinarySymbol, BinarySymbol>> GetDecodingTable(InBinaryStream& in) {
    BinarySymbol symbols_count(0, 9);
    in >> symbols_count;
    std::vector<std::pair<BinarySymbol, BinarySymbol>> decoding_table(symbols_count.symbol,
                                                                      {BinarySymbol(0, 9), BinarySymbol(0, 0)});
    for (size_t i = 0; i < symbols_count.symbol; ++i) {
        in >> decoding_table[i].first;
    }

    BinarySymbol current_code;
    size_t sum_elements = 0;
    size_t decoding_id = 0;

    while (sum_elements < symbols_count.symbol) {
        current_code <<= 1;
        BinarySymbol equal_size_count(0, 9);
        in >> equal_size_count;
        sum_elements += equal_size_count.symbol;

        for (size_t i = 0; i < equal_size_count.symbol; ++i) {
            decoding_table[decoding_id++].second = current_code;
            current_code.symbol += 1;
        }
    }

    return decoding_table;
}

void DecoderInit(BinaryTrie& decoder_trie, InBinaryStream& in) {
    std::vector<std::pair<BinarySymbol, BinarySymbol>> decoding_table = GetDecodingTable(in);

    decoder_trie.AddNode(0, BinarySymbol(0));
    decoder_trie.root = 0;

    for (const auto& [code, canonical_code] : decoding_table) {
        decoder_trie.AddCode(*decoder_trie.root, canonical_code, 0, code);
    }
}

BinarySymbol DecodeSymbol(BinaryTrie& decoder_trie, InBinaryStream& in) {
    BinarySymbol decoded_symbol;
    bool find_symbol_in_progress = true;
    size_t current_node = *decoder_trie.root;

    while (find_symbol_in_progress) {
        BinarySymbol one_bit(0, 1);
        in >> one_bit;
        current_node = decoder_trie.MakeStep(current_node, one_bit);
        if (decoder_trie.IsTerminal(current_node)) {
            decoded_symbol = decoder_trie.GetSymbol(current_node);
            find_symbol_in_progress = false;
        }
    }

    return decoded_symbol;
}

std::string DecodeFileName(BinaryTrie& decoder_trie, InBinaryStream& in) {
    std::string file_name;
    bool file_name_in_progress = true;

    while (file_name_in_progress) {
        BinarySymbol decoded_symbol = DecodeSymbol(decoder_trie, in);
        if (decoded_symbol.symbol == FILENAME_END) {
            file_name_in_progress = false;
        } else {
            file_name += static_cast<char>(decoded_symbol.symbol);
        }
    }

    return file_name;
}

void Decode(InBinaryStream& in) {
    bool decode_in_progress = true;
    while (decode_in_progress) {
        BinaryTrie decoder_trie;
        DecoderInit(decoder_trie, in);

        std::string file_name = DecodeFileName(decoder_trie, in);
        OutBinaryStream out(file_name);

        bool current_file_decoding = true;
        while (current_file_decoding) {
            BinarySymbol current_symbol = DecodeSymbol(decoder_trie, in);
            if (current_symbol.symbol == ONE_MORE_FILE) {
                current_file_decoding = false;
            } else if (current_symbol.symbol == ARCHIVE_END) {
                current_file_decoding = false;
                decode_in_progress = false;
            } else {
                --current_symbol.last_bit;  // convert from 9bit to 8bit
                out << current_symbol;
            }
        }
    }
}