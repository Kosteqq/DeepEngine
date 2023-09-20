#include "Logger.h"

namespace DeepEngine::Core::Debug
{
    std::shared_ptr<Logger> Logger::_engineLogger = nullptr;
    std::shared_ptr<spdlog::sinks::stdout_color_sink_st> Logger::_consoleSink = nullptr;
    std::shared_ptr<spdlog::sinks::basic_file_sink_st> Logger::_fileSink = nullptr;

    Logger::Logger(const char* p_name) : _logger(p_name, { _consoleSink, _fileSink })
    {
    }

    void Logger::Initialize(const char* p_filepath)
    {
        spdlog::set_level(spdlog::level::trace);
        
        _fileSink = std::make_shared<spdlog::sinks::basic_file_sink_st>(p_filepath);
        _fileSink->set_level(spdlog::level::trace);
        _fileSink->set_pattern("[%D %T] %-32s (%#) [%=16n][%^%=7l%$]: %v");
        
        _engineLogger = CreateLoggerInstance("Engine");
    }
 
    void Logger::Initialize(const std::string& p_filepath)
    {
        Initialize(p_filepath.c_str());
    }

    std::shared_ptr<Logger> Logger::CreateLoggerInstance(const char* p_name)
    { 
        if (_consoleSink == nullptr)
        {
            _consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
            _consoleSink->set_level(spdlog::level::trace);
            _consoleSink->set_pattern("[%T]%=32s(%#) [%=16n][%^%=7l%$]: %v");
        }

        auto* logger = new Logger(p_name);
        logger->GetLogger()->set_level(spdlog::level::trace);
        std::shared_ptr<Logger> loggerPtr;
        loggerPtr.reset(logger);
        return loggerPtr;
    }

    std::shared_ptr<Logger> Logger::CreateLoggerInstance(const std::string& p_name)
    { 
        return CreateLoggerInstance(p_name.c_str());
    }

    std::shared_ptr<Logger> Logger::GetBaseEngineLogger()
    {
        return _engineLogger;
    }
    
}
