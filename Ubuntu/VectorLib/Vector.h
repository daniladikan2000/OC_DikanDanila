#pragma once
#include "../NumberLib/Number.h"
#include "SharedLib_API.h"
class SHAREDLIB_API Vector {
private:
    Number x, y;
public:
    Vector(const Number& x, const Number& y);
    Number getRadius() const;
    Number getAngle() const;
    Vector operator+(const Vector& other) const;
    void print() const;
    static const Vector& Zero();
    static const Vector& OneOne();
};
