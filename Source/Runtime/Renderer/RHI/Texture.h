// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/VectorView.h>
#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/RHI/ShaderResource.h>

#include <string>

namespace SE
{

enum class TextureFormat : uint8
{
    Unknown = 0,
    R8G8B8,
    R8G8B8A8,
    B8G8R8A8,
    D24S8,
    MaxEnumValue,
};

NODISCARD FORCEINLINE bool IsTextureDepthFormat(TextureFormat format)
{
    if (format == TextureFormat::D24S8) { return true; }

    return false;
}

enum TextureFlagsEnum : uint32
{
    TEXTURE_FLAG_NONE            = 0,
    TEXTURE_FLAG_RENDER_TARGET   = BIT(0),
    TEXTURE_FLAG_SHADER_RESOURCE = BIT(1),
};
using TextureFlags = uint32;

enum class TextureFilter : uint8
{
    Linear = 0,
    Nearest,
};

enum class TextureAddressMode : uint8
{
    Repeat = 0,
    ClampToEdge,
    MirroredRepeat,
    MirroredClampToEdge,
    ClampToBorder,
};

struct Texture2DInfo
{
public:
    TextureFormat          Format       { TextureFormat::Unknown };
    TextureFlags           Flags        { TEXTURE_FLAG_NONE };
    uint32                 SizeX        { 0 };
    uint32                 SizeY        { 0 };
    TextureFilter          MinFilter    { TextureFilter::Linear };
    TextureFilter          MagFilter    { TextureFilter::Linear };
    TextureAddressMode     AddressModeU { TextureAddressMode::Repeat };
    TextureAddressMode     AddressModeV { TextureAddressMode::Repeat };
    ConstVectorView<uint8> InitialData;

public:
    inline Texture2DInfo& SetFormat       (TextureFormat format)          { Format = format;              return *this; }
    inline Texture2DInfo& SetFlags        (TextureFlags flags)            { Flags = flags;                return *this; }
    inline Texture2DInfo& SetSizeX        (uint32 sizeX)                  { SizeX = sizeX;                return *this; }
    inline Texture2DInfo& SetSizeY        (uint32 sizeY)                  { SizeY = sizeY;                return *this; }
    inline Texture2DInfo& SetSize         (uint32 sizeX, uint32 sizeY)    { SizeX = sizeX; SizeY = sizeY; return *this; }
    inline Texture2DInfo& AddFlags        (TextureFlags flags)            { Flags |= flags;               return *this; }
    inline Texture2DInfo& SetMinFilter    (TextureFilter filter)          { MinFilter = filter;           return *this; }
    inline Texture2DInfo& SetMagFilter    (TextureFilter filter)          { MagFilter = filter;           return *this; }
    inline Texture2DInfo& SetAddressModeU (TextureAddressMode adressMode) { AddressModeU = adressMode;    return *this; }
    inline Texture2DInfo& SetAddressModeV (TextureAddressMode adressMode) { AddressModeV = adressMode;    return *this; }
    inline Texture2DInfo& SetInitialData  (ConstVectorView<uint8> data)   { InitialData = data;           return *this; }

    inline Texture2DInfo& SetFilters      (TextureFilter minFilter, TextureFilter magFilter)
    {
        MinFilter = minFilter;
        MagFilter = magFilter;
        return *this;
    }

    inline Texture2DInfo& SetAddressModes (TextureAddressMode addressModeU, TextureAddressMode addressModeV)
    {
        AddressModeU = addressModeU;
        AddressModeV = addressModeV;
        return *this;
    }
};

class Texture2D : public ShaderResource
{
    SE_MAKE_RENDERER_RHI_INTERFACE(Texture2D);

public:
    NODISCARD virtual TextureFormat GetFormat() const = 0;
    NODISCARD virtual TextureFlags GetFlags() const = 0;
    NODISCARD virtual uint32 GetSizeX() const = 0;
    NODISCARD virtual uint32 GetSizeY() const = 0;
};

}
