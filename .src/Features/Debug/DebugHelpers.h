#pragma once

#include <string>
#include <sstream>
#include <format>
#include <unordered_map>

#include "DebugColors.h"

using namespace DebugColors;

namespace DebugHelpers
{

    /**
     * @brief Converts an array of three elements into a formatted string.
     * @tparam T The array element type.
     * @param arr The array to convert.
     * @return A string in the form "[x, y, z]".
     */
    template <typename T>
    std::string Arr3ToStr(const T &arr)
    {
        return std::format("[{}, {}, {}]", arr[0], arr[1], arr[2]);
    }

    /**
     * @brief Converts an array of four elements into a formatted string.
     * @tparam T The array element type.
     * @param arr The array to convert.
     * @return A string in the form "[x, y, z, w]".
     */
    template <typename T>
    std::string Arr4ToStr(const T &arr)
    {
        return std::format("[{}, {}, {}, {}]", arr[0], arr[1], arr[2], arr[3]);
    }

    /**
     * @brief Converts the contents of a map or unordered_map into a formatted string.
     * @tparam Map The map type.
     * @param m The map to stringify.
     * @param indent Indentation used before each entry.
     * @return A formatted multiline string of key-value pairs.
     */
    template <typename Map>
    std::string MapToStr(const Map &m, const std::string &indent = "    ")
    {
        std::ostringstream ss;
        if (m.empty())
        {
            ss << indent << "(none)\n";
        }
        else
        {
            for (const auto &[k, v] : m)
                ss << indent << k << " = " << v << "\n";
        }
        return ss.str();
    }

    /**
     * @brief Adds indentation to every line of the input text.
     * @param text The text to indent.
     * @param indent The indentation string (default: four spaces).
     * @return The indented text.
     */
    inline std::string IndentLines(const std::string &text, const std::string &indent = "    ")
    {
        std::ostringstream out;
        std::istringstream in(text);
        std::string line;
        while (std::getline(in, line))
            out << indent << line << '\n';
        return out.str();
    }

    // === Utility functions ===

    /**
     * @brief Applies a color (and optional bold style) to the given text.
     * @param text The text to colorize.
     * @param color The ANSI color code.
     * @param bold Whether to apply bold style.
     * @return The colorized string.
     */
    inline std::string Colorize(const std::string &text, const char *color, bool bold = false)
    {
        return std::format("{}{}{}{}", bold ? Bold : "", color, text, Reset);
    }

    /**
     * @brief Converts a boolean value to a colored string ("true" in green, "false" in red).
     * @param value The boolean value.
     * @return The colorized string.
     */
    inline std::string BoolToColored(bool value)
    {
        return value
                   ? std::format("{}true{}", BrightGreen, Reset)
                   : std::format("{}false{}", BrightRed, Reset);
    }

    /**
     * @brief Formats a text as a bright cyan bold title.
     * @param text The title text.
     * @return The formatted string.
     */
    inline std::string Title(const std::string &text)
    {
        return std::format("{}{}{}{}", BrightCyan, Bold, text, Reset);
    }

    /**
     * @brief Formats a text as a bold magenta section header.
     * @param text The section title.
     * @return The formatted string.
     */
    inline std::string Section(const std::string& text)
    {
        return std::format("{}{}{}{}", DebugColors::Magenta, DebugColors::Bold, text, DebugColors::Reset);
    }

    /**
     * @brief Formats braces or structural symbols in dim style.
     * @param text The text to format.
     * @return The formatted string.
     */
    inline std::string Brace(const std::string &text)
    {
        return std::format("{}{}{}", Dim, text, Reset);
    }

    /**
     * @brief Formats a map key in yellow.
     * @param text The key text.
     * @return The formatted string.
     */
    inline std::string Key(const std::string &text)
    {
        return std::format("{}{}{}", Yellow, text, Reset);
    }

    /**
     * @brief Formats a map value with a custom color.
     * @param text The value text.
     * @param color The ANSI color code (default: white).
     * @return The formatted string.
     */
    inline std::string Value(const std::string &text, const char *color = White)
    { 
        return std::format("{}{}{}", color, text, Reset); 
    } 

} // namespace DebugHelpers