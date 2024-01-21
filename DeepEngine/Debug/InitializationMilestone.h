#pragma once
#include <cstdint>

#include "InitializationTracker.h"

namespace DeepEngine::Debug
{
    class InitializationMilestone
    {
    private:
        InitializationMilestone(const InitializationMilestone& p_other) = delete;
        InitializationMilestone(const InitializationMilestone&& p_other) = delete;
        
        InitializationMilestone(uint32_t p_id) : _id(p_id)
        { }

    public:
        static InitializationMilestone Create(const char* p_name)
        {
            uint32_t id = InitializationTracker::RegisterMilestone(p_name);
            return InitializationMilestone(id);
        }
        
    public:
        void MarkFulfilled()
        {
            InitializationTracker::FulfilMilestone(_id);
        }
        
        void MarkFailed()
        {
            InitializationTracker::FailMilestone(_id);
        }

    private:
        const uint32_t _id;
    };
}

#define DEFINE_MILESTONE(Name) DeepEngine::Debug::InitializationMilestone (Name) = DeepEngine::Debug::InitializationMilestone::Create(#Name)
#define FULFIL_MILESTONE(Name) Name.MarkFulfilled()
#define FAIL_MILESTONE(Name) Name.MarkFailed()
