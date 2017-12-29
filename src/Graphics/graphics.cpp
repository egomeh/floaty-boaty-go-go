#include "graphics.hpp"

#include <algorithm>
#include <sstream>

OpenGLVersion::OpenGLVersion(std::size_t major, std::size_t minor) :
    m_VersionMajor(major), m_VersionMinor(minor)
{
}

std::string OpenGLVersion::GetShaderVersionTag()
{
    std::stringstream versionTagStream;

    versionTagStream << "#version ";
    versionTagStream << m_VersionMajor << m_VersionMinor << "0";
    versionTagStream << std::endl << std::endl;

    return versionTagStream.str();
}

std::size_t OpenGLVersion::GetMajorVersion() const
{
    return m_VersionMajor;
}

std::size_t OpenGLVersion::GetMinorVersion() const
{
    return m_VersionMinor;
}
