#include "EngineSystem.h"

namespace DeepEngine::Architecture
{
    EngineSubsystemsManager::EngineSubsystemsManager() : _subsystemsMap(16)
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


    void EngineSubsystemsManager::Tick()
    {
        for (int i = 0; i < _subsystems.size(); i++)
        {
            if (_subsystems[i] == nullptr)
            {
                continue;
            }
            
            _subsystems[i]->Tick();
        }
    }
}