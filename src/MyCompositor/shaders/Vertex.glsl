attribute vec4 vertexPosition;
uniform vec2 screen;
uniform vec4 rect;
varying vec2 v_texcoord;

void main()
{
    float width = rect.z/screen.x;
    float height = rect.w/screen.y;
    float left = rect.x/screen.x;
    float top = rect.y/screen.y;
    float x = 2.0*(width*vertexPosition.x + left) -1.0;
    float y = 1.0  - 2.0*(height*vertexPosition.y + top);
    gl_Position = vec4(x,y,0.0,1.0);
    v_texcoord = vertexPosition.zw;
}






/*
precision lowp float;
precision lowp int;

attribute vec4 vertexPosition;
uniform vec2 screen;
uniform vec4 rect;
varying vec2 v_texcoord;

void main()
{
    float width = rect.z/screen.x;
    float height = rect.w/screen.y;
    float left = rect.x/screen.x;
    float top = rect.y/screen.y;
    float x = 2.0*(width*vertexPosition.x + left) -1.0;
    float y = 1.0  - 2.0*(height*vertexPosition.y + top);
    gl_Position = vec4(x,y,0.0,0.0);

    v_texcoord = vertexPosition.zw;
}
*/
