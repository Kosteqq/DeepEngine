#pragma once
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE 
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

// TODO Replace Engine logger with assertion/custom logger

#define ASSERT_COND(p_condition)                                    \
    if (!(p_condition))                                             \
        return;                                                     \
    else                                                            \
        ((void)0);

#define ASSERT_COND_MSG(p_condition, ...)                           \
    if (!(p_condition)) {                                           \
        ENGINE_ERR(__VA_ARGS__);                                    \
        return;                                                     \
    } else                                                          \
        ((void)0);

#define ASSERT_COND_V(p_condition, p_return)                        \
    if (!(p_condition))                                             \
        return p_return;                                            \
    else                                                            \
        ((void)0);

#define ASSERT_COND_V_MSG(p_condition, p_return, ...)               \
    if (!(p_condition)) {                                           \
        ENGINE_ERR(__VA_ARGS__);                                    \
        return p_return;                                            \
    } else                                                          \
        ((void)0);

#define ASSERT_COND_BREAK(p_condition)                              \
    if (!(p_condition))                                             \
        break;                                                      \
    else                                                            \
        ((void)0);

#define ASSERT_COND_BREAK_MSG(p_condition, ...)                     \
    if ((p_condition)) {                                            \
        ENGINE_ERR(__VA_ARGS__);                                    \
        break;                                                      \
    } else                                                          \
        ((void)0);

#define ASSERT_COND_CONTINUE(p_condition)                           \
    if ((p_condition))                                              \
        continue;                                                   \
    else                                                            \
        ((void)0);

#define ASSERT_COND_CONTINUE_MSG(p_condition, ...)                  \
    if ((p_condition)) {                                            \
        ENGINE_ERR(__VA_ARGS__);                                    \
        continue;                                                   \
    } else                                                          \
        ((void)0);

#define ASSERT_NULL(p_value)                      ASSERT_COND(             (p_value) != nullptr)
#define ASSERT_NULL_MSG(p_value, ...)             ASSERT_COND_MSG(         (p_value) != nullptr, "Failed null assertion on value " #p_value ". Message: {0}", __VA_ARGS__)
#define ASSERT_NULL_V(p_value, p_return)          ASSERT_COND_V(           (p_value) != nullptr, p_return)
#define ASSERT_NULL_V_MSG(p_value, p_return, ...) ASSERT_COND_V_MSG(       (p_value) != nullptr, p_return, "Failed null assertion on value "  #p_value ". Message: {0}", __VA_ARGS__)
#define ASSERT_NULL_BREAK(p_value)                ASSERT_COND_BREAK(       (p_value) != nullptr)
#define ASSERT_NULL_BREAK_MSG(p_value, ...)       ASSERT_COND_BREAK_MSG(   (p_value) != nullptr, "Failed null assertion on value " #p_value ". Message: {0}", __VA_ARGS__)
#define ASSERT_NULL_CONTINUE(p_value)             ASSERT_COND_CONTINUE(    (p_value) != nullptr)                       
#define ASSERT_NULL_CONTINUE_MSG(p_value, ...)    ASSERT_COND_CONTINUE_MSG((p_value) != nullptr, "Failed null assertion on value " #p_value ". Message: {0}", __VA_ARGS__)

#define LOG_TRACE(logger, ...)  SPDLOG_LOGGER_TRACE(logger->GetLogger(), __VA_ARGS__)
#define LOG_DEBUG(logger, ...)  SPDLOG_LOGGER_DEBUG(logger->GetLogger(), __VA_ARGS__)
#define LOG_INFO(logger, ...)   SPDLOG_LOGGER_INFO(logger->GetLogger(), __VA_ARGS__)
#define LOG_WARN(logger, ...)   SPDLOG_LOGGER_WARN(logger->GetLogger(), __VA_ARGS__)
#define LOG_ERR(logger, ...)    SPDLOG_LOGGER_ERROR(logger->GetLogger(), __VA_ARGS__)

#define ENGINE_TRACE(...)       LOG_TRACE(DeepEngine::Debug::Logger::GetBaseEngineLogger(), __VA_ARGS__)
#define ENGINE_DEBUG(...)       LOG_DEBUG(DeepEngine::Debug::Logger::GetBaseEngineLogger(), __VA_ARGS__)
#define ENGINE_INFO(...)        LOG_INFO(DeepEngine::Debug::Logger::GetBaseEngineLogger(), __VA_ARGS__)
#define ENGINE_WARN(...)        LOG_WARN(DeepEngine::Debug::Logger::GetBaseEngineLogger(), __VA_ARGS__)
#define ENGINE_ERR(...)         LOG_ERR(DeepEngine::Debug::Logger::GetBaseEngineLogger(), __VA_ARGS__)


namespace DeepEngine::Debug
{

    class Logger
    {
    private:
        Logger() = delete;
        Logger(const char* p_name);

    public:
        spdlog::logger* GetLogger()
        {
            return &_logger;
        }
        
    public:
        static void Initialize(const char* p_filepath);
        static void Initialize(const std::string& p_filepath);
        static std::shared_ptr<Logger> CreateLoggerInstance(const char* p_name);
        static std::shared_ptr<Logger> CreateLoggerInstance(const std::string& p_name);
        static std::shared_ptr<Logger> GetBaseEngineLogger();

    private:
        spdlog::logger _logger;
        
    private:
        static std::shared_ptr<Logger> _engineLogger;
        static std::shared_ptr<spdlog::sinks::stdout_color_sink_st> _consoleSink;
        static std::shared_ptr<spdlog::sinks::basic_file_sink_st> _fileSink;
    };
    
}
