#pragma once
#include <chrono>
#include <deque>
#include <iostream>
#include <fmt/format.h>

namespace DeepEngine::Core::Debug
{
    class Timer;
    
    class Timer
    {
    public:
        Timer(const Timer& other) = default;
        
        Timer(float* p_durationResult)
            : _startTime(std::chrono::steady_clock::now()), _durationResult(p_durationResult)
        {
        }
        
        ~Timer()
        {
            const auto endTime = std::chrono::steady_clock::now();

            using namespace std::literals;
            *_durationResult = (endTime - _startTime) / 1ns;
            *_durationResult /=  1000000.f;
        }

    private:
        std::chrono::time_point<std::chrono::steady_clock> _startTime;
        float* _durationResult;
    };
    
    class TimerTracker
    {
    public:
        TimerTracker()
        {
            _trackerInstances.push_back(this);
            _initialized = false;
        }

        Timer CreateTimer(const std::string& p_funcName)
        {
            if (!_initialized)
            {
                _funcName = p_funcName;
                _initialized = true;
            }
            
            _durations.push_back(0);
            return Timer(&_durations[_durations.size() - 1]);
        }

        static void PrintSummary()
        {
            fmt::print(
                "/{0:-^147}\\\n"
                "{1}\n"
                "|{0:-^147}|\n",
                "",
                fmt::format("| {:^85} | {:^12} | {:^12} | {:^12} | {:^12} |", "FUNCTION", "MIN", "MAX", "AVERAGE", "TOTAL"));
            
            for (int i = 0; i < _trackerInstances.size(); i++)
            {
                std::cout <<  _trackerInstances[i]->GetTimerSummary() << '\n';
            }
            
            fmt::print("\\{:-^147}/\n", "");
        }
        
    private:
        std::string GetTimerSummary()
        {
            float totalDuration = 0;
            float minDuration = _durations[0];
            float maxDuration = _durations[0];
            
            float averageDuration = 0.f;
            float buffer = 0.f;
            
            for (int i = 0; i < _durations.size(); i++)
            {
                buffer += _durations[i];
                totalDuration += _durations[i];
                minDuration = std::min(minDuration, _durations[i]);
                maxDuration = std::max(maxDuration, _durations[i]);

                if (i % 100 == 0 || i == _durations.size() - 1)
                {
                    averageDuration += buffer / _durations.size();
                    buffer = 0.f;
                }
            }

            // float avarageDuration = totalDuration / (float)_durations.size();
            
            return fmt::format("| {:<85} | {:^10.2f}ms | {:^10.2f}ms | {:^10.2f}ms | {:^10.2f}ms |", _funcName.c_str(),
                minDuration, maxDuration, averageDuration, totalDuration);
        }

    private:
        bool _initialized;
        std::string _funcName;
        
        std::deque<float> _durations;
        static std::vector<TimerTracker*> _trackerInstances;
    };
}

#define TIMER()                                                                                 \
    static DeepEngine::Core::Debug::TimerTracker __timerTracker;                                \
    DeepEngine::Core::Debug::Timer __timerInstance = __timerTracker.CreateTimer(__FUNCTION__)   \
