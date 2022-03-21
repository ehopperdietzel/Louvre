#include <MyCompositor.h>
#include <math.h>
#include <WOpenGL.h>
#include <unistd.h>
#include <WWayland.h>
#include <linux/input-event-codes.h>
#include <stdio.h>
#include <MyClient.h>
#include <WBackend.h>
#include <WOutputManager.h>
#include <SOIL/SOIL.h>

MyCompositor::MyCompositor()
{
    /* *******************************************************************
     * Suggests clients to scale their buffers.
     * 1 is the default ( 1 virtual pixel = 1 screen pixel )
     * 2 is for HDPi screens ( 1 virtual pixel = 2 screen pixels )
     * Clients can ignore this suggestion and use a different scale.
     * Get the client's surface scale with the surface->getScale() method.
     * You can invoke this method at any time but only new clients will
     * be notified.
     *********************************************************************/
    //WOutputManager *outputManager = new WOutputManager(this);

    setOutputScale(2);
}


void MyCompositor::initializeGL()
{
    /*************************************************
     * Here you initialize your OpenGL ES 2 context
     *************************************************/

    GLuint vertexShader,fragmentShader,programObject;

    // Open the vertex/fragment shaders
    GLchar *vShaderStr = WOpenGL::openShader("../MyCompositor/shaders/Vertex.glsl");
    GLchar *fShaderStr = WOpenGL::openShader("../MyCompositor/shaders/Fragment.glsl");

    // Load the vertex/fragment shaders
    vertexShader = WOpenGL::LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = WOpenGL::LoadShader(GL_FRAGMENT_SHADER, fShaderStr);

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
    activeTextureUniform = glGetUniformLocation(programObject, "application");

    // Set screen size
    glUniform2f(screenUniform,screenWidth()/getOutputScale(),screenHeight()/getOutputScale());

    // Reserve unit 0 for cursor
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(activeTextureUniform,0);

    // Create default cursor texture (64x64)
    int w,h;
    unsigned char * img = SOIL_load_image("../MyCompositor/Images/Cursor.png", &w, &h, 0, SOIL_LOAD_RGBA);
    defaultCursorTexture->setData(w,h,img);
    SOIL_free_image_data(img);

    img = SOIL_load_image("../MyCompositor/Images/Wallpaper.png", &w, &h, 0, SOIL_LOAD_RGBA);
    backgroundTexture->setData(w,h,img);
    SOIL_free_image_data(img);

}

void MyCompositor::paintGL()
{
    /*************************************************
     *  Here you do your OpenGL drawing.
     *  Never invoke this method directly,
     *  use WCompositor::repaint() instead
     *  otherwise you may break the internal page
     *  flipping mechanism.
     *************************************************/

    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,backgroundTexture->textureId());
    glUniform1i(activeTextureUniform,0);
    glUniform4f(rectUniform,0,0,screenWidth()/getOutputScale(),screenHeight()/getOutputScale());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    for(list<MySurface*>::iterator surface = surfacesList.begin(); surface != surfacesList.end(); ++surface)
    {
        if((*surface)->getType() == SurfaceType::Undefined) continue;

        if((*surface)->isDamaged())
            (*surface)->applyDamages();
        else
        {
            glActiveTexture(GL_TEXTURE0 + (*surface)->getTexture()->textureUnit());
            glBindTexture(GL_TEXTURE_2D,(*surface)->getTexture()->textureId());
        }



        if(resizingSurface == (*surface) && isLeftMouseButtonPressed)
        {
            Rect ir = resizeInitialSurfaceDecoration;

            if(resizeEdge == ResizeEdge::Top || resizeEdge == ResizeEdge::TopLeft || resizeEdge == ResizeEdge::TopRight)
                resizingSurface->setY(ir.y + ir.height - resizingSurface->getHeight());

            if(resizeEdge == ResizeEdge::Left || resizeEdge == ResizeEdge::TopLeft || resizeEdge == ResizeEdge::BottomLeft)
                resizingSurface->setX(ir.x + ir.width - resizingSurface->getWidth());
        }


        glUniform1i(activeTextureUniform,(*surface)->getTexture()->textureUnit());

        glUniform4f(rectUniform,(*surface)->getX(),(*surface)->getY(),(*surface)->getWidth(),(*surface)->getHeight());

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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
    cursorSurface = (MySurface*)_cursorSurface;
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
    MySurface *surface;

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

    for(list<MySurface*>::reverse_iterator surfaceIt = surfacesList.rbegin(); surfaceIt != surfacesList.rend(); ++surfaceIt)
    {

        surface =*surfaceIt;

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
            surfacesList.remove((MySurface*)getPointerFocusSurface());
            surfacesList.push_back((MySurface*)getPointerFocusSurface());

        }
    }

    repaint();
}

void MyCompositor::keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    if(getKeyboardFocusSurface())
        getKeyboardFocusSurface()->sendKeyModifiersEvent(depressed,latched,locked,group);

    repaint();
}

void MyCompositor::keyEvent(UInt32 keyCode, UInt32 keyState)
{
    if(getKeyboardFocusSurface())
    {
        getKeyboardFocusSurface()->sendKeyEvent(keyCode,keyState);
        repaint();
    }

    printf("Key:%i State:%i\n",keyCode,keyState);

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
                system("falkon --platform wayland");
                exit(0);
            }
        }
    }

}

void MyCompositor::drawCursor()
{

    if(cursorSurface != nullptr)
    {
        glActiveTexture(GL_TEXTURE0+cursorSurface->getTexture()->textureUnit());
        glUniform1i(activeTextureUniform,cursorSurface->getTexture()->textureUnit());

        if(cursorSurface->isDamaged())
            cursorSurface->applyDamages();

        glBindTexture(GL_TEXTURE_2D,cursorSurface->getTexture()->textureId());
        glUniform4f(rectUniform,getPointerX()-cursorXOffset,getPointerY()-cursorYOffset,cursorSurface->getWidth(),cursorSurface->getHeight());

    }
    else
    {
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(activeTextureUniform,0);
        glBindTexture(GL_TEXTURE_2D,defaultCursorTexture->textureId());
        glUniform4f(rectUniform,getPointerX(),getPointerY(),24,24);
    }
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
