#include "LRegion.h"
#include <stdio.h>
using namespace Louvre;

LRegion::LRegion()
{

}

void LRegion::copy(const LRegion &region)
{
    clear();
    std::copy(region.m_rects.begin(),region.m_rects.end(),std::back_inserter(m_rects));
}

void LRegion::clear()
{
    m_rects.clear();
}

void LRegion::addRect(const LRect &rect)
{
    if(rect.w() == 0 || rect.h() == 0)
        return;
    Int32 x0;
    Int32 y0;
    Int32 x1;
    Int32 y1;

    Int32 rx0;
    Int32 ry0;
    Int32 rx1;
    Int32 ry1;

    std::list<LRect>adds;
    adds.push_back(rect);

    for(std::list<LRect>::iterator i = m_rects.begin(); i != m_rects.end(); i++)
    {
        loopA:

        if(i == m_rects.end())
            break;

        LRect &r = *i;

        rx0 = r.x();
        ry0 = r.y();
        rx1 = rx0 + r.w();
        ry1 = ry0 + r.h();

        for(std::list<LRect>::iterator j = adds.begin(); j != adds.end(); j++)
        {
            loopB:

            if(j == adds.end())
                break;

            LRect &add = *j;

            x0 = add.x();
            y0 = add.y();
            x1 = x0 + add.w();
            y1 = y0 + add.h();

            // Case 1: If the new rect is contained in another
            if(x0 >= rx0 && y0 >= ry0 && x1 <= rx1 && y1 <= ry1)
            {
                j = adds.erase(j);
                goto loopB;
            }

            // Case 2: If the new rect doesnt intersect the current one
            if(x0 >= rx1 || x1 <= rx0 || y0 >= ry1 || y1 <= ry0)
               continue;

            // Case 3: If the new rect contains the current one
            if(x0 <= rx0 && x1 >= rx1 && y0 <= ry0 && y1 >= ry1)
            {
                i = m_rects.erase(i);
                goto loopA;
            }

            // Case 4 LR:
            if(y0 == ry0 && y1 == ry1)
            {
                // R extends right side
                if(x0 < rx1 && x1 > rx1)
                {
                    add.setX(rx0);
                    add.setW(x1 - rx0);
                    i = m_rects.erase(i);
                    goto loopA;
                }

                // L extendes left side
                if(x1 > rx0 && x0 < rx0)
                {
                    add.setW(rx1 - x0);
                    i = m_rects.erase(i);
                    goto loopA;
                }
            }

            // Case 4 TB:
            if(x0 == rx0 && x1 == rx1)
            {
                // B extends bottom side
                if(y0 < ry1 && y1 > ry1)
                {
                    add.setY(ry0);
                    add.setH(y1 - ry0);
                    i = m_rects.erase(i);
                    goto loopA;
                }

                // T extendes top side
                if(y1 > ry0 && y0 < ry0)
                {
                    add.setH(ry1 - y0);
                    i = m_rects.erase(i);
                    goto loopA;
                }
            }

            // Case 5 LR:
            if(y0 >= ry0 && y0 < ry1 && y1 <= ry1 && y1 > ry0)
            {
                // R extends right side
                if(x0 < rx1 && x0 >= rx0 && x1 > rx1)
                {
                    add.setX(rx1);
                    add.setW(x1 - rx1);
                    continue;
                }

                // L extendes left side
                if(x1 > rx0 && x0 < rx0 && x1 <= rx1)
                {
                    add.setW(rx0 - x0);
                    continue;
                }
            }

            // Case 5 TB:
            if(x0 >= rx0 && x0 < rx1 && x1 <= rx1 && x1 > rx0)
            {
                // B extends bottom side
                if(y0 < ry1 && y0 > ry0 && y1 >= ry1)
                {
                    add.setY(ry1);
                    add.setH(y1 - ry1);
                    continue;
                }

                // T extendes top side
                if(y1 > ry0 && y0 <= ry0 && y1 < ry1)
                {
                    add.setH(ry0 - y0);
                    continue;
                }
            }

            // Case 6 LR:
            if(y0 >= ry0 && y1 <= ry1)
            {
                if(x0 <= rx0 && x1 >= rx1)
                {
                    // R
                    adds.push_front(LRect(rx1,add.y(),x1-rx1,add.h()));

                    // L
                    add.setW(rx0-x0);

                    continue;
                }
            }

            // Case 6 TB:
            if(x0 >= rx0 && x1 <= rx1)
            {
                if(y0 <= ry0 && y1 >= ry1)
                {
                    // B
                    adds.push_front(LRect(add.x(),ry1,add.w(),y1 - ry1));

                    // T
                    add.setH(ry0-y0);

                    continue;
                }
            }

            // Case 7 LR:
            if(y0 <= ry0 && y1 >= ry1)
            {
                // R
                if(x0 > rx0 && x0 < rx1 && x1 >= rx1)
                {
                    // L
                    adds.push_front(LRect(rx0,ry0,x0-rx0,r.h()));
                    i = m_rects.erase(i);
                    goto loopA;
                }

                // L
                if(x0 <= rx0 && x1 > rx0 && x1 < rx1)
                {
                    // R
                    adds.push_front(LRect(x1,ry0,rx1-x1,r.h()));
                    i = m_rects.erase(i);
                    goto loopA;
                }
            }

            // Case 7 TB:
            if(x0 <= rx0 && x1 >= rx1)
            {
                // B
                if(y0 > ry0 && y0 < ry1 && y1 >= ry1)
                {
                    // T
                    adds.push_front(LRect(rx0,ry0,r.w(),y0-ry0));
                    i = m_rects.erase(i);
                    goto loopA;
                }

                // T
                if(y0 <= ry0 && y1 > ry0 && y1 < ry1)
                {
                    // B
                    adds.push_front(LRect(rx0,y1,r.w(),ry1-y1));
                    i = m_rects.erase(i);
                    goto loopA;
                }
            }

            // Case 8 TR BR
            if(x0 > rx0 && x0 < rx1 && x1 >= rx1)
            {
                // B
                if(y0 < ry1 && y0 > ry0 && y1 >= ry1)
                {
                    // R
                    adds.push_front(LRect(rx1,y0,x1-rx1,ry1-y0));

                    // B
                    add.setY(ry1);
                    add.setH(y1-ry1);
                    continue;
                }

                // T
                if(y0 <= ry0 && y1 > ry0 && y1 < ry1)
                {
                    // R
                    adds.push_front(LRect(rx1,ry0,x1-rx1,y1-ry0));

                    // T
                    add.setH(ry0-y0);
                    continue;
                }
            }

            // Case 8 TL BL
            if(x0 <= rx0 && x1 > rx0 && x1 < rx1)
            {
                // B
                if(y0 < ry1 && y0 > ry0 && y1 >= ry1)
                {
                    // L
                    adds.push_front(LRect(x0,y0,rx0-x0,ry1-y0));

                    // B
                    add.setY(ry1);
                    add.setH(y1-ry1);
                    continue;
                }

                // T
                if(y0 <= ry0 && y1 > ry0 && y1 < ry1)
                {
                    // L
                    adds.push_front(LRect(x0,ry0,rx0-x0,y1-ry0));

                    // T
                    add.setH(ry0-y0);
                    continue;
                }
            }
        }
    }

    for(LRect &a : adds)
        m_rects.push_back(a);
}

void LRegion::subtractRect(const LRect &rect)
{
    Int32 x0 = rect.x();
    Int32 y0 = rect.y();
    Int32 x1 = x0 + rect.w();
    Int32 y1 = y0 + rect.h();

    Int32 rx0;
    Int32 ry0;
    Int32 rx1;
    Int32 ry1;

    for(std::list<LRect>::iterator i = m_rects.begin(); i != m_rects.end(); i++)
    {
        loopA:

        if(i == m_rects.end())
            return;

        LRect &r = *i;
        rx0 = r.x();
        ry0 = r.y();
        rx1 = rx0 + r.w();
        ry1 = ry0 + r.h();

        // Case 1: No intersection
        if(x0 >= rx1 || x1 <= rx0 || y0 >= ry1 || y1 <= ry0)
           continue;

        // Case 2: If the new rect contains the current one
        if(x0 <= rx0 && x1 >= rx1 && y0 <= ry0 && y1 >= ry1)
        {
            i = m_rects.erase(i);
            goto loopA;
        }

        // Case 3 LR:
        if(y0 <= ry0 && y1 >= ry1)
        {
            // R
            if(x0 >= rx0 && x0 < rx1 && x1 > rx1)
            {
                r.setW(x0-rx0);
                continue;
            }

            // L
            if(x0 <= rx0 && x1 > rx0 && x1 < rx1)
            {
                r.setX(x1);
                r.setW(rx1 - x1);
                continue;
            }
        }

        // Case 3 TB:
        if(x0 <= rx0 && x1 >= rx1)
        {
            // B
            if(y0 > ry0 && y0 < ry1 && y1 >= ry1)
            {
                r.setH(y0-ry0);
                continue;
            }

            // T
            if(y0 <= ry0 && y1 > ry0 && y1 < ry1)
            {
                r.setY(y1);
                r.setH(ry1 - y1);
                continue;
            }
        }

        // Case 4 LR:
        if(y0 > ry0 && y1 < ry1)
        {
            // R
            if(x0 > rx0 && x0 < rx1 && x1 >= rx1)
            {
                // T
                m_rects.push_front(LRect(rx0,ry0,r.w(),y0-ry0));

                // B
                m_rects.push_front(LRect(rx0,y1,r.w(),ry1 - y1));

                // C
                r.setY(y0);
                r.setW(x0-rx0);
                r.setH(rect.h());

                continue;
            }

            // L
            if(x0 <= rx0 && x1 > rx0 && x1 < rx1)
            {
                // T
                m_rects.push_front(LRect(rx0,ry0,r.w(),y0-ry0));

                // B
                m_rects.push_front(LRect(rx0,y1,r.w(),ry1 - y1));

                // C
                r.setY(y0);
                r.setX(x1);
                r.setW(rx1 - x1);
                r.setH(rect.h());

                continue;
            }
        }

        // Case 4 TB:
        if(x0 > rx0 && x1 < rx1)
        {
            // B
            if(y0 > ry0 && y0 < ry1 && y1 >= ry1)
            {
                // L
                m_rects.push_front(LRect(rx0,ry0,x0-rx0,r.h()));

                // R
                m_rects.push_front(LRect(x1,ry0,rx1-x1,r.h()));

                // C
                r.setX(x0);
                r.setW(rect.w());
                r.setH(y0-ry0);

                continue;
            }

            // T
            if(y0 <= ry0 && y1 > ry0 && y1 < ry1)
            {
                // L
                m_rects.push_front(LRect(rx0,ry0,x0-rx0,r.h()));

                // R
                m_rects.push_front(LRect(x1,ry0,rx1-x1,r.h()));

                // C
                r.setY(y1);
                r.setX(x0);
                r.setW(rect.w());
                r.setH(ry1-y1);

                continue;
            }
        }

        // Case 5
        if(x0 > rx0 && x0 < rx1 && x1 > rx0 && x1 < rx1 && y0 > ry0 && y0 < ry1 && y1 > ry0 && y1 < ry1 )
        {
            // L
            m_rects.push_front(LRect(rx0,ry0,x0-rx0,r.h()));

            // R
            m_rects.push_front(LRect(x1,ry0,rx1-x1,r.h()));

            // T
            m_rects.push_front(LRect(x0,ry0,rect.w(),y0-ry0));

            // B
            r.setX(x0);
            r.setW(rect.w());
            r.setY(y1);
            r.setH(ry1 - y1);

            return;
        }

        // Case 6 BR TR
        if(x0 > rx0 && x0 <rx1 && x1 >= rx1)
        {
            // BR
            if(y0 > ry0 && y0 < ry1 && y1 >= ry1)
            {
                // T
                m_rects.push_front(LRect(rx0,ry0,r.w(),y0-ry0));

                // B
                r.setY(y0);
                r.setW(x0-rx0);
                r.setH(ry1-y0);
                continue;
            }

            // TR
            if(y0 <= ry0 && y1 > ry0 && y1 < ry1)
            {
                // T
                m_rects.push_front(LRect(rx0,ry0,x0-rx0,y1-ry0));

                // B
                r.setY(y1);
                r.setH(ry1-y1);
                continue;
            }
        }

        // Case 6 BL TL
        if(x0 <= rx0 && x1 > rx0 && x1 < rx1)
        {
            // BL
            if(y0 > ry0 && y0 < ry1 && y1 >= ry1)
            {
                // T
                m_rects.push_front(LRect(rx0,ry0,r.w(),y0-ry0));

                // B
                r.setX(x1);
                r.setY(y0);
                r.setW(rx1-x1);
                r.setH(ry1-y0);
                continue;
            }

            // TR
            if(y0 <= ry0 && y1 > ry0 && y1 < ry1)
            {
                // T
                m_rects.push_front(LRect(x1,ry0,rx1-x1,y1-ry0));

                // B
                r.setY(y1);
                r.setH(ry1-y1);
                continue;
            }
        }

        // Case 7 Vertical
        if(x0 > rx0 && x0 < rx1 && x1 > rx0 && x1 < rx1 && y0 <= ry0 && y1 >= ry1)
        {
            // R
            m_rects.push_front(LRect(x1,ry0,rx1-x1,r.h()));

            // L
            r.setW(x0-rx0);
        }

        // Case 7 Horizontal
        if(y0 > ry0 && y0 < ry1 && y1 > ry0 && y1 < ry1 && x0 <= rx0 && x1 >= rx1)
        {
            // B
            m_rects.push_front(LRect(rx0,y1,r.w(),ry1-y1));

            // T
            r.setH(y0-ry0);
        }
    }

}

void LRegion::multiply(double factor)
{
    for(LRect &rect : m_rects)
        rect *= factor;
}

bool LRegion::containsPoint(const LPoint &point)
{
    for(LRect &rect : m_rects)
    {
        if(rect.containsPoint(point))
            return true;
    }
    return false;
}

void LRegion::simplify()
{
    Int32 x0;
    Int32 y0;
    Int32 x1;
    Int32 y1;

    Int32 rx0;
    Int32 ry0;
    Int32 rx1;
    Int32 ry1;

    for(std::list<LRect>::iterator i = m_rects.begin(); i != m_rects.end(); i++)
    {
        loopA:

        if(i == m_rects.end())
            break;

        LRect &r = *i;

        rx0 = r.x();
        ry0 = r.y();
        rx1 = rx0 + r.w();
        ry1 = ry0 + r.h();

        for(std::list<LRect>::iterator j = m_rects.begin(); j != m_rects.end(); j++)
        {

            if(*i == *j)
                continue;

            LRect &add = *j;

            x0 = add.x();
            y0 = add.y();
            x1 = x0 + add.w();
            y1 = y0 + add.h();

            if(y0 == ry0 && y1 == ry1)
            {
                // R
                if(x1 == rx0)
                {
                    r.setX(x0);
                    r.setW(rx1-x0);
                    m_rects.erase(j);
                    goto loopA;
                }

                // L
                if(x0 == rx1)
                {
                    r.setW(x1 - rx0);
                    m_rects.erase(j);
                    goto loopA;
                }
            }

            if(x0 == rx0 && x1 == rx1)
            {
                // T
                if(y1 == ry0)
                {
                    r.setY(y0);
                    r.setH(ry1-y0);
                    m_rects.erase(j);
                    goto loopA;
                }

                // B
                if(y0 == ry1)
                {
                    r.setH(y1 - ry0);
                    m_rects.erase(j);
                    goto loopA;
                }

            }

        }
    }
}

void LRegion::clip(const LRect &rect)
{
    for(std::list<LRect>::iterator i = m_rects.begin(); i != m_rects.end(); i++)
    {
        loopA:

        if((*i).clip(rect))
        {
            i = m_rects.erase(i);

            if(i != m_rects.end())
                goto loopA;
        }
    }

    if(m_rects.size() > 1)
        simplify();
}

const list<LRect> &LRegion::rects() const
{
    return m_rects;
}
