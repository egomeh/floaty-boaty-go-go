#include "definitions.hpp"

#include <iostream>
#include <string>
#include <ctime>

#include <windows.h>

#include "util.hpp"

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#if !_DEBUG
void DebugLog(const std::string &message) {}
#else
void DebugLog(const std::string &message)
{
#ifdef GUIONLY
    std::wstring wMessage = std::wstring(message.begin(), message.end());
    OutputDebugString(wMessage.c_str());
    OutputDebugString(L"\n");
#else
    std::cout << message << std::endl;
#endif
}
#endif

#if !_DEBUG
void DebugLog(const std::wstring &message) {}
#else
void DebugLog(const std::wstring &message)
{
#ifdef GUIONLY
    OutputDebugString(message.c_str());
    OutputDebugString(L"\n");
#else
    std::wcout << message << std::endl;
#endif
}
#endif

std::string GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}

std::string TrimBlankNonNewline(const std::string &line)
{
    std::size_t firstNonSpace = line.find_first_not_of(" \t");
    std::size_t lastNonSpace = line.find_last_not_of(" \t");

    if (firstNonSpace == std::string::npos || lastNonSpace == std::string::npos)
    {
        return "";
    }
    else
    {
        return std::string(line.begin() + firstNonSpace, line.begin() + lastNonSpace + 1);
    }
}

glm::vec4 HexToVec4(UINT32 hexColor)
{
    float r = ((hexColor >> 16) & 0xFF) / 255.f;
    float g = ((hexColor >> 8) & 0xFF) / 255.f;
    float b = (hexColor & 0xFF) / 255.f;
    return glm::vec4(r, g, b, 1.f);
}

glm::vec3 lerp(const glm::vec3 &a, const glm::vec3 &b, float lerpFactor)
{
    return lerpFactor * b + (1.0f - lerpFactor) * a;
}

void GetPixelsFromBinaryImage(const std::vector<uint8_t> &textureData, std::vector<glm::vec4> &pixelData, int &width, int &height, int &channels)
{
    pixelData.clear();

    unsigned char* imageData = stbi_load_from_memory(textureData.data(), static_cast<int>(textureData.size()), &width, &height, &channels, STBI_rgb_alpha);

    for (int i = 0; i < width * height; ++i)
    {
        glm::vec4 pixel;
        int index = i * 4;
        pixel.r = imageData[index] / 255.0f;
        pixel.g = imageData[index + 1] / 255.0f;
        pixel.b = imageData[index + 2] / 255.0f;
        pixel.a = imageData[index + 3] / 255.0f;

        pixelData.push_back(pixel);
    }

    stbi_image_free(imageData);
}

void GetPixelsFromBinaryImage(const std::vector<uint8_t> &textureData, std::vector<unsigned char> &pixelData, int &width, int &height, int &channels)
{
    pixelData.clear();

    unsigned char* imageData = stbi_load_from_memory(textureData.data(), static_cast<int>(textureData.size()), &width, &height, &channels, STBI_rgb_alpha);

    for (int i = 0; i < width * height; ++i)
    {
        pixelData.push_back(imageData[i]);
    }

    stbi_image_free(imageData);
}

glm::vec3 GetNormalFromHeightSamples(float height, float heightdx, float heightdy, float epsilon)
{
    glm::vec3 offsetx = glm::vec3(epsilon, heightdx - height, 0.0f);
    glm::vec3 offsety = glm::vec3(0.0f, heightdy - height, epsilon);

    const glm::vec3 normal =  glm::normalize(glm::cross(offsety, offsetx));

    return normal;
}

//  From https://stackoverflow.com/questions/20446201/how-to-check-if-string-ends-with-txt
bool StringEndsWith(std::string const & string, std::string const & postfix)
{
    return postfix.size() <= string.size()
        && string.find(postfix, string.size() - postfix.size()) != string.npos;
}

TimeStamp::TimeStamp() : m_ClockMark(std::clock())
{
}

std::istream& SafeGetline(std::istream& is, std::string& t)
{
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for (;;)
    {
        int c = sb->sbumpc();
        switch (c)
        {
            case '\n':
                return is;
            case '\r':
                if (sb->sgetc() == '\n')
                    sb->sbumpc();
                return is;
            case EOF:
                // Also handle the case when the last line has no line ending
                if (t.empty())
                    is.setstate(std::ios::eofbit);
                return is;
            default:
                t += (char)c;
        }
    }
}

double TimeStamp::GetElapsedTimeMiliSeconds()
{
    std::clock_t deltaTime = std::clock() - m_ClockMark;
    return deltaTime / static_cast<double>(CLOCKS_PER_SEC / 1000);
}

double TimeStamp::GetElapsedTimeSeconds()
{
    std::clock_t deltaTime = std::clock() - m_ClockMark;
    return deltaTime / static_cast<double>(CLOCKS_PER_SEC);
}
