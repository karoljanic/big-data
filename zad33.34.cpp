#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <numeric>

#include "kmv.hpp"
#include "split_kmv.hpp"

int main() {
    const size_t k = 400;
    const size_t true_n_unique = 10000;
    const size_t n_trials = 100;
    const size_t num_splits = 5;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<double> kmv_estimations;
    std::vector<double> kmv_split_estimations;

    std::vector<double> kmv_errors;
    std::vector<double> kmv_split_errors;

    for (size_t trial = 0; trial < n_trials; ++trial) {
        KMV kmv(k);
        SplitKMV kmv_split(k, num_splits);

        std::vector<size_t> stream;
        for (size_t rep = 0; rep < 3; ++rep) {
            for (size_t i = 0; i < true_n_unique; ++i) {
                stream.push_back(i);
            }
        }
        std::shuffle(stream.begin(), stream.end(), gen);

        for (size_t item : stream) {
            kmv.update(std::to_string(item));
            kmv_split.update(std::to_string(item));
        }

        double kmv_rel_error = (std::abs(kmv.estimate() - true_n_unique) / true_n_unique) * 100.0;
        double kmv_split_rel_error = (std::abs(kmv_split.estimate() - true_n_unique) / true_n_unique) * 100.0;

        kmv_estimations.push_back(kmv.estimate());
        kmv_split_estimations.push_back(kmv_split.estimate());

        kmv_errors.push_back(kmv_rel_error);
        kmv_split_errors.push_back(kmv_split_rel_error);
    }

    double kmv_mean_estimation = std::accumulate(kmv_estimations.begin(), kmv_estimations.end(), 0.0) / n_trials;
    double kmv_split_mean_estimation = std::accumulate(kmv_split_estimations.begin(), kmv_split_estimations.end(), 0.0) / n_trials;

    double kmv_mean_error = std::accumulate(kmv_errors.begin(), kmv_errors.end(), 0.0) / n_trials;
    double kmv_split_mean_error = std::accumulate(kmv_split_errors.begin(), kmv_split_errors.end(), 0.0) / n_trials;

    double kmv_stddev = 0.0;
    for (double est: kmv_estimations) {
        kmv_stddev += (est - kmv_mean_estimation) * (est - kmv_mean_estimation);
    }
    kmv_stddev = std::sqrt(kmv_stddev / n_trials);

    double kmv_split_stddev = 0.0;
    for (double est: kmv_split_estimations) {
        kmv_split_stddev += (est - kmv_split_mean_estimation) * (est - kmv_split_mean_estimation);
    }
    kmv_split_stddev = std::sqrt(kmv_split_stddev / n_trials);


    std::cout << "== KMV ==\n";
    std::cout << "k = " << k << "\n";
    std::cout << "True Cardinality = " << true_n_unique << "\n";
    std::cout << "Mean Estimated Cardinality: " << kmv_mean_estimation << "\n";
    std::cout << "Mean Relative Error: " << kmv_mean_error << "%\n";
    std::cout << "Standard Deviation of Mean: " << kmv_stddev << "\n\n";

    std::cout << "== SplitKMV ==\n";
    std::cout << "k = " << k << ", num_splits = " << num_splits << "\n";
    std::cout << "True Cardinality = " << true_n_unique << "\n";
    std::cout << "Mean Estimated Cardinality: " << kmv_split_mean_estimation << "\n";
    std::cout << "Mean Relative Error: " << kmv_split_mean_error << "%\n";
    std::cout << "Standard Deviation of Mean: " << kmv_split_stddev << "\n\n";

    return 0;
}
