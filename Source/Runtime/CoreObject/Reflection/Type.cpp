// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#include <Runtime/CoreObject/Reflection/Type.h>

namespace SE
{

String ObjectTypeKindToString(ObjectTypeKind objectTypeKind)
{
    switch (objectTypeKind)
    {
#define _SE_CASE(x) case ObjectTypeKind::x: return VIEW(#x);
        SE_ENUMERATE_OBJECT_TYPE_KINDS(_SE_CASE);
#undef _SE_CASE

        default: SE_ASSERT_NOT_REACHED;
    }

    return VIEW("<unstringifyable>");
}

StringView ObjectTypeKindToStringView(ObjectTypeKind objectTypeKind)
{
    switch (objectTypeKind)
    {
#define _SE_CASE(x) case ObjectTypeKind::x: return VIEW(#x);
        SE_ENUMERATE_OBJECT_TYPE_KINDS(_SE_CASE);
#undef _SE_CASE

        default: SE_ASSERT_NOT_REACHED;
    }

    return VIEW("<unstringifyable>");
}

const char* ObjectTypeKindToRawString(ObjectTypeKind objectTypeKind)
{
    switch (objectTypeKind)
    {
#define _SE_CASE(x) case ObjectTypeKind::x: return #x;
        SE_ENUMERATE_OBJECT_TYPE_KINDS(_SE_CASE);
#undef _SE_CASE

        default: SE_ASSERT_NOT_REACHED;
    }

    return "<unstringifyable>";
}

ObjectTypeKind ObjectTypeKindFromString(StringView string)
{
#define SE_IF_STATEMENT(x) if (string == VIEW(#x)) { return ObjectTypeKind::x; }
    SE_ENUMERATE_OBJECT_TYPE_KINDS(SE_IF_STATEMENT)
#undef SE_IF_STATEMENT

    SE_ASSERT_NOT_REACHED;
    return ObjectTypeKind::Primitive;
}

String PrimitiveTypeToString(PrimitiveType primitiveType)
{
    switch (primitiveType)
    {
#define _SE_CASE(x) case PrimitiveType::x: return VIEW(#x);
        SE_ENUMERATE_PRIMITIVE_TYPES(_SE_CASE);
#undef _SE_CASE

        default: SE_ASSERT_NOT_REACHED;
    }

    return {};
}

StringView PrimitiveTypeToStringView(PrimitiveType primitiveType)
{
    switch (primitiveType)
    {
#define _SE_CASE(x) case PrimitiveType::x: return VIEW(#x);
        SE_ENUMERATE_PRIMITIVE_TYPES(_SE_CASE);
#undef _SE_CASE

        default: SE_ASSERT_NOT_REACHED;
    }

    return {};
}

const char* PrimitiveTypeToRawString(PrimitiveType primitiveType)
{
    switch (primitiveType)
    {
#define _SE_CASE(x) case PrimitiveType::x: return #x;
        SE_ENUMERATE_PRIMITIVE_TYPES(_SE_CASE);
#undef _SE_CASE

        default: SE_ASSERT_NOT_REACHED;
    }

    return {};
}

PrimitiveType PrimitiveTypeFromString(StringView string)
{
#define SE_IF_STATEMENT(x) if (string == VIEW(#x)) { return PrimitiveType::x; }
    SE_ENUMERATE_PRIMITIVE_TYPES(SE_IF_STATEMENT)
#undef SE_IF_STATEMENT

    SE_ASSERT_NOT_REACHED;
    return PrimitiveType::Unknown;
}

}
