#pragma once

#include <random>

class Random {
public:
    static void init() {
        _random = new std::random_device();
        _generator = new std::mt19937(_random->operator()());
    }

    static void cleanup() {
        delete _random;
        delete _generator;
    }

    static int getInteger(const int min, const int max) {
        std::uniform_int_distribution distribution(min, max);
        return distribution(*_generator);
    }

    Random() = delete;
    ~Random() = delete;

private:
    static std::random_device *_random;
    static std::mt19937 *_generator;
};
