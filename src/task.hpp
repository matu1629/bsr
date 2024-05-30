#ifndef __TASK_H__
#define __TASK_H__

#include <memory>
#include <vector>

namespace bsr
{

enum class ReturnType
{
    Head,
    Next,
    End
};

class Task
{
public:
    virtual ~Task() = default;
    virtual ReturnType execute() = 0;
};

class Manager
{
public:
    Manager() noexcept = default;
    ~Manager() noexcept = default;
    Manager(const Manager &) = default;
    Manager &operator=(const Manager &) = default;
    Manager(Manager &&) = default;
    Manager &operator=(Manager &&) = default;
    void stock(std::unique_ptr<Task> &&task);
    void operator()();
private:
    std::vector<std::unique_ptr<Task>> tasks;
};

} // namespace bsr

#endif /* __TASK_H__ */
