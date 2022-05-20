precision lowp float;
precision lowp int;

uniform sampler2D tex;
uniform vec2 texSize;
uniform vec4 srcRect;
uniform int mode;
uniform vec4 colorRGBA;

varying vec2 v_texcoord;

void main()
{
  if(mode == 0)
  {
    float x = (srcRect.x)/texSize.x + v_texcoord.x*srcRect.z/texSize.x;
    float y = (srcRect.y)/texSize.y + v_texcoord.y*srcRect.w/texSize.y;
    gl_FragColor = texture2D(tex,vec2(x,-y));
  }
  else
    gl_FragColor = colorRGBA;
}


/*
// ORIGEN BL
precision lowp float;
precision lowp int;

// Textura de la app a dibujar
uniform sampler2D application;

// Tamaño de la pantalla en pixeles
uniform vec2 screen;

// Posición y tamaño de app en pixeles
uniform vec4 rect;

// Coordenadas de la textura calculada en el vshader
varying vec2 v_texcoord;


void main()
{
    gl_FragColor = texture2D(application,vec2(v_texcoord.x,-v_texcoord.y));
}
*/
