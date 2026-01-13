#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <string>

#include "loglog.hpp"
#include "hyperloglog.hpp"


void shuffle(std::vector<size_t>& arr) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(arr.begin(), arr.end(), gen);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <N>\n";
        return 1;
    }

    const size_t N = std::stoul(argv[1]);
    const size_t elementsNumber = N * N;

    std::vector<size_t> elements(elementsNumber);
    for (size_t i = 0; i < elementsNumber; i++) {
        elements[i] = i;
    }

    shuffle(elements);

    std::cout << "n,b,loglog_estimate,hyperloglog_estimate\n";

    for (size_t b = 4; b <= 16; b+=2) {
        size_t elementsCounter = 0;

        for (size_t n = 1; n <= N; n++) {
            LogLog ll(b);
            HyperLogLog hll(b);

            for (size_t i = 0; i < n; i++) {
                ll.update(elements[elementsCounter]);
                hll.update(elements[elementsCounter]);
                elementsCounter++;
            }

            double llEstimate = ll.estimate();
            double hllEstimate = hll.estimate();

            std::cout << n << "," << b << "," << llEstimate << "," << hllEstimate << "\n";
        }
    }

    return 0;
}