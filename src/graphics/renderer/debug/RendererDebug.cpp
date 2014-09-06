#include "RendererDebug.h"

#include <sstream>

#include "graphics/renderer/core/RendererCoreConfig.h"

bool hasGLError(std::string& errorText)
{
	GLenum code;
	bool fail = false;
	std::stringstream error;

	code = glGetError();
	// Build error string
	while (code != GL_NO_ERROR)
	{
		if (code == GL_INVALID_ENUM)
		{
			error << "An unacceptable value is specified for an enumerated argument. ";
			fail = true;
		}
		else if (code == GL_INVALID_VALUE)
		{
			error << "A numeric argument is out of range. ";
			fail = true;
		}
		else if (code == GL_INVALID_OPERATION)
		{
			error << "The specified operation is not allowed in the current state. ";
			fail = true;
		}
		else if (code == GL_INVALID_FRAMEBUFFER_OPERATION)
		{
			error << "The framebuffer object is not complete. ";
			fail = true;
		}
		else if (code == GL_OUT_OF_MEMORY)
		{
			error << "There is not enough memory left to execute the command. ";
			fail = true;
		}
		code = glGetError();
		if (code != GL_NO_ERROR)
		{
			error << std::endl;
		}
	}

	if (fail)
	{
		errorText = error.str();
	}
	return fail;
}