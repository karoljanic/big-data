#include "braverman_sampler.hpp"

#include <iostream>
#include <random>
#include <vector>
#include <iomanip>

int main(int argc, char* argv[]) {
    if(argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <window_size> <N>\n";
        return 1;
    }

    const size_t window_size = std::stoul(argv[1]);
    const size_t N = std::stoul(argv[2]);

    std::mt19937 rng{std::random_device{}()};
    BravermanSampler<size_t> sampler{rng, window_size};

    for(size_t i = 0; i < N; ++i) {
        sampler.tick(i);
        
        auto [sample_value, sample_timestamp] = sampler.get_sample_with_timestamp();
        size_t local_index = i % window_size;
        
        std::cout << i << " " << local_index << " " << sample_timestamp << "\n";
    }

    return 0;
}