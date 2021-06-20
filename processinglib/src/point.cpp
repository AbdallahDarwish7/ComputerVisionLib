//
// Created by abdulla167 on ١٦‏/٥‏/٢٠٢١.
//

#include "point.h"

point::point() {}

point::point(const point &point): x{point.x}, y{point.y} {}

point::point(int x, int y): x{x}, y{y} {}

point point::operator+ (point & p2){
    point p3{x + p2.x, y + p2.y};
    return p3;
}