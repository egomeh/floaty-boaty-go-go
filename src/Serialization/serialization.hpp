#pragma once

#include "definitions.hpp"
#include <string>

#include "glm/glm.hpp"

#define SERIALIZE(variable, serializer) serializer.Serialize(#variable, variable)
#define DESERIALIZE(variable, serializer) serializer.Deserialize(#variable, variable)

#define STRING_TO_ENUM(type) #type

class Serializer
{
public:
    virtual bool Serialize(std::string identifier, bool value) = 0;
    virtual bool Serialize(std::string identifier, int value) = 0;
    virtual bool Serialize(std::string identifier, float value) = 0;
    virtual bool Serialize(std::string identifier, double value) = 0;
    virtual bool Serialize(std::string identifier, std::string value) = 0;
    virtual bool Serialize(std::string identifier, glm::vec2 value) = 0;
    virtual bool Serialize(std::string identifier, glm::vec3 value) = 0;

    virtual bool Deserialize(std::string identifier, bool &value) = 0;
    virtual bool Deserialize(std::string identifier, int &value) = 0;
    virtual bool Deserialize(std::string identifier, float &value) = 0;
    virtual bool Deserialize(std::string identifier, double &value) = 0;
    virtual bool Deserialize(std::string identifier, std::string &value) = 0;
    virtual bool Deserialize(std::string identifier, glm::vec2 &value) = 0;
    virtual bool Deserialize(std::string identifier, glm::vec3 &value) = 0;

public:
    template<typename T>
    bool Serialize(std::string identifier, T object);

    template<typename T>
    bool Serialize(std::string identifier, T &object);
};

template<typename T>
inline bool Serializer::Serialize(std::string identifier, T object)
{
    T.Serialize(this);
}

template<typename T>
inline bool Serializer::Serialize(std::string identifier, T & object)
{
    T.Deserialize(this);
}


