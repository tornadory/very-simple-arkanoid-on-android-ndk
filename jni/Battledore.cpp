#include "Battledore.h"
#include <cmath>
#include <stdlib.h>
#include <time.h>

static const char gVertexShader_Battledore[] =
"attribute vec4 vPosition;\n"
"uniform highp vec2 uTrans;\n"
"void main() {\n"
"  gl_Position = vPosition;\n"
"  gl_Position.xy += uTrans;\n"
"}\n";

static const char gFragmentShader_Battledore[] =
"precision mediump float;\n"
"void main() {\n"
"  gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);\n"
"}\n";

GLfloat uColor_Battledore[] = { 0.0f, 0.0f, 1.0f };

Battledore::Battledore() :
m_screen_w(0),
m_screen_h(0)
{
	m_Program = createProgram(gVertexShader_Battledore, gFragmentShader_Battledore);
	m_vPositionHandle = glGetAttribLocation(m_Program, "vPosition");
	m_vTransHandle = glGetUniformLocation(m_Program, "uTrans");
	m_ColorHandle = glGetUniformLocation(m_Program, "uColor");
}

Battledore::~Battledore() {
	// TODO Auto-generated destructor stub
}

void Battledore::Init(int32_t width, int32_t height)
{
	m_screen_w = width;
	m_screen_h = height;
	px = 2.0f / m_screen_w;
	py = 2.0f / m_screen_h;
	m_speed = m_screen_w*0.0025f; //0.5%
	m_edge = m_screen_w / 4;
	m_width = px*m_edge;
	m_height = py*BATTLEDORE_HEIGHT;

	srand(time(0));
	m_pt.x = m_screen_w/2 + ((rand() % m_screen_w)/6 * ((rand() % 2 ? -1.0f : 1.0f)));
	m_pt.y = BATTLEDORE_HEIGHT / 2;
	InitArray();
}

void Battledore::InitArray(){

	int  vertexCount = 4;

	int idx = 0;

	m_Arr[idx++] = -m_width/2;
	m_Arr[idx++] = -m_height/2;

	m_Arr[idx++] = -m_width/2;
	m_Arr[idx++] = m_height/2;

	m_Arr[idx++] = m_width / 2;
	m_Arr[idx++] = -m_height/2;

	m_Arr[idx++] = m_width / 2;
	m_Arr[idx++] = m_height/2;

}

void Battledore::Update() {
	m_speed += (m_speed > 0.0f ? 1.0 : -1.0f) * 0.001f;
	m_pt.x += m_speed;

	if (m_pt.x - m_edge/2 <= 0.0f)
	{
		m_pt.x = m_edge/2;
	}
	if (m_pt.x + m_edge/2 >= m_screen_w)
	{
		m_pt.x = m_screen_w - m_edge/2;
	}
}

void Battledore::Draw() {
	glUseProgram(m_Program);
	glEnableVertexAttribArray(m_vPositionHandle);
	glVertexAttribPointer(m_vPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, m_Arr);
	m_gl_pt.x = m_pt.x * px - 1.0f;
	m_gl_pt.y = m_pt.y * py - 1.0f;
	glUniform2fv(m_vTransHandle, 1, (const GLfloat*)&m_gl_pt);
	glUniform3fv(m_ColorHandle, 1, uColor_Battledore);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


void Battledore::Move(int32_t x)
{
	if (x > m_pt.x && m_speed < 0.0f || x <= m_pt.x && m_speed > 0.0f)
		m_speed *= -1.0f;
}

Point Battledore::GetPositionForBall()
{
	Point pt;
	pt.x = m_pt.x;
	pt.y = m_pt.y + BATTLEDORE_HEIGHT / 2;
	return pt;
}

Rect Battledore::GetRect()
{
	Rect rc(m_pt.x - m_edge, m_pt.y + BATTLEDORE_HEIGHT / 2, m_pt.x + m_edge, m_pt.y - BATTLEDORE_HEIGHT / 2);
	return rc;
}


