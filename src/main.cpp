#include <iostream>
#include <string>
#include <vector>

#include "options.hpp"
#include "search.hpp"

int main(int argc, char *argv[])
{
    bsr::Options options;
    if (!options.analysis(argc, argv))
    {
        std::cerr << options.message << std::endl;
        return 1;
    }
    bsr::Search search;
    if (!search.execute(options.path,
        options.condition,
        options.replacement,
        options.filecondition,
        options.mode,
        options.threadno))
    {
        std::cerr << search.error << std::endl;
        return 1;
    }
    std::cerr << options.getInfo() << std::endl;
    for (const auto &x : search.result)
    {
        std::cout << x << std::endl;
    }
    std::cerr << "done it. " << search.result.size() << std::endl;
    return 0;
}