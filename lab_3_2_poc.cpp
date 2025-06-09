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
#include <chrono>
using namespace std;

// Функция для генерации случайного числа в заданном диапазоне
int RandomGenerator(int min, int max) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}

// Простая функция для проверки простоты числа
bool IsPrime(int i, vector<int>& prime) {
    for (int j = 0; j < prime.size(); j++) {
        if (i % prime[j] == 0) {
            return false;
        }
    }
    return true;
}

vector<int> eratochene(int p) {
    vector<int> primeNumb;
    primeNumb.push_back(2);
    for (int i = 3; i < p; i += 2) {
        if (IsPrime(i, primeNumb)) {
            primeNumb.push_back(i);
        }
    }
    return primeNumb;
}

// Функция для модульного возведения в степень
uint64_t mod_pow(uint64_t number, uint64_t power, uint64_t n) {
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

// Функция для проверки теоремы Ферма
bool ferma(int a, int x, int p) {
    uint64_t result = mod_pow(a, x, p);
    return result == 1;
}

// Тест Поклингтона на простоту
bool poklingtonTest(int64_t n, const vector<int>& factors, int t) {
    vector<int> a;
    for (int i = 0; i < t; i++) {
        int aj = RandomGenerator(2, n - 1);
        if (!ferma(aj, n - 1, n)) {
            return false;
        }

        bool found = false;
        for (int qj : factors) {
            if (ferma(aj, (n - 1) / qj, n)) {
                found = true;
                break;
            }
        }

        if (!found) {
            return true;
        }
    }
    return false;
}

// Функция для вычисления количества бит в числе
int countBits(uint64_t number) {
    if (number == 0) return 1;
    return static_cast<int>(log2(number)) + 1;
}

// Функция для проверки, является ли число меньше или больше заданного количества бит
int isWithinBitRange(uint64_t number, int bitCount, int formin) {
    int numBits = countBits(number);
    if (numBits < bitCount) return 1;
    if (numBits > bitCount) return 2;
    return 3;
}

// Функция для создания числа и проведения тестов
bool createNum(int k, vector<int>& numbers, vector<string>& plusminus, vector<int>& oshibki, vector<int>& primes, bool& shouldStop, mutex& mtx) {
    vector<int> q;
    uint64_t f = 1;
    uint64_t tempf = 1;
    int qi;
    int ai;
    int64_t n;
    int targetBitCountF = (k / 2) + 1;
    int targetBitCountR = targetBitCountF - 1;
    int tempOshibki = 0;
    int formin = 1;
    uint64_t r;
    if (k < 8) {
        formin = 0;
    }

    while (true) {
        {
            lock_guard<mutex> lock(mtx);
            if (shouldStop) return false;
        }

        qi = primes[RandomGenerator(0, primes.size() / 10)];
        ai = RandomGenerator(1, 12);
        tempf *= pow(qi, ai);
        if (isWithinBitRange(tempf, targetBitCountF, formin) == 2) {
            f = 1;
            tempf = 1;
            q.clear();
        }
        else if (isWithinBitRange(tempf, targetBitCountF, formin) == 1) {
            q.push_back(qi);
        }
        else {
            f = tempf;
            q.push_back(qi);
            do {
                r = RandomGenerator(1, 30) * 2;
            } while (isWithinBitRange(r, targetBitCountR, formin) != 3);
            n = r * f + 1;
            if (poklingtonTest(n, q, 1)) {
                break;
            }
            else {
                tempOshibki++;
                f = 1;
                tempf = 1;
                n = 0;
                q.clear();
            }
        }
    }

    {
        lock_guard<mutex> lock(mtx);
        if (shouldStop) return false;
        if (find(numbers.begin(), numbers.end(), n) != numbers.end()) {
            return false;
        }
    }

    int tempOshibki2 = 0;
    int i;
    int iter = 1;
    while (true) {
        {
            lock_guard<mutex> lock(mtx);
            if (shouldStop) return false;
        }

        for (i = 0; i < 100; i++) {
            if (!poklingtonTest(n, q, iter)) {
                tempOshibki2++;
            }
        }
        if (tempOshibki2 <= i / 10) {
            break;
        }
        else {
            tempOshibki2 = 0;
            iter++;
        }
    }

    {
        lock_guard<mutex> lock(mtx);
        if (shouldStop) return false;
        if (numbers.size() >= 10) return false;
        
        if (tempOshibki2 <= i / 10) {
            plusminus.push_back("+");
        }
        else {
            plusminus.push_back("-");
        }
        numbers.push_back(n);
        oshibki.push_back(tempOshibki);
        if (numbers.size() >= 10) {
            shouldStop = true;
        }
        return true;
    }
}

int getSingleWordInput(const string& prompt) {
    string input;
    double value;

    while (true) {
        cout << prompt;
        getline(cin, input);

        bool valid = true;
        for (size_t i = 0; i < input.size(); ++i) {
            char c = input[i];
            if (!isdigit(c)) {
                valid = false;
                break;
            }
        }
        istringstream(input) >> value;
        if (value <= 3 || value > 20) {
            cout << "Некорректное число. Попробуйте снова.\n";
            continue;
        }
        return value;
    }
}

int main() {
    vector<int> numbers;
    vector<string> results;
    vector<int> attempts;
    int bit_length = 0;
    bool stop_flag = false;
    mutex mtx;

    // Ввод длины простого числа
    while (true) {
        bit_length = getSingleWordInput("Введите размерность простого числа (4-20 бит): \n");
        if (bit_length >= 4 && bit_length <= 20) break;
        cout << "Некорректный ввод. Допустимый диапазон: 4-20 бит.\n";
    }

    // Генерация простых чисел до 500 (Решето Эратосфена)
    vector<int> primes = eratochene(500);
    cout << "\nПростые числа до 500 (Решето Эратосфена):\n";
    for (size_t i = 0; i < primes.size(); ++i) {
        cout << primes[i] << (i % 10 == 9 ? "\n" : " ");
    }
    cout << "\n\n";

    // Запуск генерации в нескольких потоках
    vector<thread> threads;
    const int num_threads = min(4, static_cast<int>(thread::hardware_concurrency()));

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&]() {
            while (true) {
                {
                    lock_guard<mutex> lock(mtx);
                    if (stop_flag) break;
                }
                createNum(bit_length, numbers, results, attempts, primes, stop_flag, mtx);
            }
        });
    }

    // Ожидание завершения (максимум 10 секунд)
    auto start = chrono::steady_clock::now();
    while (chrono::steady_clock::now() - start < chrono::seconds(10)) {
        {
            lock_guard<mutex> lock(mtx);
            if (numbers.size() >= 10) {
                stop_flag = true;
                break;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    {
        lock_guard<mutex> lock(mtx);
        stop_flag = true;
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    // Обрезаем результаты до 10 чисел, если их больше
    {
        lock_guard<mutex> lock(mtx);
        if (numbers.size() > 10) {
            numbers.resize(10);
            results.resize(10);
            attempts.resize(10);
        }
    }

    // Вывод результатов
    if (!numbers.empty()) {
        cout << "\nРезультаты генерации простых чисел (" << bit_length << " бит):\n\n";

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
        for (const string& res : results) {
            cout << setw(6) << res << " |";
        }

        cout << "\n|     K     |";
        for (int att : attempts) {
            cout << setw(6) << att << " |";
        }

        cout << endl;
    }
    return 0;
}
