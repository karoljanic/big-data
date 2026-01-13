#ifndef KMV_HPP
#define KMV_HPP

#include <cstdint>
#include <set>

#include "k_independent_hash.hpp"


class KMV {
public:
    KMV(std::size_t k) : k_(k), hashFunction_(2) {}

    void update(const std::string& value) {
        uint64_t hashValue = hashFunction_(stringHash(value));
        if (minHashes_.size() < k_) {
            minHashes_.insert(hashValue);
        }
        else {
            uint64_t maxMinHash = *minHashes_.rbegin();
            if (hashValue < maxMinHash && minHashes_.find(hashValue) == minHashes_.end()) {
                minHashes_.erase(maxMinHash);
                minHashes_.insert(hashValue);
            }
        }
    }

    [[nodiscard]] uint64_t getKthMin() const {
        if(minHashes_.empty()) {
            return hashFunction_.get_prime();
        }
        return *minHashes_.rbegin();
    }

    [[nodiscard]] double estimate() const {
        if (minHashes_.size() < k_) {
            return static_cast<double>(minHashes_.size());
        }

        return static_cast<double>(k_ - 1) / get_scaled_min_hash();
    }

    [[nodiscard]] double get_scaled_min_hash() const {
        uint64_t kthMinHash = *minHashes_.rbegin();
        return static_cast<double>(kthMinHash) / static_cast<double>(hashFunction_.get_prime());
    }

private:
    std::size_t k_;
    KIndependentHash hashFunction_;
    std::set<uint64_t> minHashes_;

    uint64_t stringHash(const std::string& str) const {
        uint64_t hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + static_cast<uint64_t>(c);
        }
        return hash;
    }
};

#endif // KMV_HPP