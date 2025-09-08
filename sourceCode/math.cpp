#include <iostream>
#include <cmath>
int main(){
    int op;
    std::cout<<"Math functions\n1.sqrt\n2.pow\n3.sin\n4.cos\n5.tan\n6.Basic Calculator\n7.Exit\n";
    std::cin>>op;
    switch(op){
        case 1:{
            double num;
            std::cout<<"Enter number to find square root of: ";
            std::cin>>num;
            std::cout<<"Square root of "<<num<<" is "<<sqrt(num)<<"\n";
            break;
        }
        case 2:{
            double base, exp;
            std::cout<<"Enter base: ";
            std::cin>>base;
            std::cout<<"Enter exponent: ";
            std::cin>>exp;
            std::cout<<base<<" raised to the power of "<<exp<<" is "<<pow(base, exp)<<"\n";
            break;
        }
        case 3:{
            double angle;
            std::cout<<"Enter angle in degrees: ";
            std::cin>>angle;
            double radians = angle * (M_PI / 180.0);
            std::cout<<"Sine of "<<angle<<" degrees is "<<sin(radians)<<"\n";
            break;
        }
        case 4:{
            double angle;
            std::cout<<"Enter angle in degrees: ";
            std::cin>>angle;
            double radians = angle * (M_PI / 180.0);
            std::cout<<"Cosine of "<<angle<<" degrees is "<<cos(radians)<<"\n";
            break;
        }
        case 5:{
            double angle;
            std::cout<<"Enter angle in degrees: ";
            std::cin>>angle;
            double radians = angle * (M_PI / 180.0);
            std::cout<<"Tangent of "<<angle<<" degrees is "<<tan(radians)<<"\n";
            break;
        }
        case 6:
        int num1, num2;
        char operation;
        std::cout << "Enter first number: ";
        std::cin >> num1;
        std::cout << "Enter an operator (+, -, *, /): ";
        std::cin >> operation;
        std::cout << "Enter second number: ";
        std::cin >> num2;
        switch (operation) {
            case '+':
                std::cout << num1 << " + " << num2 << " = " << num1 + num2 << "\n";
                break;
            case '-':
                std::cout << num1 << " - " << num2 << " = " << num1 - num2 << "\n";
                break;
            case '*':
                std::cout << num1 << " * " << num2 << " = " << num1 * num2 << "\n";
                break;
            case '/':
                if (num2 != 0)
                    std::cout << num1 << " / " << num2 << " = " << static_cast<double>(num1) / num2 << "\n";
                else
                    std::cout << "Error: Division by zero is undefined.\n";
                break;
            default:
                std::cout << "Error: Invalid operator.\n";
                break;
        }
        break;
        case 7:
            std::cout<<"Exiting program.\n";
            break;
        default:
            std::cout<<"Invalid option\n";
            break;
    }
     // wait for user to press Enter before closing
    return 0;
}