#pragma once

#include <string>
#include <format>

/**
 * @namespace DebugColors
 * @brief Contains ANSI escape codes for colored and styled console output.
 *
 * These constants can be used to add color and text styles
 * (like bold, dim, underline, etc.) to terminal output.
 */
namespace DebugColors {

    // =========================================================================
    // === Basic ANSI codes ===
    // =========================================================================

    /** @brief Reset all styles and colors. */
    constexpr const char* Reset     = "\033[0m";

    /** @brief Make text bold (bright and thick). */
    constexpr const char* Bold      = "\033[1m";

    /** @brief Make text dim (reduced brightness). */
    constexpr const char* Dim       = "\033[2m";

    /** @brief Italicize text (not supported in all terminals). */
    constexpr const char* Italic    = "\033[3m";

    /** @brief Underline text. */
    constexpr const char* Underline = "\033[4m";


    // =========================================================================
    // === Standard text colors ===
    // =========================================================================

    /** @brief Black text color. */
    constexpr const char* Black     = "\033[30m";

    /** @brief Red text color. */
    constexpr const char* Red       = "\033[31m";

    /** @brief Green text color. */
    constexpr const char* Green     = "\033[32m";

    /** @brief Yellow text color. */
    constexpr const char* Yellow    = "\033[33m";

    /** @brief Blue text color. */
    constexpr const char* Blue      = "\033[34m";

    /** @brief Magenta (purple) text color. */
    constexpr const char* Magenta   = "\033[35m";

    /** @brief Cyan (light blue) text color. */
    constexpr const char* Cyan      = "\033[36m";

    /** @brief White text color. */
    constexpr const char* White     = "\033[37m";


    // =========================================================================
    // === Bright text colors ===
    // =========================================================================

    /** @brief Bright black (gray) text color. */
    constexpr const char* BrightBlack   = "\033[90m";

    /** @brief Bright red text color. */
    constexpr const char* BrightRed     = "\033[91m";

    /** @brief Bright green text color. */
    constexpr const char* BrightGreen   = "\033[92m";

    /** @brief Bright yellow text color. */
    constexpr const char* BrightYellow  = "\033[93m";

    /** @brief Bright blue text color. */
    constexpr const char* BrightBlue    = "\033[94m";

    /** @brief Bright magenta text color. */
    constexpr const char* BrightMagenta = "\033[95m";

    /** @brief Bright cyan text color. */
    constexpr const char* BrightCyan    = "\033[96m";

    /** @brief Bright white text color. */
    constexpr const char* BrightWhite   = "\033[97m";


    // =========================================================================
    // === Background colors ===
    // =========================================================================

    /** @brief Red background color. */
    constexpr const char* BgRed     = "\033[41m";

    /** @brief Green background color. */
    constexpr const char* BgGreen   = "\033[42m";

    /** @brief Yellow background color. */
    constexpr const char* BgYellow  = "\033[43m";

    /** @brief Blue background color. */
    constexpr const char* BgBlue    = "\033[44m";

    /** @brief Magenta background color. */
    constexpr const char* BgMagenta = "\033[45m";

    /** @brief Cyan background color. */
    constexpr const char* BgCyan    = "\033[46m";

    /** @brief White background color. */
    constexpr const char* BgWhite   = "\033[47m";
}