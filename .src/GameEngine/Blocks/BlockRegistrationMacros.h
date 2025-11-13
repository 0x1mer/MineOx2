#pragma once

#include "BlockFactory.h"
#include <type_traits>

/**
 * @brief Helper struct to automatically register static or dynamic blocks.
 */
template <typename T>
struct BlockRegistrarHelper {
    static void Register(BlockType type) {
        BlockFactory::RegisterDynamic<T>(type);
    }
};

// Специализация для статических блоков
template <typename T>
requires std::is_base_of_v<IStaticBlock, T>
struct BlockRegistrarHelper<T> {
    static void Register(BlockType type) {
        BlockFactory::RegisterStatic<T>(type);
    }
};

/**
 * @brief Macro for automatic block registration.
 *
 * Registers block in factory at static initialization stage.
 */
#define REGISTER_BLOCK(BlockClass, TypeEnum) \
namespace { \
    struct BlockClass##Registrar { \
        BlockClass##Registrar() { \
            BlockRegistrarHelper<BlockClass>::Register(TypeEnum); \
        } \
    }; \
    static BlockClass##Registrar global_##BlockClass##_registrar; \
}
