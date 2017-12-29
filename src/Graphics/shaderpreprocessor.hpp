#pragma once

#include <string>
#include <vector>
#include <functional>

#include "shader.hpp"


class ShaderPreprocessor
{
public:
    static std::string InterleaveLineDirectives(std::string shaderSource, std::string name);
    static std::string DoPreprocess(const std::string &shaderSource, const std::map<std::size_t, std::string> &filemap);
    static std::string DoPreprocess(const std::string &shaderSource, const std::map<std::size_t, std::string> &filemap, std::vector<std::size_t> &processedFiles);
};

