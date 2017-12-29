
#include <string>
#include <sstream>
#include <queue>
#include <regex>

#include "util.hpp"

#include "shaderpreprocessor.hpp"

std::string ShaderPreprocessor::InterleaveLineDirectives(std::string shaderSource, std::string name)
{
    std::string output;
    std::size_t bufferSpace = shaderSource.size() + name.size() + 15;
    output.reserve(bufferSpace);

    std::stringstream linestream(shaderSource);
    std::stringstream outputstream(output);

    std::size_t linecounter = 1;

    for (std::string line; SafeGetline(linestream, line);)
    {
        std::string trimmed = TrimBlankNonNewline(line);
        if (line.size() > 0 && line[0] != '#')
        {
            outputstream << "#line " << linecounter << " \"" << name << "\"\n";
        }
        outputstream << line << "\n";
        ++linecounter;
    }

    return outputstream.str();
}

std::string ShaderPreprocessor::DoPreprocess(const std::string &shaderSource, const std::map<std::size_t, std::string> &filemap)
{
    return DoPreprocess(shaderSource, filemap, std::vector<std::size_t>());
}

std::string ShaderPreprocessor::DoPreprocess(const std::string &shaderSource, const std::map<std::size_t, std::string> &filemap, std::vector<std::size_t> &processedFiles)
{
    std::stringstream preprocessedSource;

    std::vector<std::size_t> dependencies;
    std::regex includeExpression("^#include.*\"([^\"]+)\".*$");

    std::stringstream linestream(shaderSource);

    std::smatch matches;
    for (std::string line; SafeGetline(linestream, line);)
    {
        if (std::regex_match(line, matches, includeExpression))
        {
            if (matches.size() == 2)
            {
                std::string includeFile = matches[1];
                std::size_t includeFileHash = std::hash<std::string>{}(includeFile);
                if (std::find(processedFiles.begin(), processedFiles.end(), includeFileHash) == processedFiles.end())
                {
                    const std::string &includeFileContent = filemap.at(includeFileHash);
                    processedFiles.push_back(includeFileHash);
                    preprocessedSource << DoPreprocess(includeFileContent, filemap, processedFiles);
                }
                else
                {
                    preprocessedSource << "\n";
                }
            }
        }
        else
        {
            preprocessedSource << line << "\n";
        }
    }

    return preprocessedSource.str();
}

