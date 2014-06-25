#include "GameObject.h"

GameObject::GameObject():
m_width(0),
m_height(0)
{
	// TODO Auto-generated constructor stub

}

GameObject::~GameObject() {
	// TODO Auto-generated destructor stub
}


GLuint loadShader(GLenum shaderType, const char* pSource) {
	GLenum error = 0;
	GLuint shader = glCreateShader(shaderType);
	error = glGetError();
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        error = glGetError();
        glCompileShader(shader);
        error = glGetError();
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            error = glGetError();
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = new char[infoLen];
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    delete[] buf;
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    GLenum error = 0;
    if (program) {
        glAttachShader(program, vertexShader);
        error = glGetError();
        glAttachShader(program, pixelShader);
        error = glGetError();
        glLinkProgram(program);
//        error = glGetError();
//        GLint linkStatus = GL_FALSE;
//        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
//        if (linkStatus != GL_TRUE) {
//            GLint bufLength = 0;
//            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
//            error = glGetError();
//            if (bufLength) {
//            	char* buf = new char[bufLength];
//                if (buf) {
//                    glGetProgramInfoLog(program, bufLength, NULL, buf);
//                    delete[] buf;
//                }
//            }
//            glDeleteProgram(program);
//            program = 0;
//        }
    }
    return program;
}
