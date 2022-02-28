#include <MyCompositor.h>
#include <math.h>
#include <WOpenGL.h>
#include <unistd.h>
#include <WWayland.h>
#include <linux/input-event-codes.h>
#include <stdio.h>
#include <MyClient.h>

MyCompositor::MyCompositor(){}


GLuint LoadShader(GLenum type, const char *shaderSrc)
{
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader(type);
    WOpenGL::checkGLError("21");

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, nullptr);
    WOpenGL::checkGLError("22");

    // Compile the shader
    glCompileShader(shader);
    WOpenGL::checkGLError("23");

    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    WOpenGL::checkGLError("24");

    if (!compiled)
    {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        WOpenGL::checkGLError("25");

        GLchar *errorLog = new GLchar(infoLen);

        glGetShaderInfoLog(shader, infoLen, &infoLen, errorLog);
        WOpenGL::checkGLError("26");

        printf("%s",errorLog);

        delete errorLog;

        glDeleteShader(shader);
        WOpenGL::checkGLError("27");
        return 0;
    }

    return shader;
}



void MyCompositor::initializeGL()
{
    /*************************************************
     * Here you initialize the OpenGL ES 2 context
     *************************************************/

    GLchar *vShaderStr = WOpenGL::openShader("../MyCompositor/shaders/Vertex.glsl");

    GLchar *fShaderStr = WOpenGL::openShader("../MyCompositor/shaders/Fragment.glsl");

    GLuint vertexShader,fragmentShader,programObject;

    // Load the vertex/fragment shaders
    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

    delete [] vShaderStr;
    delete [] fShaderStr;

    // Create the program object
    programObject = glCreateProgram(); 

    glAttachShader(programObject, vertexShader);

    glAttachShader(programObject, fragmentShader);

    // Bind vPosition to attribute 0
    glBindAttribLocation(programObject, 0, "vertexPosition");

    // Link the program
    glLinkProgram(programObject);

    GLint linked;

    // Check the link status
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);

    if (!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        glDeleteProgram(programObject);
        return exit(-1);
    }

    // Enable alpha blending
    glEnable(GL_BLEND);

    // Set blend mode
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set clear screen color
    glClearColor(0.0f, 0.3f, 0.5f, 1.0f);

    // Use the program object
    glUseProgram(programObject);

    // Set the viewport
    glViewport(0, 0, screenWidth(), screenHeight());

    // Load the vertex data
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, square);

    // Enables the vertex array
    glEnableVertexAttribArray(0);

    // Get Uniform Variables
    screenUniform = glGetUniformLocation(programObject, "screen"); // (width,height) Screen dimensions

    rectUniform = glGetUniformLocation(programObject, "rect");     // (left,top,with,height) App window pos and size

    glUniform1i(glGetUniformLocation(programObject, "application"), 0);

    // Set screen size
    glUniform2f(screenUniform,screenWidth(),screenHeight());

    glActiveTexture(GL_TEXTURE0);

    // Create cursor texture
    unsigned char cursorPixels[4*32*32];
    for(int i = 0; i < 4*32*32; i++)
        cursorPixels[i] = 255;

    defaultCursorTexture = new WTexture();
    defaultCursorTexture->setData(32,32,&cursorPixels);

}

void MyCompositor::paintGL()
{
    /*************************************************
     *  Here you do your drawing
     *************************************************/

    glClear(GL_COLOR_BUFFER_BIT);

    /*
    for(list<WClient*>::iterator client = clients.begin(); client != clients.end();client++)
    {
        for(list<WSurface*>::iterator surface = (*client)->surfaces.begin(); surface != (*client)->surfaces.end(); ++surface)
        {
            if((*surface)->getType() == SurfaceType::Undefined) continue;

            glBindTexture(GL_TEXTURE_2D,(*surface)->texture->textureId());
            WOpenGL::checkGLError("21");

            glUniform4f(rectUniform,(*surface)->getX(),(*surface)->getY(),(*surface)->texture->width(),(*surface)->texture->height());
            WOpenGL::checkGLError("22");

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            WOpenGL::checkGLError("23");
        }
    }
    */

    for(list<WSurface*>::iterator surface = surfacesList.begin(); surface != surfacesList.end(); ++surface)
    {
        if((*surface)->getType() == SurfaceType::Undefined) continue;

        glBindTexture(GL_TEXTURE_2D,(*surface)->texture->textureId());
        WOpenGL::checkGLError("21");

        glUniform4f(rectUniform,(*surface)->getX(),(*surface)->getY(),(*surface)->texture->width(),(*surface)->texture->height());
        WOpenGL::checkGLError("22");

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        WOpenGL::checkGLError("23");
    }

    drawCursor();
}

WClient *MyCompositor::newClientRequest(wl_client *client)
{
    /*******************************************************************************
     * Notify a new client connection, it's automatically added to an internal std list
     * you can access with 'compositor->clients'
     *******************************************************************************/

    printf("New client connected.\n");

    return new MyClient(client,this);
}

void MyCompositor::clientDisconnectRequest(WClient *)
{
    /*******************************************************************************
     * Notify a client disconnection, it's automatically removed from the internal
     * 'compositor->clients' list.
     * All destroy events from resources related to the client (like surfaces, regions, etc)
     * are prevously notified.
     *******************************************************************************/
    printf("Client disconnected.\n");
}

void MyCompositor::setCursorRequest(WSurface *_cursorSurface, Int32 hotspotX, Int32 hotspotY)
{
    (void)hotspotX;(void)hotspotY;
    cursorSurface = _cursorSurface;
}

void MyCompositor::libinputEvent(libinput_event *)
{

    /*************************************************
     * If you wish to process libinput events manually
     *************************************************/

    // libinput_event_type eventType = libinput_event_get_type(event);
}

void MyCompositor::pointerPosChanged(double x, double y, UInt32 milliseconds)
{
    WSurface *surface;

    if(resizingSurface != nullptr)
    {
        switch(resizeEdge)
        {
            case ResizeEdge::Bottom:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitialSurfaceRect.width,
                            resizeInitialSurfaceRect.height + (y - resizeInitialMousePos.y),
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
            case ResizeEdge::Right:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitialSurfaceRect.width + (x - resizeInitialMousePos.x),
                            resizeInitialSurfaceRect.height,
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
            case ResizeEdge::BottomRight:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitialSurfaceRect.width + (x - resizeInitialMousePos.x),
                            resizeInitialSurfaceRect.height + (y - resizeInitialMousePos.y),
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
            case ResizeEdge::Top:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitialSurfaceRect.width,
                            resizeInitialSurfaceRect.height + (resizeInitialMousePos.y - y),
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
            case ResizeEdge::Left:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitialSurfaceRect.width + (resizeInitialMousePos.x - x),
                            resizeInitialSurfaceRect.height,
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
            case ResizeEdge::TopLeft:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitialSurfaceRect.width + (resizeInitialMousePos.x - x),
                            resizeInitialSurfaceRect.height + (resizeInitialMousePos.y - y),
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
            case ResizeEdge::BottomLeft:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitialSurfaceRect.width + (resizeInitialMousePos.x - x),
                            resizeInitialSurfaceRect.height + (y - resizeInitialMousePos.y),
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
            case ResizeEdge::TopRight:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitialSurfaceRect.width + (x - resizeInitialMousePos.x),
                            resizeInitialSurfaceRect.height + (resizeInitialMousePos.y - y),
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
        }
        return;
    }

    if(movingSurface != nullptr)
    {
        movingSurface->setXWithoutDecoration(movingSurfaceInitialPosX + int( x - movingSurfaceInitialCursorPosX ));
        movingSurface->setYWithoutDecoration(movingSurfaceInitialPosY + int( y - movingSurfaceInitialCursorPosY ));
        repaint();
        return;
    }

    for(list<WSurface*>::reverse_iterator surfaceIt = surfacesList.rbegin(); surfaceIt != surfacesList.rend(); ++surfaceIt)
    {

        surface =  *surfaceIt;

        if(surface->getType() == SurfaceType::Undefined) continue;

        // Mouse move event
        if(surface->containsPoint(x,y,false) && getPointerFocusSurface() == surface)
        {
            surface->sendPointerMotionEvent(surface->mapXtoLocal(x),surface->mapYtoLocal(y),milliseconds);
            break;
        }

        // Mouse leave surface
        if(!surface->containsPoint(x,y,false) && getPointerFocusSurface() == surface)
        {
            surface->sendPointerLeaveEvent();
            cursorSurface = nullptr;
            printf("Mouse left surface\n");
            continue;
        }

        // Mouse enter surface
        if(surface->containsPoint(x,y,false) && getPointerFocusSurface() != surface)
        {

            surface->sendPointerEnterEvent(surface->mapXtoLocal(x),surface->mapXtoLocal(y));
            printf("Mouse entered surface\n");
            break;
        }
    }

    repaint();
}

void MyCompositor::pointerClickEvent(int x, int y, UInt32 button, UInt32 state, UInt32 milliseconds)
{
    //printf("%i\n",button);
    (void)x;(void)y;

    if(state == LIBINPUT_BUTTON_STATE_RELEASED && button == 272)
    {
        isLeftMouseButtonPressed = false;

        if(resizingSurface != nullptr)
        {
            resizingSurface->sendConfigureEvent(
                        resizingSurface->getRectWithoutDecoration().width,
                        resizingSurface->getRectWithoutDecoration().height,
                        SurfaceState::Activated);
            resizingSurface = nullptr;
        }
        movingSurface = nullptr;
    }

    if(getPointerFocusSurface() != nullptr)
    {
        getPointerFocusSurface()->sendPointerButtonEvent(button,state,milliseconds);

        if(state == LIBINPUT_BUTTON_STATE_PRESSED && button == 272)
        {
            isLeftMouseButtonPressed = true;

            if(getKeyboardFocusSurface() != getPointerFocusSurface())
                getPointerFocusSurface()->sendKeyboardEnterEvent();

            // Raise view
            surfacesList.remove(getPointerFocusSurface());
            surfacesList.push_back(getPointerFocusSurface());

        }
    }

}

void MyCompositor::keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    if(getKeyboardFocusSurface())
        getKeyboardFocusSurface()->sendKeyModifiersEvent(depressed,latched,locked,group);
}

void MyCompositor::keyEvent(UInt32 keyCode, UInt32 keyState, UInt32 milliseconds)
{
    if(getKeyboardFocusSurface())
        getKeyboardFocusSurface()->sendKeyEvent(keyCode,keyState,milliseconds);

    printf("Key:%i\n",keyCode);

    if(keyState == LIBINPUT_KEY_STATE_RELEASED)
    {
        // Ends compositor if ESC is pressed
        if(keyCode == 1)
        {
            WWayland::terminateDisplay();
            exit(0);
        }
        if(keyCode == 2)
        {
            pid_t pid = fork();
            if (pid==0)
            {
                system("weston-terminal");
                //system("terminology");
                exit(0);
            }
        }
        if(keyCode == 3)
        {
            pid_t pid = fork();
            if (pid==0)
            {
                //system("gedit");
                system("/home/eduardo/Escritorio/build-wiggly-Desktop_Qt_5_15_2_GCC_64bit-Debug/wiggly --platform wayland");
                exit(0);
            }
        }
    }
}

void MyCompositor::drawCursor()
{
    if(cursorSurface != nullptr)
    {
        WSurface *cursor = getCursorSurface();
        glBindTexture(GL_TEXTURE_2D,cursor->texture->textureId());
        glUniform4f(rectUniform,getPointerX()-cursorXOffset,getPointerY()-cursorYOffset,cursor->getWidth(),cursor->getHeight());
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D,defaultCursorTexture->textureId());
        glUniform4f(rectUniform,getPointerX(),getPointerY(),5,5);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
}
