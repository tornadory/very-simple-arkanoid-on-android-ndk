#include <jni.h>

#include <android/log.h>
#include <android_native_app_glue.h>

#include <unistd.h>
#include <memory.h>

#include "Ball.h"
#include "Battledore.h"


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Arkanoid", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "Arkanoid", __VA_ARGS__))

struct coord_state {
    int32_t x;
    int32_t y;
};
class Battledore;
class Ball;
struct engine {
	struct android_app* app;

	int playing;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t width;
	int32_t height;
	coord_state state;
	Battledore* pBattledore;
	Ball*		pBall;
};


static void engine_init_objects(struct engine* engine) {
	if (engine->pBattledore == NULL)
		engine->pBattledore = new Battledore();
	engine->pBattledore->Init(engine->width, engine->height);
	if (engine->pBall == NULL)
		engine->pBall = new Ball();
	engine->pBall->Init(engine->width, engine->height, engine->pBattledore->GetPositionForBall());
}

static void engine_term_objects(struct engine* engine) {
	delete engine->pBattledore;
	engine->pBattledore = NULL;
	delete engine->pBall;
	engine->pBall = NULL;
}


static EGLint findConfigAttrib(EGLDisplay display, EGLConfig config, EGLint attribute, EGLint defaultValue) {
	EGLint value;
	if (eglGetConfigAttrib(display, config, attribute, &value)) {
		return value;
	}
	return defaultValue;
}

static int engine_init_display(struct engine* engine) {
	// initialize OpenGL ES and EGL

	/*
	 * Here specify the attributes of the desired configuration.
	 * Below, we select an EGLConfig with at least 8 bits per color
	 * component compatible with on-screen windows
	 */
	const EGLint attribs[] = {
			EGL_RED_SIZE, 4,
	        EGL_GREEN_SIZE, 4,
	        EGL_BLUE_SIZE, 4,
	        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
	        EGL_NONE };
	EGLint w, h, dummy, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY );

	eglInitialize(display, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
	 * sample, we have a very simplified selection process, where we pick
	 * the first EGLConfig that matches our criteria */
	int num_configs;
	eglChooseConfig(display, attribs, NULL, 0, &num_configs);
	if (num_configs <= 0)
		LOGW("No configs match configSpec");
	EGLConfig* configs = new EGLConfig[num_configs];
	numConfigs = num_configs;
	eglChooseConfig(display, attribs, configs, numConfigs, &num_configs);
	int i_conf = -1;
	for(int i = 0; i < numConfigs; i++){
		config = configs[i];
		EGLint d = findConfigAttrib(display, config, EGL_DEPTH_SIZE, 0);
		EGLint s = findConfigAttrib(display, config, EGL_STENCIL_SIZE, 0);

		// We need at least mDepthSize and mStencilSize bits
		if (d < 0 || s < 0)
			continue;

		// We want an *exact* match for red/green/blue/alpha
		int r = findConfigAttrib(display, config, EGL_RED_SIZE, 0);
		int g = findConfigAttrib(display, config, EGL_GREEN_SIZE, 0);
		int b = findConfigAttrib(display, config, EGL_BLUE_SIZE, 0);
		int a = findConfigAttrib(display, config, EGL_ALPHA_SIZE, 0);

		if (r == 5 && g == 6 && b == 5 && a == 0)
		{
			i_conf = i;
			break;
		}
	}
	if(i_conf == -1)
	{
		LOGW("Unable to get config");
		delete[] configs;
		return -1;
	}
	config = configs[i_conf];
	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	 * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	 * As soon as we picked a EGLConfig, we can safely reconfigure the
	 * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, engine->app->window,
			NULL);

	EGLint attrib_list[] = {
	    EGL_CONTEXT_CLIENT_VERSION, 2,
	    EGL_NONE
	  };


	context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		delete[] configs;
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	engine->display = display;
	engine->context = context;
	engine->surface = surface;
	engine->width = w;
	engine->height = h;

	// Initialize GL state.
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	//glEnable(GL_CULL_FACE);
	//glShadeModel(GL_SMOOTH);
	//glDisable(GL_DEPTH_TEST);
	delete[] configs;
	return 0;
}

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine) {
	if (engine->display == NULL) {
		// No display.
		return;
	}

	// Just fill the screen with a color.
	glClearColor(1., 1., 1., 1.);
	glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


	engine->pBattledore->Draw();
	engine->pBall->Draw();

	eglSwapBuffers(engine->display, engine->surface);
}

static bool check_collision(Battledore* pBattledore, Ball* pBall)
{
	Rect rectBattledore = pBattledore->GetRect();
	Rect rectBall = pBall->GetRect();

	return (rectBall.bottom < rectBattledore.top && rectBall.left >= rectBattledore.left && rectBall.right <= rectBattledore.right);
}

static void engine_update(struct engine* engine) {
	engine->pBattledore->Update();
	engine->pBall->Update();
	if (check_collision(engine->pBattledore, engine->pBall))
		engine->pBall->Collision();
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine) {
	if (engine->display != EGL_NO_DISPLAY ) {
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE,
				EGL_NO_CONTEXT );
		if (engine->context != EGL_NO_CONTEXT ) {
			eglDestroyContext(engine->display, engine->context);
		}
		if (engine->surface != EGL_NO_SURFACE ) {
			eglDestroySurface(engine->display, engine->surface);
		}
		eglTerminate(engine->display);
	}
	engine->playing = 0;
	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the next input event.
 */
static int32_t engine_handle_input(struct android_app* app,
		AInputEvent* event) {
	struct engine* engine = (struct engine*) app->userData;
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		engine->playing = 1;
		engine->state.x = AMotionEvent_getX(event, 0);
		engine->state.y = AMotionEvent_getY(event, 0);
		engine->pBattledore->Move(engine->state.x);
		LOGI("To move %d", engine->state.x);
		return 1;
	}
	return 0;
}

/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
	struct engine* engine = (struct engine*) app->userData;

	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		// The system has asked us to save our current state.
		break;
	case APP_CMD_INIT_WINDOW:
		// The window is being shown, get it ready.
		if (engine->app->window != NULL) {
			engine_init_display(engine);
			engine_init_objects(engine);
			engine_draw_frame(engine);
		}
		break;
	case APP_CMD_TERM_WINDOW:
		// The window is being hidden or closed, clean it up.
		engine_term_objects(engine);
		engine_term_display(engine);
		break;
	case APP_CMD_GAINED_FOCUS:
		// When our app gains focus, we start monitoring the accelerometer.
		break;
	case APP_CMD_LOST_FOCUS:
		// When our app loses focus, we stop monitoring the accelerometer.
		// This is to avoid consuming battery while not being used.
		// Also stop animating.
		engine->playing = 0;
		engine_draw_frame(engine);
		break;
	}
}

void android_main(struct android_app* state) {
	struct engine engine;

	//sleep(5); //for wait GDB


	app_dummy();

	memset(&engine, 0, sizeof(engine));

	engine.app = state;
	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	

	while (1) {
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		while ((ident = ALooper_pollAll(engine.playing ? 10 : -1, NULL, &events,
				(void**) &source)) >= 0) {

			if (source != NULL) {
				source->process(state, source);
			}

			if (state->destroyRequested != 0) {
				return;
			}
		}

		if (engine.playing) {
			engine_update(&engine);
			engine_draw_frame(&engine);
		}

	}
}
