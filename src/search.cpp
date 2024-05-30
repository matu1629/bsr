#include "search.hpp"

#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <mutex>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "options.hpp"
#include "changer.hpp"
#include "task.hpp"

namespace bsr
{

const auto INPUT_OPTION = std::ios_base::in | std::ios_base::binary;
const auto OUTPUT_OPTION = std::ios_base::out | std::ios_base::trunc | std::ios_base::binary;

class Environment
{
public:
    Environment(const std::regex &fre)
        : fre(fre)
    {
    }
    void debuglog(const std::vector<std::string> &msgs)
    {
        std::stringstream ss;
        for (const auto &msg : msgs)
        {
            ss << msg;
        }
        std::lock_guard<std::mutex> lock(this->cerr_gard);
        std::cerr << ss.str() << std::endl;
    }
    const std::regex &fre;
    std::mutex cerr_gard;
};

class Info
{
public:
    Info(std::unique_ptr<Changer> &&chan,
        const std::unique_ptr<Condition> &con,
        const std::unique_ptr<Replacement> &rep,
        std::unique_ptr<Target> &&tar,
        const std::vector<std::string>::iterator &begin,
        const std::vector<std::string>::iterator &end)
        : chan(std::move(chan)), con(con), rep(rep), tar(std::move(tar)), file_iter(begin), end(end)
    {
    }
    std::unique_ptr<Changer> chan;
    const std::unique_ptr<Condition> &con;
    const std::unique_ptr<Replacement> &rep;
    std::unique_ptr<Target> tar;
    std::vector<std::string>::iterator file_iter;
    std::vector<std::string>::iterator end;
    std::vector<std::string> filtered_files;
    std::string error;
};

class TaskEnv : public Task
{
public:
    TaskEnv(Environment &env, Info &info)
        : env(env), info(info)
    {
    }
    Environment &env;
    Info &info;
};

class JudgeTask : public TaskEnv
{
public:
    JudgeTask(Environment &env, Info &info)
        : TaskEnv(env, info), processing(false)
    {
    }
    ReturnType execute() override
    {
        if (!this->processing)
        {
            this->processing = true;
            return ReturnType::Next;
        }
        ++this->info.file_iter;
        if (this->info.file_iter != this->info.end)
        {
            return ReturnType::Next;
        }
        return ReturnType::End;
    }
private:
    bool processing;
};

class ReadAndSearchTask : public TaskEnv
{
public:
    ReadAndSearchTask(Environment &env, Info &info)
        : TaskEnv(env, info)
    {
    }
    ReturnType execute() override
    {
        const auto &filename = *this->info.file_iter;
        if (!std::regex_search(filename, this->env.fre))
        {
#ifdef _DEBUG
            this->env.debuglog({"untargeted file ", filename});
#endif
            return ReturnType::Head;
        }
        std::ifstream ifs(filename, INPUT_OPTION);
        if (!ifs.is_open())
        {
#ifdef _DEBUG
            this->env.debuglog({"file open error(in, binary) ", filename});
#endif
            return ReturnType::Head;
        }
        std::vector<char> before;
        char b = 0;
        while (ifs.get(b))
        {
            before.push_back(b);
        }
        ifs.close();
        if (before.empty())
        {
#ifdef _DEBUG
            this->env.debuglog({"file is empty ", filename});
#endif
            return ReturnType::Head;
        }
        this->info.tar->set(std::move(before));
        if (!this->info.chan->search(*this->info.con, *this->info.tar, this->info.error))
        {
            if (this->info.error.empty())
            {
#ifdef _DEBUG
                this->env.debuglog({"not hit ", filename});
#endif
                return ReturnType::Head;
            }
            return ReturnType::End;
        }
        return ReturnType::Next;
    }
};

class ReplaceAndWriteTask : public TaskEnv
{
public:
    ReplaceAndWriteTask(Environment &env, Info &info)
        : TaskEnv(env, info)
    {
    }
    ReturnType execute() override
    {
        const auto &filename = *this->info.file_iter;
        std::vector<char> after;
        if (!this->info.chan->replace(*this->info.con, *this->info.rep, *this->info.tar, after, this->info.error))
        {
            return ReturnType::End;
        }
        std::ofstream ofs(filename, OUTPUT_OPTION);
        if (!ofs.is_open())
        {
#ifdef _DEBUG
            this->env.debuglog({"file open error(out, trunc, binary) ", filename});
#endif
            return ReturnType::Head;
        }
        for (const auto a : after)
        {
            ofs.write(&a, 1);
        }
        ofs.close();
        return ReturnType::Next;
    }
};

class StoreTask : public TaskEnv
{
public:
    StoreTask(Environment &env, Info &info)
        : TaskEnv(env, info)
    {
    }
    ReturnType execute() override
    {
        this->info.filtered_files.push_back(*this->info.file_iter);
        return ReturnType::Head;
    }
};

bool getFiles(const std::string &path, std::vector<std::string> &files, std::string &error)
{
    if (path.empty())
    {
        std::string file;
        std::set<std::string> file_set;
        while (std::getline(std::cin, file))
        {
            if (file.empty())
            {
                break;
            }
            auto ret = file_set.insert(file);
#ifdef _DEBUG
            if (!ret.second)
            {
                std::cerr << "duplicated file: " << file << std::endl;
            }
#endif
        }
        files.resize(file_set.size());
        std::copy(file_set.begin(), file_set.end(), files.begin());
    }
    else
    {
        if (!std::filesystem::exists(path))
        {
            error = "invalid path";
            return false;
        }
        for (const auto &x : std::filesystem::recursive_directory_iterator(path))
        {
            if (x.is_directory())
            {
                continue;
            }
            files.push_back(x.path().string());
        }
    }
    if (files.empty())
    {
        error = "no file";
        return false;
    }
    return true;
}

std::vector<Info> makeInfo(const std::size_t threadno,
    std::vector<std::string> &files,
    Creater &creater,
    const std::unique_ptr<bsr::Condition> &con,
    const std::unique_ptr<bsr::Replacement> &rep)
{
    std::vector<std::vector<std::string>::iterator> range;
    std::size_t step = 0;
    range.push_back(files.begin());
    for (std::size_t i = 0, end = (threadno - 1); i < end; ++i)
    {
        const std::size_t alloc_step = (files.size() + i) / threadno;
        if (alloc_step == 0)
        {
            continue;
        }
        step += alloc_step;
        range.push_back(files.begin() + step);
    }
    range.push_back(files.end());
    std::vector<Info> infos;
    for (std::size_t i = 0, end = (range.size() - 1); i < end; ++i)
    {
        infos.emplace_back(creater.changer(), con, rep, creater.target(), range[i], range[i + 1]);
    }
#ifdef _DEBUG
    std::cerr << "adjust number of threads " << threadno << " -> " << infos.size() << std::endl;
#endif
    return infos;
}

bool Search::execute(const std::string &path,
    const std::string &condition,
    const std::string &replacement,
    const std::string &filecondition,
    const Mode mode,
    const std::size_t threadno)
{
    this->error.clear();
    this->result.clear();
    Creater creater(mode);
    const auto type = creater.getType(condition, replacement, this->error);
    if (type == ProcessType::Error)
    {
        return false;
    }
    auto con = creater.condition();
    if (!con->set(condition, this->error))
    {
        return false;
    }
    auto rep = creater.replacement();
    if (!rep->set(replacement, this->error))
    {
        return false;
    }
    std::regex fre;
    if (!getRegex(filecondition, fre, this->error))
    {
        return false;
    }
    std::vector<std::string> files;
    if (!getFiles(path, files, this->error))
    {
        return false;
    }
    Environment env(fre);
    auto infos = makeInfo(threadno, files, creater, con, rep);
    std::vector<std::thread> threads;
    for (auto &info : infos)
    {
        Manager manager;
        manager.stock(std::make_unique<JudgeTask>(env, info));
        manager.stock(std::make_unique<ReadAndSearchTask>(env, info));
        if (type == ProcessType::Replace)
        {
            manager.stock(std::make_unique<ReplaceAndWriteTask>(env, info));
        }
        manager.stock(std::make_unique<StoreTask>(env, info));
        threads.emplace_back(std::move(manager));
    }
    for (auto &t : threads)
    {
        t.join();
    }
    for (const auto &info : infos)
    {
        if (!info.error.empty())
        {
            this->error = info.error;
            return false;
        }
    }
    for (const auto &info : infos)
    {
        this->result.insert(this->result.end(), info.filtered_files.begin(), info.filtered_files.end());
    }
    std::sort(this->result.begin(), this->result.end());
    return true;
}

} // namespace bsr