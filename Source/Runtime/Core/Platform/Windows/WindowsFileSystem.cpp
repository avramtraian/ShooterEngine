/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Core/FileSystem/FileSystem.h"
#include "Core/Math/MathUtils.h"
#include "Core/Platform/Windows/WindowsHeaders.h"

namespace SE {

//==============================================================================================================
// FILE SYSTEM.
//==============================================================================================================

bool FileSystem::file_exists(const String& filepath)
{
    const DWORD file_attributes = GetFileAttributesA(filepath_to_cstr(filepath));
    return (file_attributes != INVALID_FILE_ATTRIBUTES);
}

Optional<bool> FileSystem::is_directory(const String& filepath)
{
    const DWORD file_attributes = GetFileAttributesA(filepath_to_cstr(filepath));
    if (file_attributes == INVALID_FILE_ATTRIBUTES)
        return {};
    return (file_attributes & FILE_ATTRIBUTE_DIRECTORY);
}

Optional<usize> FileSystem::get_file_size(const String& filepath)
{
    HANDLE file_handle =
        CreateFileA(filepath_to_cstr(filepath), 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file_handle == INVALID_HANDLE_VALUE)
        return {};

    LARGE_INTEGER file_size;
    BOOL success = GetFileSizeEx(file_handle, &file_size);
    CloseHandle(file_handle);

    return success ? Optional<usize>(file_size.QuadPart) : Optional<usize>();
}

} // namespace SE