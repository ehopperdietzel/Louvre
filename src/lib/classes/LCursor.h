#ifndef LCURSOR_H
#define LCURSOR_H

#include <LNamespaces.h>
#include <LPointF.h>
#include <LPoint.h>
#include <LTexture.h>
#include <LRect.h>

class Louvre::LCursor
{
public:
    LCursor(LOutput *output);
    ~LCursor();

    enum CursorNames : UChar8
    {
        Arrow = 0
    };

    //void setCursorTheme(const char *themeName);
    //void setCursor(const char *cursorName);
    void setCursor(CursorNames cursor);
    void setTexture(LTexture *texture, const LPointF &hotspot);
    void setOutput(LOutput *output);
    void move(float x, float y);
    void setPosition(const LPointF &position);
    void setHotspot(const LPointF &hotspot);
    void paint();
    void setSize(const LSizeF &size);
    void setVisible(bool state);

    bool visible() const;
    bool hasHardwareSupport() const;
    const LPointF &position() const;
    const LPointF &hotspot()  const;
    LTexture *texture() const;
    LCompositor *compositor() const;
    LOutput *output() const;

    const LRect &rect() const;

    class LCursorPrivate;
    LCursorPrivate *imp() const;
private:
    LCursorPrivate *m_imp = nullptr;


};

#endif // LCURSOR_H
