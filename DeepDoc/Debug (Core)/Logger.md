
Used 3rdParty:
* https://github.com/gabime/spdlog
* https://github.com/ocornut/imgui

___
* Fast printing
* Quick info about from where log went


Logger class contains static shared console & file sinks pointers. Contains in itself static pointer to global Engine pointer. Also can be instanced with different name for better split information.

Log Levels: Trace, Debug, Info, Warn, Err

### Macros:
* ENGINE_INFO (message)
* LOG_INFO (Logger, message)

