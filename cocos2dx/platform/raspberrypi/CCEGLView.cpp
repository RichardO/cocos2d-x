/*
 * CCEGLViewlinux.cpp
 *
 *  Created on: Aug 8, 2011
 *      Author: laschweinski
 */

#include "CCEGLView.h"
#include "CCGL.h"
#include "bcm_host.h"
#include "ccMacros.h"
#include "CCDirector.h"
#include "touch_dispatcher/CCTouch.h"
#include "touch_dispatcher/CCTouchDispatcher.h"
#include "text_input_node/CCIMEDispatcher.h"

NS_CC_BEGIN

CCEGLView::CCEGLView()
: bIsInit(false)
, m_fFrameZoomFactor(1.0f)
{
	 // this pretty much follows the example from the "sdk".
	   // See hello_triangle2/triangle.c for the original code
	   EGLBoolean result;
	   EGLint num_config;

	   DISPMANX_UPDATE_HANDLE_T dispman_update;
	   VC_RECT_T dst_rect;
	   VC_RECT_T src_rect;

	   bcm_host_init();

	   memset(&state, 0, sizeof(state));
	     // state.keyboard[0].fd = -1;
	     // state.mouse[0].fd = -1;
	     // for(i = 0;i<4;++i)
	        // state.controller[i].fd = -1;

	   if(0 > graphics_get_display_size(0, &state.screen_width, &state.screen_height))
	   {
		   bIsInit = false;
	      return;
	   }

	   EGLint attribute_list[20];
	   int i;
	   for(i = 0;i<20;++i)
	      attribute_list[i] = EGL_NONE;

	   attribute_list[ 0] = EGL_RED_SIZE;
	   attribute_list[ 1] = 8;
	   attribute_list[ 2] = EGL_GREEN_SIZE;
	   attribute_list[ 3] = 8;
	   attribute_list[ 4] = EGL_BLUE_SIZE;
	   attribute_list[ 5] = 8;
	   attribute_list[ 6] = EGL_ALPHA_SIZE;
	   attribute_list[ 7] = 8;
	   attribute_list[ 8] = EGL_DEPTH_SIZE;
	   attribute_list[ 9] = 24;
	   attribute_list[10] = EGL_STENCIL_SIZE;
	   attribute_list[11] = 8;
	   attribute_list[12] = EGL_SURFACE_TYPE;
	   attribute_list[13] = EGL_WINDOW_BIT;

	   // only set these if we actually want multisampling. It seems the moment
	   // a sample count is set it automatically ends up doing 4x MSAA
	   /*
	   if(samples>2)
	   {
	      attribute_list[14] = EGL_SAMPLES;
	      attribute_list[15] = samples;
	      attribute_list[16] = EGL_SAMPLE_BUFFERS;
	      attribute_list[17] = 1;
	   }
	   */

	   EGLint context_attributes[3];
	   context_attributes[0] = EGL_CONTEXT_CLIENT_VERSION;
	   context_attributes[1] = 2;
	   context_attributes[2] = EGL_NONE;

	   state.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	   if(state.display==EGL_NO_DISPLAY)
	   {
		   bIsInit = false;
	      return;
	   }

	   result = eglInitialize(state.display, NULL, NULL);
	   if(EGL_FALSE == result)
	   {
		   bIsInit = false;
	      return;
	   };

	   result = eglChooseConfig(state.display, attribute_list, &state.config, 1, &num_config);
	   if(EGL_FALSE == result)
	   {
		   bIsInit = false;
	      return;
	   };

	   result = eglBindAPI(EGL_OPENGL_ES_API);
	   if(EGL_FALSE == result)
	   {
		   bIsInit = false;
	      return;
	   };

	   state.context = eglCreateContext(state.display, state.config, EGL_NO_CONTEXT, context_attributes);
	   if(state.context==EGL_NO_CONTEXT)
	   {
		   bIsInit = false;
	      return;
	   }

	   dst_rect.x = 0;
	   dst_rect.y = 0;
	   dst_rect.width = state.screen_width;
	   dst_rect.height = state.screen_height;

	   src_rect.x = 0;
	   src_rect.y = 0;
	   src_rect.width = state.screen_width << 16;
	   src_rect.height = state.screen_height << 16;

	   state.dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
	   dispman_update = vc_dispmanx_update_start( 0 );

	   VC_DISPMANX_ALPHA_T alpha_flags;
	   alpha_flags.flags = DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS;
	   alpha_flags.opacity = 255;
	   alpha_flags.mask = 0;

	   state.dispman_element = vc_dispmanx_element_add (
			   dispman_update,
			   state.dispman_display,
			   0/*layer*/,
			   &dst_rect,
			   0/*src*/,
			   &src_rect,
			   DISPMANX_PROTECTION_NONE,
			   &alpha_flags /*alpha*/,
			   0/*clamp*/,
			   (DISPMANX_TRANSFORM_T)0/*transform*/
			   );

	   state.nativewindow.element = state.dispman_element;
	   state.nativewindow.width = state.screen_width;//width;
	   state.nativewindow.height = state.screen_height;//height;
	   vc_dispmanx_update_submit_sync( dispman_update );

	   state.surface = eglCreateWindowSurface( state.display, state.config, &state.nativewindow, NULL );
	   if(state.surface == EGL_NO_SURFACE)
	   {
		   bIsInit = false;
	      return;
	   }

	   result = eglMakeCurrent(state.display, state.surface, state.surface, state.context);
	   if(EGL_FALSE == result)
	   {
		   bIsInit = false;
	      return;
	   }

	   state.window_open = 1;

	   bIsInit = true;
}

CCEGLView::~CCEGLView()
{
}

void keyEventHandle(int iKeyID,int iKeyState) {
	/*
	if (iKeyState ==GLFW_RELEASE) {
		return;
	}

	if (iKeyID == GLFW_KEY_DEL) {
		CCIMEDispatcher::sharedDispatcher()->dispatchDeleteBackward();
	} else if (iKeyID == GLFW_KEY_ENTER) {
		CCIMEDispatcher::sharedDispatcher()->dispatchInsertText("\n", 1);
	} else if (iKeyID == GLFW_KEY_TAB) {

	}*/
}

void charEventHandle(int iCharID,int iCharState) {

	/*

	 if (iCharState ==GLFW_RELEASE) {
		return;
	}

	// ascii char
	CCIMEDispatcher::sharedDispatcher()->dispatchInsertText((const char *)&iCharID, 1);
	*/
}

void mouseButtonEventHandle(int iMouseID,int iMouseState) {
	/*if (iMouseID == GLFW_MOUSE_BUTTON_LEFT) {
        CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();
		//get current mouse pos
		int x,y;
		glfwGetMousePos(&x, &y);
		CCPoint oPoint((float)x,(float)y);

		if (!CCRect::CCRectContainsPoint(s_pMainWindow->m_rcViewPort,oPoint))
		{
			CCLOG("not in the viewport");
			return;
		}

         oPoint.x /= pEGLView->m_fFrameZoomFactor;
         oPoint.y /= pEGLView->m_fFrameZoomFactor;
		int id = 0;
		if (iMouseState == GLFW_PRESS) {
			pEGLView->handleTouchesBegin(1, &id, &oPoint.x, &oPoint.y);

		} else if (iMouseState == GLFW_RELEASE) {
			pEGLView->handleTouchesEnd(1, &id, &oPoint.x, &oPoint.y);
		}
	}*/
}

void mousePosEventHandle(int iPosX,int iPosY) {
	/*int iButtonState = glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT);

	//to test move
	if (iButtonState == GLFW_PRESS) {
            CCEGLView* pEGLView = CCEGLView::sharedOpenGLView();
            int id = 0;
            float x = (float)iPosX;
            float y = (float)iPosY;
            x /= pEGLView->m_fFrameZoomFactor;
            y /= pEGLView->m_fFrameZoomFactor;
            pEGLView->handleTouchesMove(1, &id, &x, &y);
	}*/
}

int closeEventHandle() {
	CCDirector::sharedDirector()->end();
	return GL_TRUE;
}

void CCEGLView::setFrameSize(float width, float height)
{

	//check
	CCAssert(width!=0&&height!=0, "invalid window's size equal 0");

	EGLBoolean result;

	   VC_RECT_T dst_rect;
	   VC_RECT_T src_rect;


	   if(!state.window_open)
	      return;

	   eglMakeCurrent(state.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	   eglDestroySurface(state.display, state.surface);

	   int32_t aWidth = ((int)width);
	   int32_t aHeight = ((int)height);

	   dst_rect.x = 0;
	   dst_rect.y = 0;
	   dst_rect.width = aWidth;
	   dst_rect.height = aHeight;

	   src_rect.x = 0;
	   src_rect.y = 0;
	   src_rect.width = aWidth << 16;
	   src_rect.height = aHeight << 16;

	   DISPMANX_UPDATE_HANDLE_T dispman_update = vc_dispmanx_update_start( 0 );

	   vc_dispmanx_element_remove(dispman_update, state.dispman_element);

	   state.dispman_element = vc_dispmanx_element_add (
			   dispman_update,
			   state.dispman_display,
			   0/*layer*/,
			   &dst_rect,
			   0/*src*/,
			   &src_rect,
			   DISPMANX_PROTECTION_NONE,
			   0 /*alpha*/,
			   0/*clamp*/,
			   (DISPMANX_TRANSFORM_T)0/*transform*/
			   );

	   state.nativewindow.element = state.dispman_element;
	   state.nativewindow.width = aWidth;//width;
	   state.nativewindow.height = aHeight;//height;
	   vc_dispmanx_update_submit_sync( dispman_update );

	   state.surface = eglCreateWindowSurface( state.display, state.config, &state.nativewindow, NULL );
	   if(state.surface == EGL_NO_SURFACE)
	     // return -1;
		   return;

	   result = eglMakeCurrent(state.display, state.surface, state.surface, state.context);
	   if(EGL_FALSE == result)
	      //return -1;
		   return;

	   //update all the CC stuff
	   CCEGLViewProtocol::setFrameSize(width, height);

	   return;
/*
	bool eResult = false;
	int u32GLFWFlags = GLFW_WINDOW;
	//create the window by glfw.

	//check
	CCAssert(width!=0&&height!=0, "invalid window's size equal 0");

	//Inits GLFW
	eResult = glfwInit() != GL_FALSE;

	if (!eResult) {
		CCAssert(0, "fail to init the glfw");
	}

	 Updates window hint
	glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);

	int iDepth = 16; // set default value
	 Depending on video depth
	switch(iDepth)
	{
		 16-bit
		case 16:
		{
			 Updates video mode
			eResult = (glfwOpenWindow(width, height, 5, 6, 5, 0, 16, 8, (int)u32GLFWFlags) != false) ? true : false;

			break;
		}

		 24-bit
		case 24:
		{
			 Updates video mode
			eResult = (glfwOpenWindow(width, height, 8, 8, 8, 0, 16, 8, (int)u32GLFWFlags) != false) ? true : false;

			break;
		}

		 32-bit
		default:
		case 32:
		{
			 Updates video mode
			eResult = (glfwOpenWindow(width, height, 8, 8, 8, 8, 16, 8, (int)u32GLFWFlags) != GL_FALSE) ? true :false;
			break;
		}
	}

	 Success?
	if(eResult)
	{

		 Updates actual size
	  //		glfwGetWindowSize(&width, &height);

		CCEGLViewProtocol::setFrameSize(width, height);		

		 Updates its title
		glfwSetWindowTitle("Cocos2dx-Linux");

		//set the init flag
		bIsInit = true;

		//register the glfw key event
		glfwSetKeyCallback(keyEventHandle);
		//register the glfw char event
		glfwSetCharCallback(charEventHandle);
		//register the glfw mouse event
		glfwSetMouseButtonCallback(mouseButtonEventHandle);
		//register the glfw mouse pos event
		glfwSetMousePosCallback(mousePosEventHandle);

		glfwSetWindowCloseCallback(closeEventHandle);

		//Inits extensions
		eResult = initExtensions();

		if (!eResult) {
			CCAssert(0, "fail to init the extensions of opengl");
		}
		initGL();
	}
*/
}

void CCEGLView::setFrameZoomFactor(float fZoomFactor)
{
    m_fFrameZoomFactor = fZoomFactor;

    setFrameSize(m_obScreenSize.width * fZoomFactor, m_obScreenSize.height * fZoomFactor);

    //glfwSetWindowSize(m_obScreenSize.width * fZoomFactor, m_obScreenSize.height * fZoomFactor);
    CCDirector::sharedDirector()->setProjection(CCDirector::sharedDirector()->getProjection());
}

float CCEGLView::getFrameZoomFactor()
{
    return m_fFrameZoomFactor;
}

void CCEGLView::setViewPortInPoints(float x , float y , float w , float h)
{
    glViewport((GLint)(x * m_fScaleX * m_fFrameZoomFactor+ m_obViewPortRect.origin.x * m_fFrameZoomFactor),
        (GLint)(y * m_fScaleY * m_fFrameZoomFactor + m_obViewPortRect.origin.y * m_fFrameZoomFactor),
        (GLsizei)(w * m_fScaleX * m_fFrameZoomFactor),
        (GLsizei)(h * m_fScaleY * m_fFrameZoomFactor));
}

void CCEGLView::setScissorInPoints(float x , float y , float w , float h)
{
    glScissor((GLint)(x * m_fScaleX * m_fFrameZoomFactor + m_obViewPortRect.origin.x * m_fFrameZoomFactor),
              (GLint)(y * m_fScaleY * m_fFrameZoomFactor + m_obViewPortRect.origin.y * m_fFrameZoomFactor),
              (GLsizei)(w * m_fScaleX * m_fFrameZoomFactor),
              (GLsizei)(h * m_fScaleY * m_fFrameZoomFactor));
}


bool CCEGLView::isOpenGLReady()
{
	return bIsInit;
}

void CCEGLView::end()
{
	/* Exits from GLFW */
	//glfwTerminate();
	//Force close the EGL windows
	bcm_host_deinit();


	delete this;
	exit(0);
}

void CCEGLView::swapBuffers() {
	if (bIsInit) {
		/* Swap buffers */
		//glfwSwapBuffers();
		   if(state.window_open)
		      eglSwapBuffers(state.display, state.surface);
	}
}

void CCEGLView::setIMEKeyboardState(bool bOpen) {

}

bool CCEGLView::initGL()
{
	//bcm_host_init();

 /*   GLenum GlewInitResult = glewInit();
    if (GLEW_OK != GlewInitResult) 
    {
    	CCLog("ERROR: %s\n",glewGetErrorString(GlewInitResult));
        fprintf(stderr,"ERROR: %s\n",glewGetErrorString(GlewInitResult));
        return false;
    }

    if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
    {
        CCLog("Ready for GLSL");
    }
    else 
    {
        CCLog("Not totally ready :(");
    }

    if (glewIsSupported("GL_VERSION_2_0"))
    {
        CCLog("Ready for OpenGL 2.0");
    }
    else
    {
        CCLog("OpenGL 2.0 not supported");
    }

    // Enable point size by default on linux.
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

*/    return true;
}

void CCEGLView::destroyGL()
{
	/*
    if (m_hDC != NULL && m_hRC != NULL)
    {
        // deselect rendering context and delete it
        wglMakeCurrent(m_hDC, NULL);
        wglDeleteContext(m_hRC);
    }
	*/
}

CCEGLView* CCEGLView::sharedOpenGLView()
{
    static CCEGLView* s_pEglView = NULL;
    if (s_pEglView == NULL)
    {
        s_pEglView = new CCEGLView();
    }
    return s_pEglView;
}

NS_CC_END
