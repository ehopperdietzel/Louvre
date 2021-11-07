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
