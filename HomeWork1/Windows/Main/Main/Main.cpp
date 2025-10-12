#include <iostream>
#include "Number.h"
#include "Vector.h"

int main() {
    std::cout << "--- Demonstrating NumberLib ---" << std::endl;

    // Создание чисел из NumberLib
    Number numA(10.5);
    Number numB(2.0);

    // Использование статических переменных
    std::cout << "Number::Zero = " << Number::Zero.getValue() << std::endl;
    std::cout << "Number::One = " << Number::One.getValue() << std::endl;

    // Операции над числами
    Number sum = numA + numB;
    Number diff = numA - numB;
    Number prod = numA * numB;
    Number quot = numA / numB;

    std::cout << "A = " << numA.getValue() << ", B = " << numB.getValue() << std::endl;
    std::cout << "A + B = " << sum.getValue() << std::endl;
    std::cout << "A - B = " << diff.getValue() << std::endl;
    std::cout << "A * B = " << prod.getValue() << std::endl;
    std::cout << "A / B = " << quot.getValue() << std::endl;

    std::cout << "\n--- Demonstrating VectorLib ---" << std::endl;

    // Создание векторов с использованием чисел из NumberLib
    Vector vec1(Number(3.0), Number(4.0));
    std::cout << "Vector 1: ";
    vec1.print();

    // Использование статических векторов
    std::cout << "Vector::Zero: ";
    Vector::Zero.print();
    std::cout << "Vector::OneOne: ";
    Vector::OneOne.print();

    // Сложение векторов
    Vector vec2 = vec1 + Vector::OneOne;
    std::cout << "Vector 1 + Vector(1,1): ";
    vec2.print();

    // Расчет полярных координат
    Number radius = vec1.getRadius();
    Number angle = vec1.getAngle();
    std::cout << "Polar coordinates for Vector 1:" << std::endl;
    std::cout << "Radius: " << radius.getValue() << std::endl;
    std::cout << "Angle (radians): " << angle.getValue() << std::endl;

    return 0;
}