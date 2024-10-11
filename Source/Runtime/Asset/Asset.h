/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/Containers/RefPtr.h>
#include <Core/Containers/String.h>

namespace SE
{

struct AssetHandle
{
public:
    // Generates a new asset handle, usually randomly.
    NODISCARD SHOOTER_API static AssetHandle create();

    // Returns a special value that marks the asset handle as being invalid.
    NODISCARD ALWAYS_INLINE static constexpr AssetHandle invalid() { return AssetHandle(0); }

public:
    ALWAYS_INLINE constexpr AssetHandle()
        : m_handle_value(invalid().m_handle_value)
    {}

    ALWAYS_INLINE constexpr explicit AssetHandle(u64 handle_value)
        : m_handle_value(handle_value)
    {}

    AssetHandle(const AssetHandle&) = default;
    AssetHandle& operator=(const AssetHandle&) = default;

    NODISCARD ALWAYS_INLINE operator u64&() { return m_handle_value; }
    NODISCARD ALWAYS_INLINE operator const u64&() const { return m_handle_value; }

    NODISCARD ALWAYS_INLINE bool operator==(const AssetHandle& other) const { return m_handle_value == other.m_handle_value; }
    NODISCARD ALWAYS_INLINE bool operator!=(const AssetHandle& other) const { return m_handle_value != other.m_handle_value; }

private:
    u64 m_handle_value;
};

//
// Enumeration of all asset types used by the engine.
//
enum class AssetType : u16
{
    Unknown = 0,
    Texture,
};

//
// Enumeration of all asset states used by the engine.
//
enum class AssetState : u8
{
    Unknown = 0,
    Unloaded,
    Ready,
};

class AssetMetadata
{
public:
    AssetType type = AssetType::Unknown;
    AssetState state = AssetState::Unknown;
    AssetHandle handle = AssetHandle::invalid();
};

//
// Base class for assets in the engine.
// Represents a generic asset that is managed by the engine asset manager.
//
// NOTE: Any class derived from this must implement the 'get_static_type' function, otherwise the
//       asset is not completely registered by the engine.
//
class Asset : public RefCounted
{
public:
    NODISCARD ALWAYS_INLINE static AssetType get_static_type() { return AssetType::Unknown; }
    NODISCARD ALWAYS_INLINE AssetType get_type() const { return m_type; }

    virtual ~Asset() = default;

protected:
    ALWAYS_INLINE Asset(AssetType type)
        : m_type(type)
    {}

private:
    const AssetType m_type;
};

// Convert between an asset type and its string representation.
// Usually, an asset type string is the same with the AssetType enum member.
NODISCARD SHOOTER_API String get_asset_type_string(AssetType asset_type);
NODISCARD SHOOTER_API AssetType get_asset_type_from_string(StringView asset_type_string);

// Convert between an asset type and the corresponding asset file extension.
NODISCARD SHOOTER_API String get_asset_type_file_extension(AssetType asset_type);
NODISCARD SHOOTER_API AssetType get_asset_type_from_file_extension(StringView file_extension);

} // namespace SE
