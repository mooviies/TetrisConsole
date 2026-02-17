#pragma once

#include <random>

class Random {
public:
    static int getInteger(const int min, const int max) {
        std::uniform_int_distribution distribution(min, max);
        return distribution(getGenerator());
    }

    Random() = delete;
    ~Random() = delete;

private:
    static std::mt19937 &getGenerator() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return gen;
    }
};
