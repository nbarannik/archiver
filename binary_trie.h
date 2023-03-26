#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "binary_symbol.h"
#include <optional>

struct BinaryTrie {
    struct Node {
        size_t priority = 0;
        std::optional<size_t> left_child;
        std::optional<size_t> right_child;
        BinarySymbol symbol;
        bool is_terminal = false;

        explicit Node(const std::vector<Node>& nodes) : nodes_(nodes) {
        }
        Node(size_t priority, const BinarySymbol& symbol, const std::vector<Node>& nodes)
            : priority(priority), symbol(symbol), is_terminal(true), nodes_(nodes) {
        }
        Node(size_t priority, size_t left_child, size_t right_child, const std::vector<Node>& nodes);

        bool operator<(const Node& other);

    private:
        const std::vector<Node>& nodes_;
    };

    void AddNode(size_t priority, const BinarySymbol& symbol);
    void AddNode(size_t priority, size_t left_child, size_t right_child);
    void AddCode(size_t current_node, const BinarySymbol& canonical_code, size_t bit, const BinarySymbol& code);
    size_t GetSize();
    void GetCodes(size_t current_node, BinarySymbol& current_code,
                  std::unordered_map<BinarySymbol, BinarySymbol>& codes);
    size_t MakeStep(size_t current_node, const BinarySymbol& bit) const;
    bool IsTerminal(size_t current_node) const;
    BinarySymbol GetSymbol(size_t current_node) const;

    std::optional<size_t> root;

private:
    std::vector<Node> nodes_;
};