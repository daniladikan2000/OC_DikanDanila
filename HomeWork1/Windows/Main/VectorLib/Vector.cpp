#include "pch.h"
#include "Vector.h"
#include <cmath>
#include <iostream>

const Vector Vector::Zero = Vector(Number::Zero, Number::Zero);
const Vector Vector::OneOne = Vector(Number::One, Number::One);

Vector::Vector(const Number& x, const Number& y) : x(x), y(y) {}

Number Vector::getRadius() const {
    Number x2 = x * x;
    Number y2 = y * y;
    return Number(sqrt((x2 + y2).getValue()));
}

Number Vector::getAngle() const {
    return Number(atan2(y.getValue(), x.getValue()));
}

Vector Vector::operator+(const Vector& other) const {
    return Vector(this->x + other.x, this->y + other.y);
}

void Vector::print() const {
    std::cout << "(" << x.getValue() << ", " << y.getValue() << ")" << std::endl;
}