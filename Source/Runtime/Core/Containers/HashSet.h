// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/Allocator.h>
#include <Runtime/Core/Containers/Optional.h>
#include <Runtime/Core/CoreAssertions.h>
#include <Runtime/Core/CoreTypes.h>

namespace SE
{

template<typename T>
concept HasGetHash = requires (const T& value)
{
    // Requires that T has a static method 'GetHash' that takes a const reference to some object instance
    // of type T and returns something that can be converted to a 64-bit unsigned integer.
    { T::GetHash(value) };
};

template<typename T>
struct Hash
{
    NODISCARD FORCEINLINE static uint64 Get(const T&)
    {
        static_assert(false, "You must implement T::GetHash(const T&) or specialize the Hash<T> structure!");
        return 0;
    }
};

template<HasGetHash T>
struct Hash<T>
{
    NODISCARD FORCEINLINE static uint64 Get(const T& value)
    {
        return T::GetHash(value);
    }
};

template<> struct Hash<int64>   { NODISCARD FORCEINLINE static uint64 Get(const int64& value)   { return static_cast<uint64>(value); } };
template<> struct Hash<uint64>  { NODISCARD FORCEINLINE static uint64 Get(const uint64& value)  { return static_cast<uint64>(value); } };
template<> struct Hash<Float64> { NODISCARD FORCEINLINE static uint64 Get(const Float64& value) { return *reinterpret_cast<const uint64*>(&value); } };
template<> struct Hash<Bool8>   { NODISCARD FORCEINLINE static uint64 Get(const Bool8& value)   { return static_cast<uint64>(value); } };

template<> struct Hash<uint8>   { NODISCARD FORCEINLINE static uint64 Get(const uint8& value)   { return Hash<uint64>::Get(static_cast<uint64>(value)); } };
template<> struct Hash<uint16>  { NODISCARD FORCEINLINE static uint64 Get(const uint16& value)  { return Hash<uint64>::Get(static_cast<uint64>(value)); } };
template<> struct Hash<uint32>  { NODISCARD FORCEINLINE static uint64 Get(const uint32& value)  { return Hash<uint64>::Get(static_cast<uint64>(value)); } };

template<> struct Hash<int8>    { NODISCARD FORCEINLINE static uint64 Get(const int8& value)    { return Hash<int64>::Get(static_cast<int64>(value)); } };
template<> struct Hash<int16>   { NODISCARD FORCEINLINE static uint64 Get(const int16& value)   { return Hash<int64>::Get(static_cast<int64>(value)); } };
template<> struct Hash<int32>   { NODISCARD FORCEINLINE static uint64 Get(const int32& value)   { return Hash<int64>::Get(static_cast<int64>(value)); } };

template<> struct Hash<Float32> { NODISCARD FORCEINLINE static uint64 Get(const Float32& value) { return Hash<Float64>::Get(static_cast<Float64>(value)); } };

template<typename T>
requires(std::is_enum_v<T>)
struct Hash<T>
{
    NODISCARD FORCEINLINE static uint64 Get(const T& value)
    {
        return Hash<uint64>::Get(static_cast<uint64>(value));
    }
};

template<typename T>
requires(std::is_pointer_v<T>)
struct Hash<T>
{
    NODISCARD FORCEINLINE static uint64 Get(const T& value)
    {
        return Hash<uint64>::Get(reinterpret_cast<uint64>(value));
    }
};

//
// When performing certain operations, containers can invoke arbitrary constructors and destructors, which might in turn
// call an arbitrary function of the same container. For example, calling the 'RemoveIfExists' method will invoke the object
// destructor, which might call the 'Add' method! Since the hash set might be in an intermediate state when calling the 'Add'
// method, it could cause undefined behaviour. Since it might be hard for the developer to always avoid operations on the
// same hash set that started the call-stack, a protection system is implemented, which triggers an assert when such a
// situation is encountered. Enable it by setting the following macro to 1.
//
#define SE_HASH_SET_CHECK_PROTECTION 1

template<typename ElementType, typename Allocator = DefaultAllocator>
requires (!std::is_const_v<ElementType> && !std::is_reference_v<ElementType>)
class HashSet
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    template<typename FriendElementType, typename FriendAllocator>
    requires (!std::is_const_v<FriendElementType> && !std::is_reference_v<FriendElementType>)
    friend class HashSet;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename KeyType, typename ValueType, typename FriendAllocator>
    requires (
        !std::is_const_v<KeyType> && !std::is_reference_v<KeyType> &&
        !std::is_const_v<ValueType> && !std::is_reference_v<ValueType>
    )
    friend class HashMap;

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    static constexpr usize GROWTH_FACTOR_NUMERATOR = 3;
    static constexpr usize GROWTH_FACTOR_DENOMINATOR = 2;
    static_assert(GROWTH_FACTOR_NUMERATOR > GROWTH_FACTOR_DENOMINATOR);
    static_assert(GROWTH_FACTOR_DENOMINATOR != 0);
    
    static constexpr usize MAX_LOAD_FACTOR_NUMERATOR = 3;
    static constexpr usize MAX_LOAD_FACTOR_DENOMINATOR = 4;
    static_assert(MAX_LOAD_FACTOR_NUMERATOR <= MAX_LOAD_FACTOR_DENOMINATOR);
    static_assert(MAX_LOAD_FACTOR_NUMERATOR != 0 && MAX_LOAD_FACTOR_DENOMINATOR != 0);

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class EntryState : uint8
    {
        Empty = 0,
        Occupied = 1,
        Deleted = 2,
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct EntriesBuffer
    {
        usize       Count  { 0 };
        ElementType*          Slots  { nullptr };
        EntryState* States { nullptr };
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class Iterator
    {
    public:
        FORCEINLINE Iterator(const ElementType* slots, const ElementType* slotsEnd, EntryState* states)
            : m_Slots(slots)
            , m_SlotsEnd(slotsEnd)
            , m_States(states)
        {}

        Iterator(const Iterator&) = default;
        Iterator(Iterator&&) noexcept = default;

        Iterator& operator=(const Iterator&) = default;
        Iterator& operator=(Iterator&&) noexcept = default;

    public:
        NODISCARD FORCEINLINE const ElementType& operator*() const
        {
            SE_ASSERT(m_Slots != m_SlotsEnd);
            SE_ASSERT(*m_States == EntryState::Occupied);
            return *m_Slots;
        }

        NODISCARD FORCEINLINE const ElementType* operator->() const
        {
            SE_ASSERT(m_Slots != m_SlotsEnd);
            SE_ASSERT(*m_States == EntryState::Occupied);
            return m_Slots;
        }

        // Pre-increment operator.
        FORCEINLINE Iterator& operator++()
        {
            if (m_Slots == m_SlotsEnd)
                return *this;

            while (true)
            {
                ++m_Slots;
                ++m_States;

                if (m_Slots == m_SlotsEnd)
                    break;

                if (*m_States == EntryState::Occupied)
                    break;
            }

            return *this;
        }

        // Post-increment operator.
        FORCEINLINE Iterator operator++(int)
        {
            Iterator preIncrementValue = *this;
            this->operator++();
            return preIncrementValue;
        }

        NODISCARD FORCEINLINE bool operator==(const Iterator& other) const
        {
            return (m_Slots == other.m_Slots);
        }

        NODISCARD FORCEINLINE bool operator!=(const Iterator& other) const
        {
            return (m_Slots != other.m_Slots);
        }

    private:
        const ElementType* m_Slots;
        const ElementType* m_SlotsEnd;
        EntryState* m_States;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if SE_HASH_SET_CHECK_PROTECTION
    class ScopedProtectionLock
    {
        SE_MAKE_NONCOPYABLE(ScopedProtectionLock);
        SE_MAKE_NONMOVABLE(ScopedProtectionLock);

    public:
        FORCEINLINE ScopedProtectionLock(const HashSet<ElementType>& hashSet)
            : m_HashSet(hashSet)
        {
            SE_ENSURE(m_HashSet.IsNotProtected());
            m_HashSet.SetIsProtected(true);
        }

        FORCEINLINE ~ScopedProtectionLock()
        {
            SE_ENSURE(m_HashSet.IsProtected());
            m_HashSet.SetIsProtected(false);
        }

    private:
        const HashSet<ElementType>& m_HashSet;
    };
#endif // SE_HASH_SET_CHECK_PROTECTION

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if SE_HASH_SET_CHECK_PROTECTION
    template<typename OtherAllocator>
    using OtherScopedProtectionLock = HashSet<ElementType, OtherAllocator>::ScopedProtectionLock;
#endif // SE_HASH_SET_CHECK_PROTECTION

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    FORCEINLINE HashSet()
        : m_Count(0)
        , m_IsProtected(0)
    {}

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE ~HashSet()
    {
        ClearAndShrink();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename OtherAllocatorType>
    FORCEINLINE HashSet(const HashSet<ElementType, OtherAllocatorType>& other)
        : m_Count(0)
        , m_IsProtected(0)
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(other.IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        if (other.m_Count)
        {
            const usize requiredEntryCount = CalculateRequiredEntryCount(other.m_Count);
            // NOTE(Traian): We don't lock the hash set before re-allocating the internal buffer because we can be sure
            // that no elements are currently stored in the container, and thus no constructors or destructors will be invoked.
            ReAllocateEntriesBuffer(requiredEntryCount);

#if SE_HASH_SET_CHECK_PROTECTION
            // NOTE(Traian): Since we iterate over the other's entries, we should lock it.
            OtherScopedProtectionLock<OtherAllocatorType> otherProtectionLock(other);
#endif // SE_HASH_SET_CHECK_PROTECTION

            m_Count = other.m_Count;
            for (usize otherEntryIndex = 0; otherEntryIndex < other.m_Entries.Count; ++otherEntryIndex)
            {
                if (other.m_Entries.States[otherEntryIndex] == EntryState::Occupied)
                {
                    const ElementType& value = other.m_Entries.Slots[otherEntryIndex];
                    Add(value);
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE HashSet(const HashSet& other)
        : m_Count(0)
        , m_IsProtected(0)
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(other.IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        if (other.m_Count)
        {
            const usize requiredEntryCount = CalculateRequiredEntryCount(other.m_Count);
            // NOTE(Traian): We don't lock the hash set before re-allocating the internal buffer because we can be sure
            // that no elements are currently stored in the container, and thus no constructors or destructors will be invoked.
            ReAllocateEntriesBuffer(requiredEntryCount);

#if SE_HASH_SET_CHECK_PROTECTION
            // NOTE(Traian): Since we iterate over the other's entries, we should lock it.
            OtherScopedProtectionLock<Allocator> otherProtectionLock(other);
#endif // SE_HASH_SET_CHECK_PROTECTION

            m_Count = other.m_Count;
            for (usize otherEntryIndex = 0; otherEntryIndex < other.m_Entries.Count; ++otherEntryIndex)
            {
                if (other.m_Entries.States[otherEntryIndex] == EntryState::Occupied)
                {
                    const ElementType& value = other.m_Entries.Slots[otherEntryIndex];
                    Add(value);
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE HashSet(HashSet&& other) noexcept
        : m_Count(0)
        , m_IsProtected(0)
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(other.IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        m_Entries = other.m_Entries;
        m_Count = other.m_Count;

        other.m_Entries = {};
        other.m_Count = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template<typename OtherAllocator>
    FORCEINLINE HashSet& operator=(const HashSet<ElementType, OtherAllocator>& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        // NOTE(Traian): Since in this function we never worked with the raw elements buffer directly,
        // and instead use other functions part of the public API (which acquire their own locks),
        // there is no need and we even can't acquire the protection lock!

#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(other.IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        Clear();

        if (other.m_Count > 0)
        {
            const usize requiredEntryCount = CalculateRequiredEntryCount(other.m_Count);
            if (requiredEntryCount > m_Entries.Count)
            {
                // NOTE(Traian): We don't lock the hash set before re-allocating the internal buffer because we can be sure
                // that no elements are currently stored in the container, and thus no constructors or destructors will be invoked.
                ReAllocateEntriesBuffer(requiredEntryCount);
            }

#if SE_HASH_SET_CHECK_PROTECTION
            // NOTE(Traian): Since we iterate over the other's entries, we should lock it.
            OtherScopedProtectionLock<OtherAllocator> otherProtectionLock(other);
#endif // SE_HASH_SET_CHECK_PROTECTION

            for (usize otherEntryIndex = 0; otherEntryIndex < other.m_Entries.Count; ++otherEntryIndex)
            {
                if (other.m_Entries.States[otherEntryIndex] == EntryState::Occupied)
                {
                    const ElementType& value = other.m_Entries.Slots[otherEntryIndex];
                    Add(value);
                }
            }
        }

        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE HashSet& operator=(const HashSet& other)
    {
        // Forward the implementation to the templated version.
        return this->operator=<Allocator>(other);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE HashSet& operator=(HashSet&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

#if SE_HASH_SET_CHECK_PROTECTION
        // NOTE(Traian): We don't acquire any protection locks because outside the 'Clear' function,
        // which acquires its own protection lock, no external constructors/destructors are called.
        SE_ENSURE(other.IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        Clear();

        m_Entries = other.m_Entries;
        m_Count = other.m_Count;

        other.m_Entries = {};
        other.m_Count = 0;

        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    NODISCARD FORCEINLINE usize Count() const
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        return static_cast<usize>(m_Count);
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE bool IsEmpty() const { return (Count() == 0); }
    NODISCARD FORCEINLINE bool HasElements() const { return (Count() > 0); }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    NODISCARD FORCEINLINE bool Contains(const ElementType& element) const
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        const uint64 elementHash = Hash<ElementType>::Get(element);
        const Optional<usize> entryIndex = GetEntryIndexOf(element, elementHash);
        return entryIndex.HasValue();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    FORCEINLINE void Add(const ElementType& element)
    {
        // TODO(Traian): Combine the search for the element with the search for the first available index
        // into a single operation to avoid a redundant hash evaluation and entries iteration.
        if (Contains(element))
            return;

        EnsureCapacity(m_Count + 1);

#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_HASH_SET_CHECK_PROTECTION

        const uint64 elementHash = Hash<ElementType>::Get(element);
        const Optional<usize> entryIndex = GetFirstAvailableEntryIndex(element, elementHash);
        SE_ASSERT(entryIndex.HasValue());

        const usize entryIndexValue = entryIndex.Value();
        m_Entries.States[entryIndexValue] = EntryState::Occupied;
        new (m_Entries.Slots + entryIndexValue) ElementType(element);
        ++m_Count;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void Add(ElementType&& element)
    {
        // TODO(Traian): Combine the search for the element with the search for the first available index
        // into a single operation to avoid a redundant hash evaluation and entries iteration.
        if (Contains(element))
            return;

        EnsureCapacity(m_Count + 1);

#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_HASH_SET_CHECK_PROTECTION

        const uint64 elementHash = Hash<ElementType>::Get(element);
        const Optional<usize> entryIndex = GetFirstAvailableEntryIndex(element, elementHash);
        SE_ASSERT(entryIndex.HasValue());

        const usize entryIndexValue = entryIndex.Value();
        m_Entries.States[entryIndexValue] = EntryState::Occupied;
        new (m_Entries.Slots + entryIndexValue) ElementType(Move(element));
        ++m_Count;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void Add(std::initializer_list<ElementType> elements)
    {
        EnsureCapacity(m_Count + elements.size());

        for (usize index = 0; index < elements.size(); ++index)
            Add(elements.begin()[index]);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    FORCEINLINE void RemoveIfExists(const ElementType& element)
    {
        const uint64 elementHash = Hash<ElementType>::Get(element);
        const Optional<usize> entryIndex = GetEntryIndexOf(element, elementHash);
        if (entryIndex.HasValue())
        {
#if SE_HASH_SET_CHECK_PROTECTION
            SE_ENSURE(IsNotProtected());
            ScopedProtectionLock protectionLock(*this);
#endif // SE_HASH_SET_CHECK_PROTECTION

            const usize entryIndexValue = entryIndex.Value();
            m_Entries.States[entryIndexValue] = EntryState::Deleted;
            m_Entries.Slots[entryIndexValue].~ElementType();
            --m_Count;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void RemoveUnchecked(const ElementType& element)
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_HASH_SET_CHECK_PROTECTION

        const uint64 elementHash = Hash<ElementType>::Get(element);
        const usize entryIndex = GetEntryIndexOfUnchecked(element, elementHash);
        m_Entries.States[entryIndex] = EntryState::Deleted;
        m_Entries.Slots[entryIndex].~ElementType();
        --m_Count;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    FORCEINLINE void EnsureCapacity(usize capacity)
    {
        const usize requiredEntryCount = CalculateRequiredEntryCount(capacity);
        if (requiredEntryCount > m_Entries.Count)
        {
#if SE_HASH_SET_CHECK_PROTECTION
            SE_ENSURE(IsNotProtected());
            ScopedProtectionLock protectionLock(*this);
#endif // SE_HASH_SET_CHECK_PROTECTION

            const usize newEntryCount = CalculateNextEntryCount(m_Entries.Count, requiredEntryCount);
            ReAllocateEntriesBuffer(newEntryCount);
        }
    }

    FORCEINLINE void ShrinkToFit()
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_HASH_SET_CHECK_PROTECTION

        const usize newEntryCount = CalculateRequiredEntryCount(m_Count);
        if (newEntryCount < m_Entries.Count)
        {
            ReAllocateEntriesBuffer(newEntryCount);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void Clear()
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_HASH_SET_CHECK_PROTECTION

        if (m_Count > 0)
        {
            for (uint32 entryIndex = 0; entryIndex < m_Entries.Count; ++entryIndex)
            {
                if (m_Entries.States[entryIndex] == EntryState::Occupied)
                    m_Entries.Slots[entryIndex].~ElementType();
                m_Entries.States[entryIndex] = EntryState::Empty;
            }

            m_Count = 0;
        }
        else
        {
            for (uint32 entryIndex = 0; entryIndex < m_Entries.Count; ++entryIndex)
                m_Entries.States[entryIndex] = EntryState::Empty;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void ClearAndShrink()
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
        ScopedProtectionLock protectionLock(*this);
#endif // SE_HASH_SET_CHECK_PROTECTION

        if (m_Count > 0)
        {
            for (usize entryIndex = 0; entryIndex < m_Entries.Count; ++entryIndex)
            {
                if (m_Entries.States[entryIndex] == EntryState::Occupied)
                    m_Entries.Slots[entryIndex].~ElementType();
            }
        }

        HashSet::ReleaseEntriesBuffer(m_Entries);
        m_Entries = {};
        m_Count = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    NODISCARD FORCEINLINE static usize CalculateRequiredEntryCount(usize count)
    {
        const usize requiredEntryCount = (count * MAX_LOAD_FACTOR_DENOMINATOR) / MAX_LOAD_FACTOR_NUMERATOR;
        return requiredEntryCount;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE static usize CalculateNextEntryCount(usize currentEntryCount, usize requiredEntryCount)
    {
        const usize nextGeometricEntryCount = (currentEntryCount * GROWTH_FACTOR_NUMERATOR) / GROWTH_FACTOR_DENOMINATOR;
        if (nextGeometricEntryCount >= requiredEntryCount)
            return nextGeometricEntryCount;
        return requiredEntryCount;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE static EntriesBuffer AllocateEntriesBuffer(usize entryCount)
    {
        if (entryCount == 0)
            return {};

        const usize entryByteCount = sizeof(ElementType) + sizeof(EntryState);
        const usize allocationByteCount = entryCount * entryByteCount;

        EntriesBuffer entriesBuffer = {};
        entriesBuffer.Count = entryCount;
        entriesBuffer.Slots = static_cast<ElementType*>(Allocator::Allocate(allocationByteCount));
        entriesBuffer.States = reinterpret_cast<EntryState*>(entriesBuffer.Slots + entryCount);
        return entriesBuffer;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE static void ReleaseEntriesBuffer(EntriesBuffer entriesBuffer)
    {
        if (entriesBuffer.Count == 0)
            return;

        MAYBE_UNUSED const usize entryByteCount = sizeof(ElementType) + sizeof(EntryState);
        MAYBE_UNUSED const usize allocationByteCount = entriesBuffer.Count * entryByteCount;
        Allocator::Release(entriesBuffer.Slots);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    NODISCARD FORCEINLINE Iterator IteratorAt(usize entryIndex) const
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        return Iterator(
            m_Entries.Slots + entryIndex,      // Starting slot.
            m_Entries.Slots + m_Entries.Count, // Ending slot.
            m_Entries.States + entryIndex      // Starting state.
        );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE Iterator begin() const
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        if (m_Count == 0)
            return end();

        Optional<usize> firstOccupiedEntryIndex;
        for (usize entryIndex = 0; entryIndex < m_Entries.Count; ++entryIndex)
        {
            if (m_Entries.States[entryIndex] == EntryState::Occupied)
            {
                firstOccupiedEntryIndex = entryIndex;
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

    NODISCARD FORCEINLINE Iterator end() const
    {
#if SE_HASH_SET_CHECK_PROTECTION
        SE_ENSURE(IsNotProtected());
#endif // SE_HASH_SET_CHECK_PROTECTION

        return IteratorAt(m_Entries.Count);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    NODISCARD FORCEINLINE Optional<usize> GetEntryIndexOf(const ElementType& element, uint64 elementHash) const
    {
        // Check if the set has any elements at all.
        if (m_Entries.Count == 0 || m_Count == 0)
            return {};

        usize entryIndex = elementHash % m_Entries.Count;

        usize tryCounter = 0;
        while (tryCounter++ < m_Entries.Count)
        {
            if (m_Entries.States[entryIndex] == EntryState::Occupied)
            {
                if (m_Entries.Slots[entryIndex] == element)
                    return entryIndex;
            }

            // NOTE(Traian): Because the insertion algorithm starts from the index calculated by modulo-ing the hash
            // value of the element by the number of entries and increments this index until an empty entry is found,
            // it is guaranteed that the element will not appear after the first empty entry encountered.
            if (m_Entries.States[entryIndex] == EntryState::Empty)
                return {};

            // Increment (and wrap if neccessary) the entry index.
            entryIndex = (entryIndex + 1) % m_Entries.Count;
        }

        // All entries have been checked.
        return {};
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE usize GetEntryIndexOfUnchecked(const ElementType& element, uint64 elementHash) const
    {
        usize entryIndex = elementHash % m_Entries.Count;
        while (true)
        {
            if (m_Entries.States[entryIndex] == EntryState::Occupied && m_Entries.Slots[entryIndex] == element)
                return entryIndex;
            entryIndex = (entryIndex + 1) % m_Entries.Count;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE Optional<usize> GetFirstAvailableEntryIndex(const ElementType& element, uint64 elementHash) const
    {
        // Check if the set has any available slots.
        if (m_Count == m_Entries.Count)
            return {};

        usize entryIndex = elementHash % m_Entries.Count;

        usize tryCounter = 0;
        while (tryCounter++ < m_Entries.Count)
        {
            if (m_Entries.States[entryIndex] != EntryState::Occupied)
                return entryIndex;

            // Increment (and wrap if neccessary) the entry index.
            entryIndex = (entryIndex + 1) % m_Entries.Count;
        }

        // NOTE(Traian): Since we checked that 'm_Count' is less than 'm_Entries.Count', and we checked all entry states,
        // it means that either 'm_Count' is out-dated, or a bucket set is not set correctly - both are internal container errors.
        SE_ASSERT_NOT_REACHED;
        return {};
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NODISCARD FORCEINLINE Optional<usize> GetEntryIndexOfOrFirstAvailable(const ElementType& element, uint64 elementHash) const
    {
        // Check if the set has any elements at all.
        if (m_Entries.Count == 0 || m_Count == 0)
            return {};

        usize entryIndex = elementHash % m_Entries.Count;
        Optional<usize> firstAvailableEntryIndex;

        usize tryCounter = 0;
        while (tryCounter++ < m_Entries.Count)
        {
            if (m_Entries.States[entryIndex] == EntryState::Occupied)
            {
                if (m_Entries.Slots[entryIndex] == element)
                    return entryIndex;
            }
            else if (m_Entries.States[entryIndex] == EntryState::Empty)
            {
                // NOTE(Traian): Because the insertion algorithm starts from the index calculated by modulo-ing the hash
                // value of the element by the number of entries and increments this index until an empty entry is found,
                // it is guaranteed that the element will not appear after the first empty entry encountered.
                return firstAvailableEntryIndex.ValueOr(entryIndex);
            }
            else if (m_Entries.States[entryIndex] == EntryState::Deleted)
            {
                if (!firstAvailableEntryIndex.HasValue())
                    firstAvailableEntryIndex = entryIndex;
            }

            // Increment (and wrap if neccessary) the entry index.
            entryIndex = (entryIndex + 1) % m_Entries.Count;
        }

        // All entries have been checked.
        return {};
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FORCEINLINE void ReAllocateEntriesBuffer(usize newEntryCount)
    {
        // NOTE(Traian): This should never be triggered by the user incorrectly using the API.
        // If this assert fails, there is an internal container error.
        SE_ASSERT(newEntryCount >= m_Count);

        // Allocate a new internal memory buffer.
        EntriesBuffer newEntries = HashSet::AllocateEntriesBuffer(newEntryCount);
        for (usize entryIndex = 0; entryIndex < newEntries.Count; ++entryIndex)
            newEntries.States[entryIndex] = EntryState::Empty;

        // Move the elements from the old buffer to the new buffer.
        if (m_Count > 0)
        {
            for (usize oldEntryIndex = 0; oldEntryIndex < m_Entries.Count; ++oldEntryIndex)
            {
                if (m_Entries.States[oldEntryIndex] == EntryState::Occupied)
                {
                    const uint64 elementHash = Hash<ElementType>::Get(m_Entries.Slots[oldEntryIndex]);
                    usize newEntryIndex = elementHash % newEntries.Count;
                    while (true)
                    {
                        if (newEntries.States[newEntryIndex] == EntryState::Empty)
                        {
                            newEntries.States[newEntryIndex] = EntryState::Occupied;
                            new (newEntries.Slots + newEntryIndex) ElementType(Move(m_Entries.Slots[oldEntryIndex]));
                            break;
                        }

                        newEntryIndex = (newEntryIndex + 1) % newEntries.Count;
                    }
                    m_Entries.Slots[oldEntryIndex].~ElementType();
                }
            }
        }

        // Destroy the old internal memory buffer. Since the elements stored in it were
        // destryed after they were moved in the new buffer, no clean-up is required.
        HashSet::ReleaseEntriesBuffer(m_Entries);

        // Assign the new internal memory buffer.
        m_Entries = newEntries;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if SE_HASH_SET_CHECK_PROTECTION
    NODISCARD FORCEINLINE bool IsProtected() const
    {
        return (m_IsProtected == 1);
    }

    NODISCARD FORCEINLINE bool IsNotProtected() const
    {
        return (m_IsProtected == 0);
    }

    FORCEINLINE void SetIsProtected(bool value) const
    {
        if (value)
            m_IsProtected = true;
        else
            m_IsProtected = false;
    }
#endif // SE_HASH_SET_CHECK_PROTECTION

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    EntriesBuffer m_Entries;

    // NOTE(Traian): The configuration macro 'SE_HASH_SET_CHECK_PROTECTION' should only insert checks for
    // protection, it shouldn't modify the fundamental 'HashSet' class data layout. That's why we still declare
    // the 'm_IsProtected' field and make 'm_Count' a 63-bit unsigned integer.

    uint64 m_Count : 63;
    mutable uint64 m_IsProtected : 1;
};

}
