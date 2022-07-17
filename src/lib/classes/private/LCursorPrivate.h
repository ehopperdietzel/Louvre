#ifndef LCURSORPRIVATE_H
#define LCURSORPRIVATE_H

#include <LCursor.h>

class Louvre::LCursor::LCursorPrivate
{
public:
    LCursorPrivate() = default;
    ~LCursorPrivate() = default;

    struct LCursorData
    {
        LTexture *texture = nullptr;
        LPointF hotspot = LPointF();
    };

    void loadDefaultCursors();

    LCursor *cursor;
    LCursorData cursors[1];
    LRect rect;

    enum LLastCursorType
    {
        Texture,
        String,
        Default
    };

    struct LLastCursorData
    {
        LSizeF size;
        LPointF offset;
        char *name = nullptr;
        UInt32 textureId;
        LLastCursorType type = LLastCursorType::String;
        int defaultName = -1;
    }lastCursor;

    void update();
    char *cursorTheme = NULL;
    LTexture *x11Texture;
    LTexture *texture = nullptr;
    LOutput *output = nullptr;
    LPointF pos;
    LPointF hotspot;
    LSizeF size = LSizeF(64.f,64.f);

    bool isVisible = true;


};

#endif // LCURSORPRIVATE_H
