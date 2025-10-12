#include "Number.h"
#include <stdexcept>
const Number& Number::Zero() {
    static const Number zero_instance(0.0);
    return zero_instance;
}
const Number& Number::One() {
    static const Number one_instance(1.0);
    return one_instance;
}
Number::Number(double val) : value(val) {}
double Number::getValue() const { return value; }
Number Number::operator+(const Number& other) const { return Number(this->value + other.value); }
Number Number::operator-(const Number& other) const { return Number(this->value - other.value); }
Number Number::operator*(const Number& other) const { return Number(this->value * other.value); }
Number Number::operator/(const Number& other) const {
    if (other.value == 0) { throw std::runtime_error("Division by zero!"); }
    return Number(this->value / other.value);
}
