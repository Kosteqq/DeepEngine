#include "EngineSystem.h"

namespace DeepEngine::Core
{
    EngineSubsystemsManager::EngineSubsystemsManager(Events::EventBus& p_engineEventBus)
        : _subsystemsMap(16),
        _engineEventBus(p_engineEventBus)
    {
        _subsystems.reserve(16);
    }

    EngineSubsystemsManager::~EngineSubsystemsManager()
    {
        for (int i = 0; i < _subsystems.size(); i++)
        {
            _subsystems[i]->Destroy();
        }
        
        for (int i = 0; i < _subsystems.size(); i++)
        {
            delete _subsystems[i];
        }
    }

    bool EngineSubsystemsManager::Init()
    {
        for (int i = 0; i < _subsystems.size(); i++)
        {
            if (!_subsystems[i]->Init())
            {
                _subsystems[i]->_initializeMilestone.MarkFailed();
                return false;
            }
            _subsystems[i]->_initializeMilestone.MarkFulfilled();
        }
        return true;
    }


    void EngineSubsystemsManager::Tick(const Scene::Scene& p_scene)
    {
        for (int i = 0; i < _subsystems.size(); i++)
        {
            if (_subsystems[i] == nullptr)
            {
                continue;
            }
            
            _subsystems[i]->Tick(p_scene);
        }
    }
}