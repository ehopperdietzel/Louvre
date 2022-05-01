precision lowp float;
precision lowp int;
//precision mediump float;

// Textura de la app a dibujar
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
    float x = v_texcoord.x*(srcRect.z - srcRect.x)/texSize.x + srcRect.x/texSize.x;
    float y = (1.0 - v_texcoord.y)*(srcRect.w - srcRect.y)/texSize.y + srcRect.y/texSize.y;
    gl_FragColor = texture2D(tex,vec2(x,y));
  }
  else
  {
    gl_FragColor = colorRGBA;
  }
  
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
