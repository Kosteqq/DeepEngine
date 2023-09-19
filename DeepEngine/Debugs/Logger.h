#pragma once
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE 
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace DeepEngine::Core::Debug
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

#define ENGINE_TRACE(...) SPDLOG_LOGGER_TRACE(DeepEngine::Core::Debug::Logger::GetBaseEngineLogger()->GetLogger(), __VA_ARGS__)
#define ENGINE_DEBUG(...) SPDLOG_LOGGER_DEBUG(DeepEngine::Core::Debug::Logger::GetBaseEngineLogger()->GetLogger(), __VA_ARGS__)
#define ENGINE_INFO(...) SPDLOG_LOGGER_INFO(DeepEngine::Core::Debug::Logger::GetBaseEngineLogger()->GetLogger(), __VA_ARGS__)
#define ENGINE_WARN(...) SPDLOG_LOGGER_WARN(DeepEngine::Core::Debug::Logger::GetBaseEngineLogger()->GetLogger(), __VA_ARGS__)
#define ENGINE_ERR(...) SPDLOG_LOGGER_ERROR(DeepEngine::Core::Debug::Logger::GetBaseEngineLogger()->GetLogger(), __VA_ARGS__)

#define LOG_TRACE(logger, ...) SPDLOG_LOGGER_TRACE(logger->GetLogger(), __VA_ARGS__)
#define LOG_DEBUG(logger, ...) SPDLOG_LOGGER_DEBUG(logger->GetLogger(), __VA_ARGS__)
#define LOG_INFO(logger, ...) SPDLOG_LOGGER_INFO(logger->GetLogger(), __VA_ARGS__)
#define LOG_WARN(logger, ...) SPDLOG_LOGGER_WARN(logger->GetLogger(), __VA_ARGS__)
#define LOG_ERR(logger, ...) SPDLOG_LOGGER_ERROR(logger->GetLogger(), __VA_ARGS__)
