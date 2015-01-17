#pragma once

#include <string>

class CShaderPreprocessor
{
public:
	void setIncludePath(const std::string& includePath);

	bool preprocess(const std::string& shaderCode, std::string& preprocessedShaderCode);

private:
	std::string m_includePath;
};