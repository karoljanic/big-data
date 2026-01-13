#ifndef LOGLOG_HPP
#define LOGLOG_HPP

#include <cstdint>
#include <cmath>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <numeric>


class LogLog {
public:
    explicit LogLog(std::size_t b) : b_(b), m_(1ULL << b) {
        if (b < 4 || b > 16) {
            throw std::invalid_argument("b must be in the range [4, 16]");
        }

        alpha_ = 0.39701;
        
        registers_.resize(m_, 0);
    }

    void update(std::size_t value) {
        std::uint32_t hash = fnv32Hash(value);
        std::size_t registerIndex = hash & (m_ - 1);
        std::size_t w = hash >> b_;

        std::size_t rho = countLeadingZeros(w) - b_ + 1;
        registers_[registerIndex] = std::max(registers_[registerIndex], rho);
    }

    [[nodiscard]] double estimate() const {
        double sum = std::accumulate(registers_.begin(), registers_.end(), 0.0);
        double arithmeticMean = sum / static_cast<double>(m_);

        return alpha_ * m_ * std::pow(2.0, arithmeticMean);
    }

    [[nodiscard]] std::size_t getNumRegisters() const { return m_; }
    [[nodiscard]] std::size_t getPrecision() const { return b_; }

private:
    std::size_t b_;
    std::size_t m_;
    double alpha_;
    std::vector<std::size_t> registers_;

    [[nodiscard]] static std::uint32_t fnv32Hash(std::size_t input) {
        constexpr std::uint32_t FNV_32_PRIME = 16777619;
        std::uint32_t h = 2166136261;

        while (input) {
            h ^= static_cast<std::uint8_t>(input & 0xFF);
            h *= FNV_32_PRIME;
            input >>= 8;
        }

        return h;
    }

    [[nodiscard]] static std::size_t countLeadingZeros(std::size_t value) {
        if (value == 0) {
            return sizeof(std::uint32_t) * 8;
        }
        return static_cast<std::size_t>(__builtin_clz(static_cast<std::uint32_t>(value)));
    }
};

#endif // LOGLOG_HPP