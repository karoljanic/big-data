#ifndef HYPERLOGLOG_HPP
#define HYPERLOGLOG_HPP

#include <cstdint>
#include <cmath>
#include <vector>
#include <algorithm>
#include <stdexcept>


class HyperLogLog {
public:
    explicit HyperLogLog(std::size_t b) : b_(b), m_(1ULL << b) {
        if (b < 4 || b > 16) {
            throw std::invalid_argument("b must be in the range [4, 16]");
        }

        alpha_ = computeAlpha(m_);
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
        double rawEstimate = estimateRaw();

        // small range correction
        if (rawEstimate <= 2.5 * m_) {
            auto zeroCount = static_cast<double>(
                std::count(registers_.begin(), registers_.end(), 0)
            );

            if (zeroCount == 0) {
                return rawEstimate;
            }

            return m_ * std::log(m_ / zeroCount);
        }

        // large range correction
        constexpr double twoPow32 = 4294967296.0;
        if (rawEstimate > twoPow32 / 30.0) {
            return -twoPow32 * std::log(1.0 - rawEstimate / twoPow32);
        }

        return rawEstimate;
    }

    [[nodiscard]] std::size_t getNumRegisters() const { return m_; }
    [[nodiscard]] std::size_t getPrecision() const { return b_; }

private:
    std::size_t b_;
    std::size_t m_;
    double alpha_;
    std::vector<std::size_t> registers_;

    [[nodiscard]] static double computeAlpha(std::size_t m) {
        switch (m) {
            case 16:  return 0.673;
            case 32:  return 0.697;
            case 64:  return 0.709;
            default:  return 0.7213 / (1.0 + 1.079 / m);
        }
    }

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

    [[nodiscard]] double estimateRaw() const {
        double sum = 0.0;
        for (std::size_t reg : registers_) {
            sum += std::pow(2.0, -static_cast<double>(reg));
        }

        return alpha_ * m_ * m_ / sum;
    }
};

#endif // HYPERLOGLOG_HPP