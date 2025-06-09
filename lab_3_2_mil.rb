require 'prime'
require 'securerandom'

class PrimeGenerator
  def initialize
    @primes = []
  end

  def random_generator(min, max)
    SecureRandom.random_number(max - min + 1) + min
  end

  def is_prime(i, primes)
    primes.none? { |p| i % p == 0 }
  end

  def eratosthenes_sieve(p)
    primes = [2]
    (3..p).step(2) do |i|
      primes << i if is_prime(i, primes)
    end
    primes
  end

  def mod(number, power, n)
    return 0 if n == 0
    result = 1
    while power > 0
      result = (result * number) % n if power % 2 != 0
      power /= 2
      number = (number * number) % n
    end
    result
  end

  def fermat_theorem(a, x, p)
    return false if p <= 1
    mod(a, x, p) == 1
  end

  def miller_test(n, factors, t)
    return false if n <= 1
    return true if n == 2
    return false if n.even?

    t.times do
      a = random_generator(2, n - 2)
      return false unless fermat_theorem(a, n - 1, n)
    end

    factors.each do |q|
      next if q == 0

      all_a_equal_one = true
      t.times do
        a = random_generator(2, n - 2)
        unless fermat_theorem(a, (n - 1) / q, n)
          all_a_equal_one = false
          break
        end
      end

      return false if all_a_equal_one
    end

    true
  end

  def count_bits(number)
    return 1 if number == 0
    Math.log2(number).to_i + 1
  end

  def within_bit_range(number, bit_count, formin = 0)
    num_bits = count_bits(number)
    return 1 if num_bits < bit_count - formin.abs
    return 2 if num_bits > bit_count
    3
  end

  def create_num(k, numbers, results, attempts, primes)
    q = []
    m = 1
    temp_m = 1
    n = 0
    target_bit_count = k - 1
    temp_errors = 0
    formin = k < 7 && k > 4 ? -1 : (k == 4 ? -2 : 0)
    max_attempts = 1000
    attempts_made = 0

    while attempts_made < max_attempts
      qi = primes[random_generator(0, primes.size - 1)]
      ai = random_generator(1, 12)
      temp_m *= qi**ai

      range = within_bit_range(temp_m, target_bit_count, formin)
      if range == 2
        m = 1
        temp_m = 1
        q.clear
      elsif range == 1
        q << qi
      else
        m = temp_m
        q << qi
        n = 2 * m + 1
        if miller_test(n, q, 1)
          break
        else
          temp_errors += 1
          m = 1
          temp_m = 1
          n = 0
          q.clear
        end
      end
      attempts_made += 1
    end

    return false if attempts_made >= max_attempts || numbers.include?(n)

    temp_errors2 = 0
    iter = 100
    while iter >= 10
      temp_errors2 = 0
      100.times do
        temp_errors2 += 1 unless miller_test(n, q, 1)
      end
      break if temp_errors2 <= 10
      iter -= 10
    end

    results << (temp_errors2 <= 10 ? "+" : "-")
    numbers << n
    attempts << temp_errors
    true
  end

  def get_single_word_input(prompt)
    loop do
      print prompt
      input = gets.chomp
      next if input.empty?

      if input.split.size != 1
        puts "Неправильное количество. Пожалуйста, введите одно число."
        next
      end

      unless input =~ /^\d+$/
        puts "Некорректное число. Попробуйте снова."
        next
      end

      value = input.to_i
      if value <= 3 || value > 20
        puts "Число должно быть в диапазоне 4-20. Попробуйте снова."
        next
      end

      return value
    end
  end

  def run
    t = get_single_word_input("Введите размерность простого числа (4-20 бит): ")

    @primes = eratosthenes_sieve(500)
    puts "Простые числа до 500 (Решето Эратосфена):\n\n"
    @primes.each_slice(10) { |slice| puts slice.join(" ") }
    puts "\n"

    numbers = []
    results = []
    attempts = []

    10.times do
      unless create_num(t, numbers, results, attempts, @primes)
        puts "Не удалось сгенерировать уникальное простое число после нескольких попыток."
        break
      end
    end

    unless numbers.empty?
      puts "\nРезультаты генерации простых чисел (#{t} бит):\n\n"
      puts "|    Num    |" + (1..numbers.size).map { |i| i.to_s.rjust(6) + " |" }.join
      puts "|-----------|" + "-------|" * numbers.size
      puts "|     P     |" + numbers.map { |n| n.to_s.rjust(6) + " |" }.join
      puts "| Результат |" + results.map { |r| r.rjust(6) + " |" }.join
      puts "|     K     |" + attempts.map { |a| a.to_s.rjust(6) + " |" }.join
    end
  end
end

generator = PrimeGenerator.new
generator.run
