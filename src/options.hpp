#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <string>
#include <vector>

namespace bsr
{

enum class Mode
{
    Bin,
    Hex,
    DBin,
    DHex,
    Undefined
};

class Options
{
public:
    Options();
    ~Options() noexcept = default;
    Options(const Options &) = delete;
    Options &operator=(const Options &) = delete;
    Options(Options &&) = delete;
    Options &operator=(Options &&) = delete;
    bool analysis(int argc, char **argv);
    [[nodiscard]] std::string getInfo() const;
    std::string path;
    std::string condition;
    std::string replacement;
    std::string filecondition;
    Mode mode;
    std::size_t threadno;
    std::string message;
private:
    void init() noexcept;
};

} // namespace bsr

#endif /* __OPTIONS_H__ */
