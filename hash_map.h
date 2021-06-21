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

// Hash table with separate chaining, dynamic expansion.
template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
  private:
    using KeyVal = typename std::pair<KeyType, ValueType>;
    using ConstKeyVal = typename std::pair<const KeyType, ValueType>;
    using ElemIter = typename std::list<ConstKeyVal>::iterator;
    using ConstElemIter = typename std::list<ConstKeyVal>::const_iterator;

    Hash hasher;
    size_t all_size;
    std::list<ConstKeyVal> all;
    std::vector<std::list<ElemIter>> table;

    // Bucket number by key.
    size_t bucket_id(const KeyType &key) const {
        return hasher(key) % table.size();
    }

    // Resizes the array of buckets.
    void resize(size_t table_size) {
        table.clear();
        table.resize(table_size);
        for (ElemIter it = all.begin(); it != all.end(); ++it) {
            table[bucket_id(it->first)].push_front(it);
        }
    }

    // Doubles the number of buckets if the number of elements exceeded it.
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

    // Actual number of elements in the hash table.
    size_t size() const {
        return all_size;
    }

    // Checks if the hash table is empty.
    bool empty() const {
        return all_size == 0;
    }

    // Hasher function.
    Hash hash_function() const {
        return hasher;
    }

    // Inserts pair (key, value) into the hash table. If such key exists, nothing is inserted.
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

    // Erases the given key from the hash table. If there is no such key, nothing happens.
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

    // Iterator for begin.
    iterator begin() {
        return all.begin();
    }

    // Const iterator for begin
    const_iterator begin() const {
        return all.begin();
    }

    // Iterator for end
    iterator end() {
        return all.end();
    }

    // Const iterator for end
    const_iterator end() const {
        return all.end();
    }

    // Returns iterator for the element with the given key. If there is no such key, end() is returned.
    iterator find(const KeyType &key) {
        size_t id = bucket_id(key);
        for (ElemIter it : table[id]) {
            if (it->first == key) {
                return it;
            }
        }
        return end();
    }

    // Const version of find()
    const_iterator find(const KeyType &key) const {
        size_t id = bucket_id(key);
        for (ConstElemIter it : table[id]) {
            if (it->first == key) {
                return it;
            }
        }
        return end();
    }

    // Access to value by key. If the key is absent, (key, ValueType()) is inserted.
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

    // Read access to value by key. If the key is absent, std::out_of_range is thrown.
    const ValueType &at(const KeyType &key) const {
        size_t id = bucket_id(key);
        for (ConstElemIter it : table[id]) {
            if (it->first == key) {
                return it->second;
            }
        }
        throw std::out_of_range("");
    }

    // Erases all data from the hash table.
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
