#pragma once
#include <chrono>
#include <fmt/format.h>

#define TIMER(name)                                                                                 \
    static DeepEngine::Core::Debug::TimerTracker __timerTracker;                                    \
    DeepEngine::Core::Debug::Timer __timerInstance = __timerTracker.CreateTimer(__func__, name) \

#define PRINT_TIMER_SUMMARY() DeepEngine::Core::Debug::TimerTracker::PrintSummary()

namespace DeepEngine::Core::Debug
{
    class Timer;
    
    class Timer
    {
    public:
        Timer(const Timer& p_other) = default;
        
        Timer(float* p_durationResult, uint64_t* p_totalMilliseconds)
            : _startTime(std::chrono::steady_clock::now())
        {
            _durationResult = p_durationResult;
            _totalMilliseconds = p_totalMilliseconds;
        }
        
        ~Timer()
        {
            const auto endTime = std::chrono::steady_clock::now();

            using namespace std::literals;
            *_durationResult = (endTime - _startTime) / 1ns;
            *_durationResult /= 1000000.f;

            *_totalMilliseconds += (endTime - _startTime) / 1ms;
        }

    private:
        std::chrono::time_point<std::chrono::steady_clock> _startTime;
        uint64_t* _totalMilliseconds;
        float* _durationResult;
    };

    class TimerTracker
    {
    public:
        TimerTracker()
        {
            std::memset(_durations, 0, sizeof(float) * _durationsLength);
            _currentDurationIndex = 0;
            _maxIndex = 0;
            _totalMilliseconds = 0;
            
            _trackerInstances.push_back(this);
        }

        ~TimerTracker()
        {
            delete _funcName;
            delete _name;
        }

        Timer CreateTimer(const char* p_funcName, const char* p_name)
        {
            if (_funcName == nullptr)
            {
                _funcName = new char[strlen(p_funcName) + 1];
                _name = new char[strlen(p_name) + 1];

                strcpy(const_cast<char*>(_funcName), p_funcName);
                strcpy(const_cast<char*>(_name), p_name);
            }
            
            float* currentDuration = &_durations[_currentDurationIndex];
            _currentDurationIndex++;

            if (_currentDurationIndex >= _durationsLength)
            {
                _currentDurationIndex = 0;
            }
            
            _maxIndex = std::max(_currentDurationIndex, _maxIndex);
            return Timer(currentDuration, &_totalMilliseconds);
        }
        
        static void PrintSummary()
        {
            std::string summary;
            
            for (int i = 0; i < _trackerInstances.size(); i++)
            {
                summary += _trackerInstances[i]->GetTimerSummary() + '\n';
            }
            
            fmt::print(
                "/{0:-^147}\\\n"
                "{1}\n"
                "|{0:-^147}|\n"
                "{2}"
                "\\{0:-^147}/\n",
                "",
                fmt::format("| {:^85} | {:^12} | {:^12} | {:^12} | {:^12} |", "FUNCTION", "MIN", "MAX", "AVERAGE", "TOTAL"),
                summary);
        }
        
    private:
        std::string GetTimerSummary() const
        {
            float minDuration = _durations[0];
            float maxDuration = _durations[0];
            
            float averageDuration = 0.f;
            float buffer = 0.f;
            
            for (int i = 0; i < _maxIndex; i++)
            {
                buffer += _durations[i];
                minDuration = std::min(minDuration, _durations[i]);
                maxDuration = std::max(maxDuration, _durations[i]);

                if (i % 100 == 0 || i == _maxIndex - 1)
                {
                    averageDuration += buffer / _maxIndex;
                    buffer = 0.f;
                }
            }

            return fmt::format("| {:<50} {:>34} | {:^10.2f}ms | {:^10.2f}ms | {:^10.2f}ms | {:^10}ms |",
                _funcName, _name, minDuration, maxDuration, averageDuration, _totalMilliseconds);
        }

    private:
        const char* _funcName;
        const char* _name;

        uint64_t _totalMilliseconds;
        
        const uint32_t _durationsLength = 1000;
        uint32_t _currentDurationIndex;
        uint32_t _maxIndex;
        float _durations[1000];
        
        static std::vector<TimerTracker*> _trackerInstances;
    };
}
