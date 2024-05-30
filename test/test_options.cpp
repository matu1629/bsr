#define CATCH_CONFIG_MAIN

#include <string>
#include <vector>

#include "catch.hpp"

#include "options.hpp"

class Arguments
{
public:
    Arguments() = default;
    ~Arguments()
    {
        for (int i = 0; i < this->argc; ++i)
        {
            if (this->argv[i] != nullptr)
            {
                delete[] this->argv[i];
            }
        }
        if (this->argv != nullptr)
        {
            delete[] this->argv;
        }
    }
    Arguments(const Arguments &) = delete;
    Arguments &operator=(const Arguments &) = delete;
    Arguments(Arguments &&) = delete;
    Arguments &operator=(Arguments &&) = delete;
    std::pair<int, char **> make(const std::vector<std::string> &params)
    {
        const std::size_t num = params.size();
        this->argc = 0;
        this->argv = new char *[num];
        for (std::size_t i = 0; i < num; ++i)
        {
            const std::size_t copysize = params[i].size();
            argv[i] = new char[copysize + 1];
            for (std::size_t j = 0; j < copysize; ++j)
            {
                argv[i][j] = params[i][j];
            }
            argv[i][copysize] = '\0';
            ++this->argc;
        }
        return std::make_pair(this->argc, this->argv);
    }
private:
    int argc;
    char **argv;
};

TEST_CASE("unspecified mode", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == true);
    CHECK(options.path == "..");
    CHECK(options.condition == "xxx");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == ".*");
    CHECK(options.mode == bsr::Mode::Hex);
    CHECK(options.threadno == 1);
    CHECK(options.message == "");
}

TEST_CASE("specified mode bin", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-m", "bin"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == true);
    CHECK(options.path == "..");
    CHECK(options.condition == "xxx");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == ".*");
    CHECK(options.mode == bsr::Mode::Bin);
    CHECK(options.threadno == 1);
    CHECK(options.message == "");
}

TEST_CASE("specified mode hex", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-m", "hex"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == true);
    CHECK(options.path == "..");
    CHECK(options.condition == "xxx");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == ".*");
    CHECK(options.mode == bsr::Mode::Hex);
    CHECK(options.threadno == 1);
    CHECK(options.message == "");
}

TEST_CASE("specified mode dbin", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-m", "dbin"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == true);
    CHECK(options.path == "..");
    CHECK(options.condition == "xxx");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == ".*");
    CHECK(options.mode == bsr::Mode::DBin);
    CHECK(options.threadno == 1);
    CHECK(options.message == "");
}

TEST_CASE("specified mode dhex", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-m", "dhex"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == true);
    CHECK(options.path == "..");
    CHECK(options.condition == "xxx");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == ".*");
    CHECK(options.mode == bsr::Mode::DHex);
    CHECK(options.threadno == 1);
    CHECK(options.message == "");
}

TEST_CASE("help", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-h"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == false);
    CHECK(options.path == "");
    CHECK(options.condition == "");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == "");
    CHECK(options.mode == bsr::Mode::Undefined);
    CHECK(options.threadno == 0);
    CHECK(options.message == "Usage:bsr\n"
        " <-p path>\n"
        " <-c condition>\n"
        " [-r replace]\n"
        " [-f filecondition default:.*]\n"
        " [-m mode [bin | hex | dbin | dhex] default:hex]\n"
        " [-n threadno default:1]");
}

TEST_CASE("no option", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == false);
    CHECK(options.path == "");
    CHECK(options.condition == "");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == "");
    CHECK(options.mode == bsr::Mode::Undefined);
    CHECK(options.threadno == 0);
    CHECK(options.message == "Usage:bsr\n"
        " <-p path>\n"
        " <-c condition>\n"
        " [-r replace]\n"
        " [-f filecondition default:.*]\n"
        " [-m mode [bin | hex | dbin | dhex] default:hex]\n"
        " [-n threadno default:1]");
}

TEST_CASE("no path", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == false);
    CHECK(options.path == "");
    CHECK(options.condition == "");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == "");
    CHECK(options.mode == bsr::Mode::Undefined);
    CHECK(options.threadno == 0);
    CHECK(options.message == "no path");
}

TEST_CASE("no condition", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == false);
    CHECK(options.path == "");
    CHECK(options.condition == "");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == "");
    CHECK(options.mode == bsr::Mode::Undefined);
    CHECK(options.threadno == 0);
    CHECK(options.message == "no condition");
}

TEST_CASE("replace", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-r", "yyy"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == true);
    CHECK(options.path == "..");
    CHECK(options.condition == "xxx");
    CHECK(options.replacement == "yyy");
    CHECK(options.filecondition == ".*");
    CHECK(options.mode == bsr::Mode::Hex);
    CHECK(options.threadno == 1);
    CHECK(options.message == "");
}

TEST_CASE("no replacement", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-r"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == false);
    CHECK(options.path == "");
    CHECK(options.condition == "");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == "");
    CHECK(options.mode == bsr::Mode::Undefined);
    CHECK(options.threadno == 0);
    CHECK(options.message == "no replacement");
}

TEST_CASE("file condition", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-f", ".*.txt"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == true);
    CHECK(options.path == "..");
    CHECK(options.condition == "xxx");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == ".*.txt");
    CHECK(options.mode == bsr::Mode::Hex);
    CHECK(options.threadno == 1);
    CHECK(options.message == "");
}

TEST_CASE("no filecondition", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-f"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == false);
    CHECK(options.path == "");
    CHECK(options.condition == "");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == "");
    CHECK(options.mode == bsr::Mode::Undefined);
    CHECK(options.threadno == 0);
    CHECK(options.message == "no filecondition");
}

TEST_CASE("stdin", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-c", "xxx"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == true);
    CHECK(options.path == "");
    CHECK(options.condition == "xxx");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == ".*");
    CHECK(options.mode == bsr::Mode::Hex);
    CHECK(options.threadno == 1);
    CHECK(options.message == "");
}

TEST_CASE("number of threads 1", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-n", "1"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == true);
    CHECK(options.path == "..");
    CHECK(options.condition == "xxx");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == ".*");
    CHECK(options.mode == bsr::Mode::Hex);
    CHECK(options.threadno == 1);
    CHECK(options.message == "");
}

TEST_CASE("number of threads 2", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-n", "2"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == true);
    CHECK(options.path == "..");
    CHECK(options.condition == "xxx");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == ".*");
    CHECK(options.mode == bsr::Mode::Hex);
    CHECK(options.threadno == 2);
    CHECK(options.message == "");
}

TEST_CASE("number of threads 2147483647", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-n", "2147483647"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == true);
    CHECK(options.path == "..");
    CHECK(options.condition == "xxx");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == ".*");
    CHECK(options.mode == bsr::Mode::Hex);
    CHECK(options.threadno == 2147483647);
    CHECK(options.message == "");
}

TEST_CASE("number of threads 0", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-n", "0"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == false);
    CHECK(options.path == "");
    CHECK(options.condition == "");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == "");
    CHECK(options.mode == bsr::Mode::Undefined);
    CHECK(options.threadno == 0);
    CHECK(options.message == "invalid threadno less than 1");
}

TEST_CASE("number of threads -1", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-n", "-1"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == false);
    CHECK(options.path == "");
    CHECK(options.condition == "");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == "");
    CHECK(options.mode == bsr::Mode::Undefined);
    CHECK(options.threadno == 0);
    CHECK(options.message == "invalid threadno less than 1");
}

TEST_CASE("number of threads @@@", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-n", "@@@"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == false);
    CHECK(options.path == "");
    CHECK(options.condition == "");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == "");
    CHECK(options.mode == bsr::Mode::Undefined);
    CHECK(options.threadno == 0);
    CHECK_THAT(options.message, Catch::Matchers::Contains("invalid threadno "));
}

TEST_CASE("number of threads 2147483648", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-n", "2147483648"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == false);
    CHECK(options.path == "");
    CHECK(options.condition == "");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == "");
    CHECK(options.mode == bsr::Mode::Undefined);
    CHECK(options.threadno == 0);
    CHECK_THAT(options.message, Catch::Matchers::Contains("invalid threadno "));
}

TEST_CASE("no threadno", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-n"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == false);
    CHECK(options.path == "");
    CHECK(options.condition == "");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == "");
    CHECK(options.mode == bsr::Mode::Undefined);
    CHECK(options.threadno == 0);
    CHECK(options.message == "no threadno");
}

TEST_CASE("unknown option", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "!!!", "???",
        "-p", "..",
        "-c", "xxx"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == false);
    CHECK(options.path == "");
    CHECK(options.condition == "");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == "");
    CHECK(options.mode == bsr::Mode::Undefined);
    CHECK(options.threadno == 0);
    CHECK(options.message == "unknown option \"!!!\"");
}

TEST_CASE("unknown option empty", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "", "???",
        "-p", "..",
        "-c", "xxx"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == false);
    CHECK(options.path == "");
    CHECK(options.condition == "");
    CHECK(options.replacement == "");
    CHECK(options.filecondition == "");
    CHECK(options.mode == bsr::Mode::Undefined);
    CHECK(options.threadno == 0);
    CHECK(options.message == "unknown option \"\"");
}

TEST_CASE("all", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-p", "..",
        "-c", "xxx",
        "-r", "yyy",
        "-f", "zzz",
        "-m", "dhex",
        "-n", "12"
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == true);
    CHECK(options.path == "..");
    CHECK(options.condition == "xxx");
    CHECK(options.replacement == "yyy");
    CHECK(options.filecondition == "zzz");
    CHECK(options.mode == bsr::Mode::DHex);
    CHECK(options.threadno == 12);
    CHECK(options.message == "");
}

TEST_CASE("all reverse", "[options]")
{
    Arguments args;
    auto [argc, argv] = args.make({
        "bsr",
        "-n", "12",
        "-m", "dhex",
        "-f", "zzz",
        "-r", "yyy",
        "-c", "xxx",
        "-p", ".."
    });
    bsr::Options options;
    REQUIRE(options.analysis(argc, argv) == true);
    CHECK(options.path == "..");
    CHECK(options.condition == "xxx");
    CHECK(options.replacement == "yyy");
    CHECK(options.filecondition == "zzz");
    CHECK(options.mode == bsr::Mode::DHex);
    CHECK(options.threadno == 12);
    CHECK(options.message == "");
}
