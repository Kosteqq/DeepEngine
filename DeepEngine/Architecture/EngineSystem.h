#pragma once
#include "Debugs/Logger.h"
#include <memory>
#include <type_traits>
#include <vector>

namespace DeepEngine::Core::Architecture
{
    class EngineSubsystemsManager;

    class EngineSubsystem
    {
    protected:
        friend class EngineSubsystemsManager;

        EngineSubsystem(const char* p_subsystemName)
            : _subsystemLogger(Debug::Logger::CreateLoggerInstance(p_subsystemName))
        { }

        virtual bool Init() = 0;
        virtual void Destroy() = 0;
        virtual void Tick() = 0;
        
    protected:
        std::shared_ptr<Debug::Logger> _subsystemLogger; 
    };

    class EngineSubsystemsManager
    {
    public:
        EngineSubsystemsManager();
        ~EngineSubsystemsManager();

        bool Init();
        void Tick();
        
    public:
        template <typename T, class... Args>
        requires std::is_base_of_v<EngineSubsystem, T>
        void CreateSubsystem(Args... p_args)
        {
            auto newSubmodule = new T(std::forward<Args>(p_args)...);
            _subsystems.push_back(reinterpret_cast<EngineSubsystem*>(newSubmodule));
        }

    private:
        std::vector<EngineSubsystem*> _subsystems;
    };
}

#define TRACE(...) LOG_TRACE(_subsystemLogger, __VA_ARGS__)  
#define DEBUG(...) LOG_DEBUG(_subsystemLogger, __VA_ARGS__)  
#define INFO(...) LOG_INFO(_subsystemLogger, __VA_ARGS__)  
#define WARN(...) LOG_WARN(_subsystemLogger, __VA_ARGS__)  
#define ERR(...) LOG_ERR(_subsystemLogger, __VA_ARGS__)  
