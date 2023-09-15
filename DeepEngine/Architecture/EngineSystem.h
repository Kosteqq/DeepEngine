#pragma once
#include <memory>
#include <type_traits>
#include <vector>

namespace DeepEngine::Architecture
{
    class EngineSubsystemsManager;

    class EngineSubsystem
    {
    protected:
        friend class EngineSubsystemsManager;

        virtual bool Init() = 0;
        virtual void Destroy() = 0;
        virtual void Tick() = 0;
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
