#include <iostream>
#include <cmath>
using namespace std;

double function(double x) {
    double yy;
    if ((-8 <= x) && (x < -5)) {
        return -3;
    }
    else if ((-5 <= x) && (x < -3)) {
        return ((3 * x + 9)/2);
    }
    else if ((-3 <= x) && (x < 3)) {
        return (sqrt(9 - pow(x, 2)));
    }
    else if ((3 <= x) && (x < 8)) {
        return ((3 * x - 9)/5);
    }
    else if ((8 <= x) && (x <= 10)) {
        return 3;
    }
    return -1;
}

int main() {
    double Xнач = -8;  // Начальное значение
    double Xкон = 10;  // Конечное значение
    double dx = 0.5;   // Шаг

    cout << " x" << "\t" << " y" << endl;
    cout << "-----------" << endl;
    
    for (double x = Xнач; x <= Xкон; x += dx) {
        double y = function(x);
        
        // Вывод результата на экран
        cout << x << "\t" << y << endl;
    }
    return 0;
}
