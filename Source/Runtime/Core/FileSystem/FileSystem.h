/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/Memory/Buffer.h"

//
// Macro the checks if the return value is FileError::Success.
// If not, an error message is logged and an assertions is triggered.
//
#define SE_CHECK_FILE_ERROR(expression)           \
    if ((expression) != ::SE::FileError::Success) \
    {                                             \
        SE_LOG_ERROR("File error occured!"sv);    \
        SE_ASSERT(false);                         \
    }

namespace SE {

//
// Error codes for file I/O operations.
// All abstractions over the platform layer that can fail will return one of these error codes.
//
enum class FileError : u32 {
    Success = 0,
    Unknown = 1,

    FileHandleNotOpened,
    FileNotFound,
    FileAlreadyInUse,
    PermissionDenied,
    ReadOutOfBounds,
    BufferNotLargeEnough,
};

//
// Abstraction over the platform API that allows reading from disk files.
//
class FileReader {
public:
    enum class OpenPolicy : u8 {
        // Only opens the file is it exists on disk. Otherwise, the operation is considered failed.
        OpenExisting,

        // If the file doesn't exist on disk, consider it as being empty.
        NonExistingFileIsEmpty,
    };

    enum class SharePolicy : u8 {
        // This file can't be open by any other process.
        Exclusive,
        // Other processes can only open this file for reading.
        ReadOnly,
        // Other processes can open this file with full read/write access.
        ReadWrite,
    };

public:
    SHOOTER_API FileReader();
    SHOOTER_API ~FileReader();

    SHOOTER_API FileError open(const String& filepath, OpenPolicy open_policy = OpenPolicy::OpenExisting, SharePolicy share_policy = SharePolicy::Exclusive);
    SHOOTER_API void close();
    ALWAYS_INLINE bool is_opened() const { return m_handle_is_opened; }
    
    //
    // Reads the entire file and stores its contents in a newly allocated buffer.
    // It is the responsibility of the caller to manage the lifetime of the provided buffer.
    // The provided buffer object must be empty, otherwise an assert will be triggered.
    //
    SHOOTER_API FileError read_entire(Buffer& out_buffer);

    //
    // Tries to read the entire file and store its contents in the provided buffer. If the provided
    // buffer is not large enough, nothing will be written. In this case, the number of read bytes parameter
    // will be set to zero and FileError::Success will be returned (if there are no other file IO errors).
    // If an error occurs while reading the file, the number of read bytes parameter will be set to 'invalid_size'.
    //
    SHOOTER_API FileError try_read_entire(WriteonlyByteSpan output_buffer, Optional<usize>& out_number_of_read_bytes);
    
    //
    // Reads from the file and stores the content in the given output buffer.
    // If the provided buffer is not large enough to stored the requested number of bytes, FileError::BufferNotLargeEnough
    // will be returned. If the region (determined by the offset and the number of bytes) to read exceeds the total file
    // size, FileError::ReadOutOfBounds is returned.
    //
    SHOOTER_API FileError read(WriteonlyByteSpan output_buffer, usize read_offset_in_bytes, usize number_of_bytes_to_read);

    //
    // Reads from the file and stores the content in a newly allocated buffer.
    // It is the responsibility of the caller to manage the lifetime of the provided buffer.
    // The provided buffer object must be empty, otherwise an assert will be triggered.
    //
    SHOOTER_API FileError read_to_new_buffer(Buffer& out_buffer, usize read_offset_in_bytes, usize number_of_bytes_to_read);
    
    //
    // Reads the entire file and closes the file handle if the operation is successful.
    // It directly wraps around the 'read_entire' and 'close' functions.
    //
    SHOOTER_API FileError read_entire_and_close(Buffer& out_buffer);

    //
    // Tries to read the entire file and if succeeds closes the file handle.
    // It directly wraps around the 'try_read_entire' and 'close' functions.
    //
    SHOOTER_API FileError try_read_entire_and_close(WriteonlyByteSpan output_buffer, Optional<usize>& out_number_of_read_bytes);

private:
    void* m_native_handle;
    bool m_handle_is_opened;
    OpenPolicy m_open_policy;
};

class FileWriter {
public:
    enum class OpenPolicy : u8 {
        // Only opens the file is it exists on disk. Otherwise, the operation is considered failed.
        OpenExisting,

        // Creates a new file if it doesn't already exist on disk. It will recursively generate directories
        // until the parent path is completed.
        CreateIfNotExisting,

        // Always create a new file. It will recursively generate directories until the parent path is completed.
        CreateNew,
    };

    enum class SharePolicy : u8 {
        // This file can't be open by any other process.
        Exclusive,
        // Other processes can only open this file for reading.
        ReadOnly,
        // Other processes can open this file with full read/write access.
        ReadWrite,
    };

public:
    SHOOTER_API FileWriter();
    SHOOTER_API ~FileWriter();

    SHOOTER_API FileError open(const String& filepath, OpenPolicy open_policy = OpenPolicy::CreateIfNotExisting, SharePolicy share_policy = SharePolicy::Exclusive);
    SHOOTER_API void close();
    ALWAYS_INLINE bool is_opened() const { return m_handle_is_opened; }

    //
    // Writes the given bytes to the end of the file.
    //
    SHOOTER_API FileError write(ReadonlyByteSpan bytes_to_write);

    //
    // Writes the given bytes to the end of the file and closes the file handle if the operation is successful.
    // It directly wraps around the 'write' and 'close' functions.
    //
    SHOOTER_API FileError write_and_close(ReadonlyByteSpan bytes_to_write);

private:
    void* m_native_handle;
    bool m_handle_is_opened;
};

class FileSystem {
public:
    SHOOTER_API NODISCARD static bool exists(const String& filepath);

    // If the provided filepath is not valid, this function will return an empty optional.
    SHOOTER_API NODISCARD static Optional<bool> is_directory(const String& filepath);

    // If the provided filepath is not valid or it doesn't represent a file, this function
    // will return an empty optional.
    SHOOTER_API NODISCARD static Optional<usize> get_file_size(const String& filepath);
};

} // namespace SE