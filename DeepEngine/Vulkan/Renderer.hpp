#pragma once

#include "Architecture/EngineSystem.h"

namespace DeepEngine
{
    class RendererSubsystem : public Architecture::EngineSubsystem
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
    };
}
