#include "CShaderPreprocessor.h"

#include <fstream>
#include <sstream>

#include "debug/Log.h"

void CShaderPreprocessor::setIncludePath(const std::string& includePath)
{
    m_includePath = includePath;
}

bool CShaderPreprocessor::preprocess(const std::string& shaderCode,
                                     std::string& preprocessedShaderCode)
{
    // Final string
    std::stringstream ss;

    // Include file
    std::string includeFile;
    // Current state
    int state = 0;

    for (char c : shaderCode)
    {
        if (c == '#' && state == 0)
        {
            state += 1;
        }
        else if (c == 'i' && state == 1)
        {
            state += 1;
        }
        else if (c == 'n' && state == 2)
        {
            state += 1;
        }
        else if (c == 'c' && state == 3)
        {
            state += 1;
        }
        else if (c == 'l' && state == 4)
        {
            state += 1;
        }
        else if (c == 'u' && state == 5)
        {
            state += 1;
        }
        else if (c == 'd' && state == 6)
        {
            state += 1;
        }
        else if (c == 'e' && state == 7)
        {
            state += 1;
        }
        else if (c == ' ' && state == 8)
        {
            state += 1;
        }
        else if (c == '"' && state == 9)
        {
            state += 1;  // State 10 reads includeFile
            includeFile.clear();
        }
        else if (c == '"' && state == 10)
        {
            state = 0;  // Reset state
            std::ifstream include(m_includePath + includeFile);
            if (!include.is_open())
            {
                LOG_ERROR("Failed to open include file %s.", (m_includePath + includeFile).c_str());
                return false;
            }
            ss << include.rdbuf();
            include.close();
        }
        else if (state == 10)
        {
            includeFile.push_back(c);
        }
        else
        {
            state = 0;  // Reset state
            ss << c;    // Write into buffer
        }
    }
    return true;
}