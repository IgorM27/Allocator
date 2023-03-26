#pragma once

#include "Pool.h"

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include "fstream"

size_t kDefaultCount = 10;
size_t kDefaultLength = 10000;

template<typename T>
class Allocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using void_pointer = void*;
    using const_void_pointer = const void*;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;

private:
    std::vector<Pool<value_type>*> pools_;

public:
    explicit Allocator() {
        parameters(pools_);
    }

    pointer allocate(size_t n) {
        std::sort(pools_.begin(), pools_.end(), [n](const Pool<T>* lhs, const Pool<T>* rhs) {

            if (lhs->size() < n) {
                return false;
            }

            if (rhs->size() < n) {
                return true;
            }

            size_t delta1 = lhs->length() - (n % lhs->length());
            size_t delta2 = rhs->length() - (n % rhs->length());

            if (delta1 == delta2) {
                return lhs->length() > rhs->length();
            } else {
                return delta1 < delta2;
            }
        });


        for (Pool<value_type>* i: pools_) {
            pointer tmp = i->allocate(n);
            if (tmp != nullptr) {
                return tmp;
            }
        }
        throw std::bad_alloc{};
    }

    void deallocate(pointer tmp, size_t n) {
        for (Pool<value_type>* i: pools_) {
            if (i->deallocate(tmp, n)) {
                return;
            }
        }
        throw std::out_of_range("Pointer out of range");
    }

    ~Allocator() {
        for (Pool<value_type>* i: pools_) {
            delete i;
        }
    }

    [[nodiscard]] std::vector<Pool<value_type>*> pools() const {
        return pools_;
    }

    void parameters(std::vector<Pool<value_type>*>& pool) {
        std::ifstream input(R"(..\\include\Config.txt)", std::ios::binary);
        std::vector<size_type> count_;
        std::vector<size_type> length_;
        size_type number;
        int a = 1;
        std::string tmp;
        while (!(input.eof())) {
            char ch;
            input.get(ch);
            if (input.eof()) {
                break;
            }
            if (ch == '\r') {
                if (a == 1) {
                    number = static_cast<size_type>(std::atoi(tmp.c_str()));
                }
                if (a == 2) {
                    count_.push_back(static_cast<size_type>(std::atoi(tmp.c_str())));
                }
                if (a == 3) {
                    length_.push_back(static_cast<size_type>(std::atoi(tmp.c_str())));
                }
                tmp = "";
                a++;
                input.get(ch);
                continue;
            }
            if (!std::isspace(ch)) {
                tmp += ch;
            } else {
                if (a == 1) {
                    number = static_cast<size_type>(std::atoi(tmp.c_str()));
                }
                if (a == 2) {
                    count_.push_back(static_cast<size_type>(std::atoi(tmp.c_str())));
                }
                if (a == 3) {
                    length_.push_back(static_cast<size_type>(std::atoi(tmp.c_str())));
                }
                tmp = "";
            }
        }
        if (!tmp.empty()) {
            length_.push_back(static_cast<size_type>(std::atoi(tmp.c_str())));
        }
        size_type counter = 0;
        for (size_type i = 0; i < std::min(count_.size(), length_.size()) && counter < number; ++i, ++counter) {
            pool.push_back(new Pool<T>(count_[i], length_[i]));
        }
        if (count_.size() <= length_.size()) {
            for (size_type i = count_.size(); i < length_.size() && counter < number; ++i, ++counter) {
                pool.push_back(new Pool<T>(kDefaultCount, length_[i]));
            }
        } else {
            for (size_type i = length_.size(); i < count_.size() && counter < number; ++i, ++counter) {
                pool.push_back(new Pool<T>(count_[i], kDefaultLength));
            }
        }
        for (size_type i = counter; i < number; ++i) {
            pool.push_back(new Pool<T>(kDefaultCount, kDefaultLength));
        }
    }


};

template<typename T, typename U>
bool operator==(const Allocator<T>& lhs, const Allocator<U>& rhs) {
    if (lhs.pools().size() != rhs.pools().size()) {
        return false;
    }

    for (size_t i = 0; i < lhs.pools().size(); ++i) {
        if (lhs.pools()[i] != rhs.pools()[i]) {
            return false;
        }
    }

    return true;
}

template<typename T, typename U>
bool operator!=(const Allocator<T>& lhs, const Allocator<U>& rhs) {
    return !(lhs == rhs);
}
