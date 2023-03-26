#pragma once

#include "in_binary_stream.h"
#include "out_binary_stream.h"
#include <unordered_map>
#include <vector>
#include "binary_trie.h"

#ifndef CONSTS
#define CONSTS
static const size_t ALPHABET_SIZE = 300;
static const size_t SYMBOL_SIZE = 9;
static const uint16_t FILENAME_END = 256;
static const uint16_t ONE_MORE_FILE = 257;
static const uint16_t ARCHIVE_END = 258;
#endif

std::string GetBinarySymbol(uint16_t symbol);

std::unordered_map<BinarySymbol, BinarySymbol> GetHuffmanCodes(std::array<size_t, ALPHABET_SIZE>& count_occurrences);

std::pair<std::vector<std::string>, std::vector<size_t>> GetCanonicalCodes(
    std::unordered_map<std::string, std::string>& huffman_codes);

void Encode(InBinaryStream& in, OutBinaryStream& out, bool is_last_encoding = true);