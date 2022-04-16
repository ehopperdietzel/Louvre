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
#include <WPositioner.h>

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
         * Get the client's surface scale with the surface->bufferScale() method.
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
    glViewport(0, 0, W_WIDTH, W_HEIGHT);

    // Load the vertex data
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, square);

    // Enables the vertex array
    glEnableVertexAttribArray(0);

    // Get Uniform Variables
    screenSizeUniform = glGetUniformLocation(programObject, "screenSize");
    texSizeUniform = glGetUniformLocation(programObject, "texSize");
    srcRectUniform = glGetUniformLocation(programObject, "srcRect");
    dstRectUniform = glGetUniformLocation(programObject, "dstRect");
    activeTextureUniform = glGetUniformLocation(programObject, "tex");

    // Set screen size
    glUniform2f(
        screenSizeUniform,
        W_WIDTH/output->getOutputScale(),
        W_HEIGHT/output->getOutputScale());

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
     *  the next frame for a specific output.
     *************************************************/

    //printf("Outputs count:%lu\n",getOutputs().size());


    glClear(GL_COLOR_BUFFER_BIT);

    /*
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,backgroundTexture->textureId());
    glUniform1i(activeTextureUniform,0);
    glUniform4f(rectUniform,0,0,W_WIDTH/output->getOutputScale(),W_HEIGHT/output->getOutputScale());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    */

    for(list<MySurface*>::iterator s = surfacesList.begin(); s != surfacesList.end(); s++)
        drawSurfaceTree(*s);

    drawCursor();
}

void MyCompositor::drawSurfaceTree(MySurface *surface)
{

    if( surface->type() == SurfaceType::Undefined || surface->type() == SurfaceType::Cursor)
        return;

    if( surface->isDamaged() )
        surface->applyDamages();

    // If surface has no parent
    if(surface->parent() == nullptr)
    {
        drawQuad(surface->texture(),
                 WRect(0,0,surface->texture()->size().w(),surface->texture()->size().h()),
                 WRect(surface->pos,surface->size()/surface->bufferScale()));
    }
    else if(surface->type() == SurfaceType::Popup && surface->positioner() != nullptr)
    {
        WPositioner *p = surface->positioner();
        MySurface *parent = (MySurface*)surface->parent();
        WPoint parentPos = parent->getRectWithoutDecoration().topLeft();
        WPoint anchorPos;
        WPoint popupOrigin;
        WSize popupSize = p->size();
        switch(surface->positioner()->anchor())
        {
            case XDG_POSITIONER_ANCHOR_NONE:
            {
                anchorPos = p->anchorRect().bottomRight()/2;
            }break;
            case XDG_POSITIONER_ANCHOR_TOP:
            {
                anchorPos.setX(p->anchorRect().w()/2);
            }break;
            case XDG_POSITIONER_ANCHOR_BOTTOM:
            {
                anchorPos.setX(p->anchorRect().w()/2);
                anchorPos.setY(p->anchorRect().h());
            }break;
            case XDG_POSITIONER_ANCHOR_LEFT:
            {
                anchorPos.setY(p->anchorRect().h()/2);
            }break;
            case XDG_POSITIONER_ANCHOR_RIGHT:
            {
                anchorPos.setX(p->anchorRect().w());
                anchorPos.setY(p->anchorRect().h()/2);
            }break;
            case XDG_POSITIONER_ANCHOR_TOP_LEFT:
            {
                // (0,0)
            }break;
            case XDG_POSITIONER_ANCHOR_BOTTOM_LEFT:
            {
                anchorPos.setY(p->anchorRect().h());
            }break;
            case XDG_POSITIONER_ANCHOR_TOP_RIGHT:
            {
                anchorPos.setX(p->anchorRect().w());
            }break;
            case XDG_POSITIONER_ANCHOR_BOTTOM_RIGHT:
            {
                anchorPos = p->anchorRect().bottomRight();
            }break;
        }
        printf("GRAVITY %i\n",surface->positioner()->gravity());
        switch(surface->positioner()->gravity())
        {
            case XDG_POSITIONER_GRAVITY_NONE:
            {
                popupOrigin = popupSize/2;
            }break;
            case XDG_POSITIONER_GRAVITY_TOP:
            {
                popupOrigin.setX(popupSize.w()/2);
                popupOrigin.setY(popupSize.h());
            }break;
            case XDG_POSITIONER_GRAVITY_BOTTOM:
            {
                popupOrigin.setX(popupSize.w()/2);
            }break;
            case XDG_POSITIONER_GRAVITY_LEFT:
            {
                popupOrigin.setX(popupSize.w());
                popupOrigin.setY(popupSize.h()/2);
            }break;
            case XDG_POSITIONER_GRAVITY_RIGHT:
            {
                popupOrigin.setY(popupSize.h()/2);
            }break;
            case XDG_POSITIONER_GRAVITY_TOP_LEFT:
            {
                popupOrigin = popupSize;
            }break;
            case XDG_POSITIONER_GRAVITY_BOTTOM_LEFT:
            {
                popupOrigin.setX(popupSize.w());
            }break;
            case XDG_POSITIONER_GRAVITY_TOP_RIGHT:
            {
                popupOrigin.setY(popupSize.h());
            }break;
            case XDG_POSITIONER_GRAVITY_BOTTOM_RIGHT:
            {
                // (0,0)
            }break;
        }

        WPoint popupPos = parentPos + p->anchorRect().topLeft() + anchorPos - popupOrigin + p->offset();
        drawQuad(surface->texture(),
                 WRect(WPoint(),surface->texture()->size()),
                 WRect(popupPos,surface->size()/surface->bufferScale()));
    }

    // Draw it's children
    for(list<WSurface*>::iterator s = surface->_children.begin(); s != surface->_children.end(); s++)
        drawSurfaceTree((MySurface*)*s);
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

void MyCompositor::clientDisconnectRequest(WClient *client)
{
    /*******************************************************************************
     * Notify a client disconnection, it's automatically removed from the internal
     * 'compositor->clients' list.
     * All destroy events from resources related to the client (like surfaces, regions, etc)
     * are prevously notified.
     *******************************************************************************/
    (void)client;
    printf("Client disconnected.\n");
}

void MyCompositor::setCursorRequest(WSurface *_cursorSurface, Int32 hotspotX, Int32 hotspotY)
{
    cursorHotspot = WPoint(hotspotX, hotspotY);
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

    pointer += WPointF(dx,dy);

    if(pointer.x() < 0)
        pointer.setX(0);
    if(pointer.y() < 0)
        pointer.setY(0);

    float x = pointer.x();
    float y = pointer.y();

    if(resizingSurface != nullptr)
    {
        switch(resizeEdge)
        {
            case ResizeEdge::Bottom:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitSurfaceRect.w(),
                            resizeInitSurfaceRect.h() + (y - resizeInitMousePos.y()),
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
            case ResizeEdge::Right:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitSurfaceRect.w() + (x - resizeInitMousePos.x()),
                            resizeInitSurfaceRect.h(),
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
            case ResizeEdge::BottomRight:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitSurfaceRect.w() + (x - resizeInitMousePos.x()),
                            resizeInitSurfaceRect.h() + (y - resizeInitMousePos.y()),
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
            case ResizeEdge::Top:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitSurfaceRect.w(),
                            resizeInitSurfaceRect.h() + (resizeInitMousePos.y() - y),
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
            case ResizeEdge::Left:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitSurfaceRect.w() + (resizeInitMousePos.x() - x),
                            resizeInitSurfaceRect.h(),
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
            case ResizeEdge::TopLeft:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitSurfaceRect.w() + (resizeInitMousePos.x() - x),
                            resizeInitSurfaceRect.h() + (resizeInitMousePos.y() - y),
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
            case ResizeEdge::BottomLeft:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitSurfaceRect.w() + (resizeInitMousePos.x() - x),
                            resizeInitSurfaceRect.h() + (y - resizeInitMousePos.y()),
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
            case ResizeEdge::TopRight:
            {
                resizingSurface->sendConfigureEvent(
                            resizeInitSurfaceRect.w() + (x - resizeInitMousePos.x()),
                            resizeInitSurfaceRect.h() + (resizeInitMousePos.y() - y),
                            SurfaceState::Activated | SurfaceState::Resizing);
            }break;
        }

        return;
    }

    if(movingSurface != nullptr)
    {
        movingSurface->setXWithoutDecoration(movingSurfaceInitPos.x() + int( x - movingSurfaceInitCursorPos.x() ));
        movingSurface->setYWithoutDecoration(movingSurfaceInitPos.y() + int( y - movingSurfaceInitCursorPos.y() ));
        repaintAllOutputs();
        return;
    }

    for(list<MySurface*>::reverse_iterator s = surfacesList.rbegin(); s != surfacesList.rend(); s++)
    {

        surface =*s;

        if(surface->type() == SurfaceType::Undefined) continue;

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
                        resizingSurface->getRectWithoutDecoration().w(),
                        resizingSurface->getRectWithoutDecoration().h(),
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
            if(_pointerFocusSurface->parent() == nullptr)
            {
                surfacesList.remove((MySurface*)_pointerFocusSurface);
                surfacesList.push_back((MySurface*)_pointerFocusSurface);
            }

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
                exit(0);
            }
        }
        if(keyCode == 3)
        {
            pid_t pid = fork();
            if (pid==0)
            {
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
        if(cursorSurface->isDamaged())
            cursorSurface->applyDamages();

        drawQuad(cursorSurface->texture(),
                 WRect(0,0,cursorSurface->texture()->size().w(),cursorSurface->texture()->size().h()),
                 WRect(pointer-cursorHotspot,cursorSurface->size()/cursorSurface->bufferScale()));
    }
    else
    {
        drawQuad(defaultCursorTexture,
                 WRect(0,0,defaultCursorTexture->size().w(),defaultCursorTexture->size().h()),
                 WRect(pointer,WSize(24,24)));
    }
}

void MyCompositor::drawQuad(WTexture *tex, WRect src, WRect dst)
{
    glBindTexture(GL_TEXTURE_2D,tex->textureId());
    glUniform1i(activeTextureUniform,tex->textureUnit());
    glUniform2f(texSizeUniform,tex->size().w(), tex->size().h());
    glUniform4f(srcRectUniform,src.x(), src.y(), src.w(), src.h());
    glUniform4f(dstRectUniform,dst.x(), dst.y(), dst.w(), dst.h());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void MyCompositor::setPointerPos(double x, double y)
{
    float w = W_WIDTH/pointerOutput->getOutputScale();
    float h = W_HEIGHT/pointerOutput->getOutputScale();

    if(x < 0.0)
        x = 0.0;
    if(y < 0.0)
        y = 0.0;
    if(x > w)
        x = w;
    if(y > h)
        y = h;

    pointer = WPointF(x, y);

}



