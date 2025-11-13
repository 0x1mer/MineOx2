#pragma once

#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

#include "IOptions.h"
#include "Logger.h"

using nlohmann::json;

/**
 * @brief Generic JSON-based configuration loader.
 *
 * This template class provides a simple and reusable mechanism
 * for loading configuration data from JSON files into strongly typed
 * structures.
 *
 * @tparam T Configuration structure type that:
 *  - supports deserialization via `from_json(const json&, T&)`
 *  - may optionally define a global `Validate(const T&)` function
 *
 */
template <typename T>
class Options : public IOptions<T> {
private:
    T config_;

    /**
     * @brief Loads and deserializes the configuration from a JSON file.
     *
     * @param filename Path to the JSON configuration file.
     * @return A fully constructed configuration object of type T.
     *
     * @throws nlohmann::json::parse_error If JSON parsing fails.
     * @throws std::invalid_argument If validation fails (when Validate() exists).
     */
    static T LoadFromJson(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            Logger::Instance().Critical("Cannot open config file: " + filename);
        }

        json j;
        file >> j;
        T config = j.get<T>();

        // If a global Validate() function exists for type T — call it
        if constexpr (requires(const T& c) { Validate(c); }) {
            Validate(config);
        }

        return config;
    }

public:
    /**
     * @brief Constructs and loads configuration from the specified file.
     *
     * @param filename Path to the JSON file to load.
     */
    explicit Options(const std::string& filename)
        : config_(LoadFromJson(filename)) {}

    /**
     * @brief Returns a read-only reference to the configuration object.
     *
     * Useful when you need to pass the configuration to another function.
     */
    const T& Value() const override { return config_; }

    /**
     * @brief Provides convenient pointer-style access to configuration members.
     *
     * Example:
     * @code
     * Options<Settings> opts("config.json");
     * std::cout << opts->logLevel;
     * @endcode
     */
    const T* operator->() const override { return &config_; }

    /**
     * @brief Reloads configuration from the original JSON file.
     *
     * Replaces the current configuration with a newly loaded one.
     * 
     * @throws nlohmann::json::parse_error If JSON parsing fails.
     * @throws std::invalid_argument If validation fails (when Validate() exists).
     *
     */
    void Reload(const std::string& filename) override {
        config_ = LoadFromJson(filename);
    }
};