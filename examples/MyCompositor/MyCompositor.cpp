#include <MyCompositor.h>
#include <math.h>
#include <WOpenGL.h>
#include <unistd.h>
#include <WWayland.h>
#include <linux/input-event-codes.h>
#include <stdio.h>
#include <MyClient.h>
#include <WBackend.h>
#include <MyOutputManager.h>
#include <WOutput.h>
#include <SOIL/SOIL.h>

MyCompositor::MyCompositor():WCompositor()
{

    // Use the output manager to get connected displays
    MyOutputManager *outputManager = new MyOutputManager(this);

    if(outputManager->getOutputsList()->size() == 0)
    {
        // If there aren't currently any avaliable display, you should wait for the WOutputManager::outputPluggedEvent
    }
    else
    {
        // Use the first avaliable display
        WOutput *output = outputManager->getOutputsList()->front();

        /* *******************************************************************
         * Suggests clients to scale their buffers.
         * 1 is the default ( 1 virtual pixel = 1 screen pixel )
         * 2 is for HDPi screens ( 1 virtual pixel = 2 screen pixels )
         * Clients can ignore this suggestion and use a different scale.
         * Get the client's surface scale with the surface->getScale() method.
         * You can invoke this method at any time but only new clients will
         * be notified.
         *********************************************************************/

        output->setOutputScale(2);
        pointerOutput = output;
        addOutput(output);
    }

}


void MyCompositor::initializeGL(WOutput *output)
{
    /*************************************************
     * Here you initialize your OpenGL ES 2 context
     *************************************************/

    GLuint vertexShader,fragmentShader,programObject;

    // Open the vertex/fragment shaders
    GLchar *vShaderStr = WOpenGL::openShader("res/shaders/Vertex.glsl");
    GLchar *fShaderStr = WOpenGL::openShader("res/shaders/Fragment.glsl");

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
    glViewport(0, 0, output->getCurrentMode().hdisplay, output->getCurrentMode().vdisplay);

    // Load the vertex data
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, square);

    // Enables the vertex array
    glEnableVertexAttribArray(0);

    // Get Uniform Variables
    screenUniform = glGetUniformLocation(programObject, "screen"); // (width,height) Screen dimensions
    rectUniform = glGetUniformLocation(programObject, "rect");     // (left,top,with,height) App window pos and size
    activeTextureUniform = glGetUniformLocation(programObject, "application");

    // Set screen size
    glUniform2f(
        screenUniform,
        output->getCurrentMode().hdisplay/output->getOutputScale(),
        output->getCurrentMode().vdisplay/output->getOutputScale());

    // Reserve unit 0 for cursor
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(activeTextureUniform,0);

    // Create default cursor texture (64x64)
    int w,h;
    unsigned char * img = SOIL_load_image("res/images/Cursor.png", &w, &h, 0, SOIL_LOAD_RGBA);
    defaultCursorTexture->setData(w,h,img);
    SOIL_free_image_data(img);

    img = SOIL_load_image("res/images/Wallpaper.png", &w, &h, 0, SOIL_LOAD_RGBA);
    backgroundTexture->setData(w,h,img);
    SOIL_free_image_data(img);

    printf("Initialize GL\n");
}

void MyCompositor::paintGL(WOutput *output)
{
    /*************************************************
     *  Here you do your OpenGL drawing.
     *  Each output has its own OpenGL context.
     *  Never invoke this method directly,
     *  use WOutput::repaint() instead to schedule
     *  the next frame.
     *************************************************/

    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,backgroundTexture->textureId());
    glUniform1i(activeTextureUniform,0);
    glUniform4f(rectUniform,0,0,output->getCurrentMode().hdisplay/output->getOutputScale(),output->getCurrentMode().vdisplay/output->getOutputScale());
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
    cursorHotspot = {hotspotX,hotspotY};
    cursorSurface = (MySurface*)_cursorSurface;
}

void MyCompositor::libinputEvent(libinput_event *)
{

    /*************************************************
     * If you wish to process libinput events manually
     *************************************************/

    // libinput_event_type eventType = libinput_event_get_type(event);
}

void MyCompositor::pointerMoveEvent(float dx, float dy)
{
    MySurface *surface;

    pointer.x+=dx;
    pointer.y+=dy;

    float x = pointer.x;
    float y = pointer.y;

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
        repaintAllOutputs();
        return;
    }

    for(list<MySurface*>::reverse_iterator surfaceIt = surfacesList.rbegin(); surfaceIt != surfacesList.rend(); ++surfaceIt)
    {

        surface =*surfaceIt;

        if(surface->getType() == SurfaceType::Undefined) continue;

        // Mouse move event
        if(surface->containsPoint(x,y,false) && _pointerFocusSurface == surface)
        {
            surface->sendPointerMotionEvent(surface->mapXtoLocal(x),surface->mapYtoLocal(y),getMilliseconds());
            break;
        }

        // Mouse leave surface
        if(!surface->containsPoint(x,y,false) && _pointerFocusSurface == surface)
        {
            _pointerFocusSurface->sendPointerLeaveEvent();
            _pointerFocusSurface = nullptr;
            cursorSurface = nullptr;
            printf("Mouse left surface\n");
            continue;
        }

        // Mouse enter surface
        if(surface->containsPoint(x,y,false) && _pointerFocusSurface != surface)
        {

            if(_pointerFocusSurface != nullptr)
                _pointerFocusSurface->sendPointerLeaveEvent();

            surface->sendPointerEnterEvent(surface->mapXtoLocal(x),surface->mapXtoLocal(y));

            _pointerFocusSurface = surface;

            printf("Mouse entered surface\n");

            break;
        }
    }

    repaintAllOutputs();
}


void MyCompositor::pointerClickEvent(UInt32 button, UInt32 state)
{

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

    if(_pointerFocusSurface != nullptr)
    {
        _pointerFocusSurface->sendPointerButtonEvent(button,state,getMilliseconds());

        if(state == LIBINPUT_BUTTON_STATE_PRESSED && button == 272)
        {
            isLeftMouseButtonPressed = true;

            if(_keyboardFocusSurface != _pointerFocusSurface)
            {
                if(_keyboardFocusSurface != nullptr)
                    _keyboardFocusSurface->sendKeyboardLeaveEvent();

                _pointerFocusSurface->sendKeyboardEnterEvent();
                _keyboardFocusSurface = _pointerFocusSurface;
            }

            // Raise view
            surfacesList.remove((MySurface*)_pointerFocusSurface);
            surfacesList.push_back((MySurface*)_pointerFocusSurface);

        }
    }

    repaintAllOutputs();
}

void MyCompositor::keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    if(_keyboardFocusSurface)
        _keyboardFocusSurface->sendKeyModifiersEvent(depressed,latched,locked,group);

    repaintAllOutputs();
}

void MyCompositor::keyEvent(UInt32 keyCode, UInt32 keyState)
{
    if(_keyboardFocusSurface)
    {
        _keyboardFocusSurface->sendKeyEvent(keyCode,keyState);
        repaintAllOutputs();
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
        glUniform4f(rectUniform,pointer.x - cursorHotspot.x, pointer.y - cursorHotspot.y,cursorSurface->getWidth(),cursorSurface->getHeight());

    }
    else
    {
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(activeTextureUniform,0);
        glBindTexture(GL_TEXTURE_2D,defaultCursorTexture->textureId());
        glUniform4f(rectUniform,pointer.x,pointer.y,24,24);
    }
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void MyCompositor::setPointerPos(double x, double y)
{
    float w = pointerOutput->getCurrentMode().hdisplay/pointerOutput->getOutputScale();
    float h = pointerOutput->getCurrentMode().vdisplay/pointerOutput->getOutputScale();

    if(x < 0.0)
        x = 0.0;
    if(y < 0.0)
        y = 0.0;
    if(x > w)
        x = w;
    if(y > h)
        y = h;

    pointer = { x, y };

}

