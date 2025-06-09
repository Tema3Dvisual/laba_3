class CoffeeCooling
  attr_accessor :temp, :env_t, :cool_c, :time_limit, :aprox_vector

  def initialize
    @aprox_vector = []
  end

  def korrel(temperatures, mean_y)
    sum_xy = 0.0
    sum_x2 = 0.0
    sum_y2 = 0.0
    mean_x = (time_limit - 1) / 2.0

    temperatures.each_with_index do |y, i|
      x_diff = i - mean_x
      y_diff = y - mean_y
      sum_xy += x_diff * y_diff
      sum_x2 += x_diff * x_diff
      sum_y2 += y_diff * y_diff
    end

    sum_xy / Math.sqrt(sum_x2 * sum_y2)
  end

  def aprox(x_vector, y_vector)
    sum_x = 0.0
    sum_y = 0.0
    sum_xy = 0.0
    sum_x2 = 0.0
    n = x_vector.size

    x_vector.each_with_index do |x, i|
      y = y_vector[i]
      sum_x += x
      sum_y += y
      sum_xy += x * y
      sum_x2 += x * x
    end

    a = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x)
    b = (sum_y - a * sum_x) / n

    [a, b]
  end

  def calculate_coffee_cooling
    temperatures = []
    times = []
    temp_corr = []

    (0..time_limit).each do |t|
      temperature = env_t + (temp - env_t) * Math.exp(-cool_c * t)
      temperatures << temperature
      times << t
    end

    a, b = aprox(times, temperatures)
    @aprox_vector = [a, b]

    mean_y = temperatures.sum / temperatures.size
    corr = korrel(temperatures, mean_y)

    temperatures.each_with_index do |temp, t|
      temp_corr << [temp, corr]
    end

    temp_corr
  end

  def print_results(results)
    puts "Coefficients of the approximating line a - #{aprox_vector[0]}, b - #{aprox_vector[1]}"
    puts "|------|------------|---------------|"
    puts "| Time | Temperature|CorrelationCoef|"
    puts "|------|------------|---------------|"

    results.each_with_index do |(temp, corr), t|
      puts "|#{t.to_s.rjust(6)}|#{temp.round(6).to_s.rjust(12)}|#{corr.round(6).to_s.rjust(15)}|"
      puts "|------|------------|---------------|"
    end
  end

  def run
    print "Enter start coffee temperature(Celsius): "
    @temp = gets.to_f
    print "Enter temperature of environment(Celsius): "
    @env_t = gets.to_f
    print "Enter cooling coefficient(dC): "
    @cool_c = gets.to_f
    print "Enter time of full cool(Seconds): "
    @time_limit = gets.to_i

    results = calculate_coffee_cooling
    print_results(results)
  end
end

CoffeeCooling.new.run
