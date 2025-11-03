// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/HashSet.h>
#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

namespace SE
{

template<typename KeyType, typename ValueType, typename Allocator = DefaultAllocator>
requires (!std::is_const_v<KeyType> && !std::is_reference_v<KeyType> && !std::is_const_v<ValueType> && !std::is_reference_v<ValueType>)
class HashMap
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    template<typename FriendKeyType, typename FriendValueType, typename FriendAllocator>
    requires (!std::is_const_v<FriendKeyType> && !std::is_reference_v<FriendKeyType> && !std::is_const_v<FriendValueType> &&
              !std::is_reference_v<FriendValueType>)
    friend class HashMap;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    struct Bucket
    {
        Bucket& operator=(const Bucket&)     = delete;
        Bucket& operator=(Bucket&&) noexcept = delete;

    public:
        NODISCARD ALWAYS_INLINE static uint64 GetHash(const Bucket& bucket)
        {
            // Forward the implementation to the key hashing function.
            return Hash<KeyType>::Get(bucket.Key());
        }

    public:
        ALWAYS_INLINE Bucket() {}

        ALWAYS_INLINE ~Bucket()
        {
            Key().~KeyType();
            Value().~ValueType();
        }

        ALWAYS_INLINE Bucket(const Bucket& other)
        {
            new (m_KeyStorage) KeyType(other.Key());
            new (m_ValueStorage) ValueType(other.Value());
        }

        ALWAYS_INLINE Bucket(Bucket&& other) noexcept
        {
            new (m_KeyStorage) KeyType(Move(other.Key()));
            new (m_ValueStorage) ValueType(Move(other.Value()));
        }

        NODISCARD ALWAYS_INLINE bool operator==(const Bucket& other) const
        {
            // Forward the implementation to the key equality check operator.
            return (Key() == other.Key());
        }

    public:
        NODISCARD ALWAYS_INLINE KeyType* KeyPointer() { return reinterpret_cast<KeyType*>(m_KeyStorage); }
        NODISCARD ALWAYS_INLINE const KeyType* KeyPointer() const { return reinterpret_cast<const KeyType*>(m_KeyStorage); }
        NODISCARD ALWAYS_INLINE KeyType& Key() { return *KeyPointer(); }
        NODISCARD ALWAYS_INLINE const KeyType& Key() const { return *KeyPointer(); }

        NODISCARD ALWAYS_INLINE ValueType* ValuePointer() { return reinterpret_cast<ValueType*>(m_ValueStorage); }
        NODISCARD ALWAYS_INLINE const ValueType* ValuePointer() const { return reinterpret_cast<const ValueType*>(m_ValueStorage); }
        NODISCARD ALWAYS_INLINE ValueType& Value() { return *ValuePointer(); }
        NODISCARD ALWAYS_INLINE const ValueType& Value() const { return *ValuePointer(); }

    public:
        alignas(KeyType) uint8 m_KeyStorage[sizeof(KeyType)];
        alignas(ValueType) uint8 m_ValueStorage[sizeof(ValueType)];
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    using HashSetContainer = HashSet<Bucket, Allocator>;

    using BucketState      = HashSetContainer::EntryState;

#if SE_HASH_SET_CHECK_PROTECTION
    using ScopedProtectionLock = HashSetContainer::ScopedProtectionLock;
#endif // SE_HASH_SET_CHECK_PROTECTION

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename IteratorValueType = ValueType>
    struct GenericIteratorBucket
    {
        SE_MAKE_NONMOVABLE(GenericIteratorBucket);
        GenericIteratorBucket()                                        = delete;
        GenericIteratorBucket& operator=(const GenericIteratorBucket&) = delete;

    public:
        ~GenericIteratorBucket()                            = default;
        GenericIteratorBucket(const GenericIteratorBucket&) = default;

    public:
        const KeyType     Key;
        IteratorValueType Value;
    };

    using IteratorBucket      = GenericIteratorBucket<ValueType>;
    using ConstIteratorBucket = GenericIteratorBucket<const ValueType>;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename IteratorValueType = ValueType>
    class GenericIterator
    {
    public:
        ALWAYS_INLINE GenericIterator(GenericIteratorBucket<IteratorValueType>* buckets, GenericIteratorBucket<IteratorValueType>* bucketsEnd,
                                    BucketState* states)
            : m_Buckets(buckets)
            , m_BucketsEnd(bucketsEnd)
            , m_States(states)
        {}

        GenericIterator(const GenericIterator&)                = default;
        GenericIterator(GenericIterator&&) noexcept            = default;

        GenericIterator& operator=(const GenericIterator&)     = default;
        GenericIterator& operator=(GenericIterator&&) noexcept = default;

    public:
        NODISCARD ALWAYS_INLINE GenericIteratorBucket<IteratorValueType>& operator*() const
        {
            SE_ASSERT(m_Buckets != m_BucketsEnd);
            SE_ASSERT(*m_States == BucketState::Occupied);
            return *m_Buckets;
        }

        NODISCARD ALWAYS_INLINE GenericIteratorBucket<IteratorValueType>* operator->() const
        {
            SE_ASSERT(m_Buckets != m_BucketsEnd);
            SE_ASSERT(*m_States == BucketState::Occupied);
            return m_Buckets;
        }

        // Pre-increment operator.
        ALWAYS_INLINE GenericIterator& operator++()
        {
            while (m_Buckets != m_BucketsEnd)
            {
                ++m_Buckets;
                ++m_States;

                if (*m_States == BucketState::Occupied)
                    break;
            }

            return *this;
        }

        // Post-increment operator.
        ALWAYS_INLINE GenericIterator operator++(int)
        {
            GenericIterator preIncrementValue = *this;
            this->operator++();
            return preIncrementValue;
        }

        NODISCARD ALWAYS_INLINE bool operator==(const GenericIterator& other) const { return (m_Buckets == other.m_Buckets); }

        NODISCARD ALWAYS_INLINE bool operator!=(const GenericIterator& other) const { return (m_Buckets != other.m_Buckets); }

    private:
        GenericIteratorBucket<IteratorValueType>* m_Buckets;
        GenericIteratorBucket<IteratorValueType>* m_BucketsEnd;
        BucketState*                              m_States;
    };

    using Iterator      = GenericIterator<ValueType>;
    using ConstIterator = GenericIterator<const ValueType>;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD ALWAYS_INLINE static const Bucket& GetBucketFromKey(const KeyType& key) { return reinterpret_cast<const Bucket&>(key); }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    ALWAYS_INLINE HashMap()                              = default;
    ALWAYS_INLINE ~HashMap()                             = default;

    ALWAYS_INLINE HashMap(const HashMap&)                = default;
    ALWAYS_INLINE HashMap(HashMap&&) noexcept            = default;

    ALWAYS_INLINE HashMap& operator=(const HashMap&)     = default;
    ALWAYS_INLINE HashMap& operator=(HashMap&&) noexcept = default;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename OtherAllocator>
    ALWAYS_INLINE HashMap(const HashMap<KeyType, ValueType, OtherAllocator>& other)
        : m_Buckets(other.m_Buckets)
    {}

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename OtherAllocator>
    ALWAYS_INLINE HashMap& operator=(const HashMap<KeyType, ValueType, OtherAllocator>& other)
    {
        m_Buckets = other.m_Buckets;
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    NODISCARD ALWAYS_INLINE usize Count() const { return m_Buckets.Count(); }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD ALWAYS_INLINE bool IsEmpty() const { return (Count() == 0); }
    NODISCARD ALWAYS_INLINE bool HasElements() const { return (Count() > 0); }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    NODISCARD ALWAYS_INLINE bool Contains(const KeyType& key) const
    {
        const Bucket& bucket = GetBucketFromKey(key);
        return m_Buckets.Contains(bucket);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD ALWAYS_INLINE Optional<ValueType&> GetIfExists(const KeyType& key)
    {
#if SE_HASH_SET_CHECK_PROTECTION
        // NOTE(Traian): Since this function doesn't create or destroy any buckets, there is no need to acquire a
        // protection lock, since no external function are called.
        SE_ENSURE(m_Buckets.IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        const Bucket&         keyBucket        = GetBucketFromKey(key);
        const uint64          keyBucketHash    = Bucket::GetHash(keyBucket);
        const Optional<usize> bucketEntryIndex = m_Buckets.GetEntryIndexOf(keyBucket, keyBucketHash);
        if (bucketEntryIndex.HasValue())
            return m_Buckets.m_Entries.Slots[bucketEntryIndex.Value()].Value();
        return {};
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD ALWAYS_INLINE Optional<const ValueType&> GetIfExists(const KeyType& key) const
    {
#if SE_HASH_SET_CHECK_PROTECTION
        // NOTE(Traian): Since this function doesn't create or destroy any buckets, there is no need to acquire a
        // protection lock, since no external function are called.
        SE_ENSURE(m_Buckets.IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        const Bucket&         keyBucket        = GetBucketFromKey(key);
        const uint64          keyBucketHash    = Bucket::GetHash(keyBucket);
        const Optional<usize> bucketEntryIndex = m_Buckets.GetEntryIndexOf(keyBucket, keyBucketHash);
        if (bucketEntryIndex.HasValue())
            return m_Buckets.m_Entries.Slots[bucketEntryIndex.Value()].Value();
        return {};
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD ALWAYS_INLINE ValueType& At(const KeyType& key)
    {
        Optional<ValueType&> value = GetIfExists(key);
        SE_ASSERT(value.HasValue());
        return value.Value();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD ALWAYS_INLINE const ValueType& At(const KeyType& key) const
    {
        Optional<const ValueType&> value = GetIfExists(key);
        SE_ASSERT(value.HasValue());
        return value.Value();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    ALWAYS_INLINE ValueType& Add(const KeyType& key, const ValueType& value) { return AddImplementation<const KeyType&, const ValueType&>(key, value); }

    ALWAYS_INLINE ValueType& Add(const KeyType& key, ValueType&& value) { return AddImplementation<const KeyType&, ValueType&&>(key, Move(value)); }

    ALWAYS_INLINE ValueType& Add(KeyType&& key, const ValueType& value) { return AddImplementation<KeyType&&, const ValueType&>(Move(key), value); }

    ALWAYS_INLINE ValueType& Add(KeyType&& key, ValueType&& value) { return AddImplementation<KeyType&&, ValueType&&>(Move(key), Move(value)); }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    NODISCARD ALWAYS_INLINE ValueType& operator[](const KeyType& key) { return GetOrAddImplementation<const KeyType&>(key); }

    NODISCARD ALWAYS_INLINE ValueType& operator[](KeyType&& key) { return GetOrAddImplementation<KeyType&&>(Move(key)); }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    ALWAYS_INLINE void RemoveIfExist(const KeyType& key)
    {
        const Bucket& keyBucket = GetBucketFromKey(key);
        m_Buckets.RemoveIfExists(keyBucket);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ALWAYS_INLINE void RemoveUnchecked(const KeyType& key)
    {
        const Bucket& keyBucket = GetBucketFromKey(key);
        m_Buckets.RemoveUnchecked(keyBucket);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    ALWAYS_INLINE void EnsureCapacity(usize capacity) { m_Buckets.EnsureCapacity(capacity); }

    ALWAYS_INLINE void Clear() { m_Buckets.Clear(); }

    ALWAYS_INLINE void ShrinkToFit() { m_Buckets.ShrinkToFit(); }

    ALWAYS_INLINE void ClearAndShrink() { m_Buckets.ClearAndShrink(); }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    NODISCARD ALWAYS_INLINE Iterator IteratorAt(usize bucketEntryIndex)
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(m_Buckets.IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        return Iterator(reinterpret_cast<IteratorBucket*>(m_Buckets.m_Entries.Slots + bucketEntryIndex),
                        reinterpret_cast<IteratorBucket*>(m_Buckets.m_Entries.Slots + m_Buckets.m_Entries.Count),
                        m_Buckets.m_Entries.States + bucketEntryIndex);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD ALWAYS_INLINE ConstIterator IteratorAt(usize bucketEntryIndex) const
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(m_Buckets.IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        return ConstIterator(reinterpret_cast<ConstIteratorBucket*>(m_Buckets.m_Entries.Slots + bucketEntryIndex),
                             reinterpret_cast<ConstIteratorBucket*>(m_Buckets.m_Entries.Slots + m_Buckets.m_Entries.Count),
                             m_Buckets.m_Entries.States + bucketEntryIndex);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD ALWAYS_INLINE Iterator begin()
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(m_Buckets.IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        if (m_Buckets.m_Count == 0)
            return end();

        Optional<usize> firstOccupiedEntryIndex;
        for (usize bucketEntryIndex = 0; bucketEntryIndex < m_Buckets.m_Entries.Count; ++bucketEntryIndex)
        {
            if (m_Buckets.m_Entries.States[bucketEntryIndex] == BucketState::Occupied)
            {
                firstOccupiedEntryIndex = bucketEntryIndex;
                break;
            }
        }

        // NOTE(Traian): Since we checked that the hash set has at least one occupied entry, this
        // assert is never triggered by the user incorrectly using the API. Instead, it is an
        // internal container error.
        SE_ENSURE(firstOccupiedEntryIndex.HasValue());
        return IteratorAt(firstOccupiedEntryIndex.Value());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD ALWAYS_INLINE ConstIterator begin() const
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(m_Buckets.IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        if (m_Buckets.m_Count == 0)
            return end();

        Optional<usize> firstOccupiedEntryIndex;
        for (usize bucketEntryIndex = 0; bucketEntryIndex < m_Buckets.m_Entries.Count; ++bucketEntryIndex)
        {
            if (m_Buckets.m_Entries.States[bucketEntryIndex] == BucketState::Occupied)
            {
                firstOccupiedEntryIndex = bucketEntryIndex;
                break;
            }
        }

        // NOTE(Traian): Since we checked that the hash set has at least one occupied entry, this
        // assert is never triggered by the user incorrectly using the API. Instead, it is an
        // internal container error.
        SE_ENSURE(firstOccupiedEntryIndex.HasValue());
        return IteratorAt(firstOccupiedEntryIndex.Value());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD ALWAYS_INLINE Iterator end()
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(m_Buckets.IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        return IteratorAt(m_Buckets.m_Entries.Count);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD ALWAYS_INLINE ConstIterator end() const
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(m_Buckets.IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        return IteratorAt(m_Buckets.m_Entries.Count);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    template<typename KeyParameterType, typename ValueParameterType>
    requires (std::is_same_v<RemoveConst<RemoveReference<RemoveConst<KeyParameterType>>>, KeyType> &&
              std::is_same_v<RemoveConst<RemoveReference<RemoveConst<ValueParameterType>>>, ValueType>)
    ALWAYS_INLINE ValueType& AddImplementation(KeyParameterType key, ValueParameterType value)
    {
        // TODO(Traian): Combine the search for the key with the search for the first available index
        // into a single operation to avoid a redundant hash evaluation and entries iteration.
        SE_ASSERT(!Contains(key));

#if SE_HASH_SET_CHECK_PROTECTION
        // NOTE(Traian): Since 'GetEntryIndexOf' is an internal hash set function, it doesn't check or acquire the protection lock.
        SE_ENSURE(m_Buckets.IsNotProtected());
        ScopedProtectionLock protectionLock(m_Buckets);
#endif // SE_HASH_SET_CHECK_PROTECTION

        // Expand the internal memory buffer if necessary.
        const usize requiredEntryCount = HashSetContainer::CalculateRequiredEntryCount(m_Buckets.m_Count + 1);
        if (requiredEntryCount > m_Buckets.m_Entries.Count)
        {
            m_Buckets.ReAllocateEntriesBuffer(requiredEntryCount);
        }

        // Find the first available index where to construct the bucket.
        const Bucket&         keyBucket        = GetBucketFromKey(key);
        const uint64          keyBucketHash    = Bucket::GetHash(keyBucket);
        const Optional<usize> bucketEntryIndex = m_Buckets.GetFirstAvailableEntryIndex(keyBucket, keyBucketHash);
        SE_ASSERT(bucketEntryIndex.HasValue());
        const usize entryIndex = bucketEntryIndex.Value();

        // Construct the bucket.
        Bucket& bucket                         = m_Buckets.m_Entries.Slots[entryIndex];
        m_Buckets.m_Entries.States[entryIndex] = HashSetContainer::EntryState::Occupied;
        new (bucket.KeyPointer()) KeyType(Move(key));
        new (bucket.ValuePointer()) ValueType(Move(value));
        m_Buckets.m_Count++;
        return bucket.Value();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename KeyParameterType>
    // requires (std::is_same_v<RemoveReference<RemoveConst<KeyParameterType>>, KeyType>)
    NODISCARD ALWAYS_INLINE ValueType& GetOrAddImplementation(KeyParameterType key)
    {
#if SE_HASH_SET_CHECK_PROTECTION
        // NOTE(Traian): Since 'GetEntryIndexOf' is an internal hash set function, it doesn't check or acquire the protection lock.
        SE_ENSURE(m_Buckets.IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        const Bucket&   keyBucket        = GetBucketFromKey(key);
        const uint64    keyBucketHash    = Bucket::GetHash(keyBucket);
        Optional<usize> bucketEntryIndex = m_Buckets.GetEntryIndexOfOrFirstAvailable(keyBucket, keyBucketHash);

        const usize requiredEntryCount   = HashSetContainer::CalculateRequiredEntryCount(m_Buckets.m_Count + 1);

        if (bucketEntryIndex.HasValue())
        {
            // Check if the key exists.
            const usize entryIndex = bucketEntryIndex.Value();
            if (m_Buckets.m_Entries.States[entryIndex] == HashSetContainer::EntryState::Occupied)
                return m_Buckets.m_Entries.Slots[entryIndex].Value();

            // Check if the container can store another bucket without exceeding the max load factor. If so,
            // since we already have the first available index, we can create the bucket.
            if (requiredEntryCount <= m_Buckets.m_Entries.Count)
            {
#if SE_HASH_SET_CHECK_PROTECTION
                ScopedProtectionLock protectionLock(m_Buckets);
#endif // SE_HASH_SET_CHECK_PROTECTION

                // Construct the bucket.
                Bucket& bucket                         = m_Buckets.m_Entries.Slots[entryIndex];
                m_Buckets.m_Entries.States[entryIndex] = HashSetContainer::EntryState::Occupied;
                new (bucket.KeyPointer()) KeyType(key);
                new (bucket.ValuePointer()) ValueType();
                m_Buckets.m_Count++;
                return bucket.Value();
            }
        }

#if SE_HASH_SET_CHECK_PROTECTION
        ScopedProtectionLock protectionLock(m_Buckets);
#endif // SE_HASH_SET_CHECK_PROTECTION

        // NOTE(Traian): Since we are in this codepath, it means that the key doesn't already exists in the set,
        // and either there are no available slots, either constructing a new entry would exceed the max load factor.
        // In this case, we must expand the internal memory buffer.
        m_Buckets.ReAllocateEntriesBuffer(requiredEntryCount);

        // Find the first available index where to construct the bucket.
        bucketEntryIndex = m_Buckets.GetFirstAvailableEntryIndex(keyBucket, keyBucketHash);
        SE_ASSERT(bucketEntryIndex.HasValue());
        const usize entryIndex = bucketEntryIndex.Value();

        // Construct the bucket.
        Bucket& bucket                         = m_Buckets.m_Entries.Slots[entryIndex];
        m_Buckets.m_Entries.States[entryIndex] = HashSetContainer::EntryState::Occupied;
        new (bucket.KeyPointer()) KeyType(Move(key));
        new (bucket.ValuePointer()) ValueType();
        m_Buckets.m_Count++;
        return bucket.Value();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    HashSetContainer m_Buckets;
};

} // namespace SE
