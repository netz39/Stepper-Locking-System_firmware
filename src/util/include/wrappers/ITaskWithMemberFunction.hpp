namespace util::wrappers
{

class ITaskWithMemberFunction
{
public:
    virtual void taskMain() = 0;
    virtual ~ITaskWithMemberFunction() = default;
};

} // namespace util::wrappers