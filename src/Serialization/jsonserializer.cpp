#include "jsonserializer.hpp"

#include "json.hpp"

template<typename T>
bool DeserializePrimitive(nlohmann::json json, std::string identifier, T &object)
{
    if (!json.count(identifier))
    {
        return false;
    }

    object = json[identifier];

    return true;
}

JsonSerializer::JsonSerializer(std::string serializedData)
{
    m_Json = nlohmann::json::parse(serializedData);
}

JsonSerializer::JsonSerializer(nlohmann::json json)
{
    m_Json = json;
}

bool JsonSerializer::Serialize(std::string identifier, bool value)
{
    return false;
}

bool JsonSerializer::Serialize(std::string identifier, int value)
{
    return false;
}

bool JsonSerializer::Serialize(std::string identifier, float value)
{
    return false;
}

bool JsonSerializer::Serialize(std::string identifier, double value)
{
    return false;
}

bool JsonSerializer::Serialize(std::string identifier, std::string value)
{
    return false;
}

bool JsonSerializer::Serialize(std::string identifier, glm::vec2 value)
{
    return false;
}

bool JsonSerializer::Serialize(std::string identifier, glm::vec3 value)
{
    return false;
}

bool JsonSerializer::Serialize(std::string identifier, glm::vec4 value)
{
    return false;
}

bool JsonSerializer::Deserialize(std::string identifier, int &value)
{
    return DeserializePrimitive(m_Json, identifier, value);
}

bool JsonSerializer::Deserialize(std::string identifier, bool &value)
{
    return DeserializePrimitive(m_Json, identifier, value);
}

bool JsonSerializer::Deserialize(std::string identifier, float &value)
{
    return DeserializePrimitive(m_Json, identifier, value);
}

bool JsonSerializer::Deserialize(std::string identifier, double &value)
{
    return DeserializePrimitive(m_Json, identifier, value);
}

bool JsonSerializer::Deserialize(std::string identifier, std::string &value)
{
    return DeserializePrimitive(m_Json, identifier, value);
}

bool JsonSerializer::Deserialize(std::string identifier, glm::vec2 &value)
{
    if (m_Json.count(identifier) != 0)
    {
        glm::vec2 vector = glm::vec2(0.f, 0.f);

        nlohmann::json vectorData = m_Json[identifier];
        vector.x = vectorData[0];
        vector.y = vectorData[1];

        value = vector;

        return true;
    }

    return false;
}

bool JsonSerializer::Deserialize(std::string identifier, glm::vec3 &value)
{
    if (m_Json.count(identifier) != 0)
    {
        glm::vec3 vector = glm::vec3(0.f, 0.f, 0.f);

        const nlohmann::json &vectorData = m_Json[identifier];
        vector.x = vectorData[0];
        vector.y = vectorData[1];
        vector.z = vectorData[2];

        value = vector;

        return true;
    }

    return false;
}

bool JsonSerializer::Deserialize(std::string identifier, glm::vec4 &value)
{
    if (m_Json.count(identifier) != 0)
    {
        glm::vec4 vector = glm::vec4(0.f, 0.f, 0.f, 0.f);

        const nlohmann::json &vectorData = m_Json[identifier];
        vector.x = vectorData[0];
        vector.y = vectorData[1];
        vector.z = vectorData[2];
        vector.w = vectorData[3];

        value = vector;

        return true;
    }

    return false;
}
