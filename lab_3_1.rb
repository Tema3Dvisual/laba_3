def function(x)
  if (-8 <= x) && (x < -5)
    -3
  elsif (-5 <= x) && (x < -3)
    (3 * x + 9) / 2.0
  elsif (-3 <= x) && (x < 3)
    Math.sqrt(9 - x**2)
  elsif (3 <= x) && (x < 8)
    (3 * x - 9) / 5.0
  elsif (8 <= x) && (x <= 10)
    3
  else
    -1
  end
end

# Начальные параметры
x_start = -8.0   # Начальное значение
x_end = 10.0     # Конечное значение
dx = 0.5         # Шаг

# Вывод заголовка
puts " x\t y"
puts "-----------"

# Цикл вычисления и вывода значений
(x_start..x_end).step(dx) do |x|
  y = function(x)
  puts "#{x.round(1)}\t#{y.round(4)}"
end
