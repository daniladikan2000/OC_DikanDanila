#include "pch.h"
#include "Number.h"
#include <stdexcept>

const Number Number::Zero = Number(0.0);
const Number Number::One = Number(1.0);

Number::Number(double val) : value(val) {}

double Number::getValue() const {
    return value;
}

Number Number::operator+(const Number& other) const {
    return Number(this->value + other.value);
}

Number Number::operator-(const Number& other) const {
    return Number(this->value - other.value);
}

Number Number::operator*(const Number& other) const {
    return Number(this->value * other.value);
}

Number Number::operator/(const Number& other) const {
    if (other.value == 0) {
        throw std::runtime_error("Division by zero!");
    }
    return Number(this->value / other.value);
}