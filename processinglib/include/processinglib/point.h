//
// Created by abdulla167 on ١٦‏/٥‏/٢٠٢١.
//

#ifndef CV_POINT_H
#define CV_POINT_H


class point {
public:
    int x, y;

    point();
    point(const point & point);
    point(int x, int y);
    point operator+ (point & p2);
};



#endif //CV_POINT_H
