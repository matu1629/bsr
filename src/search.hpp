#ifndef __SEARCH_H__
#define __SEARCH_H__

#include <filesystem>
#include <regex>
#include <string>
#include <vector>

#include "options.hpp"

namespace bsr
{

class Search
{
public:
    Search() noexcept = default;
    ~Search() noexcept = default;
    Search(const Search &) = delete;
    Search &operator=(const Search &) = delete;
    Search(Search &&) = delete;
    Search &operator=(Search &&) = delete;
    bool execute(const std::string &path,
        const std::string &condition,
        const std::string &replace,
        const std::string &filecondition,
        const Mode mode,
        const std::size_t threadno);
    std::string error;
    std::vector<std::string> result;
};

} // namespace bsr

#endif /* __SEARCH_H__ */
