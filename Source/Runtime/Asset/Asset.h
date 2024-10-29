/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#pragma once

#include <Core/API.h>
#include <Core/Containers/RefPtr.h>
#include <Core/String/String.h>
#include <Core/UUID.h>

namespace SE
{

struct AssetHandle
{
public:
    // Generates a new asset handle, usually randomly.
    NODISCARD SHOOTER_API static AssetHandle create();

    // Returns a special value that marks the asset handle as being invalid.
    NODISCARD ALWAYS_INLINE static constexpr AssetHandle invalid() { return AssetHandle(UUID::invalid()); }

public:
    ALWAYS_INLINE constexpr AssetHandle()
        : m_uuid(invalid().m_uuid)
    {}

    ALWAYS_INLINE constexpr explicit AssetHandle(UUID uuid_value)
        : m_uuid(uuid_value)
    {}

    AssetHandle(const AssetHandle&) = default;
    AssetHandle& operator=(const AssetHandle&) = default;

    NODISCARD ALWAYS_INLINE UUID value() const { return m_uuid; }
    NODISCARD ALWAYS_INLINE bool is_valid() const { return m_uuid.is_valid(); }

    NODISCARD ALWAYS_INLINE bool operator==(const AssetHandle& other) const { return m_uuid == other.m_uuid; }
    NODISCARD ALWAYS_INLINE bool operator!=(const AssetHandle& other) const { return m_uuid != other.m_uuid; }

private:
    UUID m_uuid;
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
