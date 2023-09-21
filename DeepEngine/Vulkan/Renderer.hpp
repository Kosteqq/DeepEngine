#pragma once

#include "Architecture/EngineSystem.h"
#include "Device.hpp"

namespace DeepEngine
{
    class RendererSubsystem : public Core::Architecture::EngineSubsystem
    {
    public:
        RendererSubsystem();
        ~RendererSubsystem();

    protected:
        bool Init() override;

        void Destroy() override
        {
        }

        void Tick() override
        {
        }

    private:

        std::unique_ptr<Device> _device;
    };
}
