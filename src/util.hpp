#pragma once

#include <stdexcept>
#include <ctime>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

#include "definitions.hpp"
#include "glm/glm.hpp"

#define ASSERT(expression, statement) assert(expression && statement)

#ifdef _DEBUG
#define DEBUG_ASSERT(expression, statement) ASSERT(expression, statement)
#else
#define DEBUG_ASSERT(expression, statement)
#endif

inline std::size_t GetUniqueTypeID() noexcept
{
    static std::size_t lastID{0u};
    return lastID++;
}

template <typename T>
inline std::size_t GetTypeID() noexcept
{
    static std::size_t typeID{GetUniqueTypeID()};
    return typeID;
}

inline std::size_t GetUniqueComponentTypeID() noexcept
{
    static std::size_t lastID{0u};
    return lastID++;
}

template <typename T>
inline std::size_t GetComponentTypeID() noexcept
{
    static std::size_t typeID{GetUniqueComponentTypeID()};
    return typeID;
}


class TimeStamp
{
public:
    TimeStamp();
    double GetElapsedTimeMiliSeconds();
    double GetElapsedTimeSeconds();

private:
    const std::clock_t m_ClockMark;
};

void DebugLog(const std::string &message);
void DebugLog(const std::wstring &message);

class StateError : public std::runtime_error
{
public:
    explicit StateError(const char *message) : runtime_error(message) {}
    explicit StateError(std::string message) : runtime_error(message) {}
};

class OSError : public std::runtime_error
{
public:
    explicit OSError(const char *message) : runtime_error(message) {}
    explicit OSError(std::string message) : runtime_error(message) {}
};

template<typename T, typename Map>
T StringToEnum(std::string const &name, const Map &enumNameMap, T defaultValue)
{
    auto result = enumNameMap.find(name);
    
    if (result != enumNameMap.end())
    {
        return result->second;
    }

    return defaultValue;
}

std::istream& SafeGetline(std::istream& is, std::string& t);

std::string GetLastErrorAsString();

std::string TrimBlankNonNewline(const std::string &line);

glm::vec4 HexToVec4(UINT32 hexColor);

struct FrameContext
{
    float deltaTime;
    glm::vec2 deltaMouse;
};

glm::vec3 lerp(const glm::vec3 &a, const glm::vec3 &b, float lerpFactor);

template<typename T>
inline int sign(T val)
{
    return static_cast<int>((T(0) < val) - (val < T(0)));
}

void GetPixelsFromBinaryImage(const std::vector<uint8_t>& textureData, std::vector<glm::vec4>& pixelData, int &width, int &height, int &channels);
void GetPixelsFromBinaryImage(const std::vector<uint8_t>& textureData, std::vector<unsigned char>& pixelData, int &width, int &height, int &channels);

glm::vec3 GetNormalFromHeightSamples(float height, float heightdx, float heightdy, float epsilon);

bool StringEndsWith(std::string const& string, std::string const& postfix);

template<typename ContainerType, typename T>
inline bool Contains(const ContainerType &container, const T &value)
{
    return std::find(container.begin(), container.end(), value) != container.end();
}
