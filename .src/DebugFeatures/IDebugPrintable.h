#pragma once

#include <string>

/**
 * @brief Interface for classes that can provide debug information as a string.
 *
 * Classes implementing this interface should define how their state
 * can be represented in both short (single-line) and detailed (formatted)
 * string forms, primarily for debugging and logging purposes.
 */
class IDebugPrintable {
public:
    /**
     * @brief Virtual destructor for safe polymorphic cleanup.
     */
    virtual ~IDebugPrintable() = default;

    /**
     * @brief Returns a short, one-line summary for logs.
     * @return A compact string representation of the object.
     */
    virtual std::string ToShortString() const = 0;
 
    /**
     * @brief Returns a detailed, formatted string representation.
     * @return A human-readable, multi-line string with detailed info.
     */
    virtual std::string ToPrettyString() const = 0; 
};