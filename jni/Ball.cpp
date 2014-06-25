#include "Ball.h"
#include <cmath>
#include <stdlib.h>
#include <time.h>


static const char gVertexShader[] =
    "attribute vec4 vPosition;\n"
	"uniform highp vec2 uTrans;\n"
    "void main() {\n"
    "  gl_Position = vPosition;\n"
	"  gl_Position.xy += uTrans;\n"
    "}\n";

static const char gFragmentShader[] =
    "precision mediump float;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
    "}\n";

GLfloat uColor[] = {1.0f, 0.0f, 0.0f};

Ball::Ball():
m_h(0),
m_w(0)

{
	m_Program = createProgram(gVertexShader, gFragmentShader);
	m_vPositionHandle = glGetAttribLocation(m_Program, "vPosition");
	m_vTransHandle 	  = glGetUniformLocation(m_Program, "uTrans");
	m_ColorHandle 	  = glGetUniformLocation(m_Program, "uColor");

}

Ball::~Ball() {
}

void Ball::Init(int32_t width, int32_t height, const Point& pt)
{
	m_w = width;
	m_h = height;
	px = 2.0f / m_w;
	py = 2.0f / m_h;
	m_speed_x = m_w*0.0025f; //0.5%
	m_speed_y = m_h*0.0025f;
	m_radius_x = px*PX_RADIUS;
	m_radius_y = py*PX_RADIUS;
	m_pt.x = pt.x;
	m_pt.y = pt.y + PX_RADIUS;
	srand(time(0));
	//[65;85] U [95;110]
	m_angle = ((rand() % 2) * PI / 6 + PI / 3 + (rand() % 16) + 5) * M_PI / PI;

	InitArray();
}


void Ball::InitArray() {
	int  vertexCount = NUM_SEGMENTS;

	GLfloat center_x = 0.0f;
	GLfloat center_y = 0.0f;

	int idx = 0;

	m_Arr[idx++] = center_x;
	m_Arr[idx++] = center_y;

	int outerVertexCount = vertexCount-1;

	for (int i = 0; i < outerVertexCount; ++i){
		GLfloat percent = (i / (float) (outerVertexCount-1));
		GLfloat rad = percent * 2*M_PI;

	    //vertex position
		GLfloat outer_x = center_x + m_radius_x * cos(rad);
		GLfloat outer_y = center_y + m_radius_y * sin(rad);

	    m_Arr[idx++] = outer_x;
	    m_Arr[idx++] = outer_y;
	}
}

void Ball::Update() {
	m_speed_x += (m_speed_x > 0.0f ? 1.0 : -1.0f) * 0.001f ;
	m_speed_y += (m_speed_y > 0.0f ? 1.0 : -1.0f) * 0.001f;
	m_pt.x += m_speed_x * cos(m_angle);
	m_pt.y += m_speed_y * sin(m_angle);

	if(m_pt.x - PX_RADIUS <= 0.0f)
	{
		m_pt.x = PX_RADIUS;
		m_speed_x *= -1.0f;
	}
	if(m_pt.x + PX_RADIUS >= m_w)
	{
		m_pt.x = m_w - PX_RADIUS;
		m_speed_x *= -1.0f;
	}


	if(m_pt.y + PX_RADIUS >= m_h)
	{
		m_pt.y = m_h - PX_RADIUS;
		m_speed_y *= -1.0f;
	}

}

void Ball::Draw() {
	GLenum error;
	glUseProgram(m_Program);
	error = glGetError();
	glEnableVertexAttribArray(m_vPositionHandle);
	error = glGetError();

	glVertexAttribPointer(m_vPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, m_Arr);
	error = glGetError();
	m_gl_pt.x = m_pt.x * px - 1.0f;
	m_gl_pt.y = m_pt.y * py - 1.0f;
	glUniform2fv(m_vTransHandle, 1, (const GLfloat*)&m_gl_pt);
	error = glGetError();
	glUniform3fv(m_ColorHandle, 1, uColor);
	error = glGetError();
	glDrawArrays(GL_TRIANGLE_FAN, 0, NUM_SEGMENTS);
}

Rect Ball::GetRect()
{
	Rect rc(m_pt.x - PX_RADIUS, m_pt.y + PX_RADIUS, m_pt.x + PX_RADIUS, m_pt.y - PX_RADIUS);
	return rc;
}

void Ball::Collision()
{
	m_speed_y *= -1.0f;
}
