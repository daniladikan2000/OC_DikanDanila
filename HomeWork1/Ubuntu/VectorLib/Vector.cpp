#include "Vector.h"
#include <cmath>
#include <iostream>
const Vector& Vector::Zero() {
    static const Vector zero_instance(Number::Zero(), Number::Zero());
    return zero_instance;
}
const Vector& Vector::OneOne() {
    static const Vector oneone_instance(Number::One(), Number::One());
    return oneone_instance;
}
Vector::Vector(const Number& x, const Number& y) : x(x), y(y) {}
Number Vector::getRadius() const {
    Number x2 = x * x;
    Number y2 = y * y;
    return Number(sqrt((x2 + y2).getValue()));
}
Number Vector::getAngle() const { return Number(atan2(y.getValue(), x.getValue())); }
Vector Vector::operator+(const Vector& other) const { return Vector(this->x + other.x, this->y + other.y); }
void Vector::print() const { std::cout << "(" << x.getValue() << ", " << y.getValue() << ")" << std::endl; }
