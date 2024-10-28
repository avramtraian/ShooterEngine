/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/Containers/Optional.h>
#include <Core/Containers/Span.h>
#include <Core/Containers/Vector.h>
#include <Core/String/String.h>
#include <Core/UUID.h>

namespace SE
{

enum class FormatErrorCode : u8
{
    Success = 0,
    Unknown = 1,

    InvalidSpecifier,
};

class FormatBuilder
{
    SE_MAKE_NONCOPYABLE(FormatBuilder);
    SE_MAKE_NONMOVABLE(FormatBuilder);

public:
    struct Specifier
    {
    };

public:
    ALWAYS_INLINE FormatBuilder(StringView string_format)
        : m_string_format(string_format)
    {}

public:
    NODISCARD SHOOTER_API Optional<String> release_string();

    SHOOTER_API FormatErrorCode consume_until_format_specifier();
    SHOOTER_API Optional<Specifier> parse_specifier();
    static Optional<Specifier> parse_specifier(StringView specifier_string);

    SHOOTER_API FormatErrorCode push_unsigned_integer(const Specifier& specifier, u64 value);
    SHOOTER_API FormatErrorCode push_signed_integer(const Specifier& specifier, i64 value);
    SHOOTER_API FormatErrorCode push_string(const Specifier& specifier, StringView value);

    template<typename T>
    requires (is_integral<T>)
    ALWAYS_INLINE FormatErrorCode push_integer(const Specifier& specifier, T value)
    {
        if constexpr (is_signed_integral<T>)
        {
            return push_signed_integer(specifier, static_cast<i64>(value));
        }
        else
        {
            static_assert(is_unsigned_integral<T>);
            return push_unsigned_integer(specifier, static_cast<u64>(value));
        }
    }

private:
    StringView m_string_format;
    Vector<char> m_formatted_string_buffer;
};

template<typename T>
struct Formatter
{
    ALWAYS_INLINE static FormatErrorCode format(FormatBuilder&, const FormatBuilder::Specifier&, const T&)
    {
        // TODO: Maybe we should signal that the Formatter<T> isn't specialized by
        //       gracefully returning an error code?
        SE_ASSERT(false);
        return FormatErrorCode::Success;
    }
};

template<typename T>
requires (is_integral<T>)
struct Formatter<T>
{
    ALWAYS_INLINE static FormatErrorCode format(FormatBuilder& builder, const FormatBuilder::Specifier& specifier, const T& value)
    {
        if constexpr (is_signed_integral<T>)
            return builder.push_signed_integer(specifier, static_cast<i64>(value));
        else
            return builder.push_unsigned_integer(specifier, static_cast<u64>(value));
    }
};

template<>
struct Formatter<bool>
{
    ALWAYS_INLINE static FormatErrorCode format(FormatBuilder& builder, const FormatBuilder::Specifier&, const bool& value)
    {
        return builder.push_string({}, value ? "true"sv : "false"sv);
    }
};

template<>
struct Formatter<StringView>
{
    ALWAYS_INLINE static FormatErrorCode format(FormatBuilder& builder, const FormatBuilder::Specifier& specifier, const StringView& value)
    {
        return builder.push_string(specifier, value);
    }
};

template<>
struct Formatter<String>
{
    ALWAYS_INLINE static FormatErrorCode format(FormatBuilder& builder, const FormatBuilder::Specifier& specifier, const String& value)
    {
        return builder.push_string(specifier, value.view());
    }
};

#define SE_INTEGER_FORMATTER_DECLARATION(type_name)                                                                                            \
    template<>                                                                                                                                 \
    struct Formatter<type_name>                                                                                                                \
    {                                                                                                                                          \
        ALWAYS_INLINE static FormatErrorCode format(FormatBuilder& builder, const FormatBuilder::Specifier& specifier, const type_name& value) \
        {                                                                                                                                      \
            return builder.push_integer<type_name>(specifier, value);                                                                          \
        }                                                                                                                                      \
    };

SE_INTEGER_FORMATTER_DECLARATION(u8)
SE_INTEGER_FORMATTER_DECLARATION(u16)
SE_INTEGER_FORMATTER_DECLARATION(u32)
SE_INTEGER_FORMATTER_DECLARATION(u64)

SE_INTEGER_FORMATTER_DECLARATION(i8)
SE_INTEGER_FORMATTER_DECLARATION(i16)
SE_INTEGER_FORMATTER_DECLARATION(i32)
SE_INTEGER_FORMATTER_DECLARATION(i64)

#undef SE_INTEGER_FORMATTER_DECLARATION

template<>
struct Formatter<UUID>
{
    ALWAYS_INLINE static FormatErrorCode format(FormatBuilder& builder, const FormatBuilder::Specifier& specifier, const UUID& value)
    {
        // Because a UUID is only a 64-bit unsigned integer, we can be sure that it will
        // fit in a 16-long character array (written in hexadecimal).
        char uuid_buffer[16] = {};
        set_memory(uuid_buffer, '0', 16);

        u64 uuid_value = value.value();
        usize buffer_offset = 0;
        while (uuid_value > 0)
        {
            const u8 digit = uuid_value % 16;
            uuid_value /= 16;

            if (digit <= 9)
                uuid_buffer[buffer_offset++] = '0' + digit;
            else
                uuid_buffer[buffer_offset++] = 'A' + (digit - 10);
        }

        // Flip the modified characters.
        for (usize offset = 0; offset < buffer_offset / 2; ++offset)
        {
            const char temporary = uuid_buffer[offset];
            uuid_buffer[offset] = uuid_buffer[buffer_offset - offset - 1];
            uuid_buffer[buffer_offset - offset - 1] = temporary;
        }

        const StringView uuid_buffer_string_view = StringView::create_from_utf8(uuid_buffer, sizeof(uuid_buffer));
        return builder.push_string(specifier, uuid_buffer_string_view);
    }
};

namespace Detail
{

NODISCARD ALWAYS_INLINE FormatErrorCode format(FormatBuilder& builder)
{
    return builder.consume_until_format_specifier();
}

template<typename T, typename... Args>
NODISCARD ALWAYS_INLINE FormatErrorCode format(FormatBuilder& builder, const T& argument, Args&&... other_arguments)
{
    FormatErrorCode error_code = builder.consume_until_format_specifier();
    if (error_code != FormatErrorCode::Success)
        return error_code;

    Optional<FormatBuilder::Specifier> format_specifier = builder.parse_specifier();
    if (!format_specifier.has_value())
        return FormatErrorCode::InvalidSpecifier;

    error_code = Formatter<T>::format(builder, format_specifier, argument);
    if (error_code != FormatErrorCode::Success)
        return error_code;

    return format(builder, forward<Args>(other_arguments)...);
}

} // namespace Detail

template<typename... Args>
NODISCARD ALWAYS_INLINE Optional<String> format(StringView string_format, Args&&... args)
{
    FormatBuilder builder = FormatBuilder(string_format);
    FormatErrorCode error_code = Detail::format(builder, forward<Args>(args)...);
    if (error_code != FormatErrorCode::Success)
        return {};

    return builder.release_string();
}

} // namespace SE
