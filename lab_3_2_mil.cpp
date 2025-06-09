#include <iostream>
#include <vector>
#include <iomanip>
#include <random>
#include <cmath>
#include <algorithm>
#include <thread>
#include <mutex>
#include <string>
#include <sstream>



using namespace std;

int RandomGenerator(int min, int max) { //
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}

bool IsPrime(int i, vector<int>& prime) {
    for (int j = 0; j < prime.size(); j++) {
        if (i % prime[j] == 0) {
            return false;
        }
    }
    return true;
}

vector<int> IsPrimeEratosfen(int p) {
    vector<int> primeNumb;
    primeNumb.push_back(2);
    for (int i = 3; i < p; i += 2) {
        if (IsPrime(i, primeNumb)) {
            primeNumb.push_back(i);
        }
    }
    return primeNumb;
}

uint64_t mod(uint64_t number, uint64_t power, uint64_t n) {
    if (n == 0) return 0;
    uint64_t ostatok = 1;
    while (power > 0) {
        if (power % 2 != 0) {
            ostatok = (ostatok * number) % n;
        }
        power /= 2;
        number = (number * number) % n;
    }
    return ostatok;
}

bool TeoremaFerma(int a, int x, int p) { //  a^(x) mod p == 1
    if (p <= 1) return false;
    uint64_t result = mod(a, x, p);
    return result == 1;
}

bool MillerTest(int64_t n, const vector<int>& factors, int t) { // проверка тривиальных случаев (n <= 1; n == 2; четные числа); проверка Малой Т Ферма для t случайных оснований a; проверка доп условий из factors
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    for (int i = 0; i < t; i++) {
        int a = RandomGenerator(2, n - 2);
        if (!TeoremaFerma(a, n - 1, n)) {
            return false;
        }
    }

    for (int q : factors) {
        if (q == 0) continue;
        bool all_a_equal_one = true;

        for (int i = 0; i < t; i++) {
            int a = RandomGenerator(2, n - 2);
            if (!TeoremaFerma(a, (n - 1) / q, n)) {
                all_a_equal_one = false;
                break;
            }
        }

        if (all_a_equal_one) {
            return false;
        }
    }

    return true;
}

int countBits(uint64_t number) { // количество бит в числе
    if (number == 0) return 1;
    return static_cast<int>(log2(number) + 1);
}

int isWithinBitRange(uint64_t number, int bitCount, int formin) { // проверка нахождения числа в заданном бмтовом диап......
    int numBits = countBits(number);
    if (numBits < bitCount - abs(formin)) return 1;
    if (numBits > bitCount) return 2;
    return 3;
}

bool createNum(int k, vector<int>& numbers, vector<string>& results, vector<int>& attempts, vector<int>& primes) { // генереция числа, почти как в lab_3_2_gost
    vector<int> q;
    int64_t m = 1;
    int tempM = 1;
    int qi;
    int ai;
    int64_t n = 0;
    int targetBitCountM = k - 1;
    int tempOshibki = 0;
    int formin = 0;
    
    if (k < 7 && k > 4) {
        formin = -1;
    }
    else if (k == 4) {
        formin = -2;
    }

    int max_attempts = 1000000;
    int attempts_made = 0;

    while (attempts_made < max_attempts) {
        qi = primes[RandomGenerator(0, primes.size() - 1)];
        ai = RandomGenerator(1, 12);
        tempM *= pow(qi, ai);
        
        int range = isWithinBitRange(tempM, targetBitCountM, formin);
        if (range == 2) {
            m = 1;
            tempM = 1;
            q.clear();
        }
        else if (range == 1) {
            q.push_back(qi);
        }
        else {
            m = tempM;
            q.push_back(qi);
            n = 2 * m + 1;
            if (MillerTest(n, q, 1)) {
                break;
            }
            else {
                tempOshibki++;
                m = 1;
                tempM = 1;
                n = 0;
                q.clear();
            }
        }
        attempts_made++;
    }

    if (attempts_made >= max_attempts) {
        return false;
    }

    // Проверка на уникальность числа
    if (find(numbers.begin(), numbers.end(), n) != numbers.end()) {
        return false;
    }

    int tempOshibki2 = 0;
    int i;
    int iter = 100;
    while (iter >= 10) {
        tempOshibki2 = 0;
        for (i = 0; i < iter; i++) {
            if (!MillerTest(n, q, 1)) {
                tempOshibki2++;
            }
        }
        if (tempOshibki2 <= i / 10) {
            break;
        }
        iter -= 10;
    }

    if (tempOshibki2 <= iter / 10) {
        results.push_back("+");
    }
    else {
        results.push_back("-");
    }
    numbers.push_back(n);
    attempts.push_back(tempOshibki);
    return true;
}

int getSingleWordInput(const string& prompt) { // корректный ввод
    string input;
    int value;

    while (true) {
        cout << prompt;
        getline(cin, input);

        istringstream iss(input);
        string word;
        if (!(iss >> word)) {
            cout << "Пустой ввод. Пожалуйста, введите число.\n";
            continue;
        }
        
        // Проверяем, есть ли еще слова
        string extra;
        if (iss >> extra) {
            cout << "Неправильное количество. Пожалуйста, введите одно число.\n";
            continue;
        }

        bool valid = true;
        for (char c : word) {
            if (!isdigit(c)) {
                valid = false;
                break;
            }
        }
        if (!valid) {
            cout << "Некорректное число. Попробуйте снова.\n";
            continue;
        }

        istringstream(word) >> value;
        if (value <= 3 || value > 20) {
            cout << "Число должно быть в диапазоне 4-20. Попробуйте снова.\n";
            continue;
        }
        return value;
    }
}

int main() {
    vector<int> numbers;
    vector<string> Res;
    vector<int> K;
    int t = 0;
    int input;

    while (true) {
        input = getSingleWordInput("Введите размерность простого числа (4-20 бит): ");
        break;
    }

    t = input;

    vector<int> primes = IsPrimeEratosfen(500);
    cout << "Простые числа до 500 (Решето Эратосфена):\n" << endl;
    for (size_t i = 0; i < primes.size(); ++i) {
        cout << primes[i] << (i % 10 == 9 ? "\n" : " ");
    }
    cout << "\n\n";

    for (int i = 0; i < 10; i++) {
        if (!createNum(t, numbers, Res, K, primes)) {
            cout << "Не удалось сгенерировать уникальное простое число после нескольких попыток." << endl;
            break;
        }
    }

    // Вывод результатов
    if (!numbers.empty()) {
        cout << "\nРезультаты генерации простых чисел (" << t << " бит):\n\n";
        cout << "|    Num    |";
        for (size_t i = 0; i < numbers.size(); ++i) {
            cout << setw(6) << i+1 << " |";
        }
        cout << "\n|-----------|";
        for (size_t i = 0; i < numbers.size(); ++i) {
            cout << "-------|";
        }
        cout << "\n|     P     |";
        for (int num : numbers) {
            cout << setw(6) << num << " |";
        }
        cout << "\n| Результат |";
        for (const string& res : Res) {
            cout << setw(6) << res << " |";
        }
        cout << "\n|     K     |";
        for (int att : K) {
            cout << setw(6) << att << " |";
        }
        cout << endl;
    }

    return 0;
}

// n = 2*m + 1 ------- КАНДИДАТ
