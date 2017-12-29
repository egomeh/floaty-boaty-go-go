#pragma once

#include "serialization.hpp"

#include "json.hpp"

class JsonSerializer : public Serializer
{
public:
    JsonSerializer(std::string data);
    JsonSerializer(nlohmann::json json);

    bool Serialize(std::string identifier, bool value);
    bool Serialize(std::string identifier, int value);
    bool Serialize(std::string identifier, float value);
    bool Serialize(std::string identifier, double value);
    bool Serialize(std::string identifier, std::string value);
    bool Serialize(std::string identifier, glm::vec2 value);
    bool Serialize(std::string identifier, glm::vec3 value);
    bool Serialize(std::string identifier, glm::vec4 value);

    bool Deserialize(std::string identifier, bool &value);
    bool Deserialize(std::string identifier, int &value);
    bool Deserialize(std::string identifier, float &value);
    bool Deserialize(std::string identifier, double &value);
    bool Deserialize(std::string identifier, std::string &value);
    bool Deserialize(std::string identifier, glm::vec2 &value);
    bool Deserialize(std::string identifier, glm::vec3 &value);
    bool Deserialize(std::string identifier, glm::vec4 &value);

public:
    template<typename T>
    bool Serialize(std::string identifier, T object);

    template<typename T>
    bool Deserialize(std::string identifier, T &object);

private:
    nlohmann::json m_Json;
};

template<typename T>
inline bool JsonSerializer::Serialize(std::string identifier, T object)
{
    object.Serialize(this);
}

template<typename T>
inline bool JsonSerializer::Deserialize(std::string identifier, T &object)
{
    if (m_Json.count(identifier))
    {
        JsonSerializer content(m_Json[identifier]);
        object.Deserialize(content);
    }
}


