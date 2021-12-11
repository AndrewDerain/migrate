
struct Time
{
    int hour;
    Time(int h) :hour(h) {}
};




struct IVirtual
{
public:
    virtual void Plus() = 0;
};

struct Virtual : public IVirtual
{
    int i = 0;
public:
    virtual void Plus()
    {
        i += std::rand();
    }
};

template<typename _Son>
struct IStaticBind
{
public:
    void Plus() { static_cast<_Son*>(this)->Plus(); }
};

struct StaticBind : public IStaticBind<StaticBind>
{
    int i = 0;
public:
    void Plus()
    {
        i += std::rand();
    }
};

void BindTest()
{
    IStaticBind<StaticBind>* StaticBindInstance = new StaticBind;
    IVirtual* VirtualInstance = new Virtual;

    std::chrono::steady_clock::time_point beg, end;

    beg = std::chrono::steady_clock::now();
    for (int i = 0; i < 100000000; i++)
        StaticBindInstance->Plus();
    end = std::chrono::steady_clock::now();
    std::cout << "Static Bind Time: " << std::chrono::duration<double>(end - beg).count() << std::endl;

    beg = std::chrono::steady_clock::now();
    for (int i = 0; i < 100000000; i++)
        VirtualInstance->Plus();
    end = std::chrono::steady_clock::now();
    std::cout << "Virtual Time: " << std::chrono::duration<double>(end - beg).count() << std::endl;
}
