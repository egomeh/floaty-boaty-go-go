#pragma once

#include <vector>
#include <map>
#include <memory>

enum RenderQueue
{
    Background = 0,
    Default,
    Opaque,
    Transparent,
    Overlay,
    RenderQueueEnd, // Keep last
};

class GraphicsError : public std::runtime_error
{
public:
    explicit GraphicsError(const char *message) : runtime_error(message) {}
    explicit GraphicsError(std::string message) : runtime_error(message) {}
};

class OpenGLVersion
{
public:
    OpenGLVersion() : m_VersionMinor(0), m_VersionMajor(0) {}
    OpenGLVersion(std::size_t major, std::size_t minor);

    std::string GetShaderVersionTag();

    std::size_t GetMajorVersion() const;
    std::size_t GetMinorVersion() const;

    bool operator < (const OpenGLVersion &other) const
    {
        std::size_t otherMajor = other.GetMajorVersion();
        std::size_t otherMinor = other.GetMinorVersion();
        std::size_t otherVersion = 10 * otherMajor + otherMinor;
        std::size_t version = 10 * m_VersionMajor + m_VersionMinor;
        return version < otherVersion;
    }
private:
    std::size_t m_VersionMajor;
    std::size_t m_VersionMinor;
};
