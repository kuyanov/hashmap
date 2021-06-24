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
// For each hash there is a linked list with elements of that hash.
// When the number of elements exceeds the number of buckets, the number of buckets double and hashes are recomputed.
// All queries work for O(length of chain) which is O(1) in average.
// Detailed explanation is there: https://en.wikipedia.org/wiki/Hash_table#Separate_chaining
template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
  private:
    using KeyVal = typename std::pair<KeyType, ValueType>;
    using ConstKeyVal = typename std::pair<const KeyType, ValueType>;
    using ElemIter = typename std::list<ConstKeyVal>::iterator;
    using ConstElemIter = typename std::list<ConstKeyVal>::const_iterator;

  public:
    static constexpr int RESIZE_RATIO = 2;

    explicit HashMap(Hash custom_hasher = Hash()) : hasher(custom_hasher) {
        allSize = 0;
        table.resize(1);
    }

    template<class ForwardIterator>
    HashMap(ForwardIterator begin, ForwardIterator end, Hash custom_hasher = Hash())
            : hasher(custom_hasher) {
        std::copy(begin, end, std::front_inserter(all));
        allSize = all.size();
        initBuckets(allSize);
    }

    HashMap(std::initializer_list<KeyVal> elements, Hash custom_hasher = Hash())
            : hasher(custom_hasher) {
        std::copy(elements.begin(), elements.end(), std::front_inserter(all));
        allSize = all.size();
        initBuckets(allSize);
    }

    HashMap(const HashMap &other) : hasher(other.hasher) {
        std::copy(other.all.begin(), other.all.end(), std::front_inserter(all));
        allSize = all.size();
        initBuckets(allSize);
    }

    HashMap &operator=(HashMap other) {
        hasher = other.hasher;
        all.clear();
        std::copy(other.all.begin(), other.all.end(), std::front_inserter(all));
        allSize = all.size();
        initBuckets(allSize);
        return *this;
    }

    // Actual number of elements in the hash table.
    inline size_t size() const {
        return allSize;
    }

    // Checks if the hash table is empty.
    inline bool empty() const {
        return allSize == 0;
    }

    // Hasher function.
    inline Hash hash_function() const {
        return hasher;
    }

    using iterator = ElemIter;
    using const_iterator = ConstElemIter;

    // Iterator for begin.
    inline iterator begin() {
        return all.begin();
    }

    // Const iterator for begin
    inline const_iterator begin() const {
        return all.begin();
    }

    // Iterator for end
    inline iterator end() {
        return all.end();
    }

    // Const iterator for end
    inline const_iterator end() const {
        return all.end();
    }

    // Returns iterator for the element with the given key. If there is no such key, end() is returned.
    iterator find(const KeyType &key) {
        size_t id = bucketId(key);
        for (ElemIter it : table[id]) {
            if (it->first == key) {
                return it;
            }
        }
        return end();
    }

    // Const version of find()
    const_iterator find(const KeyType &key) const {
        size_t id = bucketId(key);
        for (ConstElemIter it : table[id]) {
            if (it->first == key) {
                return it;
            }
        }
        return end();
    }

    // Access to value by key. If the key is absent, (key, ValueType()) is inserted and returned.
    ValueType &operator[](const KeyType &key) {
        ElemIter it = find(key);
        if (it != end()) {
            return it->second;
        }
        size_t id = bucketId(key);
        all.emplace_front(key, ValueType());
        ++allSize;
        table[id].push_front(all.begin());
        resizeIfNecessary();
        return all.begin()->second;
    }

    // Read access to value by key. If the key is absent, std::out_of_range is thrown.
    const ValueType &at(const KeyType &key) const {
        ConstElemIter it = find(key);
        if (it != end()) {
            return it->second;
        }
        throw std::out_of_range("");
    }

    // Inserts pair (key, value) into the hash table. If such key exists, nothing is inserted.
    void insert(const KeyVal &p) {
        ElemIter it = find(p.first);
        if (it != end()) {
            return;
        }
        size_t id = bucketId(p.first);
        all.push_front(p);
        ++allSize;
        table[id].push_front(all.begin());
        resizeIfNecessary();
    }

    // Erases the given key from the hash table. If there is no such key, nothing happens.
    void erase(const KeyType &key) {
        ElemIter it = find(key);
        if (it == end()) {
            return;
        }
        size_t id = bucketId(key);
        all.erase(it);
        --allSize;
        table[id].erase(std::find(table[id].begin(), table[id].end(), it));
    }

    // Erases all data from the hash table, preserving the number of buckets.
    void clear() {
        for (const auto &elem : all) {
            size_t id = bucketId(elem.first);
            table[id].clear();
        }
        all.clear();
        allSize = 0;
    }

  private:
    Hash hasher;
    size_t allSize;
    std::list<ConstKeyVal> all;
    std::vector<std::list<ElemIter>> table;

    // Bucket number by key.
    size_t bucketId(const KeyType &key) const {
        return hasher(key) % table.size();
    }

    // Initializes buckets based on all elements
    void initBuckets(size_t tableSize) {
        table.clear();
        table.resize(std::max(tableSize, 1ul));
        for (ElemIter it = all.begin(); it != all.end(); ++it) {
            table[bucketId(it->first)].push_front(it);
        }
    }

    // Doubles the number of buckets if the number of elements exceeded it.
    void resizeIfNecessary() {
        if (allSize > table.size()) {
            initBuckets(RESIZE_RATIO * table.size());
        }
    }
};

#endif //HASHMAP_HASH_MAP_H
