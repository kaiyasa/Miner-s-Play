
#ifndef __POINT_H__
#define __POINT_H__

struct Point {
    int row, col;

    Point()
      : Point(0, 0)
    {
    }

    Point(int y, int x)
      : row(y), col(x)
    {
    }

    Point operator +(const Point &rhs) const
      { return Point(row + rhs.row, col + rhs.col); }

    Point operator -(const Point &rhs) const
      { return Point(row - rhs.row, col - rhs.col); }
};

#endif
