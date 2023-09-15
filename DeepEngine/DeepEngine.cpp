#include "Debug/Debug.h"
#include "Architecture/EngineSystem.h"



class TestSubsystem : DeepEngine::Architecture::EngineSubsystem
{
public:
    TestSubsystem(int x)
    {
        
    }

protected:
    bool Init() override
    {
        LOG("INIT")
        return true;
    }

    void Destroy() override
    {
        LOG("Destroy")
    }

    void Tick() override
    { }
};

int main(int argc, char* argv[])
{
    LOG("Hello World")
    return 0;
}
