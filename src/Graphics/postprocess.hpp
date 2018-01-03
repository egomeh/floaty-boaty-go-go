#pragma once

#include "logic.hpp"

class PostProcess
{
public:
    int order;
    LogicComponent *logicComponent;

    bool operator ==(const PostProcess &other);
    bool operator <(const PostProcess &other);
};
