// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/HashMap.h>
#include <Runtime/Core/Containers/HashSet.h>
#include <Runtime/Core/Containers/Vector.h>
#include <Runtime/Core/Containers/String/String.h>
#include <Runtime/Core/Delegate.h>
#include <Runtime/Core/Math/Vector.h>
#include <Runtime/Core/UUID.h>
#include <Runtime/Scene/Reflection/PrimitiveDataType.h>

namespace SE
{

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////// FIELD GENERIC TYPE.
///////////////////////////////////////////////////////////////////////////////////////////////////

enum class FieldTypeKind : uint8
{
    Unknown = 0,
    
    // C++ types.
    Primitive,
    Enum,
    Struct,
    String,
    Vector,
    HashSet,
    HashMap,

    // Engine types.
    EntityReference,
    AssetReference,
};

// Forward declarations for all field types.
class FieldType;
class FieldPrimitiveType;
class FieldEnumType;
class FieldStructType;
class FieldStringType;
class FieldVectorType;
class FieldHashSetType;
class FieldHashMapType;
class FieldEntityReferenceType;
class FieldAssetReferenceType;

class FieldType
{
public:
    ALWAYS_INLINE explicit FieldType(FieldTypeKind kind)
        : m_Kind(kind)
    {}

    virtual ~FieldType() = default;
    NODISCARD ALWAYS_INLINE FieldTypeKind GetKind() const { return m_Kind; }

public:
    // C++ types.
    NODISCARD ALWAYS_INLINE const FieldPrimitiveType& AsPrimitive() const { return reinterpret_cast<const FieldPrimitiveType&>(*this); }
    NODISCARD ALWAYS_INLINE const FieldEnumType& AsEnum() const { return reinterpret_cast<const FieldEnumType&>(*this); }
    NODISCARD ALWAYS_INLINE const FieldStructType& AsStruct() const { return reinterpret_cast<const FieldStructType&>(*this); }
    NODISCARD ALWAYS_INLINE const FieldStringType& AsString() const { return reinterpret_cast<const FieldStringType&>(*this); }
    NODISCARD ALWAYS_INLINE const FieldVectorType& AsVector() const { return reinterpret_cast<const FieldVectorType&>(*this); }
    NODISCARD ALWAYS_INLINE const FieldHashSetType& AsHashSet() const { return reinterpret_cast<const FieldHashSetType&>(*this); }
    NODISCARD ALWAYS_INLINE const FieldHashMapType& AsHashMap() const { return reinterpret_cast<const FieldHashMapType&>(*this); }

    // Engine types.
    NODISCARD ALWAYS_INLINE const FieldEntityReferenceType& AsEntityReference() const { return reinterpret_cast<const FieldEntityReferenceType&>(*this); }
    NODISCARD ALWAYS_INLINE const FieldAssetReferenceType& AsAssetReference() const { return reinterpret_cast<const FieldAssetReferenceType&>(*this); }

private:
    FieldTypeKind m_Kind;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////// FIELD PRIMITIVE TYPE.
///////////////////////////////////////////////////////////////////////////////////////////////////

class FieldPrimitiveType : public FieldType
{
public:
    ALWAYS_INLINE explicit FieldPrimitiveType(PrimitiveDataType dataType)
        : FieldType(GetStaticKind())
        , m_DataType(dataType)
    {}

    virtual ~FieldPrimitiveType() override = default;
    NODISCARD ALWAYS_INLINE static FieldTypeKind GetStaticKind() { return FieldTypeKind::Primitive; }
    NODISCARD ALWAYS_INLINE PrimitiveDataType GetDataType() const { return m_DataType; }

private:
    PrimitiveDataType m_DataType;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////// FIELD ENUM TYPE.
///////////////////////////////////////////////////////////////////////////////////////////////////

class FieldEnumType : public FieldType
{
public:
    ALWAYS_INLINE explicit FieldEnumType(UUID enumUUID)
        : FieldType(GetStaticKind())
        , m_EnumUUID(enumUUID)
    {}

    virtual ~FieldEnumType() override = default;
    NODISCARD ALWAYS_INLINE static FieldTypeKind GetStaticKind() { return FieldTypeKind::Enum; }
    NODISCARD ALWAYS_INLINE UUID GetEnumUUID() const { return m_EnumUUID; }

private:
    UUID m_EnumUUID;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////// FIELD STRUCT TYPE.
///////////////////////////////////////////////////////////////////////////////////////////////////

class FieldStructType : public FieldType
{
public:
    ALWAYS_INLINE explicit FieldStructType(UUID structUUID)
        : FieldType(GetStaticKind())
        , m_StructUUID(structUUID)
    {}

    virtual ~FieldStructType() override = default;
    NODISCARD ALWAYS_INLINE static FieldTypeKind GetStaticKind() { return FieldTypeKind::Struct; }
    NODISCARD ALWAYS_INLINE UUID GetStructUUID() const { return m_StructUUID; }

private:
    UUID m_StructUUID;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////// FIELD STRING TYPE.
///////////////////////////////////////////////////////////////////////////////////////////////////

class FieldStringType : public FieldType
{
public:
    ALWAYS_INLINE explicit FieldStringType()
        : FieldType(GetStaticKind())
    {}

    virtual ~FieldStringType() override = default;
    NODISCARD ALWAYS_INLINE static FieldTypeKind GetStaticKind() { return FieldTypeKind::String; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////// FIELD VECTOR TYPE.
///////////////////////////////////////////////////////////////////////////////////////////////////

class FieldVectorType : public FieldType
{
public:
    ALWAYS_INLINE explicit FieldVectorType(const FieldType* elementType)
        : FieldType(GetStaticKind())
        , m_ElementType(elementType)
    {}

    virtual ~FieldVectorType() override = default;
    NODISCARD ALWAYS_INLINE static FieldTypeKind GetStaticKind() { return FieldTypeKind::Vector; }
    NODISCARD ALWAYS_INLINE const FieldType* GetElementType() const { return m_ElementType; }

public:
    NODISCARD virtual usize GetCapacity(const void* fieldMemoryBlock) const = 0;
    NODISCARD virtual usize GetCount(const void* fieldMemoryBlock) const = 0;
    NODISCARD virtual usize GetElementSize() const = 0;

    NODISCARD virtual void* GetElementsRW(void* fieldMemoryBlock) const = 0;
    NODISCARD virtual const void* GetElementsRO(const void* fieldMemoryBlock) const = 0;

    virtual void AddElement(void* fieldMemoryBlock, const void* elementMemoryBlock) const = 0;
    virtual void RemoveElement(void* fieldMemoryBlock, usize elementIndex) const = 0;

public:
    template<typename Predicate>
    ALWAYS_INLINE void ForEachElementRW(void* fieldMemoryBlock, Predicate predicate) const
    {
        const usize elementCount = GetCount(fieldMemoryBlock);
        uint8* elements = static_cast<uint8*>(GetElementsRW(fieldMemoryBlock));
        const usize elementSize = GetElementSize();

        for (usize elementIndex = 0; elementIndex < elementCount; ++elementIndex)
        {
            uint8* elementMemoryBlock = elements + (elementIndex * elementSize);
            predicate(static_cast<void*>(elementMemoryBlock));
        }
    }

    template<typename Predicate>
    ALWAYS_INLINE void ForEachElementRO(const void* fieldMemoryBlock, Predicate predicate) const
    {
        const usize elementCount = GetCount(fieldMemoryBlock);
        const uint8* elements = static_cast<const uint8*>(GetElementsRO(fieldMemoryBlock));
        const usize elementSize = GetElementSize();

        for (usize elementIndex = 0; elementIndex < elementCount; ++elementIndex)
        {
            const uint8* elementMemoryBlock = elements + (elementIndex * elementSize);
            predicate(static_cast<const void*>(elementMemoryBlock));
        }
    }

private:
    const FieldType* m_ElementType;
};

template<typename ElementType, typename Allocator>
class FieldTypedVectorType : public FieldVectorType
{
public:
    using Vector = Vector<ElementType, Allocator>;

public:
    ALWAYS_INLINE explicit FieldTypedVectorType(const FieldType* elementType)
        : FieldVectorType(elementType)
    {}

    virtual ~FieldTypedVectorType() override = default;

public:
    NODISCARD ALWAYS_INLINE virtual usize GetCapacity(const void* fieldMemoryBlock) const override
    {
        const Vector* vector = static_cast<const Vector*>(fieldMemoryBlock);
        return vector->Capacity();
    }

    NODISCARD ALWAYS_INLINE virtual usize GetCount(const void* fieldMemoryBlock) const override
    {
        const Vector* vector = static_cast<const Vector*>(fieldMemoryBlock);
        return vector->Count();
    }

    NODISCARD ALWAYS_INLINE virtual usize GetElementSize() const override
    {
        return sizeof(ElementType);
    }

    NODISCARD ALWAYS_INLINE virtual void* GetElementsRW(void* fieldMemoryBlock) const override
    {
        Vector* vector = static_cast<Vector*>(fieldMemoryBlock);
        return vector->Elements();
    }

    NODISCARD ALWAYS_INLINE virtual const void* GetElementsRO(const void* fieldMemoryBlock) const override
    {
        const Vector* vector = static_cast<const Vector*>(fieldMemoryBlock);
        return vector->Elements();
    }

    ALWAYS_INLINE virtual void AddElement(void* fieldMemoryBlock, const void* elementMemoryBlock) const override
    {
        Vector* vector = static_cast<Vector*>(fieldMemoryBlock);
        const ElementType* element = static_cast<const ElementType*>(elementMemoryBlock);
        vector->Add(*element);
    }

    ALWAYS_INLINE virtual void RemoveElement(void* fieldMemoryBlock, usize elementIndex) const override
    {
        Vector* vector = static_cast<Vector*>(fieldMemoryBlock);
        vector->RemoveIndex(elementIndex);
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////// FIELD HASH SET TYPE.
///////////////////////////////////////////////////////////////////////////////////////////////////

SE_DECLARE_DELEGATE_ONE_PARAM(ForEachHashSetEntryDelegate, const void*);

class FieldHashSetType : public FieldType
{
public:
    ALWAYS_INLINE FieldHashSetType(const FieldType* elementType)
        : FieldType(GetStaticKind())
        , m_ElementType(elementType)
    {}

    virtual ~FieldHashSetType() override = default;
    NODISCARD ALWAYS_INLINE static FieldTypeKind GetStaticKind() { return FieldTypeKind::HashSet; }
    NODISCARD ALWAYS_INLINE const FieldType* GetElementType() const { return m_ElementType; }

public:
    NODISCARD virtual usize GetCount(const void* fieldMemoryBlock) const = 0;

    virtual void Add(void* fieldMemoryBlock, const void* keyMemoryBlock) const = 0;
    virtual void Remove(void* fieldMemoryBlock, const void* keyMemoryBlock) const = 0;

    virtual void ForEachEntry(const void* fieldMemoryBlock, const ForEachHashSetEntryDelegate& delegate) const = 0;

private:
    const FieldType* m_ElementType;
};

template<typename ElementType, typename Allocator>
class FieldTypedHashSetType : public FieldHashSetType
{
public:
    using HashSet = HashSet<ElementType, Allocator>;

public:
    ALWAYS_INLINE FieldTypedHashSetType(const FieldType* elementType)
        : FieldHashSetType(elementType)
    {}

    virtual ~FieldTypedHashSetType() override = default;

public:
    NODISCARD ALWAYS_INLINE virtual usize GetCount(const void* fieldMemoryBlock) const override
    {
        const HashSet* hashSet = static_cast<const HashSet*>(fieldMemoryBlock);
        return hashSet->Count();
    }

    ALWAYS_INLINE virtual void Add(void* fieldMemoryBlock, const void* elementMemoryBlock) const override
    {
        HashSet* hashSet = static_cast<HashSet*>(fieldMemoryBlock);
        const ElementType* element = static_cast<const ElementType*>(elementMemoryBlock);
        hashSet->Add(*element);
    }

    ALWAYS_INLINE virtual void Remove(void* fieldMemoryBlock, const void* elementMemoryBlock) const override
    {
        HashSet* hashSet = static_cast<HashSet*>(fieldMemoryBlock);
        const ElementType* element = static_cast<const ElementType*>(elementMemoryBlock);
        hashSet->RemoveIfExists(*element);
    }

    ALWAYS_INLINE virtual void ForEachEntry(const void* fieldMemoryBlock, const ForEachHashSetEntryDelegate& delegate) const override
    {
        const HashSet* hashSet = static_cast<const HashSet*>(fieldMemoryBlock);
        for (const ElementType& element : *hashSet)
        {
            const void* elementMemoryBlock = &element;
            delegate.Execute(elementMemoryBlock);
        }
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
////////////// FIELD HASH MAP TYPE.
///////////////////////////////////////////////////////////////////////////////////////////////////

SE_DECLARE_DELEGATE_TWO_PARAMS(ForEachHashMapEntryRWDelegate, const void*, void*);
SE_DECLARE_DELEGATE_TWO_PARAMS(ForEachHashMapEntryRODelegate, const void*, const void*);

class FieldHashMapType : public FieldType
{
public:
    ALWAYS_INLINE FieldHashMapType(const FieldType* keyType, const FieldType* valueType)
        : FieldType(GetStaticKind())
        , m_KeyType(keyType)
        , m_ValueType(valueType)
    {}

    virtual ~FieldHashMapType() override = default;
    NODISCARD ALWAYS_INLINE static FieldTypeKind GetStaticKind() { return FieldTypeKind::HashMap; }
    NODISCARD ALWAYS_INLINE const FieldType* GetKeyType() const { return m_KeyType; }
    NODISCARD ALWAYS_INLINE const FieldType* GetValueType() const { return m_ValueType; }

public:
    NODISCARD virtual usize GetCount(const void* fieldMemoryBlock) const = 0;

    virtual void Add(void* fieldMemoryBlock, const void* keyMemoryBlock, const void* valueMemoryBlock) const = 0;
    virtual void Remove(void* fieldMemoryBlock, const void* keyMemoryBlock) const = 0;

    virtual void ForEachEntryRW(void* fieldMemoryBlock, const ForEachHashMapEntryRWDelegate& delegate) const = 0;
    virtual void ForEachEntryRO(const void* fieldMemoryBlock, const ForEachHashMapEntryRODelegate& delegate) const = 0;

private:
    const FieldType* m_KeyType;
    const FieldType* m_ValueType;
};

template<typename KeyType, typename ValueType, typename Allocator>
class FieldTypedHashMapType : public FieldHashMapType
{
public:
    using HashMap = HashMap<KeyType, ValueType, Allocator>;

public:
    ALWAYS_INLINE FieldTypedHashMapType(const FieldType* keyType, const FieldType* valueType)
        : FieldHashMapType(keyType, valueType)
    {}

    virtual ~FieldTypedHashMapType() override = default;

public:
    NODISCARD ALWAYS_INLINE virtual usize GetCount(const void* fieldMemoryBlock) const override
    {
        const HashMap* hashMap = static_cast<const HashMap*>(fieldMemoryBlock);
        return hashMap->Count();
    }

    ALWAYS_INLINE virtual void Add(void* fieldMemoryBlock, const void* keyMemoryBlock, const void* valueMemoryBlock) const override
    {
        HashMap* hashMap = static_cast<HashMap*>(fieldMemoryBlock);
        const KeyType* key = static_cast<const KeyType*>(keyMemoryBlock);
        const ValueType* value = static_cast<const ValueType*>(valueMemoryBlock);
        hashMap->Add(*key, *value);
    }

    ALWAYS_INLINE virtual void Remove(void* fieldMemoryBlock, const void* keyMemoryBlock) const override
    {
        HashMap* hashMap = static_cast<HashMap*>(fieldMemoryBlock);
        const KeyType* key = static_cast<const KeyType*>(keyMemoryBlock);
        hashMap->RemoveIfExist(*key);
    }

    ALWAYS_INLINE virtual void ForEachEntryRW(void* fieldMemoryBlock, const ForEachHashMapEntryRWDelegate& delegate) const override
    {
        HashMap* hashMap = static_cast<HashMap*>(fieldMemoryBlock);
        for (auto& [key, value] : *hashMap)
        {
            const void* keyMemoryBlock = &key;
            void* valueMemoryBlock = &value;
            delegate.Execute(keyMemoryBlock, valueMemoryBlock);
        }
    }

    ALWAYS_INLINE virtual void ForEachEntryRO(const void* fieldMemoryBlock, const ForEachHashMapEntryRODelegate& delegate) const override
    {
        const HashMap* hashMap = static_cast<const HashMap*>(fieldMemoryBlock);
        for (const auto& [key, value] : *hashMap)
        {
            const void* keyMemoryBlock = &key;
            const void* valueMemoryBlock = &value;
            delegate.Execute(keyMemoryBlock, valueMemoryBlock);
        }
    }
};

}
