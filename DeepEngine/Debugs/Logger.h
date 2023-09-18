#pragma once
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

#define ENGINE_INFO(...) SPDLOG_LOGGER_INFO(DeepEngine::Core::Debug::Logger::GetBaseEngineLogger()->GetLogger(), __VA_ARGS__)
