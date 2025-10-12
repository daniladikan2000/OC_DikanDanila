#pragma once

class Number {
private:
    double value;

public:
    Number(double val = 0.0);

    Number operator+(const Number& other) const;
    Number operator-(const Number& other) const;
    Number operator*(const Number& other) const;
    Number operator/(const Number& other) const;

    double getValue() const;

    static const Number Zero;
    static const Number One;
};