
##### Used 3rdParty:
* fmt

#### Namespace:
DeepEngine::Core::Debug

#### About:

Initialization Tracker is simple tool to track initialization & its result

Usage:
* Create Initialization Milestone by Debug::InitializationMilestone::Create( name ) or DEFINE_MILESTONE( name ) macro
* In runtime invoke on milestone .MarkFulfilled() or .MarkFailed() to set is result
* Invoke InitializationTracker::LogSummary() to print summary to the log. Actually system is doing it itself on exit application
