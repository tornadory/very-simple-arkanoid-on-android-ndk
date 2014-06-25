#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_
#include <EGL/egl.h>
#include <GLES2/gl2.h>


class Point {
public:
	GLfloat x;
	GLfloat y;
};

class Rect {
public:
	Rect(GLfloat _left, GLfloat _top, GLfloat _right, GLfloat _bottom):
		left(_left),
		top(_top),
		right(_right),
		bottom(_bottom)
	{
	}

	GLfloat left;
	GLfloat top;
	GLfloat right;
	GLfloat bottom;
};

class GameObject {
protected:
	GLfloat m_height;
	GLfloat m_width;
	Point 	m_pt;
	Point	m_gl_pt;
public:
	GameObject();
	virtual ~GameObject();

	virtual void Update() = 0;
	virtual void Draw() = 0;
};

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);
GLuint loadShader(GLenum shaderType, const char* pSource);


#endif /* GAMEOBJECT_H_ */
