#pragma once

#include <cstdint>
#include <random>
#include <type_traits>

template<typename T>
class SecureValue {
    static_assert(std::is_integral_v<T>);
    using U = std::make_unsigned_t<T>;

    U _stored{};
    U _nonce{};

    static U randomNonce() {
        static thread_local std::mt19937_64 rng{std::random_device{}()};
        return static_cast<U>(rng());
    }

    void store(T val) {
        _nonce = randomNonce();
        _stored = static_cast<U>(val) ^ _nonce;
    }

    [[nodiscard]] T load() const {
        return static_cast<T>(_stored ^ _nonce);
    }

public:
    SecureValue() { store(T{}); }
    SecureValue(T val) { store(val); }  // NOLINT — intentionally implicit
    SecureValue(const SecureValue& o) { store(o.load()); }

    SecureValue& operator=(const SecureValue& o) {
        if (this != &o) store(o.load());
        return *this;
    }

    SecureValue& operator=(T val) { store(val); return *this; }

    operator T() const { return load(); }  // NOLINT — intentionally implicit

    SecureValue& operator+=(T val) { store(static_cast<T>(load() + val)); return *this; }
    SecureValue& operator-=(T val) { store(static_cast<T>(load() - val)); return *this; }
    SecureValue& operator++()     { store(static_cast<T>(load() + T{1})); return *this; }
    SecureValue  operator++(int)  { SecureValue tmp(*this); ++(*this); return tmp; }
};
