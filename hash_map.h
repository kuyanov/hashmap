//
// Created by Fedor Kuyanov on 05.03.2021.
//

#include <algorithm>
#include <iterator>
#include <list>
#include <stdexcept>
#include <utility>
#include <vector>

#ifndef HASHMAP_HASH_MAP_H
#define HASHMAP_HASH_MAP_H

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
    using KeyVal = typename std::pair<KeyType, ValueType>;
    using ConstKeyVal = typename std::pair<const KeyType, ValueType>;
    using ElemIter = typename std::list<ConstKeyVal>::iterator;
    using ConstElemIter = typename std::list<ConstKeyVal>::const_iterator;

    Hash hasher;
    size_t all_size;
    std::list<ConstKeyVal> all;
    std::vector<std::list<ElemIter>> table;

    size_t bucket_id(const KeyType &key) const {
        return hasher(key) % table.size();
    }

    void resize(size_t table_size) {
        table.clear();
        table.resize(table_size);
        for (ElemIter it = all.begin(); it != all.end(); ++it) {
            table[bucket_id(it->first)].push_front(it);
        }
    }

    void resize_if_necessary() {
        if (all_size > table.size()) {
            resize(2 * table.size());
        }
    }

public:
    explicit HashMap(Hash custom_hasher = Hash()) : hasher(custom_hasher) {
        all_size = 0;
        resize(1);
    }

    template<class ForwardIterator>
    HashMap(ForwardIterator begin, ForwardIterator end, Hash custom_hasher = Hash())
            : hasher(custom_hasher) {
        all_size = 0;
        while (begin != end) {
            all.push_front(*begin);
            ++all_size;
            ++begin;
        }
        resize(2 * all_size);
    }

    HashMap(std::initializer_list<KeyVal> elements, Hash custom_hasher = Hash())
            : hasher(custom_hasher) {
        std::copy(elements.begin(), elements.end(), std::back_inserter(all));
        all_size = elements.size();
        resize(2 * all_size);
    }

    HashMap(const HashMap &other) {
        hasher = other.hasher;
        all_size = other.all_size;
        table.resize(other.table.size());
        for (size_t id = 0; id < other.table.size(); ++id) {
            for (ConstElemIter it : other.table[id]) {
                all.push_front(*it);
                table[id].push_front(all.begin());
            }
        }
    }

    HashMap &operator=(HashMap other) {
        hasher = other.hasher;
        table.clear();
        table.resize(other.table.size());
        all.clear();
        all_size = 0;
        for (size_t id = 0; id < other.table.size(); ++id) {
            for (ConstElemIter it : other.table[id]) {
                all.push_front(*it);
                ++all_size;
                table[id].push_front(all.begin());
            }
        }
        return *this;
    }

    size_t size() const {
        return all_size;
    }

    bool empty() const {
        return all_size == 0;
    }

    Hash hash_function() const {
        return hasher;
    }

    void insert(const KeyVal &p) {
        size_t id = bucket_id(p.first);
        for (ConstElemIter it : table[id]) {
            if (it->first == p.first) {
                return;
            }
        }
        all.push_front(p);
        ++all_size;
        table[id].push_front(all.begin());
        resize_if_necessary();
    }

    void erase(const KeyType &key) {
        size_t id = bucket_id(key);
        for (auto iter = table[id].begin(); iter != table[id].end(); ++iter) {
            ConstElemIter it = *iter;
            if (it->first == key) {
                table[id].erase(iter);
                all.erase(it);
                --all_size;
                return;
            }
        }
    }

    using iterator = ElemIter;
    using const_iterator = ConstElemIter;

    iterator begin() {
        return all.begin();
    }

    const_iterator begin() const {
        return all.begin();
    }

    iterator end() {
        return all.end();
    }

    const_iterator end() const {
        return all.end();
    }

    iterator find(const KeyType &key) {
        size_t id = bucket_id(key);
        for (ElemIter it : table[id]) {
            if (it->first == key) {
                return it;
            }
        }
        return end();
    }

    const_iterator find(const KeyType &key) const {
        size_t id = bucket_id(key);
        for (ConstElemIter it : table[id]) {
            if (it->first == key) {
                return it;
            }
        }
        return end();
    }

    ValueType &operator[](const KeyType &key) {
        size_t id = bucket_id(key);
        for (ElemIter it : table[id]) {
            if (it->first == key) {
                return it->second;
            }
        }
        all.emplace_front(key, ValueType());
        ++all_size;
        table[id].push_front(all.begin());
        resize_if_necessary();
        return all.begin()->second;
    }

    const ValueType &at(const KeyType &key) const {
        size_t id = bucket_id(key);
        for (ConstElemIter it : table[id]) {
            if (it->first == key) {
                return it->second;
            }
        }
        throw std::out_of_range("");
    }

    void clear() {
        for (const auto &elem : all) {
            size_t id = bucket_id(elem.first);
            table[id].clear();
        }
        all.clear();
        all_size = 0;
    }
};

#endif //HASHMAP_HASH_MAP_H
