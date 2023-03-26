#pragma once

#include <cstddef>
#include <cstdlib>
#include <iostream>

template <typename ValueType>
class PriorityQueue {
public:
    using SizeType = size_t;
    using DifferenceType = ptrdiff_t;

    explicit PriorityQueue(bool (*comparator)(const ValueType&, const ValueType&)) : comparator_(comparator) {
    }

    PriorityQueue(const PriorityQueue& other) {
        try {
            Init(other.size_, other.capacity_);
            CopyData(other);
        } catch (const std::bad_alloc& exception) {
            std::cout << "Allocation failed: " << exception.what() << '\n';
        }
    }

    PriorityQueue& operator=(const PriorityQueue& other) {
        ValueType* tmp = std::move(data_);
        try {
            delete[] data_;
            Init(other.size_, other.capacity_);
            CopyData(other);
        } catch (const std::bad_alloc& exception) {
            data_ = tmp;
            std::cout << "Allocation failed: " << exception.what() << '\n';
        }

        return *this;
    }

    ~PriorityQueue() {
        size_ = 0;
        capacity_ = 0;
        delete[] data_;
    }

    SizeType Size() const {
        return size_;
    }

    void Clear() {
        size_ = 0;
    }

    void Push(const ValueType& new_element) {
        PushBack(new_element);
        SiftUp(Size() - 1);
    }

    void Pop() {
        if (size_ == 0) {
            throw std::runtime_error("PriorityQueue is already empty");
        }

        (*this)[0] = (*this)[size_ - 1];
        PopBack();
        SiftDown(0);
    }

    ValueType& Top() {
        return (*this)[0];
    }

private:
    bool (*comparator_)(const ValueType&, const ValueType&);

    void SiftUp(SizeType element_pos) {
        while (element_pos > 0) {
            SizeType ancestor_pos = (element_pos - 1) / 2;
            if (comparator_((*this)[element_pos], (*this)[ancestor_pos])) {
                std::swap((*this)[element_pos], (*this)[ancestor_pos]);
                element_pos = ancestor_pos;
            } else {
                return;
            }
        }
    }

    void SiftDown(SizeType element_pos) {
        while ((element_pos << 1) + 1 < size_) {
            SizeType left_child_pos = (element_pos << 1) + 1;
            SizeType right_child_pos = (element_pos << 1) + 2;

            if (left_child_pos >= size_) {
                return;
            }

            SizeType next_child = left_child_pos;
            if (right_child_pos < size_ && comparator_((*this)[right_child_pos], (*this)[left_child_pos])) {
                next_child = right_child_pos;
            }

            if (comparator_((*this)[element_pos], (*this)[next_child])) {
                return;
            }

            std::swap((*this)[element_pos], (*this)[next_child]);
            element_pos = next_child;
        }
    }

    void PushBack(const ValueType& new_element) {
        if (size_ + 1 > capacity_) {
            Reallocate(std::max(static_cast<SizeType>(1), (capacity_ << 1)));
        }

        *(data_ + size_) = new_element;
        ++size_;
    }

    void PopBack() {
        --size_;
    }

    ValueType& operator[](size_t position) {
        return *(data_ + position);
    }

    ValueType operator[](size_t position) const {
        return *(data_ + position);
    }

    SizeType Capacity() const {
        return capacity_;
    }

    void CopyData(const PriorityQueue& other) {
        for (DifferenceType i = 0; i < size_; ++i) {
            *(data_ + i) = *(other.data_ + i);
        }
    }

    void Reallocate(SizeType new_capacity) {
        try {
            ValueType* data_new = new ValueType[new_capacity];

            for (DifferenceType i = 0; i < size_; ++i) {
                *(data_new + i) = *(data_ + i);
            }

            capacity_ = new_capacity;
            delete[] data_;
            data_ = data_new;
        } catch (const std::bad_alloc& exception) {
            std::cout << "Allocation failed: " << exception.what() << '\n';
        }
    }

    void Init(SizeType size, SizeType capacity) {
        size_ = size;
        capacity_ = capacity;
        data_ = new ValueType[capacity_];
    }

    ValueType* data_ = nullptr;
    SizeType size_ = 0;
    SizeType capacity_ = 0;
};