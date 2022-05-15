#ifndef WVIEW_H
#define WVIEW_H


class WView
{
public:
    WView();
};

#endif // WVIEW_H


/*
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,backgroundTexture->textureId());
    glUniform1i(activeTextureUniform,0);
    glUniform4f(rectUniform,0,0,W_WIDTH/output->getOutputScale(),W_HEIGHT/output->getOutputScale());
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glClear(GL_COLOR_BUFFER_BIT);


















    */
