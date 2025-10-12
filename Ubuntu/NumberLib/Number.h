#pragma once
class Number {
private:
    double value;
public:
    Number(double val = 0.0);
    double getValue() const;
    Number operator+(const Number& other) const;
    Number operator-(const Number& other) const;
    Number operator*(const Number& other) const;
    Number operator/(const Number& other) const;
    static const Number& Zero();
    static const Number& One();
};
