#include "GameObject.h"

#ifndef BATTLEDORE_H_
#define BATTLEDORE_H_

#define BATTLEDORE_HEIGHT		30

class Battledore: public GameObject {
	GLfloat m_Arr[4 * 2];

	GLuint m_Program;
	GLuint m_vPositionHandle;
	GLuint m_vTransHandle;
	GLuint m_ColorHandle;

	GLfloat m_speed;

	int m_screen_w;
	int m_screen_h;

	GLfloat m_widtn;
	GLfloat m_height;

	GLfloat px;
	GLfloat py;
	int m_edge;
	void InitArray();
public:
	Battledore();
	virtual ~Battledore();

	virtual void Update();
	virtual void Draw();
	void Init(int32_t width, int32_t height);
	void Move(int32_t x);
	Point GetPositionForBall();
	Rect GetRect();
};

#endif /* BATTLEDORE_H_ */
