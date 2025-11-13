#pragma once

/**
 * @brief Base struct for all objects that can be loaded from JSON.
 *
 * Provides a common flag @c wasLoaded indicating whether
 * the object was successfully parsed and initialized.
 *
 * @note This struct is intended to be inherited by other
 *       JSON-representable data types (e.g. BlockState, BlockModel, etc.).
 */
struct JsonLoadable {
    /** @brief Indicates whether the object was successfully loaded from JSON. */
    bool wasLoaded = false;
};