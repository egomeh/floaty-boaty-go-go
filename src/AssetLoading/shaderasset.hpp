#pragma once

#include <memory>
#include <unordered_map>

#include "assetdatabase.hpp"
#include "graphics/shader.hpp"
#include "serialization/serialization.hpp"

namespace ShaderKeywords
{
    static const std::string Name("name");
    static const std::string Vertex("vertex");
    static const std::string Fragment("fragment");
    static const std::string Ignored(" \t");
    static const std::string KeyDelimiter("=");
    static const std::string ValueDelimiter(",");
    static const std::string Quote("\"");
    static const std::string BlendMode("blend");
    static const std::string RenderQueue("queue");
}

namespace ShaderBlendKeywords
{
    static const std::string Off("off");
    static const std::string Add("add");
    static const std::string Alpha("alpha");
    static const std::string Sub("subtract");
    static const std::string Mul("multiply");
}

namespace ShaderRenderQueueKeywords
{
    static const std::string Backgound("background");
    static const std::string Transparent("transparent");
    static const std::string Default("default");
    static const std::string Opaque("opaque");
    static const std::string Overlay("overlay");
}

const std::map<std::string, ShaderBlendMode> blendModeWordMap =
{
    {ShaderBlendKeywords::Off, Off},
    {ShaderBlendKeywords::Add, Add},
    {ShaderBlendKeywords::Alpha, Alpha},
    {ShaderBlendKeywords::Sub, Subtract},
    {ShaderBlendKeywords::Mul, Multiply},
};

const std::map<std::string, RenderQueue> renderQueueMap =
{
    {ShaderRenderQueueKeywords::Backgound, Background},
    {ShaderRenderQueueKeywords::Default, Default},
    {ShaderRenderQueueKeywords::Opaque, Opaque},
    {ShaderRenderQueueKeywords::Overlay, Overlay},
    {ShaderRenderQueueKeywords::Transparent, Transparent},
};

class ShaderImportData
{
public:
    template<typename Serializer>
    void Deserialize(Serializer serializer)
    {
        std::string blend;
        std::string renderqueue;

        DESERIALIZE(blend, serializer);
        DESERIALIZE(renderqueue, serializer);

        DESERIALIZE(vertex, serializer);
        DESERIALIZE(fragment, serializer);

        m_BlendMode = StringToEnum(blend, blendModeWordMap, Off);
        m_RenderQueue = StringToEnum(renderqueue, renderQueueMap, Default);
    }

    std::string vertex;
    std::string fragment;
    ShaderBlendMode m_BlendMode;
    RenderQueue m_RenderQueue;
};

class ShaderAssetFactory : public AssetFactory<Shader>
{
public:
    Shader *GetAsset(const std::string &name) override;
    void RefreshAsset(const std::string &name) override;

    void SetOpenGLVersion(OpenGLVersion version);

private:
    std::unordered_map<std::string, std::shared_ptr<Shader>> m_LoadedAssets;
    OpenGLVersion m_OpenGLVersion;
};
