#ifndef SPILT_KMV_HPP
#define SPILT_KMV_HPP

#include <cstdint>
#include <vector>
#include <stdexcept>

#include "kmv.hpp"


class SplitKMV {
public:
    SplitKMV(size_t k, size_t numSplits)
        : k_(k), numSplits_(numSplits) {
            if(k_ % numSplits_ != 0) {
                throw std::invalid_argument("k must be divisible by numSplits");
            }

            kmvs_.resize(numSplits_, KMV(k_ / numSplits_));
        }

    void update(const std::string& value) {
        uint64_t hashValue = stringHash(value);
        size_t splitIndex = hashValue % numSplits_;
        kmvs_[splitIndex].update(value);
    }

    [[nodiscard]] double estimate() const {
        double sum = 0.0;
        for (const auto& kmv : kmvs_) {
            sum += kmv.get_scaled_min_hash();
        }
        return (numSplits_ * k_ - numSplits_) / sum;
    }

private:
    size_t k_;
    size_t numSplits_;
    std::vector<KMV> kmvs_;

    uint64_t stringHash(const std::string& str) const {
        uint64_t hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + static_cast<uint64_t>(c);
        }
        return hash;
    }
};

#endif // SPILT_KMV_HPP