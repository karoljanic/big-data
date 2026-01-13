#include <iostream>
#include <vector>
#include <numeric>
#include <iomanip>
#include <cmath>

struct Result { double mean; double variance; };

Result two_pass(const std::vector<double>& x) {
    if (x.empty()) {
        return {0.0, 0.0};
    }

    const double sum = std::accumulate(x.begin(), x.end(), 0.0);
    const double mean = sum / static_cast<double>(x.size());

    double var = 0.0;
    for (double xi : x) {
        const double d = xi - mean;
        var += d * d;
    }
    var /= static_cast<double>(x.size());

    return {mean, var};
}

Result naive_one_pass(const std::vector<double>& x) {
    if (x.empty()) {
        return {0.0, 0.0};
    }

    double sum = 0.0;
    double squaresum = 0.0;
    for (double xi : x) {
        sum += xi;
        squaresum += xi * xi;
    }

    const double mean = sum / static_cast<double>(x.size());
    const double var = squaresum / static_cast<double>(x.size()) - mean * mean;

    return {mean, var};
}

Result welford(const std::vector<double>& x) {
    if (x.empty()) {
        return {0.0, 0.0};
    }

    size_t n = 0;
    double mean = 0.0;
    double scaled_var = 0.0;
    for (double xi : x) {
        ++n;
        const double delta1 = xi - mean;
        mean += delta1 / static_cast<double>(n);
        const double delta2 = xi - mean;
        scaled_var += delta1 * delta2;
    }
    
    const double var = scaled_var / static_cast<double>(n);
    
    return {mean, var};
}

int main() {
    std::vector<std::vector<double>> datasets;

    datasets.push_back({4.0, 7.0, 13.0, 16.0});
    datasets.push_back({1e8 + 4.0, 1e8 + 7.0, 1e8 + 13.0, 1e8 + 16.0});
    datasets.push_back({1e9 + 4.0, 1e9 + 7.0, 1e9 + 13.0, 1e9 + 16.0});

    for (const auto& data : datasets) {
        Result res_two_pass = two_pass(data);
        Result res_naive = naive_one_pass(data);
        Result res_welford = welford(data);

        std::cout << std::fixed << std::setprecision(2);
        std::cout << "\nDataset: ";
        for (double v : data) {
            std::cout << v << " ";
        }
        std::cout << "\n";

        std::cout << "Two-pass: Mean = " << res_two_pass.mean << ", Variance = " << res_two_pass.variance << "\n";
        std::cout << "Naive one-pass: Mean = " << res_naive.mean << ", Variance = " << res_naive.variance << "\n";
        std::cout << "Welford's method: Mean = " << res_welford.mean << ", Variance = " << res_welford.variance << "\n";
    }

    return 0;
}
