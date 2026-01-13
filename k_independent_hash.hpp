#ifndef K_INDEPENDENT_HASH_HPP
#define K_INDEPENDENT_HASH_HPP

#include <cstdint>
#include <random>
#include <vector>
#include <stdexcept>


class KIndependentHash {
public:
    KIndependentHash(std::size_t k) : coefficients_(k) {
        if(k < 1) {
            throw std::invalid_argument("k must be at least 1");
        }

        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint64_t> dist(1, PRIME - 1);
        for (std::size_t i = 0; i < k; ++i) {
            coefficients_[i] = dist(gen);
        }
    }

    static uint64_t get_prime() {
        return PRIME;
    }

    uint64_t operator()(uint64_t x) const {
        uint64_t result = 0;
        uint64_t x_power = 1;

        for (const auto& coeff : coefficients_) {
            result = addmod(result, mulmod(coeff, x_power, PRIME), PRIME);
            x_power = mulmod(x_power, x, PRIME);
        }

        return result;
    }

private:
    static constexpr uint64_t PRIME = (1ULL << 61) - 1;
    std::vector<uint64_t> coefficients_;

    uint64_t addmod(uint64_t a, uint64_t b, uint64_t mod) const {
        return (a % mod + b % mod) % mod;
    }

    uint64_t mulmod(uint64_t a, uint64_t b, uint64_t mod) const {
        uint64_t result = 0;
        a = a % mod;
        while (b > 0) {
            if (b & 1) {
                result = addmod(result, a, mod);
            }
            a = (2 * a) % mod;
            b >>= 1;
        }
        return result;
    }
};

#endif // K_INDEPENDENT_HASH_HPP