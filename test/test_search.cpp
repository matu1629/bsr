#define CATCH_CONFIG_MAIN

#include <array>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

#include "catch.hpp"

#include "options.hpp"
#include "search.hpp"

std::string makePath(const std::vector<std::string> &element)
{
    std::filesystem::path path;
    for (const auto &e : element)
    {
        path /= e;
    }
    return path.string();
}

bool createFile(const std::string &filename, const std::vector<int> &data)
{
    std::fstream fs(filename, (std::ios_base::out | std::ios_base::trunc | std::ios_base::binary));
    if (!fs.is_open())
    {
        return false;
    }
    char c = 0;
    for (const auto i : data)
    {
        c = static_cast<int>(i);
        fs.write(&c, 1);
    }
    fs.close();
    return true;
}

const std::string dir = makePath({"..", "test_files"});
const std::string file_a = makePath({dir, "a.txt"});
const std::string file_b = makePath({dir, "b.txt"});
const std::string file_c = makePath({dir, "c.txt"});
const std::string file_d = makePath({dir, "d.txt"});
const std::string file_e = makePath({dir, "e.txt"});
const std::string file_f = makePath({dir, "f.txt"});
const std::string file_g = makePath({dir, "g.dat"});
const std::string file_h = makePath({dir, "h.dat"});
const std::string file_i = makePath({dir, "i.dat"});

void createDefaultData()
{
    const std::vector<std::pair<std::string, std::vector<int>>> data{
        {file_a, {0xab, 0x01}},
        {file_b, {0x00, 0xab, 0x01, 0x00}},
        {file_c, {0xab, 0x11}},
        {file_d, {0xab, 0xab}},
        {file_e, {0xab, 0x91, 0xab, 0x81, 0xab, 0x71, 0xab, 0x61, 0xab, 0x51, 0xab, 0x41, 0xab, 0x31, 0xab, 0x21, 0xab, 0x11, 0xab, 0x01}},
        {file_f, {}},
        {file_g, {0xab, 0x01}},
        {file_h, {0xab, 0xab}},
        {file_i, {0xfe, 0x01}}
    };
    std::filesystem::remove_all(dir);
    std::filesystem::create_directory(dir);
    for (const auto &d : data)
    {
        createFile(d.first, d.second);
    }
}

TEST_CASE("invalid path", "[check]")
{
    bsr::Search search;
    REQUIRE(search.execute("@@@", "1010101100000001", "1100110100100011", ".*", bsr::Mode::Bin, 1) == false);
    REQUIRE(search.error == "invalid path");
    REQUIRE(search.result.size() == 0);
}

TEST_CASE("no path", "[check]")
{
    std::filesystem::remove_all(dir);
    bsr::Search search;
    REQUIRE(search.execute(dir, "1010101100000001", "1100110100100011", ".*", bsr::Mode::Bin, 1) == false);
    REQUIRE(search.error == "invalid path");
    REQUIRE(search.result.size() == 0);
}

TEST_CASE("no file", "[check]")
{
    std::filesystem::remove_all(dir);
    std::filesystem::create_directory(dir);
    bsr::Search search;
    REQUIRE(search.execute(dir, "1010101100000001", "1100110100100011", ".*", bsr::Mode::Bin, 1) == false);
    REQUIRE(search.error == "no file");
    REQUIRE(search.result.size() == 0);
    std::filesystem::remove_all(dir);
}

TEST_CASE("search bin | hex | dbin | dhex", "[search]")
{
    createDefaultData();

    SECTION("search regex error")
    {
        bsr::Search search;
        REQUIRE(search.execute(dir, "((01)", "", ".*", bsr::Mode::Bin, 1) == false);
        CHECK(search.result.size() == 0);
        CHECK_THAT(search.error, Catch::Matchers::Contains("regex error code="));
    }

    SECTION("search 1010101100000001 bin")
    {
        std::vector<std::string> result{file_a, file_b, file_e, file_g};
        bsr::Search search;
        REQUIRE(search.execute(dir, "1010101100000001", "", ".*", bsr::Mode::Bin, 1) == true);
        REQUIRE(search.result.size() == 4);
        CHECK(search.result == result);
    }

    SECTION("search 10101011(\\d{8}) bin")
    {
        std::vector<std::string> result{file_a, file_b, file_c, file_d, file_e, file_g, file_h};
        bsr::Search search;
        REQUIRE(search.execute(dir, "10101011(\\d{8})", "", ".*", bsr::Mode::Bin, 1) == true);
        REQUIRE(search.result.size() == 7);
        CHECK(search.result == result);
    }

    SECTION("search 1010101100000001 bin multi-2")
    {
        std::vector<std::string> result{file_a, file_b, file_e, file_g};
        bsr::Search search;
        REQUIRE(search.execute(dir, "1010101100000001", "", ".*", bsr::Mode::Bin, 2) == true);
        REQUIRE(search.result.size() == 4);
        CHECK(search.result == result);
    }

    SECTION("search 1010101100000001 bin multi-10")
    {
        std::vector<std::string> result{file_a, file_b, file_e, file_g};
        bsr::Search search;
        REQUIRE(search.execute(dir, "1010101100000001", "", ".*", bsr::Mode::Bin, 10) == true);
        REQUIRE(search.result.size() == 4);
        CHECK(search.result == result);
    }

    SECTION("search 1010101100000001 bin pipe")
    {
        std::vector<std::string> input{file_a, file_b, file_c, file_d, file_e, file_f, file_g, file_h, file_i, file_a};
        std::stringstream ss;
        for (const auto &i : input)
        {
            ss << i << std::endl;
        }
        std::streambuf *buf = std::cin.rdbuf(ss.rdbuf());
        std::vector<std::string> result{file_a, file_b, file_e, file_g};
        bsr::Search search;
        REQUIRE(search.execute("", "1010101100000001", "", ".*", bsr::Mode::Bin, 1) == true);
        REQUIRE(search.result.size() == 4);
        CHECK(search.result == result);
        std::cin.rdbuf(buf);
    }

    SECTION("search '' bin")
    {
        std::vector<std::string> result{};
        bsr::Search search;
        REQUIRE(search.execute(dir, "", "", ".*", bsr::Mode::Bin, 1) == false);
        REQUIRE(search.result.size() == 0);
        CHECK(search.result == result);
    }

    SECTION("search ab01 hex")
    {
        std::vector<std::string> result{file_a, file_b, file_e, file_g};
        bsr::Search search;
        REQUIRE(search.execute(dir, "ab01", "", ".*", bsr::Mode::Hex, 1) == true);
        REQUIRE(search.result.size() == 4);
        CHECK(search.result == result);
    }

    SECTION("search AB01 hex")
    {
        std::vector<std::string> result{file_a, file_b, file_e, file_g};
        bsr::Search search;
        REQUIRE(search.execute(dir, "AB01", "", ".*", bsr::Mode::Hex, 1) == true);
        REQUIRE(search.result.size() == 4);
        CHECK(search.result == result);
    }

    SECTION("search ab(\\d{2}) hex")
    {
        std::vector<std::string> result{file_a, file_b, file_c, file_e, file_g};
        bsr::Search search;
        REQUIRE(search.execute(dir, "ab(\\d{2})", "", ".*", bsr::Mode::Hex, 1) == true);
        REQUIRE(search.result.size() == 5);
        CHECK(search.result == result);
    }

    SECTION("search 1010101100000001 dbin")
    {
        std::vector<std::string> result{file_a, file_b, file_e, file_g};
        bsr::Search search;
        REQUIRE(search.execute(dir, "1010101100000001", "", ".*", bsr::Mode::DBin, 1) == true);
        REQUIRE(search.result.size() == 4);
        CHECK(search.result == result);
    }

    SECTION("search 10101011000?0001 dbin")
    {
        std::vector<std::string> result{file_a, file_b, file_c, file_e, file_g};
        bsr::Search search;
        REQUIRE(search.execute(dir, "10101011000?0001", "", ".*", bsr::Mode::DBin, 1) == true);
        REQUIRE(search.result.size() == 5);
        CHECK(search.result == result);
    }

    SECTION("search 1010101100000001 dbin multi-2")
    {
        std::vector<std::string> result{file_a, file_b, file_e, file_g};
        bsr::Search search;
        REQUIRE(search.execute(dir, "1010101100000001", "", ".*", bsr::Mode::DBin, 2) == true);
        REQUIRE(search.result.size() == 4);
        CHECK(search.result == result);
    }

    SECTION("search 1010101100000001 dbin multi-10")
    {
        std::vector<std::string> result{file_a, file_b, file_e, file_g};
        bsr::Search search;
        REQUIRE(search.execute(dir, "1010101100000001", "", ".*", bsr::Mode::DBin, 10) == true);
        REQUIRE(search.result.size() == 4);
        CHECK(search.result == result);
    }

    SECTION("search ab01 dhex")
    {
        std::vector<std::string> result{file_a, file_b, file_e, file_g};
        bsr::Search search;
        REQUIRE(search.execute(dir, "ab01", "", ".*", bsr::Mode::DHex, 1) == true);
        REQUIRE(search.result.size() == 4);
        CHECK(search.result == result);
    }

    SECTION("search AB01 dhex")
    {
        std::vector<std::string> result{file_a, file_b, file_e, file_g};
        bsr::Search search;
        REQUIRE(search.execute(dir, "AB01", "", ".*", bsr::Mode::DHex, 1) == true);
        REQUIRE(search.result.size() == 4);
        CHECK(search.result == result);
    }

    SECTION("search ab?1 dhex")
    {
        std::vector<std::string> result{file_a, file_b, file_c, file_e, file_g};
        bsr::Search search;
        REQUIRE(search.execute(dir, "ab?1", "", ".*", bsr::Mode::DHex, 1) == true);
        REQUIRE(search.result.size() == 5);
        CHECK(search.result == result);
    }

    std::filesystem::remove_all(dir);
}

TEST_CASE("replace error bin", "[replace]")
{
    createDefaultData();
    std::vector<std::string> result{};
    bsr::Search search;
    REQUIRE(search.execute(dir, "1010101100000001", "@@@@", ".*", bsr::Mode::Bin, 1) == false);
    REQUIRE(search.result.size() == 0);
    CHECK(search.result == result);
    std::filesystem::remove_all(dir);
}

TEST_CASE("replace error bin multi-2", "[replace]")
{
    createDefaultData();
    std::vector<std::string> result{};
    bsr::Search search;
    REQUIRE(search.execute(dir, "1010101100000001", "@@@@", ".*", bsr::Mode::Bin, 2) == false);
    REQUIRE(search.result.size() == 0);
    CHECK(search.result == result);
    std::filesystem::remove_all(dir);
}

TEST_CASE("replace 1010101100000001, 1100110100100011 bin", "[replace]")
{
    createDefaultData();
    std::vector<std::string> result{file_a, file_b, file_e, file_g};
    bsr::Search search;
    REQUIRE(search.execute(dir, "1010101100000001", "1100110100100011", ".*", bsr::Mode::Bin, 1) == true);
    REQUIRE(search.result.size() == 4);
    CHECK(search.result == result);
    std::filesystem::remove_all(dir);
}

TEST_CASE("replace 10101011(\\d{8}), 11001101$1 bin", "[replace]")
{
    createDefaultData();
    std::vector<std::string> result{file_a, file_b, file_c, file_d, file_e, file_g, file_h};
    bsr::Search search;
    REQUIRE(search.execute(dir, "10101011(\\d{8})", "11001101$1", ".*", bsr::Mode::Bin, 1) == true);
    REQUIRE(search.result.size() == 7);
    CHECK(search.result == result);

    std::filesystem::remove_all(dir);
}

TEST_CASE("replace 1010101100000001, 1100110100100011 bin multi-2", "[replace]")
{
    createDefaultData();
    std::vector<std::string> result{file_a, file_b, file_e, file_g};
    bsr::Search search;
    REQUIRE(search.execute(dir, "1010101100000001", "1100110100100011", ".*", bsr::Mode::Bin, 2) == true);
    REQUIRE(search.result.size() == 4);
    CHECK(search.result == result);

    std::filesystem::remove_all(dir);
}

TEST_CASE("replace 1010101100000001, 1100110100100011 bin multi-10", "[replace]")
{
    createDefaultData();
    std::vector<std::string> result{file_a, file_b, file_e, file_g};
    bsr::Search search;
    REQUIRE(search.execute(dir, "1010101100000001", "1100110100100011", ".*", bsr::Mode::Bin, 10) == true);
    REQUIRE(search.result.size() == 4);
    CHECK(search.result == result);
    std::filesystem::remove_all(dir);
}

TEST_CASE("replace ab01, cd23 hex", "[replace]")
{
    createDefaultData();
    std::vector<std::string> result{file_a, file_b, file_e, file_g};
    bsr::Search search;
    REQUIRE(search.execute(dir, "ab01", "cd23", ".*", bsr::Mode::Hex, 1) == true);
    REQUIRE(search.result.size() == 4);
    CHECK(search.result == result);
    std::filesystem::remove_all(dir);
}

TEST_CASE("replace AB01, CD23 hex", "[replace]")
{
    createDefaultData();
    std::vector<std::string> result{file_a, file_b, file_e, file_g};
    bsr::Search search;
    REQUIRE(search.execute(dir, "AB01", "CD23", ".*", bsr::Mode::Hex, 1) == true);
    REQUIRE(search.result.size() == 4);
    CHECK(search.result == result);
    std::filesystem::remove_all(dir);
}

TEST_CASE("replace ab(\\d{2}), cd$1 hex", "[replace]")
{
    createDefaultData();
    std::vector<std::string> result{file_a, file_b, file_c, file_e, file_g};
    bsr::Search search;
    REQUIRE(search.execute(dir, "ab(\\d{2})", "cd$1", ".*", bsr::Mode::Hex, 1) == true);
    REQUIRE(search.result.size() == 5);
    CHECK(search.result == result);
    std::filesystem::remove_all(dir);
}

TEST_CASE("replace 1010101100000001, 1100110100100011 dbin", "[replace]")
{
    createDefaultData();
    std::vector<std::string> result{file_a, file_b, file_e, file_g};
    bsr::Search search;
    REQUIRE(search.execute(dir, "1010101100000001", "1100110100100011", ".*", bsr::Mode::DBin, 1) == true);
    REQUIRE(search.result.size() == 4);
    CHECK(search.result == result);
    std::filesystem::remove_all(dir);
}

TEST_CASE("replace 1010101100000001, 1100110100100011 dbin multi-2", "[replace]")
{
    createDefaultData();
    std::vector<std::string> result{file_a, file_b, file_e, file_g};
    bsr::Search search;
    REQUIRE(search.execute(dir, "1010101100000001", "1100110100100011", ".*", bsr::Mode::DBin, 2) == true);
    REQUIRE(search.result.size() == 4);
    CHECK(search.result == result);
    std::filesystem::remove_all(dir);
}

TEST_CASE("replace 1010101100000001, 1100110100100011 dbin multi-10", "[replace]")
{
    createDefaultData();
    std::vector<std::string> result{file_a, file_b, file_e, file_g};
    bsr::Search search;
    REQUIRE(search.execute(dir, "1010101100000001", "1100110100100011", ".*", bsr::Mode::DBin, 10) == true);
    REQUIRE(search.result.size() == 4);
    CHECK(search.result == result);
    std::filesystem::remove_all(dir);
}