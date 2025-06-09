#include <iostream>
#include <vector>
#include <iomanip>
#include <random>
#include <cmath>
#include <algorithm>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

using namespace std;



// МНОГОПОТОК!!
// mutex - синхронизация доступа к общим данным
// atomic - безопасная остановка потоков
// threads - оптимальное количество потоков


mutex mtx;
atomic<bool> stop_flag(false);

int random_int(int min, int max) { // генерит случайное число в диапазоне
    static mt19937 gen(random_device{}());
    uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

uint64_t mod_pow(uint64_t a, uint64_t b, uint64_t n) { // считает a^b mod n и юзает алгоритм быстрого возведения
    uint64_t result = 1;
    a %= n;
    
    while (b > 0) {
        if (b % 2 == 1) {
            result = (result * a) % n;
        }
        b >>= 1;
        a = (a * a) % n;
    }
    return result;
}

vector<int> eratosthenes_sieve(int limit) { // Эратосфен
    vector<bool> is_prime(limit + 1, true);
    is_prime[0] = is_prime[1] = false;
    
    for (int p = 2; p * p <= limit; ++p) {
        if (is_prime[p]) {
            for (int i = p * p; i <= limit; i += p) {
                is_prime[i] = false;
            }
        }
    }
    
    vector<int> primes;
    for (int i = 2; i <= limit; ++i) {
        if (is_prime[i]) primes.push_back(i);
    }
    return primes;
}

bool is_prime_simple(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

bool diemitko_test(int64_t n, int N, int u) { // проверка 2^(N + u) mod n != 1 И 2^(n-1) mod n == 1 (малая теорема Ферма)
    if (mod_pow(2, N + u, n) == 1) {
        return false;
    }
    
    return mod_pow(2, n - 1, n) == 1;
}

bool is_in_bit_range(int number, int target_bits, int tolerance = 0) { // проверка на нахождение числа в заданном битовом диапазоне с допуском
    if (number == 0) return false;
    int actual_bits = static_cast<int>(log2(number)) + 1;
    return actual_bits >= target_bits - tolerance && 
           actual_bits <= target_bits + tolerance;
}

bool generate_prime(int k, const vector<int>& primes, vector<int>& numbers, vector<string>& results, vector<int>& attempts) { // выбирает простое q из primes; вычисляет N и u; кандидат p = (N+u)*q + 1; проверка р с Диемитко; добавляет в список
    const int q_bits = k / 2;
    const int tolerance = k < 10 ? (k <= 5 ? 2 : 1) : 0;
    int q, N, u = 0, p;
    int max_attempts = 1000;
    int attempt_count = 0;
    
    uint64_t upper_bound = (1ULL << k) - 1;
    uint64_t lower_bound = 1ULL << (k - 1);
    
    while (!stop_flag && attempt_count < max_attempts) {
        q = primes[random_int(0, primes.size() - 1)];
        if (!is_in_bit_range(q, q_bits, tolerance)) continue;
        
        // Вычисляем N
        N = ((1 << (k-1)) / q);
        if (N % 2 != 0) N++;
        
        p = (N + u) * q + 1;
        
        if (p > upper_bound) {
            u = 0;
            attempt_count++;
            continue;
        }
        
        if (p >= lower_bound && p <= upper_bound) {
            if (is_prime_simple(p) && diemitko_test(p, N, u)) {
                lock_guard<mutex> lock(mtx);
                
                if (find(numbers.begin(), numbers.end(), p) == numbers.end()) {
                    numbers.push_back(p);
                    results.push_back("+");
                    attempts.push_back(u);
                    return true;
                }
            }
        }
        u += 2;
        attempt_count++;
    }
    return false;
}

int valid_input(const string& prompt, int min_val, int max_val) { // КОРРЕККТНЫЙ ВВОД
    int value;
    while (true) {
        cout << prompt;
        cin >> value;
        if (cin.fail() || value < min_val || value > max_val) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ошибка. Введите число от " << min_val << " до " << max_val << "\n";
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
    }
    return value;
}

int main() {
    int t = valid_input("Введите размерность простого числа (4-19): ", 4, 19);
    
    vector<int> primes = eratosthenes_sieve(500);
    cout << "\nПростые числа до 500 (Решето Эратосфена):\n";
    for (size_t i = 0; i < primes.size(); ++i) {
        cout << primes[i] << (i % 10 == 9 ? "\n" : " ");
    }
    cout << "\n\n";

    vector<int> numbers;
    vector<string> results;
    vector<int> attempts;
    
    vector<thread> threads;
    const int target_count = 10;
    const int num_threads = min(4, static_cast<int>(thread::hardware_concurrency()));
    
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&, i]() {
            while (!stop_flag && numbers.size() < target_count) {
                generate_prime(t, primes, numbers, results, attempts);
            }
        });
    }
    
    auto start = chrono::steady_clock::now();
    while (chrono::steady_clock::now() - start < chrono::seconds(10)) {
        if (numbers.size() >= target_count) {
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    stop_flag = true;
    
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
    
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
    for (const string& res : results) {
        cout << setw(6) << res << " |";
    }
    
    cout << "\n|     K     |";
    for (int att : attempts) {
        cout << setw(6) << att << " |";
    }
    cout << endl;
    
    return 0;
}
