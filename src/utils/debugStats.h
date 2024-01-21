#ifndef DEBUG_STATS_H
#define DEBUG_STATS_H
class DebugStats
{
public:
    /// singleton function
    static DebugStats &getInstance()
    {
        static DebugStats singleton;
        return singleton;
    }
    void addFrameSkip()
    {
        ++frameSkips;
    }
    void printStats();
private:
    DebugStats() : frameSkips(0) {}
    DebugStats(DebugStats const&);
    int frameSkips;
};
#endif // DEBUG_STATS_H