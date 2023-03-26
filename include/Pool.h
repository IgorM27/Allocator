#pragma once

#include <vector>
#include <cmath>
#include <cstdint>
#include <map>

template<typename T>
class Pool {
private:

    std::vector<std::pair<size_t, size_t>> free_;
    T* start_;
    size_t count_;
    size_t length_;
    size_t size_;

public:

    explicit Pool(size_t count, size_t length) {
        count_ = count;
        length_ = length;
        size_ = count_ * length_;
        start_ = static_cast<T*>(operator new(size_ * sizeof(T)));
        free_.emplace_back(0, count);
    }

    T* allocate(size_t n) {
        size_t count_pieces;

        if (n % length_ != 0) {
            count_pieces = n / length_ + 1;
        } else {
            count_pieces = n / length_;
        }

        size_t free_index = 0;

        for (int i = 0; i < free_.size(); i++) {
            auto tmp = free_[i];
            if (tmp.second >= count_pieces) {
                free_index = tmp.first;
                if (tmp.second == count_pieces) {
                    free_.erase(free_.begin() + i);
                } else {
                    free_[i].first += count_pieces;
                    free_[i].second -= count_pieces;
                }
                return start_ + free_index * length_;
            }
        }

        return nullptr;
    }

    bool deallocate(T* tmp, size_t n) {
        if (tmp >= start_ && tmp <= (start_ + size_)) {
            size_t index = tmp - start_;
            free_.emplace_back(index, n);
            update_free(index, n);
            return true;
        } else {
            return false;
        }
    }


    [[nodiscard]] size_t size() const {
        return size_;
    }

    [[nodiscard]] size_t count() const {
        return count_;
    }

    [[nodiscard]] size_t length() const {
        return length_;
    }

    [[nodiscard]] T* start() const {
        return start_;
    }

    [[nodiscard]] std::vector<std::pair<size_t, size_t>> free() const {
        return free_;
    }

    void update_free(size_t index, size_t n) {
        for (int i = 0; i < free_.size(); i++) {
            auto tmp = free_[i];
            if (tmp.first == index + n) {
                free_.back().second += tmp.second;
                free_.erase(free_.begin() + i);
            }
            if (tmp.first + tmp.second == index) {
                free_.back().first = free_[i].first;
                free_.back().second += free_[i].second;
                free_.erase(free_.begin() + i);
            }
        }
    }

    ~Pool() {
        operator delete(start_, size_ * sizeof(T));
    }

};

template<typename T, typename U>
bool operator==(const Pool<T>& lhs, const Pool<U>& rhs) {
    if (lhs.Start() != rhs.Start()) {
        return false;
    }
    if (lhs.free().size() != rhs.free().size()) {
        return false;
    }

    for (size_t i = 0; i < lhs.free().size(); ++i) {
        if (lhs.free()[i] != rhs.free()[i]) {
            return false;
        }
    }
    return true;
}

template<typename T, typename U>
bool operator!=(const Pool<T>& lhs, const Pool<U>& rhs) {
    return !(lhs == rhs);
}