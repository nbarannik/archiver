#pragma once

#include "in_binary_stream.h"
#include "out_binary_stream.h"
#include "binary_symbol.h"
#include "binary_trie.h"

#ifndef CONSTS
#define CONSTS
static const uint16_t FILENAME_END = 256;
static const uint16_t ONE_MORE_FILE = 257;
static const uint16_t ARCHIVE_END = 258;
#endif

void Decode(InBinaryStream& in);