/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Core/Containers/StringBuilder.h"

namespace SE
{

String StringBuilder::join(std::initializer_list<StringView> views_list)
{
    usize result_byte_count = 0;
    for (StringView view : views_list)
        result_byte_count += view.byte_span().count();
    result_byte_count += sizeof('\0');

    String result;
    result.m_byte_count = result_byte_count;
    char* destination_buffer = result.m_inline_buffer;

    if (result.is_stored_on_heap())
    {
        result.m_heap_buffer = String::allocate_memory(result.m_byte_count);
        destination_buffer = result.m_heap_buffer;
    }

    usize byte_offset = 0;
    for (StringView view : views_list)
        copy_memory_from_span(destination_buffer + byte_offset, view.byte_span());
    destination_buffer[result.m_byte_count - 1] = 0;

    return result;
}

String StringBuilder::path_join(std::initializer_list<StringView> paths_list)
{
    usize result_byte_count = 0;
    for (StringView path : paths_list)
    {
        if (path.is_empty())
            continue;

        result_byte_count += path.byte_span().count();

        if (path.byte_span().first() != SE_FILEPATH_DELIMITATOR)
            result_byte_count += sizeof(SE_FILEPATH_DELIMITATOR);
    }
    result_byte_count += sizeof('\0');

    String result;
    result.m_byte_count = result_byte_count;
    char* destination_buffer = result.m_inline_buffer;

    if (result.is_stored_on_heap())
    {
        result.m_heap_buffer = String::allocate_memory(result.m_byte_count);
        destination_buffer = result.m_heap_buffer;
    }

    usize byte_offset = 0;
    for (StringView path : paths_list)
    {
        if (path.is_empty())
            continue;

        if (path.byte_span().first() != SE_FILEPATH_DELIMITATOR)
        {
            destination_buffer[byte_offset] = SE_FILEPATH_DELIMITATOR;
            byte_offset += sizeof(SE_FILEPATH_DELIMITATOR);
        }

        copy_memory_from_span(destination_buffer + byte_offset, path.byte_span());
    }

    destination_buffer[result.m_byte_count - 1] = 0;
    return result;
}

} // namespace SE