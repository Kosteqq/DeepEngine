#include "InitializationTracker.h"
#include "Logger.h"

#include <fmt/format.h>

namespace DeepEngine::Core::Debug
{
    InitializationTracker::InitializationTracker() : _milestones(32)
    { }

    InitializationTracker::~InitializationTracker()
    {
        LogSummary();   
    }

    uint32_t InitializationTracker::RegisterMilestone(const char* p_name)
    {
        const auto instance = GetInstance();
        uint32_t id = instance->_milestones.size();

        instance->_milestones[id] = std::tuple<MilestoneState, const char*>(MilestoneState::UNDEFINED, p_name);
        return id;
    }

    void InitializationTracker::FulfilMilestone(uint32_t p_id)
    {
        const auto instance = GetInstance();
        const auto milestone = &instance->_milestones[p_id];

        std::get<0>(*milestone) = MilestoneState::FULFILLED;
        LOG_DEBUG(instance->_logger, "Fulfiled Milestone: {0}", std::get<1>(*milestone));
    }

    void InitializationTracker::FailMilestone(uint32_t p_id)
    {
        const auto instance = GetInstance();
        const auto milestone = &instance->_milestones[p_id];

        std::get<0>(*milestone) = MilestoneState::FAILED;
        LOG_DEBUG(instance->_logger, "Failed Milestone: {0}", std::get<1>(*milestone));
    }

    InitializationTracker* InitializationTracker::GetInstance()
    {
        static InitializationTracker instance;
        return &instance;
    }

    void InitializationTracker::LogSummary()
    {
        const auto instance = GetInstance();
        
        LOG_INFO(instance->_logger, "Initialization Summary:");
        for (int i = 0; i < instance->_milestones.size(); i++)
        {
            auto milestone = instance->_milestones.at(i);
            auto milestoneName = fmt::format("{:<40}", fmt::format("\tMilestone \"{}\":", std::get<1>(milestone)));

            switch (std::get<0>(milestone))
            {
            case MilestoneState::UNDEFINED:
                LOG_WARN(instance->_logger, "{0} {1}", milestoneName, "Undefined");
                break;
            case MilestoneState::FAILED:
                LOG_ERR(instance->_logger, "{0} {1}", milestoneName, "Failed");
                break;
            case MilestoneState::FULFILLED:
                LOG_INFO(instance->_logger, "{0} {1}", milestoneName, "Passed");
                break;
            }
        }
        
        LOG_INFO(instance->_logger, "");
    }
}
