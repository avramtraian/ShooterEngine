// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Renderer/RHI/RHICore.h>

#include <string>

namespace SE
{

enum class TextureFormat : uint8
{
    Unknown = 0,
    R8G8B8,
    R8G8B8A8,
    B8G8R8A8,
    MaxEnumValue,
};

enum class TextureLayout : uint16
{
    Undefined = 0,
    PresentSource,
    ShaderReadOptimal,
};

enum TextureFlagsEnum : uint32
{
    TEXTURE_FLAG_NONE            = 0,
    TEXTURE_FLAG_RENDER_TARGET   = BIT(0),
    TEXTURE_FLAG_SHADER_RESOURCE = BIT(1),
};
using TextureFlags = uint32;

struct Texture2DInfo
{
public:
    std::string DebugName;
    TextureFormat Format     { TextureFormat::Unknown };
    TextureFlags Flags       { TEXTURE_FLAG_NONE };
    uint32 SizeX             { 0 };
    uint32 SizeY             { 0 };

public:
    inline Texture2DInfo& SetDebugName (std::string_view debugName) { DebugName = debugName;        return *this; }
    inline Texture2DInfo& SetFormat    (TextureFormat format)       { Format = format;              return *this; }
    inline Texture2DInfo& SetFlags     (TextureFlags flags)         { Flags = flags;                return *this; }
    inline Texture2DInfo& SetSizeX     (uint32 sizeX)               { SizeX = sizeX;                return *this; }
    inline Texture2DInfo& SetSizeY     (uint32 sizeY)               { SizeY = sizeY;                return *this; }
    inline Texture2DInfo& SetSize      (uint32 sizeX, uint32 sizeY) { SizeX = sizeX; SizeY = sizeY; return *this; }
    inline Texture2DInfo& AddFlags     (TextureFlags flags)         { Flags |= flags;               return *this; }
};

class Texture2D
{
    SE_MAKE_RENDERER_RHI_INTERFACE(Texture2D);

public:
    NODISCARD virtual TextureFormat GetFormat() const = 0;
    NODISCARD virtual TextureFlags GetFlags() const = 0;
    NODISCARD virtual uint32 GetSizeX() const = 0;
    NODISCARD virtual uint32 GetSizeY() const = 0;
};

}
