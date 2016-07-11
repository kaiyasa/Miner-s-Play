
#ifndef __VIEWPORT_H__
#define __VIEWPORT_H__

#include "point.h"

class Viewport {
  public:
    Viewport()
      : Viewport(Point(), Point())
    {
    }

    Viewport(int uy, int ux, int ly, int lx)
      : Viewport(Point(uy, ux), Point(ly, lx))
    {
    }

    Viewport(const Point &up, const Point &low)
      : upper(up), lower(low)
    {
    }

    int width() const  { return lower.col - upper.col; }
    int height() const { return lower.row - upper.row; }

    Point  top() const { return upper; }
    Point& top()       { return upper; }
    Point  bot() const { return lower; }
    Point& bot()       { return lower; }

  private:
    Point upper, lower;
};

#endif
