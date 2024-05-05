/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Containers/Optional.h"
#include "Core/Containers/Span.h"
#include "Core/Containers/String.h"
#include "Core/Containers/Vector.h"

namespace SE {

enum class FormatErrorCode : u8 {
    Success = 0,
    Unknown = 1,

    InvalidSpecifier,
};

class FormatBuilder {
    SE_MAKE_NONCOPYABLE(FormatBuilder);
    SE_MAKE_NONMOVABLE(FormatBuilder);

public:
    struct Specifier {};

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

private:
    StringView m_string_format;
    Vector<char> m_formatted_string_buffer;
};

template<typename T>
struct Formatter {
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
struct Formatter<T> {
    ALWAYS_INLINE static FormatErrorCode
    format(FormatBuilder& builder, const FormatBuilder::Specifier& specifier, const T& value)
    {
        if constexpr (is_signed_integral<T>)
            return builder.push_signed_integer(specifier, static_cast<i64>(value));
        else
            return builder.push_unsigned_integer(specifier, static_cast<u64>(value));
    }
};

template<>
struct Formatter<bool> {
    ALWAYS_INLINE static FormatErrorCode
    format(FormatBuilder& builder, const FormatBuilder::Specifier&, const bool& value)
    {
        return builder.push_string({}, value ? "true"sv : "false"sv);
    }
};

template<>
struct Formatter<StringView> {
    ALWAYS_INLINE static FormatErrorCode
    format(FormatBuilder& builder, const FormatBuilder::Specifier& specifier, const StringView& value)
    {
        return builder.push_string(specifier, value);
    }
};

template<>
struct Formatter<String> {
    ALWAYS_INLINE static FormatErrorCode
    format(FormatBuilder& builder, const FormatBuilder::Specifier& specifier, const String& value)
    {
        return builder.push_string(specifier, value.view());
    }
};

namespace Detail {

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

#ifdef SE_INCLUDE_GLOBALLY
using SE::format;
using SE::FormatBuilder;
using SE::Formatter;
#endif // SE_INCLUDE_GLOBALLY