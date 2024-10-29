/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <Core/Math/Color.h>
#include <Core/Math/Vector.h>
#include <Core/String/Format.h>
#include <Core/UUID.h>
#include <yaml-cpp/yaml.h>

namespace YAML
{

#pragma region StringView

ALWAYS_INLINE static YAML::Emitter& operator<<(YAML::Emitter& out, const SE::StringView& value)
{
    const std::string std_string = std::string(value.characters(), value.byte_count());
    out << std_string;
    return out;
}

#pragma endregion StringView

#pragma region String

template<>
struct convert<SE::String>
{
    static Node encode(const SE::String& rhs)
    {
        Node node;
        node.push_back(rhs.characters());
        return node;
    }

    static bool decode(const Node& node, SE::String& rhs)
    {
        const std::string std_string = node.as<std::string>();
        rhs = SE::StringView::create_from_utf8(std_string.data(), std_string.size());
        return true;
    }
};

ALWAYS_INLINE static YAML::Emitter& operator<<(YAML::Emitter& out, const SE::String& value)
{
    const std::string std_string = std::string(value.characters(), value.byte_count());
    out << std_string;
    return out;
}

#pragma endregion String

#pragma region UUID

template<>
struct convert<SE::UUID>
{
    static Node encode(const SE::UUID& rhs)
    {
        const SE::Optional<SE::String> uuid_base_16 = SE::format(SE::StringView::create_from_utf8("{}"), rhs);
        Node node;
        node.push_back(uuid_base_16.value());
        return node;
    }

    static bool decode(const Node& node, SE::UUID& rhs)
    {
        const SE::String uuid_base_16 = node.as<SE::String>();
        SE_ASSERT(uuid_base_16.byte_count() == 16);

        SE::u64 uuid_value = 0;
        for (SE::usize offset = 0; offset < uuid_base_16.byte_count(); ++offset)
        {
            const char digit = uuid_base_16.characters()[offset];

            uuid_value *= 16;
            if ('0' <= digit && digit <= '9')
                uuid_value += (digit - '0');
            else if ('A' <= digit && digit <= 'F')
                uuid_value += (digit - 'A') + 10;
        }

        rhs = SE::UUID(uuid_value);
        return true;
    }
};

ALWAYS_INLINE static YAML::Emitter& operator<<(YAML::Emitter& out, const SE::UUID& value)
{
    const SE::Optional<SE::String> uuid_base_16 = SE::format(SE::StringView::create_from_utf8("{}"), value);
    SE_ASSERT(uuid_base_16.has_value());
    out << uuid_base_16.value();
    return out;
}

#pragma endregion UUID

#pragma region Vector2

template<>
struct convert<SE::Vector2>
{
    static Node encode(const SE::Vector2& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    static bool decode(const Node& node, SE::Vector2& rhs)
    {
        if (!node.IsSequence() || node.size() != 2)
            return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        return true;
    }
};

ALWAYS_INLINE static YAML::Emitter& operator<<(YAML::Emitter& out, const SE::Vector2& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
    return out;
}

#pragma endregion Vector2

#pragma region Vector3

template<>
struct convert<SE::Vector3>
{
    static Node encode(const SE::Vector3& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    static bool decode(const Node& node, SE::Vector3& rhs)
    {
        if (!node.IsSequence() || node.size() != 3)
            return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        return true;
    }
};

ALWAYS_INLINE static YAML::Emitter& operator<<(YAML::Emitter& out, const SE::Vector3& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
    return out;
}

#pragma endregion Vector3

#pragma region Vector4

template<>
struct convert<SE::Vector4>
{
    static Node encode(const SE::Vector4& rhs)
    {
        Node node;
        node.push_back(rhs.x);
        node.push_back(rhs.y);
        node.push_back(rhs.z);
        node.push_back(rhs.w);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    static bool decode(const Node& node, SE::Vector4& rhs)
    {
        if (!node.IsSequence() || node.size() != 4)
            return false;

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        rhs.w = node[3].as<float>();
        return true;
    }
};

ALWAYS_INLINE static YAML::Emitter& operator<<(YAML::Emitter& out, const SE::Vector4& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
    return out;
}

#pragma endregion Vector4

#pragma region Color3

template<>
struct convert<SE::Color3>
{
    static Node encode(const SE::Color3& rhs)
    {
        Node node;
        node.push_back(rhs.r);
        node.push_back(rhs.g);
        node.push_back(rhs.b);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    static bool decode(const Node& node, SE::Color3& rhs)
    {
        if (!node.IsSequence() || node.size() != 3)
            return false;

        rhs.r = node[0].as<float>();
        rhs.g = node[1].as<float>();
        rhs.b = node[2].as<float>();
        return true;
    }
};

ALWAYS_INLINE static YAML::Emitter& operator<<(YAML::Emitter& out, const SE::Color3& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.r << v.g << v.b << YAML::EndSeq;
    return out;
}

#pragma endregion Color3

#pragma region Color4

template<>
struct convert<SE::Color4>
{
    static Node encode(const SE::Color4& rhs)
    {
        Node node;
        node.push_back(rhs.r);
        node.push_back(rhs.g);
        node.push_back(rhs.b);
        node.push_back(rhs.a);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    static bool decode(const Node& node, SE::Color4& rhs)
    {
        if (!node.IsSequence() || node.size() != 4)
            return false;

        rhs.r = node[0].as<float>();
        rhs.g = node[1].as<float>();
        rhs.b = node[2].as<float>();
        rhs.a = node[3].as<float>();
        return true;
    }
};

ALWAYS_INLINE static YAML::Emitter& operator<<(YAML::Emitter& out, const SE::Color4& v)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << v.r << v.g << v.b << v.a << YAML::EndSeq;
    return out;
}

#pragma endregion Color4

} // namespace YAML
