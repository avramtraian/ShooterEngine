/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include "Core/FileSystem/FileSystem.h"
#include "Core/Math/MathUtils.h"
#include "Core/Platform/Windows/WindowsHeaders.h"

namespace SE {

//==============================================================================================================
// UTILITIES.
//==============================================================================================================

ALWAYS_INLINE static LPSTR filepath_to_cstr(const String& filepath)
{
    return filepath.byte_span_with_null_termination().as<CHAR>().elements();
}

static FileError read_from_file(HANDLE file_handle, WriteonlyByteSpan output_buffer, usize read_offset_in_bytes, usize number_of_bytes_to_read)
{
    OVERLAPPED overlapped = {};
    constexpr DWORD dword_mask = static_cast<DWORD>(-1);
    overlapped.Offset = read_offset_in_bytes & dword_mask;
    overlapped.OffsetHigh = read_offset_in_bytes >> (8 * sizeof(DWORD));

    usize byte_offset = 0;
    while (byte_offset < number_of_bytes_to_read)
    {
        constexpr usize max_bytes_to_read = static_cast<DWORD>(-1);
        const usize bytes_to_read = Math::min(number_of_bytes_to_read - byte_offset, max_bytes_to_read);

        DWORD bytes_read;
        BOOL read_success = ReadFile(file_handle, output_buffer.elements() + byte_offset, (DWORD)(bytes_to_read), &bytes_read, &overlapped);
        if (!read_success || bytes_read != bytes_to_read)
            return FileError::Unknown;

        byte_offset += bytes_read;
    }

    SE_ASSERT(byte_offset == number_of_bytes_to_read);
    return FileError::Success;
}

//==============================================================================================================
// FILE READER.
//==============================================================================================================

FileReader::FileReader()
    : m_native_handle(INVALID_HANDLE_VALUE)
    , m_handle_is_opened(false)
    , m_open_policy(OpenPolicy::OpenExisting)
{}

FileReader::~FileReader()
{
    close();
}

FileError FileReader::open(const String& filepath, OpenPolicy open_policy /*= OpenPolicy::OpenExisting*/, SharePolicy share_policy /*= SharePolicy::Exclusive*/)
{
    // Close the previously opened file handle.
    close();

    m_open_policy = open_policy;
    DWORD share_mode = 0;

    switch (share_policy)
    {
        case SharePolicy::Exclusive: share_mode = 0; break;
        case SharePolicy::ReadOnly:  share_mode = FILE_SHARE_READ; break;
        case SharePolicy::ReadWrite: share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE; break;
    }

    m_native_handle = CreateFileA(filepath_to_cstr(filepath), GENERIC_READ, share_mode, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (m_native_handle == INVALID_HANDLE_VALUE && m_open_policy != OpenPolicy::NonExistingFileIsEmpty)
        return FileError::FileNotFound;
    
    m_handle_is_opened = true;
    return FileError::Success;
}

void FileReader::close()
{
    if (!m_handle_is_opened && m_native_handle == INVALID_HANDLE_VALUE)
        return;

    CloseHandle(m_native_handle);
    m_native_handle = INVALID_HANDLE_VALUE;
    m_handle_is_opened = false;
}

FileError FileReader::read_entire(Buffer& out_buffer)
{
    SE_ASSERT(out_buffer.is_empty());

    // Ensure that the file stream is ready to read.
    if (!m_handle_is_opened)
        return FileError::FileHandleNotOpened;

    if (m_native_handle == INVALID_HANDLE_VALUE && m_open_policy == OpenPolicy::NonExistingFileIsEmpty)
        return FileError::Success;

    // Get the size of the file.
    LARGE_INTEGER file_size_large_integer;
    if (!GetFileSizeEx(m_native_handle, &file_size_large_integer))
        return FileError::Unknown;
    const usize file_size = file_size_large_integer.QuadPart;

    // Allocate memory for the file contents.
    out_buffer.allocate(file_size);

    // Read from the file.
    FileError file_error = read_from_file(m_native_handle, out_buffer.span(), 0, file_size);
    if (file_error != FileError::Success)
    {
        out_buffer.release();
        return file_error;
    }

    return FileError::Success;
}

FileError FileReader::try_read_entire(WriteonlyByteSpan output_buffer, usize& out_number_of_read_bytes)
{
    out_number_of_read_bytes = invalid_size;

    // Ensure that the file stream is ready to read.
    if (!m_handle_is_opened)
        return FileError::FileHandleNotOpened;

    if (m_native_handle == INVALID_HANDLE_VALUE && m_open_policy == OpenPolicy::NonExistingFileIsEmpty)
        return FileError::Success;

    // Get the size of the file.
    LARGE_INTEGER file_size_large_integer;
    if (!GetFileSizeEx(m_native_handle, &file_size_large_integer))
        return FileError::Unknown;
    const usize file_size = file_size_large_integer.QuadPart;

    // Check if the provided output buffer is large enough.
    if (output_buffer.count() < file_size)
    {
        out_number_of_read_bytes = 0;
        return FileError::Success;
    }

    // Read from the file.
    FileError file_error = read_from_file(m_native_handle, output_buffer, 0, file_size);
    if (file_error != FileError::Success)
        return file_error;

    out_number_of_read_bytes = file_size;
    return FileError::Success;
}

FileError FileReader::read(Buffer& out_buffer, usize read_offset_in_bytes, usize number_of_bytes_to_read)
{
    SE_ASSERT(out_buffer.is_empty());

    // Ensure that the file stream is ready to read.
    if (!m_handle_is_opened)
        return FileError::FileHandleNotOpened;

    if (m_native_handle == INVALID_HANDLE_VALUE && m_open_policy == OpenPolicy::NonExistingFileIsEmpty)
        return FileError::Success;

    // Get the size of the file.
    LARGE_INTEGER file_size_large_integer;
    if (!GetFileSizeEx(m_native_handle, &file_size_large_integer))
        return FileError::Unknown;
    const usize file_size = file_size_large_integer.QuadPart;

    // Ensure that the reading regions is between the file bounds.
    if (read_offset_in_bytes + number_of_bytes_to_read > file_size)
        return FileError::ReadOutOfBounds;

    // Allocate memory for the file contents.
    out_buffer.allocate(number_of_bytes_to_read);

    // Read from the file.
    FileError file_error = read_from_file(m_native_handle, out_buffer.span(), read_offset_in_bytes, number_of_bytes_to_read);
    if (file_error != FileError::Success)
    {
        out_buffer.release();
        return file_error;
    }

    return FileError::Success;
}

FileError FileReader::try_read(WriteonlyByteSpan output_buffer, usize read_offset_in_bytes, usize number_of_bytes_to_read, usize& out_number_of_read_bytes)
{
    out_number_of_read_bytes = invalid_size;

    // Ensure that the file stream is ready to read.
    if (!m_handle_is_opened)
        return FileError::FileHandleNotOpened;

    if (m_native_handle == INVALID_HANDLE_VALUE && m_open_policy == OpenPolicy::NonExistingFileIsEmpty)
        return FileError::Success;

    // Get the size of the file.
    LARGE_INTEGER file_size_large_integer;
    if (!GetFileSizeEx(m_native_handle, &file_size_large_integer))
        return FileError::Unknown;
    const usize file_size = file_size_large_integer.QuadPart;

    // Ensure that the reading regions is between the file bounds.
    if (read_offset_in_bytes + number_of_bytes_to_read > file_size)
        return FileError::ReadOutOfBounds;

    // Read from the file.
    FileError file_error = read_from_file(m_native_handle, output_buffer, read_offset_in_bytes, number_of_bytes_to_read);
    if (file_error != FileError::Success)
        return file_error;

    out_number_of_read_bytes = number_of_bytes_to_read;
    return FileError::Success;
}

//==============================================================================================================
// FILE SYSTEM.
//==============================================================================================================

bool FileSystem::exists(const String& filepath)
{
    // An empty filepath always exists.
    if (filepath.byte_span().is_empty())
        return true;

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