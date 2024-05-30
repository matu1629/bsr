#define CATCH_CONFIG_MAIN

#include <memory>
#include <string>
#include <vector>

#include "catch.hpp"

#include "task.hpp"

class Enviroment
{
public:
    Enviroment()
        : result(0)
    {
    }
    ~Enviroment() = default;
    int result;
};

class InitTask : public bsr::Task
{
public:
    InitTask(Enviroment &env, const int i)
        : env(env), i(i)
    {
    }
    bsr::ReturnType execute() override
    {
        this->env.result = i;
        return bsr::ReturnType::Next;
    }
    Enviroment &env;
    int i;
};

class AddTask : public bsr::Task
{
public:
    AddTask(Enviroment &env, const int i)
        : env(env), i(i)
    {
    }
    bsr::ReturnType execute() override
    {
        this->env.result += i;
        return bsr::ReturnType::Next;
    }
    Enviroment &env;
    int i;
};

class SubTask : public bsr::Task
{
public:
    SubTask(Enviroment &env, const int i)
        : env(env), i(i)
    {
    }
    bsr::ReturnType execute() override
    {
        this->env.result -= i;
        return bsr::ReturnType::Next;
    }
    Enviroment &env;
    int i;
};

class MulTask : public bsr::Task
{
public:
    MulTask(Enviroment &env, const int i)
        : env(env), i(i)
    {
    }
    bsr::ReturnType execute() override
    {
        this->env.result *= i;
        return bsr::ReturnType::Next;
    }
    Enviroment &env;
    int i;
};

class DivTask : public bsr::Task
{
public:
    DivTask(Enviroment &env, const int i)
        : env(env), i(i)
    {
    }
    bsr::ReturnType execute() override
    {
        this->env.result /= i;
        return bsr::ReturnType::Next;
    }
    Enviroment &env;
    int i;
};

class ExitTask : public bsr::Task
{
public:
    ExitTask()
    {
    }
    bsr::ReturnType execute() override
    {
        return bsr::ReturnType::End;
    }
};

class AddLimitTask : public bsr::Task
{
public:
    AddLimitTask(Enviroment &env, const int i)
        : env(env), i(i)
    {
    }
    bsr::ReturnType execute() override
    {
        ++this->env.result;
        if (this->env.result < this->i)
        {
            return bsr::ReturnType::Next;
        }
        else
        {
            return bsr::ReturnType::End;
        }
    }
    Enviroment &env;
    int i;
};

class SkipEvenTask : public bsr::Task
{
public:
    SkipEvenTask(Enviroment &env)
        : env(env)
    {
    }
    bsr::ReturnType execute() override
    {
        if (this->env.result % 2 == 0)
        {
            return bsr::ReturnType::Head;
        }
        else
        {
            return bsr::ReturnType::Next;
        }
    }
    Enviroment &env;
};

class GoToHeadTask : public bsr::Task
{
public:
    GoToHeadTask(Enviroment &env)
        : env(env)
    {
    }
    bsr::ReturnType execute() override
    {
            return bsr::ReturnType::Head;
    }
    Enviroment &env;
};

TEST_CASE("no task", "[task]")
{
    Enviroment env;
    bsr::Manager manager;
    manager();
    CHECK(env.result == 0);
}

TEST_CASE("line task", "[task]")
{
    Enviroment env;
    bsr::Manager manager;
    manager.stock(std::make_unique<InitTask>(env, 10));
    manager.stock(std::make_unique<AddTask>(env, 50));
    manager.stock(std::make_unique<SubTask>(env, 30));
    manager.stock(std::make_unique<MulTask>(env, 100));
    manager.stock(std::make_unique<DivTask>(env, 20));
    manager.stock(std::make_unique<AddTask>(env, 1));
    manager.stock(std::make_unique<AddTask>(env, 2));
    manager.stock(std::make_unique<AddTask>(env, 3));
    manager.stock(std::make_unique<ExitTask>());
    manager();
    CHECK(env.result == 156);
}

TEST_CASE("loop task", "[task]")
{
    Enviroment env;
    bsr::Manager manager;
    manager.stock(std::make_unique<AddLimitTask>(env, 100));
    manager.stock(std::make_unique<SkipEvenTask>(env));
    manager.stock(std::make_unique<AddTask>(env, 50));
    manager.stock(std::make_unique<GoToHeadTask>(env));
    manager();
    CHECK(env.result == 104);
}