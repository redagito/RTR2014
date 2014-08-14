#pragma once

template <class T>
class Shader
{
   public:
    Shader() {}
    ~Shader() {}

    GLuint getProgramID() { return m_program_id; }

    bool init()
    {
        // TODO code is copied from opengl-tutorials.org
        // needs to be moved into utility class

        // TODO should return false, if fails

        // Create the shaders
        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        GLint Result = GL_FALSE;
        int InfoLogLength;

        // Compile Vertex Shader
        char const* VertexSourcePointer = T::VS;
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
        glCompileShader(VertexShaderID);

        // Check Vertex Shader
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0)
        {
            std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            printf("%s\n", &VertexShaderErrorMessage[0]);
        }

        // Compile Fragment Shader
        char const* FragmentSourcePointer = T::FS;
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
        glCompileShader(FragmentShaderID);

        // Check Fragment Shader
        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0)
        {
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL,
                               &FragmentShaderErrorMessage[0]);
            printf("%s\n", &FragmentShaderErrorMessage[0]);
        }

        // Link the program
        printf("Linking program\n");
        m_program_id = glCreateProgram();
        glAttachShader(m_program_id, VertexShaderID);
        glAttachShader(m_program_id, FragmentShaderID);
        glLinkProgram(m_program_id);

        // Check the program
        glGetProgramiv(m_program_id, GL_LINK_STATUS, &Result);
        glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0)
        {
            std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
            glGetProgramInfoLog(m_program_id, InfoLogLength, NULL, &ProgramErrorMessage[0]);
            printf("%s\n", &ProgramErrorMessage[0]);
        }

        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);

        return true;
    }

   private:
    GLuint m_program_id;
};
