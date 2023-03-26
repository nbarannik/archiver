#include "binary_trie.h"

BinaryTrie::Node::Node(size_t priority, size_t left_child, size_t right_child, const std::vector<Node>& nodes)
    : priority(priority), left_child(left_child), right_child(right_child), nodes_(nodes) {
    is_terminal = false;
    symbol = nodes_[left_child].symbol;
    if (nodes_[right_child].symbol < symbol) {
        symbol = nodes_[right_child].symbol;
    }
}

bool BinaryTrie::Node::operator<(const Node& other) {
    return std::tie(priority, symbol) < std::tie(other.priority, other.symbol);
}

size_t BinaryTrie::GetSize() {
    return nodes_.size();
}

void BinaryTrie::AddNode(size_t priority, const BinarySymbol& symbol) {
    nodes_.push_back(Node(priority, symbol, nodes_));
}

void BinaryTrie::AddNode(size_t priority, size_t left_child, size_t right_child) {
    nodes_.push_back(Node(priority, left_child, right_child, nodes_));
}

void BinaryTrie::AddCode(size_t current_node, const BinarySymbol& canonical_code, size_t bit,
                         const BinarySymbol& code) {
    if (bit == canonical_code.Size()) {
        nodes_[current_node].symbol = code;
        nodes_[current_node].is_terminal = true;
        return;
    }

    if (canonical_code[bit] == 0) {
        if (!nodes_[current_node].left_child) {
            nodes_[current_node].left_child = nodes_.size();
            nodes_.push_back(Node(nodes_));
        }
        AddCode(*nodes_[current_node].left_child, canonical_code, bit + 1, code);
    } else {
        if (!nodes_[current_node].right_child) {
            nodes_[current_node].right_child = nodes_.size();
            nodes_.push_back(Node(nodes_));
        }
        AddCode(*nodes_[current_node].right_child, canonical_code, bit + 1, code);
    }
}

void BinaryTrie::GetCodes(size_t current_node, BinarySymbol& current_code,
                          std::unordered_map<BinarySymbol, BinarySymbol>& codes) {
    if (current_node > nodes_.size() or current_node < 0) {
        throw std::runtime_error("out of range");
    }

    if (nodes_[current_node].is_terminal) {
        codes[nodes_[current_node].symbol] = current_code;
        return;
    }

    if (nodes_[current_node].left_child) {
        current_code <<= 1;
        GetCodes(*nodes_[current_node].left_child, current_code, codes);
        current_code >>= 1;
    }

    if (nodes_[current_node].right_child) {
        current_code <<= 1;
        current_code.symbol |= 1;

        GetCodes(*nodes_[current_node].right_child, current_code, codes);
        current_code >>= 1;
    }
}

size_t BinaryTrie::MakeStep(size_t current_node, const BinarySymbol& bit) const {
    size_t next_node = 0;

    if (bit.symbol == 0) {
        if (!nodes_[current_node].left_child) {
            throw std::runtime_error("no such child in node");
        }
        next_node = *nodes_[current_node].left_child;
    } else {
        if (!nodes_[current_node].right_child) {
            throw std::runtime_error("no such child in node");
        }
        next_node = *nodes_[current_node].right_child;
    }

    return next_node;
}

bool BinaryTrie::IsTerminal(size_t current_node) const {
    return nodes_[current_node].is_terminal;
}

BinarySymbol BinaryTrie::GetSymbol(size_t current_node) const {
    return nodes_[current_node].symbol;
}