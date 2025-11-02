// Copyright (c) 2024-2025 Traian Avram. All rights reserved.

#pragma once

#include <Runtime/Core/Containers/VectorView.h>
#include <Runtime/Renderer/RHI/RHICore.h>
#include <Runtime/Renderer/RHI/ShaderResource.h>

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
    TEXTURE_FLAG_RENDER_TARGET   = SE_BIT(0),
    TEXTURE_FLAG_SHADER_RESOURCE = SE_BIT(1),
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

enum class Texture2DUploadDataPolicy : uint8
{
    // Updates the contents of the texture immediately. Assumes that the application has ensured that
    // the resource is not in use by the time it called 'UploadData'. Offers the best performance due
    // to not needing to cache the texture data (unlike 'OnNextCommandListUse').
    Immediately,

    // Waits for all command lists submitted until calling 'UploadData' to finish execution. This ensures
    // that the resource is not in use by the time the data is uploaded. Offers the worst performance.
    WaitForDeviceIdle,

    // Upload the data when the texture is first bound by a command list. Assumes that the application ensures
    // that the resource will only be used by that first command list until the upload is complete. Offers good
    // performance, as the device is not waiting for anything, but requires the texture upload data to be internally
    // cached until it is uploaded (unlike 'OnNextCommandListUse').
    OnNextCommandListUse,
};

class Texture2D : public ShaderResource
{
    SE_MAKE_RENDERER_RHI_INTERFACE(Texture2D);

public:
    NODISCARD virtual TextureFormat GetFormat() const = 0;
    NODISCARD virtual TextureFlags GetFlags() const = 0;
    NODISCARD virtual uint32 GetSizeX() const = 0;
    NODISCARD virtual uint32 GetSizeY() const = 0;

    // Can only be called when the texture hasn't been created with the 'TEXTURE_FLAG_RENDER_TARGET' flag.
    // Textures that represent render targets can't be updated, and the single time you can upload data to
    // it is during the initialization process (by using the 'Texture2DInfo::InitialData' field).
    virtual void UploadData(ConstVectorView<uint8> textureData, Texture2DUploadDataPolicy policy) = 0;
};

}
