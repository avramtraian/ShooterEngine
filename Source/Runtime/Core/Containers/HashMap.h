/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "Core/Containers/HashTable.h"

namespace SE {

namespace Detail {

template<typename KeyType, typename ValueType, typename InternalHashTable>
class HashMapIterator {
public:
    using TableIterator = typename InternalHashTable::Iterator;

    struct KeyValuePair {
        ALWAYS_INLINE KeyValuePair(const KeyType& in_key, ValueType& in_value)
            : key(in_key)
            , value(in_value)
        {}

        const KeyType& key;
        ValueType& value;
    };

public:
    ALWAYS_INLINE explicit HashMapIterator(const TableIterator& table_iterator)
        : m_table_iterator(table_iterator)
    {}

    NODISCARD ALWAYS_INLINE bool operator==(const HashMapIterator& other) const
    {
        return m_table_iterator == other.m_table_iterator;
    }
    NODISCARD ALWAYS_INLINE bool operator!=(const HashMapIterator& other) const
    {
        return m_table_iterator != other.m_table_iterator;
    }

    NODISCARD ALWAYS_INLINE KeyValuePair operator*()
    {
        auto& bucket = *m_table_iterator;
        return KeyValuePair(bucket.key(), const_cast<ValueType&>(bucket.value()));
    }

    NODISCARD ALWAYS_INLINE KeyValuePair operator->()
    {
        auto& bucket = *m_table_iterator;
        return KeyValuePair(bucket.key(), const_cast<ValueType&>(bucket.value()));
    }

    ALWAYS_INLINE HashMapIterator& operator++()
    {
        ++m_table_iterator;
        return *this;
    }

    ALWAYS_INLINE HashMapIterator operator++(int)
    {
        HashTableIterator current = *this;
        ++(*this);
        return current;
    }

private:
    TableIterator m_table_iterator;
};

} // namespace Detail

enum class HashMapAddResult {
    InsertedNewKey,
    KeyAlreadyExists,
};

enum class HashMapRemoveResult {
    RemovedExistingKey,
    KeyDoesNotExist,
};

template<typename KeyType, typename ValueType>
class HashMap {
public:
    class Bucket {
    public:
        Bucket() = default;

        ALWAYS_INLINE ~Bucket()
        {
            key().~KeyType();
            value().~ValueType();
        }

        // NOTE: The equality operator only checks the equality of the keys.
        NODISCARD ALWAYS_INLINE bool operator==(const Bucket& other) const { return (key() == other.key()); }

    public:
        ALWAYS_INLINE KeyType* key_ptr() { return reinterpret_cast<KeyType*>(m_key_storage); }
        ALWAYS_INLINE const KeyType* key_ptr() const { return reinterpret_cast<const KeyType*>(m_key_storage); }
        ALWAYS_INLINE ValueType* value_ptr() { return reinterpret_cast<ValueType*>(m_value_storage); }
        ALWAYS_INLINE const ValueType* value_ptr() const { return reinterpret_cast<const ValueType*>(m_value_storage); }

        ALWAYS_INLINE KeyType& key() { return *key_ptr(); }
        ALWAYS_INLINE const KeyType& key() const { return *key_ptr(); }
        ALWAYS_INLINE ValueType& value() { return *value_ptr(); }
        ALWAYS_INLINE const ValueType& value() const { return *value_ptr(); }

    private:
        alignas(KeyType) u8 m_key_storage[sizeof(KeyType)];
        alignas(ValueType) u8 m_value_storage[sizeof(ValueType)];
    };

    struct BucketHasher {
        NODISCARD ALWAYS_INLINE static u64 get_hash(const Bucket& value)
        {
            // NOTE: The hash of a bucket only depends on the key.
            return Hasher<RemoveConst<KeyType>>::get_hash(value.key());
        }
    };

    // NOTE: The internal table that the map wraps around.
    using InternalHashTable = HashTable<Bucket, BucketHasher>;

    using Iterator = Detail::HashMapIterator<KeyType, ValueType, InternalHashTable>;
    using ConstIterator = Detail::HashMapIterator<KeyType, const ValueType, InternalHashTable>;

public:
    NODISCARD ALWAYS_INLINE usize count() const { return m_buckets.count(); }
    NODISCARD ALWAYS_INLINE bool is_empty() const { return m_buckets.is_empty(); }
    NODISCARD ALWAYS_INLINE bool has_elements() const { return m_buckets.has_elements(); }

public:
    NODISCARD ALWAYS_INLINE Optional<usize> find(const KeyType& key)
    {
        const Bucket& key_as_bucket = unsafe_bucket_from_key(key);
        return m_buckets.find(key_as_bucket);
    }

    NODISCARD ALWAYS_INLINE Optional<ValueType&> get_if_exists(const KeyType& key)
    {
        auto slot_index = find(key);
        if (slot_index.has_value()) {
            return m_buckets.m_slots[slot_index].value();
        }
        return {};
    }

    NODISCARD ALWAYS_INLINE Optional<const ValueType&> get_if_exists(const KeyType& key) const
    {
        auto slot_index = find(key);
        if (slot_index.has_value()) {
            return m_buckets.m_slots[slot_index].value();
        }
        return {};
    }

    NODISCARD ALWAYS_INLINE ValueType& at(const KeyType& key)
    {
        auto optional_value = get_if_exists(key);
        SE_ASSERT(optional_value.has_value());
        return *optional_value;
    }

    NODISCARD ALWAYS_INLINE const ValueType& at(const KeyType& key) const
    {
        auto optional_value = get_if_exists(key);
        SE_ASSERT(optional_value.has_value());
        return *optional_value;
    }

public:
    ALWAYS_INLINE void add(const KeyType& key, const ValueType& value)
    {
        const usize slot_index = add_without_constructing_bucket(key);
        SE_ASSERT(slot_index != invalid_size); // Key already exists.

        Bucket& bucket = m_buckets.m_slots[slot_index];
        new (bucket.key_ptr()) KeyType(key);
        new (bucket.value_ptr()) ValueType(value);
    }

    ALWAYS_INLINE void add(const KeyType& key, ValueType&& value)
    {
        const usize slot_index = add_without_constructing_bucket(key);
        SE_ASSERT(slot_index != invalid_size); // Key already exists.

        Bucket& bucket = m_buckets.m_slots[slot_index];
        new (bucket.key_ptr()) KeyType(key);
        new (bucket.value_ptr()) ValueType(move(value));
    }

    ALWAYS_INLINE void add(KeyType&& key, const ValueType& value)
    {
        const usize slot_index = add_without_constructing_bucket(key);
        SE_ASSERT(slot_index != invalid_size); // Key already exists.

        Bucket& bucket = m_buckets.m_slots[slot_index];
        new (bucket.key_ptr()) KeyType(move(key));
        new (bucket.value_ptr()) ValueType(value);
    }

    ALWAYS_INLINE void add(KeyType&& key, ValueType&& value)
    {
        const usize slot_index = add_without_constructing_bucket(key);
        SE_ASSERT(slot_index != invalid_size); // Key already exists.

        Bucket& bucket = m_buckets.m_slots[slot_index];
        new (bucket.key_ptr()) KeyType(move(key));
        new (bucket.value_ptr()) ValueType(move(value));
    }

    template<typename... Args>
    ALWAYS_INLINE void emplace(const KeyType& key, Args&&... args)
    {
        const usize slot_index = add_without_constructing_bucket(key);
        SE_ASSERT(slot_index != invalid_size); // Key already exists.

        Bucket& bucket = m_buckets.m_slots[slot_index];
        new (bucket.key_ptr()) KeyType(key);
        new (bucket.value_ptr()) ValueType(forward<Args>(args)...);
    }

    template<typename... Args>
    ALWAYS_INLINE void emplace(KeyType&& key, Args&&... args)
    {
        const usize slot_index = add_without_constructing_bucket(key);
        SE_ASSERT(slot_index != invalid_size); // Key already exists.

        Bucket& bucket = m_buckets.m_slots[slot_index];
        new (bucket.key_ptr()) KeyType(move(key));
        new (bucket.value_ptr()) ValueType(forward<Args>(args)...);
    }

    ALWAYS_INLINE ValueType& get_or_add(const KeyType& key)
    {
        const Bucket& key_as_bucket = unsafe_bucket_from_key(key);
        const u64 bucket_hash = InternalHashTable::get_element_hash(key_as_bucket);
        const u8 low_bucket_hash = InternalHashTable::get_low_hash(bucket_hash);

        usize slot_index = invalid_size;
        
        if (m_buckets.m_occupied_slot_count != 0)
        {
            slot_index = m_buckets.unchecked_find_element_or_first_available_slot(key_as_bucket, bucket_hash, low_bucket_hash);

            if (m_buckets.m_slots_metadata[slot_index] == low_bucket_hash && m_buckets.m_slots[slot_index].key() == key) {
                // NOTE: The key already exists, so no more action is needed.
                return m_buckets.m_slots[slot_index].value();
            }
        }

        bool has_re_allocated = m_buckets.re_allocate_if_overloaded(m_buckets.m_occupied_slot_count + 1);
        if (has_re_allocated || slot_index == invalid_size) {
            // NOTE: We already know that the element doesn't exist in the table.
            slot_index = m_buckets.unchecked_find_first_available_slot(bucket_hash);
        }
        SE_ASSERT(slot_index != invalid_size);

        Bucket& bucket = m_buckets.m_slots[slot_index];
        new (bucket.key_ptr()) KeyType(key);
        new (bucket.value_ptr()) ValueType();

        m_buckets.m_slots_metadata[slot_index] = low_bucket_hash;
        ++m_buckets.m_occupied_slot_count;
        return bucket.value();
    }

    ALWAYS_INLINE ValueType& operator[](const KeyType& key) { return get_or_add(key); }

public:
    ALWAYS_INLINE void clear()
    {
        m_buckets.clear();
    }

    ALWAYS_INLINE void clear_and_shrink()
    {
        m_buckets.clear_and_shrink();
    }

public:
    NODISCARD ALWAYS_INLINE Iterator begin() { return Iterator(m_buckets.begin()); }
    NODISCARD ALWAYS_INLINE Iterator end() { return Iterator(m_buckets.end()); }

    NODISCARD ALWAYS_INLINE ConstIterator begin() const { return ConstIterator(m_buckets.begin()); }
    NODISCARD ALWAYS_INLINE ConstIterator end() const { return ConstIterator(m_buckets.end()); }

private:
    // NOTE: All operation that use the bucket created with this function must not access the
    //       bucket value, as it doesn't really exist.
    //       Because we know the bucket layout, we can assume that the key is always the first
    //       field in the structure, and it is not padded. This allows us to "cast" the key
    //       variable to a bucket, as long as we don't access the value field.
    // WARNING: Not using this API correctly *will* create annoying bugs.
    NODISCARD ALWAYS_INLINE static const Bucket& unsafe_bucket_from_key(const KeyType& key)
    {
        return *reinterpret_cast<const Bucket*>(&key);
    }

    ALWAYS_INLINE usize add_without_constructing_bucket(const KeyType& key)
    {
        m_buckets.re_allocate_if_overloaded(m_buckets.m_occupied_slot_count + 1);

        const Bucket& key_as_bucket = unsafe_bucket_from_key(key);
        const u64 bucket_hash = InternalHashTable::get_element_hash(key_as_bucket);
        const u8 low_bucket_hash = InternalHashTable::get_low_hash(bucket_hash);
        const usize slot_index =
            m_buckets.unchecked_find_element_or_first_available_slot(key_as_bucket, bucket_hash, low_bucket_hash);

        if (m_buckets.m_slots_metadata[slot_index] == low_bucket_hash)
            return invalid_size;

        m_buckets.m_slots_metadata[slot_index] = low_bucket_hash;
        ++m_buckets.m_occupied_slot_count;
        return slot_index;
    }

private:
    InternalHashTable m_buckets;
};

} // namespace SE