#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <iterator>

template <typename Type>
class SingleLinkedList {
    // Объявление шаблона класса "Итератор" для списка
    // ValueType совпадает с Type для итератора
    // ValueType будет <const Type> для константного итератора
    template<typename ValueType>
    class BasicIterator;

    // Узел cписка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val)
            , next_node(next) {
        }
        Type value{};
        Node* next_node = nullptr;
    };

public:
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

    SingleLinkedList() = default;

    SingleLinkedList(std::initializer_list<Type> values) {
        FillList(values.begin(), values.end());
    }

    SingleLinkedList(const SingleLinkedList& other) {
        assert(size_ == 0 && head_.next_node == nullptr);
        FillList(other.begin(), other.end());
    }

    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            auto rhs_copy(rhs);
            swap(rhs_copy);
        }
        return *this;
    }

    void PushFront(const Type& value) {
        head_.next_node = new Node(value, head_.next_node);
        ++size_;
    }

    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        assert(pos.node_);
        pos.node_->next_node = new Node(value, pos.node_->next_node);
        ++size_;

        return Iterator(pos.node_->next_node);
    }

    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(!IsEmpty() && pos.node_);
        Node* node_after_erase = pos.node_->next_node->next_node;
        delete pos.node_->next_node;
        pos.node_->next_node = node_after_erase;
        --size_;

        return Iterator(node_after_erase);
    }

    void PopFront() {
        assert(!IsEmpty());
        Node* tmp = head_.next_node->next_node;
        delete head_.next_node;
        head_.next_node = tmp;
        --size_;
    }

    void Clear() noexcept {
        while (head_.next_node) {
            Node* tmp = head_.next_node;
            head_.next_node = head_.next_node->next_node;
            delete tmp;
        }
        size_ = 0;
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(head_.next_node, other.head_.next_node);
    }

    ~SingleLinkedList() {
        Clear();
    }

    size_t GetSize() const noexcept {
        return size_;
    }

    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    Iterator begin() noexcept {
        return Iterator(head_.next_node);
    }

    Iterator end() noexcept {
        return Iterator(nullptr);
    }

    ConstIterator begin() const noexcept {
        return cbegin();
    }

    ConstIterator end() const noexcept {
        return cend();
    }

    ConstIterator cbegin() const noexcept {
        return ConstIterator(head_.next_node);
    }

    ConstIterator cend() const noexcept {
        return ConstIterator(nullptr);
    }

    Iterator before_begin() noexcept {
        return Iterator(&head_);
    }

    ConstIterator cbefore_begin() const noexcept {
        return ConstIterator(const_cast<Node*>(&head_));
    }

    ConstIterator before_begin() const noexcept {
        return cbefore_begin();
    }

private:
    Node head_;
    size_t size_ = 0;

    template <typename It>
    void FillList(It first, It last) {
        SingleLinkedList tmp;
        Iterator pos = tmp.before_begin();

        for (auto it = first; it != last; ++it) {
            pos = tmp.InsertAfter(pos, *it);
        }
        swap(tmp);
    }

    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        // Конвертирующий конструктор итератора из указателя на узел списка
        explicit BasicIterator(Node* node) {
            node_ = node;
        }
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept {
            node_ = other.node_;
        }

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(node_ == rhs.node_);
        }

        bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(node_ == rhs.node_);
        }

        BasicIterator& operator++() noexcept {
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            auto old_value(*this); // Сохраняем прежнее значение объекта для последующего возврата
            ++(*this); // используем логику префиксной формы инкремента
            return old_value;
        }

        reference operator*() const noexcept {
            return node_->value;
        }

        pointer operator->() const noexcept {
            return &node_->value;
        }

    private:
        Node* node_ = nullptr;
    };
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (lhs < rhs) || (lhs == rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return (rhs < lhs) || (rhs == lhs);
}