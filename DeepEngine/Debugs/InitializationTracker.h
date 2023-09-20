#pragma once
#include <unordered_map>
#include <string.h>

#include "Logger.h"

namespace DeepEngine::Core::Debug
{
    class InitializationTracker
    {
    private:
        InitializationTracker(const InitializationTracker& p_other) = delete;
        InitializationTracker(const InitializationTracker&& p_other) = delete;
        InitializationTracker();
        ~InitializationTracker();
        
    public:
        static void LogSummary();

    public:
        static uint32_t RegisterMilestone(const char* p_name);
        static void FulfilMilestone(uint32_t p_id);
        static void FailMilestone(uint32_t p_id);

    private:
        static InitializationTracker* GetInstance();

    private:
        enum class MilestoneState
        {
            UNDEFINED, FULFILLED, FAILED
        };

        
        std::unordered_map<uint32_t, std::tuple<MilestoneState, const char*>> _milestones;
        std::shared_ptr<Logger> _logger = Logger::CreateLoggerInstance("Initialization");;
    };
}
