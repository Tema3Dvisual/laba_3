# Функция для проверки, является ли число рациональным
def is_rational(sum, max_denominator = 1000)
  (1..max_denominator).each do |denom|
    numerator = (sum * denom).round
    if (numerator.to_f / denom - sum).abs < 1e-9
      return true
    end
  end
  false
end

# Функция для вычисления НОД
def gcd(a, b)
  while b != 0
    a, b = b, a % b
  end
  a
end

# Функция для представления числа в виде дроби
def to_fraction(sum, max_denominator = 1000)
  numerator = 0
  denominator = 1
  
  (1..max_denominator).each do |denom|
    num = (sum * denom).round
    if (num.to_f / denom - sum).abs < 1e-9
      common_divisor = gcd(num.abs, denom)
      numerator = num / common_divisor
      denominator = denom / common_divisor
      return [numerator, denominator]
    end
  end
  
  [(sum.round), 1]
end

# Основная программа
a, b = gets.split.map(&:to_i)

# Проверка сходимости по признаку Даламбера
if b <= 1
  puts "infinity"
  exit
end

# Численное приближение суммы ряда
sum = 0.0
epsilon = 1e-10 # Точность
term = 0.0
n = 1

loop do
  term = (n**a).to_f / (b**n)
  sum += term
  n += 1
  break if term <= epsilon
end

# Проверка на рациональность и вывод результата
if is_rational(sum)
  numerator, denominator = to_fraction(sum)
  puts "#{numerator}/#{denominator}"
else
  puts "irrational"
end
