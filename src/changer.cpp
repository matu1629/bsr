#include "changer.hpp"

#include <any>
#include <bitset>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include "options.hpp"

namespace bsr
{

[[nodiscard]] constexpr std::size_t getWidth(const int base)
{
    assert(base > 0);
    std::size_t width = 0;
    for (int value = 256; value > 1; value /= base)
    {
        ++width;
    }
    return width;
}

[[nodiscard]] constexpr std::size_t getShift(std::size_t width)
{
    assert(width > 0);
    return 8 / width;
}

[[nodiscard]] constexpr char getMaskUnit(std::size_t shift)
{
    char value = 1;
    for (std::size_t i = 1; i < shift; ++i)
    {
        value <<= 1;
        value |= 1;
    }
    return value;
}

[[nodiscard]] constexpr std::string_view getUsable(int base)
{
    if (base == 2)
    {
        return "01?";
    }
    else
    {
        return "0123456789ABCDEFabcdef?";
    }
}

template <int N>
class ConstSet
{
    static_assert((N == 2 || N == 16), "unusable base");
};

template <int N>
class NormalConstSet : public ConstSet<N>
{
protected:
    static const int BASE = N;
    static const std::size_t WIDTH = getWidth(BASE);
};

template <int N>
class DirectConstSet : public ConstSet<N>
{
protected:
    static const int BASE = N;
    static const std::size_t WIDTH = getWidth(BASE);
    static const std::size_t SHIFT = getShift(WIDTH);
    static const char MASKUNIT = getMaskUnit(SHIFT);
    static constexpr std::string_view USABLE = getUsable(BASE);
};

using BinNormalConstSet = NormalConstSet<2>;
using HexNormalConstSet = NormalConstSet<16>;
using BinDirectConstSet = DirectConstSet<2>;
using HexDirectConstSet = DirectConstSet<16>;

bool hasNull(const std::vector<std::optional<std::any>> optionals)
{
    for (const auto &o : optionals)
    {
        if (!o)
        {
            return true;
        }
    }
    return false;
}

[[nodiscard]] std::pair<char, char> makeMask(const std::string &str,
    const std::size_t shift,
    const char maskUnit,
    const int base)
{
    std::string result(str);
    char mask = 0;
    for (std::size_t i = 0, end = (str.size() - 1); i <= end; ++i)
    {
        if (str[i] == '?')
        {
            result[i] = '0';
        }
        else
        {
            mask |= maskUnit;
        }

        if (i < end)
        {
            mask <<= shift;
        }
    }
    return std::make_pair(static_cast<char>(std::stoi(result, nullptr, base)), mask);
}

bool makeInfoVec(const std::string &str,
    const std::string_view usable,
    const int base,
    const std::size_t width,
    const std::size_t shift,
    const char maskUnit,
    std::vector<char> &ret,
    std::vector<char> &retMask,
    std::string &error)
{
    if (str.empty())
    {
        return true;
    }
    if (str.size() % width != 0)
    {
        std::stringstream ss;
        ss << "data length error. data lenght must be mutiple of " << width << ". data is " << str;
        error = ss.str();
        return false;
    }
    for (const auto x : str)
    {
        if (usable.find(x) == std::string::npos)
        {
            std::stringstream ss;
            ss << "invalid charactor. only " << usable;
            error = ss.str();
            return false;
        }
    }
    ret.reserve(str.size() / width);
    retMask.reserve(ret.size());
    for (std::size_t i = 0, end = (str.size() - width); i <= end; i += width)
    {
        const auto mask = makeMask(str.substr(i, width), shift, maskUnit, base);
        ret.push_back(mask.first);
        retMask.push_back(mask.second);
    }
    return true;
}

class NormalCondition : public Condition
{
public:
    bool set(const std::string &str, std::string &error) override
    {
        return getRegex(str, this->condition, error);
    }
    std::optional<std::regex> getN() const override
    {
        return std::make_optional<std::regex>(this->condition);
    }
    std::optional<std::vector<char>> getD() const override
    {
        return std::nullopt;
    }
    std::optional<std::vector<char>> getDM() const override
    {
        return std::nullopt;
    }
private:
    std::regex condition;
};

class DirectCondition : public Condition
{
public:
    bool set(const std::string &str, std::string &error) override
    {
        return this->setInfo(str, error);
    }
    std::optional<std::regex> getN() const override
    {
        return std::nullopt;
    }
    std::optional<std::vector<char>> getD() const override
    {
        return std::make_optional<std::vector<char>>(this->condition);
    }
    std::optional<std::vector<char>> getDM() const override
    {
        return std::make_optional<std::vector<char>>(this->conMask);
    }
    virtual bool setInfo(const std::string &str, std::string &error) = 0;
protected:
    std::vector<char> condition;
    std::vector<char> conMask;
};

class BinDirectCondition : public DirectCondition, BinDirectConstSet
{
    bool setInfo(const std::string &str, std::string &error) override
    {
        return makeInfoVec(str,
            this->USABLE,
            this->BASE,
            this->WIDTH,
            this->SHIFT,
            this->MASKUNIT,
            this->condition,
            this->conMask,
            error);
    }
};

class HexDirectCondition : public DirectCondition, HexDirectConstSet
{
    bool setInfo(const std::string &str, std::string &error) override
    {
        return makeInfoVec(str,
            this->USABLE,
            this->BASE,
            this->WIDTH,
            this->SHIFT,
            this->MASKUNIT,
            this->condition,
            this->conMask,
            error);
    }
};

class NormalReplacement : public Replacement
{
public:
    bool set(const std::string &str, std::string &error) override
    {
        this->replacement = str;
        return true;
    }
    std::optional<std::string> getN() const override
    {
        return std::make_optional<std::string>(this->replacement);
    }
    std::optional<std::vector<char>> getD() const override
    {
        return std::nullopt;
    }
    std::optional<std::vector<char>> getDM() const override
    {
        return std::nullopt;
    }
private:
    std::string replacement;
};

class DirectReplacement : public Replacement
{
public:
    bool set(const std::string &str, std::string &error) override
    {
        return this->setInfo(str, error);
    }
    std::optional<std::string> getN() const override
    {
        return std::nullopt;
    }
    std::optional<std::vector<char>> getD() const override
    {
        return std::make_optional<std::vector<char>>(this->replacement);
    }
    std::optional<std::vector<char>> getDM() const override
    {
        return std::make_optional<std::vector<char>>(this->repMask);
    }
    virtual bool setInfo(const std::string &str, std::string &error) = 0;
protected:
    std::vector<char> replacement;
    std::vector<char> repMask;
};

class BinDirectReplacement : public DirectReplacement, BinDirectConstSet
{
    bool setInfo(const std::string &str, std::string &error) override
    {
        return makeInfoVec(str,
            this->USABLE,
            this->BASE,
            this->WIDTH,
            this->SHIFT,
            this->MASKUNIT,
            this->replacement,
            this->repMask,
            error);
    }
};

class HexDirectReplacement : public DirectReplacement, HexDirectConstSet
{
    bool setInfo(const std::string &str, std::string &error) override
    {
        return makeInfoVec(str,
            this->USABLE,
            this->BASE,
            this->WIDTH,
            this->SHIFT,
            this->MASKUNIT,
            this->replacement,
            this->repMask,
            error);
    }
};

class NormalTaget : public Target
{
public:
    void set(std::vector<char> &&target) override
    {
        this->target = this->convert(std::move(target));
    }
    std::optional<std::string> getN() const override
    {
        return std::make_optional<std::string>(this->target);
    }
    std::optional<std::vector<char>> getD() const override
    {
        return std::nullopt;
    }
    virtual std::string convert(std::vector<char> &&target) const = 0;
private:
    std::string target;
};

class BinNormalTaget : public NormalTaget, BinNormalConstSet
{
    std::string convert(std::vector<char> &&target) const override
    {
        std::stringstream ss;
        for (const auto x : target)
        {
            ss << std::setfill('0') << std::right << std::setw(WIDTH) << std::bitset<WIDTH>(x);
        }
        return ss.str();
    }
};

class HexNormalTaget : public NormalTaget, HexNormalConstSet
{
    std::string convert(std::vector<char> &&target) const override
    {
        std::stringstream ss;
        ss << std::hex;
        for (const auto x : target)
        {
            ss << std::setfill('0') << std::right << std::setw(WIDTH) << (0xff & static_cast<const int>(x));
        }
        return ss.str();
    }
};

class DirectTaget : public Target
{
public:
    void set(std::vector<char> &&target) override
    {
        this->target = std::move(target);
    }
    std::optional<std::string> getN() const override
    {
        return std::nullopt;
    }
    std::optional<std::vector<char>> getD() const override
    {
        return std::make_optional<std::vector<char>>(this->target);
    }
private:
    std::vector<char> target;
};

bool subReConvert(const std::string &target,
    const int base,
    const std::size_t width,
    std::vector<char> &result,
    std::string &error)
{
    if (target.size() < width)
    {
        error = "data length is short";
        return false;
    }
    if (target.size() % width != 0)
    {
        error = "data length is invalid";
        return false;
    }
    result.reserve(target.size() / width);
    for (std::size_t i = 0, end = (target.size() - width); i <= end; i += width)
    {
        int value = 0;
        try
        {
            value = std::stoi(target.substr(i, width), nullptr, base);
        }
        catch (std::invalid_argument &e)
        {
            result.clear();
            error = e.what();
            return false;
        }
        catch (std::out_of_range &e)
        {
            result.clear();
            error = e.what();
            return false;
        }

        result.push_back(static_cast<char>(value));
    }
    return true;
}

class NormalChanger : public Changer
{
public:
    bool search(const Condition &con, const Target &tar, std::string &error) const override
    {
        if (hasNull({tar.getN(), con.getN()}))
        {
            error = "invalid argument value";
            return false;
        }
        return std::regex_search(*tar.getN(), *con.getN());
    }
    bool replace(const Condition &con,
        const Replacement &rep,
        const Target &tar,
        std::vector<char> &result,
        std::string &error) override
    {
        if (hasNull({tar.getN(), con.getN(), rep.getN()}))
        {
            error = "invalid argument value";
            return false;
        }
        return this->reconvert(std::regex_replace(*tar.getN(), *con.getN(), *rep.getN()), result, error);
    }
    virtual bool reconvert(const std::string &target, std::vector<char> &result, std::string &error) const = 0;
};

class BinNormalChanger : public NormalChanger, BinNormalConstSet
{
    bool reconvert(const std::string &target, std::vector<char> &result, std::string &error) const override
    {
        return subReConvert(target, this->BASE, this->WIDTH, result, error);
    }
};

class HexNormalChanger : public NormalChanger, HexNormalConstSet
{
    bool reconvert(const std::string &target, std::vector<char> &result, std::string &error) const override
    {
        return subReConvert(target, this->BASE, this->WIDTH, result, error);
    }
};

[[nodiscard]] inline bool isSame(const char condition, const char target, const char mask) noexcept
{
    return condition == (target & mask);
}

[[nodiscard]] bool subSearch(const std::vector<char> &target,
    const std::vector<char> &condition,
    const std::vector<char> &mask,
    const std::size_t start_index,
    std::size_t &hit_index)
{
    std::size_t target_index = start_index;
    std::size_t condition_index = 0;
    std::size_t pre_target_index = 0;
    std::size_t pre_condition_index = 0;
    const std::size_t condition_len = condition.size();
    const std::size_t target_len = target.size();
    bool hit_start_flag = false;
    while (true)
    {
        if (condition_index >= condition_len)
        {
            hit_index = pre_target_index;
            return true;
        }
        else if (target_index >= target_len)
        {
            hit_index = 0;
            return false;
        }

        if (isSame(condition[condition_index], target[target_index], mask[condition_index]))
        {
            if (!hit_start_flag)
            {
                hit_start_flag = true;
                pre_target_index = target_index;
                pre_condition_index = condition_index;
            }
            ++target_index;
            ++condition_index;
        }
        else
        {
            if (hit_start_flag)
            {
                hit_start_flag = false;
                target_index = pre_target_index;
                condition_index = pre_condition_index;
            }
            ++target_index;
        }
    }
}

[[nodiscard]] bool directSearch(const std::vector<char> &target,
    const std::vector<char> &condition,
    const std::vector<char> &mask)
{
    std::size_t hit_index = 0;
    return subSearch(target, condition, mask, 0, hit_index);
}

[[nodiscard]] inline char getNewChar(const char replacement, const char mask, const char original) noexcept
{
    return (replacement & mask) | (original & (0xff ^ mask));
}

[[nodiscard]] std::vector<char> directReplace(const std::vector<char> &target,
    const std::vector<char> &condition,
    const std::vector<char> &conMask,
    const std::vector<char> &replacement,
    const std::vector<char> &repMask)
{
    std::size_t start_index = 0;
    std::size_t hit_index = 0;
    const std::size_t replacement_len = replacement.size();
    auto result(target);
    while (subSearch(target, condition, conMask, start_index, hit_index))
    {
        for (std::size_t i = 0; i < replacement_len; ++i)
        {
            result[hit_index + i] = getNewChar(replacement[i], repMask[i], result[hit_index + i]);
        }
        start_index = hit_index + replacement_len;
    }
    return result;
}

class DirectChanger : public Changer
{
public:
    bool search(const Condition &con, const Target &tar, std::string &error) const override
    {
        if (hasNull({tar.getD(), con.getD(), con.getDM()}))
        {
            error = "invalid argument value for 'search'";
            return false;
        }
        return directSearch(*tar.getD(), *con.getD(), *con.getDM());
    }
    bool replace(const Condition &con,
        const Replacement &rep,
        const Target &tar,
        std::vector<char> &result,
        std::string &error) override
    {
        if (hasNull({tar.getD(), con.getD(), con.getDM(), rep.getD(), rep.getDM()}))
        {
            error = "invalid argument value for 'replace'";
            return false;
        }
        result = directReplace(*tar.getD(), *con.getD(), *con.getDM(), *rep.getD(), *rep.getDM());
        return true;
    }
};

ProcessType Checker::check(const std::string &condition,
        const std::string &replacement,
        std::string &error) noexcept
{
    if (condition.empty())
    {
        error = "no condition";
        return ProcessType::Error;
    }
    if (replacement.empty())
    {
        return ProcessType::Search;
    }
    return ProcessType::Replace;
}

class DirectChecker : public Checker
{
public:
    ProcessType check(const std::string &condition,
        const std::string &replacement,
        std::string &error) noexcept
    {
        auto type = Checker::check(condition, replacement, error);
        if (type != ProcessType::Replace)
        {
            return type;
        }
        if (condition.size() != replacement.size())
        {
            error = "condition size is different from replacement size";
            return ProcessType::Error;
        }
        return type;
    }
};

template<class T>
[[nodiscard]] std::unique_ptr<T> makeUnique()
{
    return std::make_unique<T>();
}

Creater::Creater(const Mode mode) noexcept
    : makeConditon(nullptr),
    makeReplacement(nullptr),
    makeTarget(nullptr),
    makeChanger(nullptr),
    makeChecker(nullptr)
{
    switch (mode)
    {
    case Mode::DBin:
        this->makeConditon = makeUnique<BinDirectCondition>;
        this->makeReplacement = makeUnique<BinDirectReplacement>;
        this->makeTarget = makeUnique<DirectTaget>;
        this->makeChanger = makeUnique<DirectChanger>;
        this->makeChecker = makeUnique<DirectChecker>;
        break;
    case Mode::DHex:
        this->makeConditon = makeUnique<HexDirectCondition>;
        this->makeReplacement = makeUnique<HexDirectReplacement>;
        this->makeTarget = makeUnique<DirectTaget>;
        this->makeChanger = makeUnique<DirectChanger>;
        this->makeChecker = makeUnique<DirectChecker>;
        break;
    case Mode::Bin:
        this->makeConditon = makeUnique<NormalCondition>;
        this->makeReplacement = makeUnique<NormalReplacement>;
        this->makeTarget = makeUnique<BinNormalTaget>;
        this->makeChanger = makeUnique<BinNormalChanger>;
        this->makeChecker = makeUnique<Checker>;
        break;
    default:
        this->makeConditon = makeUnique<NormalCondition>;
        this->makeReplacement = makeUnique<NormalReplacement>;
        this->makeTarget = makeUnique<HexNormalTaget>;
        this->makeChanger = makeUnique<HexNormalChanger>;
        this->makeChecker = makeUnique<Checker>;
        break;
    }
}

ProcessType Creater::getType(const std::string &condition,
    const std::string &replacement,
    std::string &error) const noexcept
{
    return this->makeChecker()->check(condition, replacement, error);
}

std::unique_ptr<Condition> Creater::condition() const
{
    return this->makeConditon();
}

std::unique_ptr<Replacement> Creater::replacement() const
{
    return this->makeReplacement();
}

std::unique_ptr<Target> Creater::target() const
{
    return this->makeTarget();
}

std::unique_ptr<Changer> Creater::changer() const
{
    return this->makeChanger();
}

const auto REGEX_OPTION = std::regex::ECMAScript | std::regex::icase;

bool getRegex(const std::string &str, std::regex &re, std::string &error)
{
    try
    {
        re = std::regex(str, REGEX_OPTION);
    }
    catch (std::regex_error &e)
    {
        std::stringstream ss;
        ss << "regex error code=" << e.code() << ": " << e.what();
        error = ss.str();
        return false;
    }
    return true;
}

} // namespace bsr