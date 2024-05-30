#include "task.hpp"

#include <cassert>
#include <memory>

namespace bsr
{

void Manager::stock(std::unique_ptr<Task> &&task)
{
    this->tasks.push_back(std::move(task));
}

void Manager::operator()()
{
    std::size_t index = 0;
    const std::size_t end = this->tasks.size();
    while (index < end)
    {
        switch (this->tasks[index]->execute())
        {
        case ReturnType::Next:
            ++index;
            break;
        case ReturnType::Head:
            index = 0;
            break;
        default:
            return;
        }
    }
}

} // namespace bsr