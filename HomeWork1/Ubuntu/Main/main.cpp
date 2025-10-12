#include <iostream>
#include "../NumberLib/Number.h"
#include "../VectorLib/Vector.h"
int main() {
    std::cout << "--- Demonstrating NumberLib ---" << std::endl;
    Number numA(10.5), numB(2.0);
    std::cout << "Number::Zero = " << Number::Zero().getValue() << std::endl;
    std::cout << "Number::One = " << Number::One().getValue() << std::endl;
    Number sum = numA + numB;
    std::cout << "A + B = " << sum.getValue() << std::endl;
    std::cout << "\n--- Demonstrating VectorLib ---" << std::endl;
    Vector vec1(Number(3.0), Number(4.0));
    std::cout << "Vector 1: ";
    vec1.print();
    std::cout << "Vector::OneOne: ";
    Vector::OneOne().print();
    Vector vec2 = vec1 + Vector::OneOne();
    std::cout << "Vector 1 + Vector(1,1): ";
    vec2.print();
    std::cout << "Radius: " << vec1.getRadius().getValue() << std::endl;
    return 0;
}
