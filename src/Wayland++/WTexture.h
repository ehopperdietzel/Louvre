#ifndef WTEXTURE_H
#define WTEXTURE_H

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>

static PFNEGLBINDWAYLANDDISPLAYWL eglBindWaylandDisplayWL = NULL;
static PFNEGLQUERYWAYLANDBUFFERWL eglQueryWaylandBufferWL = NULL;
static PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = NULL;

struct texture {
    GLuint identifier;
    int width, height;
};

static void texture_create (struct texture *texture, int width, int height, void *data) {
    texture->width = width;
    texture->height = height;
    glGenTextures (1, &texture->identifier);
    glBindTexture (GL_TEXTURE_2D, texture->identifier);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
    glBindTexture (GL_TEXTURE_2D, 0);
}

static void texture_create_from_egl_image (struct texture *texture, int width, int height, EGLImage image) {
    texture->width = width;
    texture->height = height;
    glGenTextures (1, &texture->identifier);
    glBindTexture (GL_TEXTURE_2D, texture->identifier);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glEGLImageTargetTexture2DOES (GL_TEXTURE_2D, image);
    glBindTexture (GL_TEXTURE_2D, 0);
}

static void texture_draw (struct texture *texture, int x, int y) {
    if (!texture->identifier) return;

    GLint vertices[] = {
        x, y,
        x+texture->width, y,
        x+texture->width, y+texture->height,
        x, y+texture->height
    };
    GLint tex_coords[] = {
        0, 0,
        1, 0,
        1, 1,
        0, 1
    };

    glEnable (GL_TEXTURE_2D);
    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    glBindTexture (GL_TEXTURE_2D, texture->identifier);
    glVertexPointer (2, GL_INT, 0, vertices);
    glTexCoordPointer (2, GL_INT, 0, tex_coords);

    glDrawArrays (GL_QUADS, 0, 4);

    glBindTexture (GL_TEXTURE_2D, 0);
    glDisable (GL_TEXTURE_RECTANGLE);
    glDisableClientState (GL_VERTEX_ARRAY);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
}

static void texture_delete (struct texture* texture) {
    if (texture->identifier)
        glDeleteTextures (1, &texture->identifier);
}

#endif
