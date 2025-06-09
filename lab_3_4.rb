def main
  # Чтение входных данных
  n, m = gets.split.map(&:to_i)
  nums = gets.split.map(&:to_i)

  # Проверка ограничений
  raise "Invalid n" unless (5..50000).cover?(n)
  raise "Invalid m" unless (4..100).cover?(m)
  raise "Invalid nums size" unless nums.size == n

  # Вычисление префиксных сумм
  prefix = Array.new(n + 1, 0)
  (0...n).each { |i| prefix[i+1] = prefix[i] + nums[i] }

  # Инициализация массива dp
  dp = Array.new(n + 1, -Float::INFINITY)
  dp[n] = 0 # Базовый случай

  # Заполнение массива dp
  (n-1).downto(0) do |i|
    max_k = [m, n - i].min
    (1..max_k).each do |k|
      current_sum = prefix[i + k] - prefix[i]
      dp[i] = [dp[i], current_sum - dp[i + k]].max
    end
  end

  # Определение победителя
  puts dp[0] > 0 ? 1 : 0
end

main
