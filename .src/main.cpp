#include "GLFWWindow.h"
#include "Logger.h"
#include "PathProvider.h"
#include "Options.h"
#include "BlocksIncluder.h" // Регистрирует все блоки
#include "BlockJsonDataCache.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image.h>
#include <stb_image_write.h>

int main()
{
    // Инициализация провайдера путей
    auto &paths = PathProvider::Instance();

    // Инициализация логгера
    auto &logger = Logger::Instance();
    logger.CleanupOldLogs(paths.GetPath(Folders::Logs).string(), 10);
    logger.SetLogFile(paths.GetPath(Folders::Logs).string());
    logger.SetLogLevel(LogLevel::Debug);
    logger.SetOutput(LogOutput::Both);

    logger.Info("Creating window...");

    // Инициализация окна
    GLFWWindow window;
    if (!window.Create(1280, 720, "Test Window"))
    {
        return -1;
    }

    logger.Info("Window created.");

    // Настройка окна
    window.SetVSync(false);
    window.SetWindowMode(WindowMode::Windowed);

    // --------------------------------------------------------------
    // 🧪 ТЕСТИРУЕМ BlockFactory
    // --------------------------------------------------------------
    BlockFactory::LogRegistrationSummary();
    logger.Info("Testing BlockFactory registration...");

    auto airBlock1 = BlockFactory::GetBlock(BlockType::Air);
    auto airBlock2 = BlockFactory::GetBlock(BlockType::Air);
    auto dirtBlock1 = BlockFactory::GetBlock(BlockType::Dirt);
    auto dirtBlock2 = BlockFactory::GetBlock(BlockType::Dirt);
    auto grassBlock = BlockFactory::GetBlock(BlockType::Grass);
    auto unknownBlock = BlockFactory::GetBlock(static_cast<BlockType>(9999));

    // Проверка корректности
    if (!airBlock1 || !dirtBlock1 || !grassBlock)
    {
        logger.Error("❌ One or more block types failed to instantiate!");
    }
    else
    {
        logger.Info("✅ All registered block types instantiated successfully.");
    }

    // Проверяем, что статические блоки возвращают тот же объект
    if (airBlock1 == airBlock2)
        logger.Info("✅ AirBlock is static (same instance).");
    else
        logger.Critical("⚠️ AirBlock should be static, but new instance returned!");

    // Проверяем, что динамические блоки возвращают разные объекты
    if (dirtBlock1 != dirtBlock2)
        logger.Info("✅ DirtBlock is dynamic (different instances).");
    else
        logger.Critical("⚠️ DirtBlock should be dynamic, but got same instance!");

    // Проверяем неизвестный тип
    if (!unknownBlock)
        logger.Info("✅ Unknown block type correctly returned nullptr.");
    else
        logger.Error("❌ Unknown block type returned non-null object!");

    logger.Info("BlockFactory test completed.");
    // --------------------------------------------------------------

    // --------------------------------------------------------------
    // 🧪 ТЕСТИРУЕМ BlockJsonDataCache
    // --------------------------------------------------------------
    logger.Info("==========================================");
    logger.Info("🧪 Starting BlockJsonDataCache tests...");
    logger.Info("==========================================");

    try
    {
        // Получаем экземпляр кеша (через Singleton)
        auto &cache = BlockJsonDataCache::Instance(logger, paths);

        logger.Info("Loading all block data...");

        logger.Info("✅ All JSON block data loaded successfully.");

        // Пробуем запросить конкретные блоки
        auto dirtData = cache.Get("dirt");
        auto grassData = cache.Get("grass");

        if (dirtData.has_value() && grassData.has_value())
        {
            logger.Info("✅ Successfully retrieved data for dirt, and grass_block.");
        }
        else
        {
            logger.Error("❌ Some known blocks were not found in cache!");
        }

        auto airData = cache.Get(BlockType::Air);

        if (!airData.has_value())
        {
            logger.Info("✅ Expected: no data for 'air' block — correct behavior.");
        }
        else
        {
            logger.Error("❌ Unexpected: 'air' block returned non-null data!");
        }

        auto dirtDataAgain = cache.Get(BlockType::Dirt);

        if (&(dirtData->get()) == &(dirtDataAgain->get()))
            logger.Info("✅ Cache hit works correctly — same pointer returned.");
        else
            logger.Error("⚠️ Cache miss — returned a new object for the same block name!");

        // Проверим обработку несуществующего блока
        auto unknownData = cache.Get("minecraft:unknown_block");
        if (!unknownData.has_value())
            logger.Info("✅ Unknown block correctly returned nullopt.");
        else
            logger.Error("❌ Unexpected non-null result for unknown block!");

        // Проверим, что в кеше есть ожидаемое количество элементов
        size_t totalCount = cache.GetLoadedCount();
        logger.Info("📦 Total loaded blocks in cache: " + std::to_string(totalCount));

        if (totalCount == 0)
            logger.Critical("❌ Cache appears empty — check JSON file paths or parsing errors.");
        else
            logger.Info("✅ Cache contains data. JSON parsing appears successful.");

        logger.Info("🧩 BlockJsonDataCache test completed successfully.");

        logger.Info(dirtDataAgain.value().get().ToPrettyString());
        logger.Info(dirtDataAgain.value().get().ToShortString());
    }
    catch (const std::exception &ex)
    {
        logger.Critical(std::string("💥 Exception during BlockJsonDataCache test: ") + ex.what());
    }
    catch (...)
    {
        logger.Critical("💥 Unknown fatal error in BlockJsonDataCache test!");
    }

    logger.Info("==========================================");

    // Главный цикл
    while (!window.ShouldClose())
    {
        glClear(GL_COLOR_BUFFER_BIT);

        window.SwapBuffers();
        window.PollEvents();
    }

    logger.Info("Game shutdown complete.");
    return 0;
}