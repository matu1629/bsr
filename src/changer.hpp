#ifndef __CHANGER_H__
#define __CHANGER_H__

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <regex>
#include <vector>
#include <tuple>

#include "options.hpp"

namespace bsr
{

enum class ProcessType
{
    Error,
    Search,
    Replace
};

class Condition
{
public:
    virtual ~Condition() = default;
    virtual bool set(const std::string &str, std::string &error) = 0;
    [[nodiscard]] virtual std::optional<std::regex> getN() const = 0;
    [[nodiscard]] virtual std::optional<std::vector<char>> getD() const = 0;
    [[nodiscard]] virtual std::optional<std::vector<char>> getDM() const = 0;
};

class Replacement
{
public:
    virtual ~Replacement() = default;
    virtual bool set(const std::string &str, std::string &error) = 0;
    [[nodiscard]] virtual std::optional<std::string> getN() const = 0;
    [[nodiscard]] virtual std::optional<std::vector<char>> getD() const = 0;
    [[nodiscard]] virtual std::optional<std::vector<char>> getDM() const = 0;
};

class Target
{
public:
    virtual ~Target() = default;
    virtual void set(std::vector<char> &&data) = 0;
    [[nodiscard]] virtual std::optional<std::string> getN() const = 0;
    [[nodiscard]] virtual std::optional<std::vector<char>> getD() const = 0;
};

class Changer
{
public:
    virtual ~Changer() = default;
    virtual bool search(const Condition &con, const Target &tar, std::string &error) const = 0;
    virtual bool replace(const Condition &con,
        const Replacement &rep,
        const Target &tar,
        std::vector<char> &result,
        std::string &error) = 0;
};

class Checker
{
public:
    virtual ProcessType check(const std::string &condition,
        const std::string &replacement,
        std::string &error) noexcept;
};

class Creater
{
public:
    Creater(const Mode mode) noexcept;
    ~Creater() noexcept = default;
    Creater(const Creater &) = delete;
    Creater &operator=(const Creater &) = delete;
    Creater(Creater &&) = delete;
    Creater &operator=(Creater &&) = delete;
    [[nodiscard]] ProcessType getType(const std::string &condition,
        const std::string &replacement,
        std::string &error) const noexcept;
    [[nodiscard]] std::unique_ptr<Condition> condition() const;
    [[nodiscard]] std::unique_ptr<Replacement> replacement() const;
    [[nodiscard]] std::unique_ptr<Target> target() const;
    [[nodiscard]] std::unique_ptr<Changer> changer() const;
private:
    std::function<std::unique_ptr<Condition>()> makeConditon;
    std::function<std::unique_ptr<Replacement>()> makeReplacement;
    std::function<std::unique_ptr<Target>()> makeTarget;
    std::function<std::unique_ptr<Changer>()> makeChanger;
    std::function<std::unique_ptr<Checker>()> makeChecker;
};

bool getRegex(const std::string &str, std::regex &re, std::string &error);

} // namespace bsr

#endif /* __CHANGER_H__ */