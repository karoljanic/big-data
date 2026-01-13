#ifndef BRAVERMAN_SAMPLER_HPP
#define BRAVERMAN_SAMPLER_HPP

#include <cstddef>
#include <vector>
#include <random>

template<typename T>
class ReservoirSampler {
public:
    ReservoirSampler(std::mt19937& rng)
        : rng_{rng}, count_{0}, next_moment_{1}, sample_{} {}

    ReservoirSampler(const ReservoirSampler& other) {
        rng_ = other.rng_;
        count_ = other.count_;
        next_moment_ = other.next_moment_;
        sample_ = other.sample_;
    }

    ReservoirSampler& operator=(const ReservoirSampler& other) {
        if(this != &other) {
            rng_ = other.rng_;
            count_ = other.count_;
            next_moment_ = other.next_moment_;
            sample_ = other.sample_;
        }
        return *this;
    }

    ReservoirSampler(ReservoirSampler&& other) {
        rng_ = other.rng_;
        count_ = other.count_;
        next_moment_ = other.next_moment_;
        sample_ = std::move(other.sample_);
    }

    ReservoirSampler& operator=(ReservoirSampler&& other) noexcept {
        if(this != &other) {
            rng_ = other.rng_;
            count_ = other.count_;
            next_moment_ = other.next_moment_;
            sample_ = std::move(other.sample_);
        }
        return *this;
    }

    void tick(const T& item, size_t timestamp) {
        count_++;
        if(count_ == next_moment_) {
            sample_ = item;
            timestamp_ = timestamp;

            std::uniform_real_distribution<double> dist(0.0, 1.0);
            const double u = dist(rng_);
            next_moment_ += static_cast<uint64_t>(u  / (1.0 - u) * static_cast<double>(count_));
        }
    }

    [[nodiscard]] T get_sample() const noexcept {
        return sample_;
    }

    [[nodiscard]] std::pair<T, size_t> get_sample_with_timestamp() const noexcept {
        return {sample_, timestamp_};
    }

    template<typename> friend class BravermanSampler;
    template<typename> friend class BravermanKSampler;

protected:
    size_t count_;
    size_t timestamp_;

private:
    std::mt19937& rng_;
    uint64_t next_moment_;
    T sample_;
};


template<typename T>
class BravermanSampler {
public:
    BravermanSampler(std::mt19937& rng, size_t window_size)
        : timer_{0}, rng_{rng}, active_sampler_{rng}, partial_sampler_{rng}, window_size_{window_size}{}

    void tick(const T& item) {
        if(partial_sampler_.count_ == window_size_) {
            active_sampler_ = partial_sampler_;
            partial_sampler_ = ReservoirSampler<T>(rng_);
        }

        partial_sampler_.tick(item, timer_);
        timer_++;
    }

    [[nodiscard]] T get_sample() const noexcept {
        auto result = active_sampler_.get_sample_with_timestamp();
        T sample = result.first;
        size_t timestamp = result.second;

        if(timestamp + window_size_ >= timer_) {
            return sample;
        } else {
            return partial_sampler_.get_sample();
        }
    }

    [[nodiscard]] std::pair<T, size_t> get_sample_with_timestamp() const noexcept {
        if(active_sampler_.count_ >= 0) {
            auto result = active_sampler_.get_sample_with_timestamp();
            T sample = result.first;
            size_t timestamp = result.second;

            if(timestamp + window_size_ >= timer_) {
                return {sample, timestamp};
            } else {
                return partial_sampler_.get_sample_with_timestamp();
            }
        }
        return partial_sampler_.get_sample_with_timestamp();
    }

private:
    std::mt19937& rng_;
    size_t timer_;
    ReservoirSampler<T> active_sampler_;
    ReservoirSampler<T> partial_sampler_;
    size_t window_size_;
};


template<typename T>
class BravermanKSampler {
public:
    BravermanKSampler(std::mt19937& rng, size_t window_size, size_t k)
        : samplers_(k, BravermanSampler<T>(rng, window_size)) { }

    void tick(const T& item) {
        for(auto& sampler : samplers_) {
            sampler.tick(item);
        }
    }

    [[nodiscard]] std::vector<T> get_samples() const noexcept {
        std::vector<T> samples;
        for(const auto& sampler : samplers_) {
            samples.push_back(sampler.get_sample());
        }
        return samples;
    }

private:
    std::vector<BravermanSampler<T>> samplers_;
};


#endif // BRAVERMAN_SAMPLER_HPP
