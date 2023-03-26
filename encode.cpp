#include "encode.h"
#include <array>
#include <queue>
#include <algorithm>
#include "priority_queue.h"

std::array<size_t, ALPHABET_SIZE> CountOccurrences(InBinaryStream& in) {
    std::array<size_t, ALPHABET_SIZE> count;
    count.fill(0);

    {
        char symbol = '\0';
        while (in >> symbol) {
            count[static_cast<unsigned char>(symbol)]++;
        }
    }

    {
        for (const char& symbol : in.file_name) {
            count[static_cast<size_t>(symbol)]++;
        }
    }

    return count;
}

bool Comparator(const std::tuple<size_t, size_t, size_t>& t1, const std::tuple<size_t, size_t, size_t>& t2) {
    return t1 < t2;
}

std::unordered_map<BinarySymbol, BinarySymbol> GetHuffmanCodes(std::array<size_t, ALPHABET_SIZE>& count_occurrences) {
    PriorityQueue<std::tuple<size_t, size_t, size_t>> nodes(Comparator);

    BinaryTrie trie;

    for (uint16_t symbol = 0; symbol < ALPHABET_SIZE; ++symbol) {
        if (count_occurrences[symbol]) {
            nodes.Push({count_occurrences[symbol], trie.GetSize(), trie.GetSize()});
            trie.AddNode(count_occurrences[symbol], BinarySymbol(symbol));
        }
    }

    nodes.Push({1, trie.GetSize(), trie.GetSize()});
    trie.AddNode(1, BinarySymbol(FILENAME_END));

    nodes.Push({1, trie.GetSize(), trie.GetSize()});
    trie.AddNode(1, BinarySymbol(ONE_MORE_FILE));

    nodes.Push({1, trie.GetSize(), trie.GetSize()});
    trie.AddNode(1, BinarySymbol(ARCHIVE_END));

    while (nodes.Size() > 1) {
        std::tuple<size_t, size_t, size_t> node1 = nodes.Top();
        nodes.Pop();
        std::tuple<size_t, size_t, size_t> node2 = nodes.Top();
        nodes.Pop();
        nodes.Push({get<0>(node1) + get<0>(node2), std::min(get<1>(node1), get<1>(node2)), trie.GetSize()});
        trie.AddNode(get<0>(node1) + get<0>(node2), static_cast<ssize_t>(get<2>(node1)),
                     static_cast<ssize_t>(get<2>(node2)));
    }

    trie.root = static_cast<ssize_t>(get<2>(nodes.Top()));
    nodes.Pop();

    std::unordered_map<BinarySymbol, BinarySymbol> codes;
    BinarySymbol current_code;
    trie.GetCodes(*trie.root, current_code, codes);
    return codes;
}

std::pair<std::vector<BinarySymbol>, std::vector<size_t>> GetCanonicalCodes(
    std::unordered_map<BinarySymbol, BinarySymbol>& huffman_codes) {
    std::vector<std::pair<size_t, BinarySymbol>> sorted_codes;
    for (const auto& code : huffman_codes) {
        sorted_codes.push_back({code.second.Size(), code.first});
    }

    sort(sorted_codes.begin(), sorted_codes.end());
    std::pair<std::vector<BinarySymbol>, std::vector<size_t>> canonical_codes;

    BinarySymbol current_code;
    current_code <<= static_cast<int>(sorted_codes[0].first);

    for (size_t i = 1; i < sorted_codes[0].first; ++i) {
        canonical_codes.second.push_back(0);
    }

    canonical_codes.first.push_back(sorted_codes[0].second);
    size_t count_equal_size_codes = 1;

    for (size_t i = 1; i < sorted_codes.size(); ++i) {
        current_code.symbol += 1;
        current_code <<= static_cast<int>((sorted_codes[i].first - current_code.Size()));
        if (current_code.Size() != sorted_codes[i - 1].first) {
            canonical_codes.second.push_back(count_equal_size_codes);
            for (size_t j = sorted_codes[i].first - 1; j > sorted_codes[i - 1].first; --j) {
                canonical_codes.second.push_back(0);
            }
            count_equal_size_codes = 0;
        }
        ++count_equal_size_codes;
        canonical_codes.first.push_back(sorted_codes[i].second);
    }

    canonical_codes.second.push_back(count_equal_size_codes);
    return canonical_codes;
}

void FillEncodingTable(std::array<BinarySymbol, ALPHABET_SIZE>& encoding_table,
                       const std::pair<std::vector<BinarySymbol>, std::vector<size_t>>& canonical_codes) {
    BinarySymbol canonical_code;
    size_t symbol_pos = 0;

    for (size_t i = 0; i < canonical_codes.second.size(); ++i) {
        canonical_code <<= 1;
        for (size_t j = 0; j < canonical_codes.second[i]; ++j) {
            encoding_table[canonical_codes.first[symbol_pos].symbol] = canonical_code;
            canonical_code.symbol += 1;
            symbol_pos++;
        }
    }
}

std::array<BinarySymbol, ALPHABET_SIZE> InitEncodingTable(InBinaryStream& in, OutBinaryStream& out) {
    std::array<size_t, ALPHABET_SIZE> count_occurrences = CountOccurrences(in);
    std::unordered_map<BinarySymbol, BinarySymbol> huffman_codes = GetHuffmanCodes(count_occurrences);
    std::pair<std::vector<BinarySymbol>, std::vector<size_t>> canonical_codes = GetCanonicalCodes(huffman_codes);

    out << BinarySymbol(canonical_codes.first.size());

    for (const auto& code : canonical_codes.first) {
        out << code;
    }

    for (const auto& count : canonical_codes.second) {
        out << BinarySymbol(count);
    }

    std::array<BinarySymbol, ALPHABET_SIZE> encoding_table;
    FillEncodingTable(encoding_table, canonical_codes);

    return encoding_table;
}

void Encode(InBinaryStream& in, OutBinaryStream& out, bool is_last_encoding) {
    std::array<BinarySymbol, ALPHABET_SIZE> encoding_table = InitEncodingTable(in, out);

    for (const auto& c : in.file_name) {
        out << encoding_table[static_cast<unsigned char>(c)];
    }

    out << encoding_table[FILENAME_END];

    in.Reset();

    char byte = '0';
    while (in >> byte) {
        out << encoding_table[static_cast<unsigned char>(byte)];
    }

    if (is_last_encoding) {
        out << encoding_table[ARCHIVE_END];
        out.Close();
    } else {
        out << encoding_table[ONE_MORE_FILE];
    }
}