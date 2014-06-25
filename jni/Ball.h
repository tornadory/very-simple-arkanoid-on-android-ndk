#include "GameObject.h"
typedef void (*callback_out)(void* data);

#ifndef BALL_H_
#define BALL_H_


#define PI 180
#define NUM_SEGMENTS 	30
#define PX_RADIUS		20

class Ball: public GameObject {
	GLfloat m_Arr[NUM_SEGMENTS*2];

	GLuint m_Program;
	GLuint m_vPositionHandle;
	GLuint m_vTransHandle;
	GLuint m_ColorHandle;

	GLfloat m_speed;
	double m_angle;

	int m_w;
	int m_h;

	GLfloat m_speed_x;
	GLfloat m_speed_y;
	GLfloat m_radius_x;
	GLfloat m_radius_y;

	GLfloat px;
	GLfloat py;

	void InitArray();
public:
	Ball();
	virtual ~Ball();

	virtual void Update();
	virtual void Draw();
			void Init(int32_t width, int32_t height, const Point& pt);
			Rect GetRect();
			void Collision();
};

#endif /* BALL_H_ */
