#include <iostream>
#include <cmath>
#include <numeric>
#include <limits>

using namespace std;

// Функция для проверки, является ли число рациональным
bool is_rational(double sum, long long max_denominator = 1000) {
    for (long long denom = 1; denom <= max_denominator; ++denom) {
        double numerator = round(sum * denom);
        if (fabs(numerator / denom - sum) < 1e-9) {
            return true;
        }
    }
    return false;
}

// Функция для вычисления НОД
long long gcd(long long a, long long b) {
    while (b != 0) {
        long long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Функция для представления числа в виде дроби
void to_fraction(double sum, long long &numerator, long long &denominator, long long max_denominator = 1000) {
    for (denominator = 1; denominator <= max_denominator; ++denominator) {
        numerator = round(sum * denominator);
        if (fabs(static_cast<double>(numerator) / denominator - sum) < 1e-9) {
            long long common_divisor = gcd(abs(numerator), denominator);
            numerator /= common_divisor;
            denominator /= common_divisor;
            return;
        }
    }
    numerator = round(sum);
    denominator = 1;
}

int main() {
    int a, b;
    cin >> a >> b;

    // Проверка сходимости по признаку Даламбера
    if (b <= 1) {
        cout << "infinity" << endl;
        return 0;
    }

    // Численное приближение суммы ряда
    double sum = 0.0;
    double epsilon = 1e-10; // Точность
    double term;
    int n = 1;
    do {
        term = pow(n, a) / pow(b, n);
        sum += term;
        n++;
    } while (term > epsilon);

    // Проверка на рациональность
    if (is_rational(sum)) {
        long long numerator, denominator;
        to_fraction(sum, numerator, denominator);
        cout << numerator << "/" << denominator << endl;
    } else {
        cout << "irrational" << endl;
    }

    return 0;
}



// краткое 
