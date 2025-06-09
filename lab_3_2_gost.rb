require 'prime'
require 'securerandom'
require 'thread'
require 'bigdecimal'
require 'bigdecimal/util'

# Многопоточная генерация простых чисел по алгоритму ГОСТ
class PrimeGenerator
  def initialize
    @stop_flag = false
    @mutex = Mutex.new
  end

  def random_int(min, max)
    SecureRandom.random_number(max - min + 1) + min
  end

  def mod_pow(a, b, n)
    a.to_d.mod_exp(b, n).to_i
  end

  def eratosthenes_sieve(limit)
    Prime.each(limit).to_a
  end

  def is_prime_simple(n)
    Prime.prime?(n)
  end

  def diemitko_test(n, n_param, u)
    return false if mod_pow(2, n_param + u, n) == 1
    mod_pow(2, n - 1, n) == 1
  end

  def is_in_bit_range(number, target_bits, tolerance = 0)
    return false if number == 0
    actual_bits = number.bit_length
    actual_bits >= target_bits - tolerance && actual_bits <= target_bits + tolerance
  end

  def generate_prime(k, primes, numbers, results, attempts)
    q_bits = k / 2
    tolerance = k < 10 ? (k <= 5 ? 2 : 1) : 0
    max_attempts = 1000
    attempt_count = 0
    upper_bound = (1 << k) - 1
    lower_bound = 1 << (k - 1)

    while !@stop_flag && attempt_count < max_attempts
      q = primes[random_int(0, primes.size - 1)]
      next unless is_in_bit_range(q, q_bits, tolerance)

      n_param = ((1 << (k-1)) / q)
      n_param += 1 if n_param.odd?

      u = 0
      while !@stop_flag && u < 100 # Ограничим u для предотвращения бесконечного цикла
        p = (n_param + u) * q + 1

        if p > upper_bound
          u = 0
          attempt_count += 1
          break
        end

        if p >= lower_bound && p <= upper_bound
          if is_prime_simple(p) && diemitko_test(p, n_param, u)
            @mutex.synchronize do
              unless numbers.include?(p)
                numbers << p
                results << "+"
                attempts << u
                return true
              end
            end
          end
        end

        u += 2
        attempt_count += 1
      end
    end
    false
  end

  def valid_input(prompt, min_val, max_val)
    loop do
      print prompt
      input = gets.chomp
      if input =~ /^\d+$/ && (value = input.to_i).between?(min_val, max_val)
        return value
      end
      puts "Ошибка. Введите число от #{min_val} до #{max_val}"
    end
  end

  def run
    t = valid_input("Введите размерность простого числа (4-19): ", 4, 19)
    
    primes = eratosthenes_sieve(500)
    puts "\nПростые числа до 500 (Решето Эратосфена):"
    primes.each_slice(10) { |slice| puts slice.join(" ") }
    puts

    numbers = []
    results = []
    attempts = []
    threads = []
    target_count = 10
    num_threads = [4, Concurrent.processor_count].min

    num_threads.times do
      threads << Thread.new do
        while !@stop_flag && numbers.size < target_count
          generate_prime(t, primes, numbers, results, attempts)
        end
      end
    end

    start_time = Time.now
    while Time.now - start_time < 10
      sleep 0.1
      @stop_flag = true if numbers.size >= target_count
    end
    @stop_flag = true

    threads.each(&:join)

    puts "\nРезультаты генерации простых чисел (#{t} бит):\n\n"
    
    puts "|    Num    |" + (1..numbers.size).map { |i| i.to_s.rjust(6) + " |" }.join
    puts "|-----------|" + "-------|" * numbers.size
    puts "|     P     |" + numbers.map { |n| n.to_s.rjust(6) + " |" }.join
    puts "| Результат |" + results.map { |r| r.rjust(6) + " |" }.join
    puts "|     K     |" + attempts.map { |a| a.to_s.rjust(6) + " |" }.join
  end
end

generator = PrimeGenerator.new
generator.run
