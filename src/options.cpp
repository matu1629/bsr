#include "options.hpp"

#include <sstream>
#include <string>
#include <vector>
#include <utility>

namespace bsr
{

const std::string USAGE = "Usage:bsr\n"
    " <-p path>\n"
    " <-c condition>\n"
    " [-r replace]\n"
    " [-f filecondition default:.*]\n"
    " [-m mode [bin | hex | dbin | dhex] default:hex]\n"
    " [-n threadno default:1]";

const std::vector<std::pair<Mode, std::string>> MODENAME
{
    {Mode::Bin, "bin"},
    {Mode::Hex, "hex"},
    {Mode::DBin, "dbin"},
    {Mode::DHex, "dhex"},
    {Mode::Undefined, "undefined"}
};

class MODESUPPORT
{
public:
    [[nodiscard]] static std::string name(const Mode mode) noexcept
    {
        for (const auto &x : MODENAME)
        {
            if (x.first == mode)
            {
                return x.second;
            }
        }
        return MODENAME.back().second;
    }
    [[nodiscard]] static Mode mode(const std::string &name) noexcept
    {
        for (const auto &x : MODENAME)
        {
            if (x.second == name)
            {
                return x.first;
            }
        }
        return MODENAME.back().first;
    }
};

[[nodiscard]] std::size_t getSize_t(const std::string &str, std::string &error) noexcept
{
    int value = 0;
    try
    {
        value = std::stoi(str);
    }
    catch (std::invalid_argument &e)
    {
        error = e.what();
        return 0;
    }
    catch (std::out_of_range &e)
    {
        error = e.what();
        return 0;
    }

    if (value < 1)
    {
        error = "less than 1";
        return 0;
    }
    return static_cast<std::size_t>(value);
}

class Args
{
public:
    Args(int argc, char **argv, bool checkeExe)
        : argc(argc), argv(argv), checkedIndex(-1), checked(argc, false)
    {
        if (checkeExe)
        {
            this->setChecked(0);
        }
    }
    ~Args() noexcept
    {
    }
    bool check(const std::string &target) noexcept
    {
        for (int i = 0; i < this->argc; ++i)
        {
            if (target != this->argv[i])
            {
                continue;
            }
            if (this->checked[i])
            {
                continue;
            }
            this->setChecked(i);
            this->checkedIndex = i;
            return true;
        }
        return false;
    }
    [[nodiscard]] std::string getParam() noexcept
    {
        const int i = this->checkedIndex + 1;
        if (this->isInsideRange(i))
        {
            return "";
        }
        if (this->checked[i])
        {
            return "";
        }
        this->setChecked(i);
        return this->argv[i];
    }
    bool getUncheckedFirstOption(std::string &option) const noexcept
    {
        for (int i = 0; i < this->argc; ++i)
        {
            if (!this->checked[i])
            {
                option = this->argv[i];
                return true;
            }
        }
        return false;
    }
private:
    bool isInsideRange(const int i) noexcept
    {
        if (i < 0 || i >= this->argc)
        {
            return true;
        }
        return false;
    }
    void setChecked(const int i) noexcept
    {
        if (this->isInsideRange(i))
        {
            return;
        }
        this->checked[i] = true;
    }
    int argc;
    char **argv;
    int checkedIndex;
    std::vector<bool> checked;
};

Options::Options()
    : mode(Mode::Undefined), threadno(0)
{
}

bool Options::analysis(int argc, char **argv)
{
    this->init();
    Args args(argc, argv, true);

    if (args.check("-h"))
    {
        this->message = USAGE;
        return false;
    }

    std::string path;
    if (args.check("-p"))
    {
        path = args.getParam();
        if (path.empty())
        {
            this->message = "no path";
            return false;
        }
    }

    std::string condition;
    if (args.check("-c"))
    {
        condition = args.getParam();
        if (condition.empty())
        {
            this->message = "no condition";
            return false;
        }
    }

    std::string replacement;
    if (args.check("-r"))
    {
        replacement = args.getParam();
        if (replacement.empty())
        {
            this->message = "no replacement";
            return false;
        }
    }

    std::string filecondition(".*");
    if (args.check("-f"))
    {
        filecondition = args.getParam();
        if (filecondition.empty())
        {
            this->message = "no filecondition";
            return false;
        }
    }

    Mode mode = Mode::Hex;
    if (args.check("-m"))
    {
        auto param = args.getParam();
        if (param.empty())
        {
            this->message = "no mode";
            return false;
        }
        mode = MODESUPPORT::mode(param);
        if (mode == Mode::Undefined)
        {
            this->message = "invalid mode";
            return false;
        }
    }

    std::size_t threadno = 1;
    if (args.check("-n"))
    {
        auto param = args.getParam();
        if (param.empty())
        {
            this->message = "no threadno";
            return false;
        }
        std::string error;
        threadno = getSize_t(param, error);
        if (threadno == 0)
        {
            this->message = "invalid threadno " + error;
            return false;
        }
    }

    std::string option;
    if (args.getUncheckedFirstOption(option))
    {
        this->message = "unknown option \"" + option + "\"";
        return false;
    }

    if (condition.empty())
    {
        this->message = USAGE;
        return false;
    }

    this->path = path;
    this->condition = condition;
    this->replacement = replacement;
    this->filecondition = filecondition;
    this->mode = mode;
    this->threadno = threadno;
    return true;
}

std::string Options::getInfo() const
{
    std::stringstream ss;
    ss << "parameter:";
    if (!this->path.empty())
    {
        ss << " -p \"" << this->path << "\"";
    }
    ss << " -c \"" << this->condition << "\"";
    if (!this->replacement.empty())
    {
        ss << " -r \"" << this->replacement << "\"";
    }
    ss << " -f \"" << this->filecondition << "\"";
    ss << " -m " << MODESUPPORT::name(this->mode);
    if (this->threadno > 0)
    {
        ss << " -n " << this->threadno;
    }
    return ss.str();
}

void Options::init() noexcept
{
    this->path.clear();
    this->condition.clear();
    this->replacement.clear();
    this->filecondition.clear();
    this->mode = Mode::Undefined;
    this->threadno = 0;
}

} // namespace bsr