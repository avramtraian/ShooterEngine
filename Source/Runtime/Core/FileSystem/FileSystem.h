/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include "Core/CoreTypes.h"
#include "Core/EngineAPI.h"
#include "Core/Memory/Buffer.h"

namespace SE {

enum class FileError : u32 {
    Success = 0,
    Unknown = 1,

    FileHandleNotOpened,
    FileNotFound,
    FileAlreadyInUse,
    PermissionDenied,
    ReadOutOfBounds,
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
    SHOOTER_API FileError try_read_entire(WriteonlyByteSpan output_buffer, usize& out_number_of_read_bytes);
    
    //
    // Reads the from the file and stores its contents in a newly allocated buffer.
    // It is the responsibility of the caller to manage the lifetime of the provided buffer.
    // The provided buffer object must be empty, otherwise an assert will be triggered.
    //
    SHOOTER_API FileError read(Buffer& out_buffer, usize read_offset_in_bytes, usize number_of_bytes_to_read);
    
    //
    // Tries to read from the file and store the contents in the provided buffer. If the provided
    // buffer is not large enough, nothing will be written. In this case, the number of read bytes parameter
    // will be set to zero and FileError::Success will be returned (if there are no other file IO errors).
    // If an error occurs while reading the file, the number of read bytes parameter will be set to 'invalid_size'.
    //
    SHOOTER_API FileError try_read(WriteonlyByteSpan output_buffer, usize read_offset_in_bytes, usize number_of_bytes_to_read, usize& out_number_of_read_bytes);
    
private:
    void* m_native_handle;
    bool m_handle_is_opened;
    OpenPolicy m_open_policy;
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