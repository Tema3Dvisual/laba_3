require 'prime'
require 'securerandom'
require 'thread'
require 'bigdecimal'
require 'bigdecimal/util'

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

  def poklington_test(n, factors, t)
    factors.each do |q|
      next if q == 0

      found = false
      t.times do
        a = random_int(2, n - 1)
        unless mod_pow(a, n - 1, n) == 1
          return false
        end

        if mod_pow(a, (n - 1) / q, n) == 1
          found = true
          break
        end
      end

      return true unless found
    end
    false
  end

  def count_bits(number)
    number.zero? ? 1 : number.bit_length
  end

  def within_bit_range(number, bit_count, tolerance = 0)
    return false if number == 0
    actual_bits = count_bits(number)
    actual_bits >= bit_count - tolerance && actual_bits <= bit_count + tolerance
  end

  def create_num(k, numbers, results, attempts, primes)
    q = []
    f = 1
    temp_f = 1
    n = 0
    target_bit_count_f = (k / 2) + 1
    target_bit_count_r = target_bit_count_f - 1
    temp_errors = 0
    formin = k < 8 ? 0 : 1
    max_attempts = 1000
    attempt_count = 0

    while !@stop_flag && attempt_count < max_attempts
      qi = primes[random_int(0, primes.size / 10)]
      ai = random_int(1, 12)
      temp_f *= qi**ai

      range = if within_bit_range(temp_f, target_bit_count_f, formin)
                3
              elsif count_bits(temp_f) < target_bit_count_f - formin.abs
                1
              else
                2
              end

      if range == 2
        f = 1
        temp_f = 1
        q.clear
      elsif range == 1
        q << qi
      else
        f = temp_f
        q << qi
        begin
          r = random_int(1, 30) * 2
        end until within_bit_range(r, target_bit_count_r, formin)
        n = r * f + 1

        if poklington_test(n, q, 1)
          break
        else
          temp_errors += 1
          f = 1
          temp_f = 1
          n = 0
          q.clear
        end
      end
      attempt_count += 1
    end

    return false if attempt_count >= max_attempts

    @mutex.synchronize do
      return false if @stop_flag || numbers.include?(n) || numbers.size >= 10

      temp_errors2 = 0
      iter = 1
      loop do
        temp_errors2 = 0
        100.times do
          temp_errors2 += 1 unless poklington_test(n, q, iter)
        end

        break if temp_errors2 <= 10
        iter += 1
      end

      if temp_errors2 <= 10
        results << "+"
      else
        results << "-"
      end

      numbers << n
      attempts << temp_errors

      @stop_flag = true if numbers.size >= 10
      true
    end
  end

  def get_single_word_input(prompt, min_val, max_val)
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
    t = get_single_word_input("Введите размерность простого числа (4-20 бит): ", 4, 20)
    
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
          create_num(t, numbers, results, attempts, primes)
        end
      end
    end

    start_time = Time.now
    while Time.now - start_time < 10
      sleep 0.1
      @mutex.synchronize { @stop_flag = true if numbers.size >= target_count }
    end
    @stop_flag = true

    threads.each(&:join)

    @mutex.synchronize do
      if numbers.size > 10
        numbers = numbers.take(10)
        results = results.take(10)
        attempts = attempts.take(10)
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
end

generator = PrimeGenerator.new
generator.run
