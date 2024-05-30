#define CATCH_CONFIG_MAIN

#include <cassert>
#include <string>
#include <vector>

#include "catch.hpp"

#include "options.hpp"
#include "changer.hpp"

const char *char256 =
    "000102030405060708090a0b0c0d0e0f"
    "101112131415161718191a1b1c1d1e1f"
    "202122232425262728292a2b2c2d2e2f"
    "303132333435363738393a3b3c3d3e3f"
    "404142434445464748494a4b4c4d4e4f"
    "505152535455565758595a5b5c5d5e5f"
    "606162636465666768696a6b6c6d6e6f"
    "707172737475767778797a7b7c7d7e7f"
    "808182838485868788898a8b8c8d8e8f"
    "909192939495969798999a9b9c9d9e9f"
    "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf"
    "b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
    "c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
    "d0d1d2d3d4d5d6d7d8d9dadbdcdddedf"
    "e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
    "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff";

std::vector<char> getStoCV(const std::string &target)
{
    assert(target.size() % 2 == 0);
    std::vector<char> result;
    if (target.size() < 2)
    {
        return result;
    }
    for (std::size_t i = 0, end = (target.size() - 1); i < end; i += 2)
    {
        result.push_back(static_cast<char>(std::stoi(target.substr(i, 2), nullptr, 16)));
    }
    return result;
}

TEST_CASE("condition '', replace '' dhex", "[type]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    std::string error;
    REQUIRE(creater.getType("", "", error) == bsr::ProcessType::Error);
}

TEST_CASE("condition '1', replace '' dhex", "[type]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    std::string error;
    REQUIRE(creater.getType("1", "", error) == bsr::ProcessType::Search);
}

TEST_CASE("condition '1', replace '1' dhex", "[type]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    std::string error;
    REQUIRE(creater.getType("1", "1", error) == bsr::ProcessType::Replace);
}

TEST_CASE("condition '12', replace '1' dhex", "[type]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    std::string error;
    REQUIRE(creater.getType("12", "1", error) == bsr::ProcessType::Error);
}

TEST_CASE("search regex errror", "[search]")
{
    bsr::Creater creater(bsr::Mode::Hex);
    auto con = creater.condition();
    std::string error;
    REQUIRE(con->set("((01)", error) == false);
    CHECK_THAT(error, Catch::Matchers::Contains("regex error code="));
}

TEST_CASE("case ab012 dhex", "[search]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    std::string error;
    REQUIRE(con->set("ab012", error) == false);
}

TEST_CASE("case ab01 dhex", "[search]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("ab01", error) == true);
    tar->set(getStoCV("ab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("abab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab0101"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab02"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab11"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("abab02"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab0201"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("00ab10"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ac01"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV(""));
    CHECK(chan->search(*con, *tar, error) == false);
}

TEST_CASE("case ab01 dhex large", "[search]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("AB01", error) == true);
    tar->set(getStoCV("ab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("abab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab0101"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab02"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab11"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("abab02"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab0201"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("00ab10"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ac01"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV(""));
    CHECK(chan->search(*con, *tar, error) == false);
}

TEST_CASE("case ab?1 dhex", "[search]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("ab?1", error) == true);
    tar->set(getStoCV("ab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("abab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab0101"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab02"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab11"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("abab02"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab0201"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("00ab10"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ac01"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV(""));
    CHECK(chan->search(*con, *tar, error) == false);
}

TEST_CASE("case a??1 dhex", "[search]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("a??1", error) == true);
    tar->set(getStoCV("ab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("abab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab0101"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab02"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab11"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("abab02"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab0201"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("00ab10"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ac01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV(""));
    CHECK(chan->search(*con, *tar, error) == false);
}

TEST_CASE("case ???? dhex", "[search]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("????", error) == true);
    tar->set(getStoCV("ab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("abab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab0101"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab02"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab11"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("abab02"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab0201"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("00ab10"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ac01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV(""));
    CHECK(chan->search(*con, *tar, error) == false);
}

TEST_CASE("replace errror", "[replace]")
{
    bsr::Creater creater(bsr::Mode::Hex);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("ab01", error) == true);
    REQUIRE(rep->set("$9", error) == true);
    std::vector<char> result;
    tar->set(getStoCV("ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == false);
}

TEST_CASE("replace not errror", "[replace]")
{
    bsr::Creater creater(bsr::Mode::Hex);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("ab01", error) == true);
    REQUIRE(rep->set("$1", error) == true);
    std::vector<char> result;
    tar->set(getStoCV("ab01cd23"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd23"));
}

TEST_CASE("condition '', replace '' dbin", "[type]")
{
    bsr::Creater creater(bsr::Mode::DBin);
    std::string error;
    REQUIRE(creater.getType("", "", error) == bsr::ProcessType::Error);
}

TEST_CASE("condition '1', replace '' dbin", "[type]")
{
    bsr::Creater creater(bsr::Mode::DBin);
    std::string error;
    REQUIRE(creater.getType("1", "", error) == bsr::ProcessType::Search);
}

TEST_CASE("condition '1', replace '1' dbin", "[type]")
{
    bsr::Creater creater(bsr::Mode::DBin);
    std::string error;
    REQUIRE(creater.getType("1", "1", error) == bsr::ProcessType::Replace);
}

TEST_CASE("condition '12', replace '1' dbin", "[type]")
{
    bsr::Creater creater(bsr::Mode::DBin);
    std::string error;
    REQUIRE(creater.getType("12", "1", error) == bsr::ProcessType::Error);
}

TEST_CASE("case 10101011000000010 dbin", "[search]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    std::string error;
    REQUIRE(con->set("10101011000000010", error) == false);
}

TEST_CASE("case 1010101100000001 dbin", "[search]")
{
    bsr::Creater creater(bsr::Mode::DBin);
    auto con = creater.condition();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("1010101100000001", error) == true);
    tar->set(getStoCV("ab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("abab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab0101"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab02"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab11"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("abab02"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab0201"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("00ab10"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ac01"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV(""));
    CHECK(chan->search(*con, *tar, error) == false);
}

TEST_CASE("case 10101011000?0001 dbin", "[search]")
{
    bsr::Creater creater(bsr::Mode::DBin);
    auto con = creater.condition();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("10101011000?0001", error) == true);
    tar->set(getStoCV("ab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("abab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab0101"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("ab02"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab11"));
    CHECK(chan->search(*con, *tar, error) == true);
    tar->set(getStoCV("abab02"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab0201"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("00ab10"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ac01"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV("ab"));
    CHECK(chan->search(*con, *tar, error) == false);
    tar->set(getStoCV(""));
    CHECK(chan->search(*con, *tar, error) == false);
}

TEST_CASE("condition '', replace '' hex", "[type]")
{
    bsr::Creater creater(bsr::Mode::Hex);
    std::string error;
    REQUIRE(creater.getType("", "", error) == bsr::ProcessType::Error);
}

TEST_CASE("condition '1', replace '' hex", "[type]")
{
    bsr::Creater creater(bsr::Mode::Hex);
    std::string error;
    REQUIRE(creater.getType("1", "", error) == bsr::ProcessType::Search);
}

TEST_CASE("condition '1', replace '1' hex", "[type]")
{
    bsr::Creater creater(bsr::Mode::Hex);
    std::string error;
    REQUIRE(creater.getType("1", "1", error) == bsr::ProcessType::Replace);
}

TEST_CASE("condition '12', replace '1' hex", "[type]")
{
    bsr::Creater creater(bsr::Mode::Hex);
    std::string error;
    REQUIRE(creater.getType("12", "1", error) == bsr::ProcessType::Replace);
}

TEST_CASE("case ff, ff hex", "[replace]")
{
    bsr::Creater creater(bsr::Mode::Hex);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("ff", error) == true);
    REQUIRE(rep->set("ff", error) == true);
    tar->set(getStoCV(char256));
    std::vector<char> result;
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV(char256));
}

TEST_CASE("condition '', replace '' bin", "[type]")
{
    bsr::Creater creater(bsr::Mode::Bin);
    std::string error;
    REQUIRE(creater.getType("", "", error) == bsr::ProcessType::Error);
}

TEST_CASE("condition '1', replace '' bin", "[type]")
{
    bsr::Creater creater(bsr::Mode::Bin);
    std::string error;
    REQUIRE(creater.getType("1", "", error) == bsr::ProcessType::Search);
}

TEST_CASE("condition '1', replace '1' bin", "[type]")
{
    bsr::Creater creater(bsr::Mode::Bin);
    std::string error;
    REQUIRE(creater.getType("1", "1", error) == bsr::ProcessType::Replace);
}

TEST_CASE("condition '12', replace '1' bin", "[type]")
{
    bsr::Creater creater(bsr::Mode::Bin);
    std::string error;
    REQUIRE(creater.getType("12", "1", error) == bsr::ProcessType::Replace);
}

TEST_CASE("case 11111111, 11111111 bin", "[replace]")
{
    bsr::Creater creater(bsr::Mode::Bin);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("11111111", error) == true);
    REQUIRE(rep->set("11111111", error) == true);
    tar->set(getStoCV(char256));
    std::vector<char> result;
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV(char256));
}

TEST_CASE("case fff, ff dhex", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    std::string error;
    REQUIRE(con->set("fff", error) == false);
}

TEST_CASE("case ff, fff dhex", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto rep = creater.replacement();
    std::string error;
    REQUIRE(con->set("ff", error) == true);
    REQUIRE(rep->set("fff", error) == false);
}

TEST_CASE("case ffff, ff dhex", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto rep = creater.replacement();
    std::string error;
    REQUIRE(con->set("ffff", error) == true);
    REQUIRE(rep->set("ff", error) == true);
}

TEST_CASE("case ff, ff dhex", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("ff", error) == true);
    REQUIRE(rep->set("ff", error) == true);
    tar->set(getStoCV(char256));
    std::vector<char> result;
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV(char256));
}

TEST_CASE("case ab01, cd23 dhex", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("ab01", error) == true);
    REQUIRE(rep->set("cd23", error) == true);
    std::vector<char> result;
    tar->set(getStoCV("ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd23"));
    tar->set(getStoCV("abab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abcd23"));
    tar->set(getStoCV("ab0101"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd2301"));
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd23cd23"));
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00cd2300cd2300"));
    tar->set(getStoCV("ab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab02"));
    tar->set(getStoCV("ab11"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab11"));
    tar->set(getStoCV("abab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abab02"));
    tar->set(getStoCV("ab0201"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab0201"));
    tar->set(getStoCV("00ab10"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00ab10"));
    tar->set(getStoCV("ac01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ac01"));
    tar->set(getStoCV("ab"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab"));
    tar->set(getStoCV(""));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV(""));
}

TEST_CASE("case ab01, cd23 dhex large", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("AB01", error) == true);
    REQUIRE(rep->set("CD23", error) == true);
    std::vector<char> result;
    tar->set(getStoCV("ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd23"));
    tar->set(getStoCV("abab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abcd23"));
    tar->set(getStoCV("ab0101"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd2301"));
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd23cd23"));
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00cd2300cd2300"));
    tar->set(getStoCV("ab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab02"));
    tar->set(getStoCV("ab11"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab11"));
    tar->set(getStoCV("abab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abab02"));
    tar->set(getStoCV("ab0201"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab0201"));
    tar->set(getStoCV("00ab10"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00ab10"));
    tar->set(getStoCV("ac01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ac01"));
    tar->set(getStoCV("ab"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab"));
    tar->set(getStoCV(""));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV(""));
}

TEST_CASE("case ab?1, cd23 dhex", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("ab?1", error) == true);
    REQUIRE(rep->set("cd23", error) == true);
    std::vector<char> result;
    tar->set(getStoCV("ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd23"));
    tar->set(getStoCV("abab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abcd23"));
    tar->set(getStoCV("ab0101"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd2301"));
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd23cd23"));
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00cd2300cd2300"));
    tar->set(getStoCV("ab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab02"));
    tar->set(getStoCV("ab11"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd23"));
    tar->set(getStoCV("abab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abab02"));
    tar->set(getStoCV("ab0201"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab0201"));
    tar->set(getStoCV("00ab10"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00ab10"));
    tar->set(getStoCV("ac01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ac01"));
    tar->set(getStoCV("ab"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab"));
    tar->set(getStoCV(""));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV(""));
}

TEST_CASE("case ab01, cd?3 dhex", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("ab01", error) == true);
    REQUIRE(rep->set("cd?3", error) == true);
    std::vector<char> result;
    tar->set(getStoCV("ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd03"));
    tar->set(getStoCV("abab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abcd03"));
    tar->set(getStoCV("ab0101"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd0301"));
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd03cd03"));
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00cd0300cd0300"));
    tar->set(getStoCV("ab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab02"));
    tar->set(getStoCV("ab11"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab11"));
    tar->set(getStoCV("abab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abab02"));
    tar->set(getStoCV("ab0201"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab0201"));
    tar->set(getStoCV("00ab10"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00ab10"));
    tar->set(getStoCV("ac01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ac01"));
    tar->set(getStoCV("ab"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab"));
    tar->set(getStoCV(""));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV(""));
}

TEST_CASE("case ab?1, cd?3 dhex", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("ab?1", error) == true);
    REQUIRE(rep->set("cd?3", error) == true);
    std::vector<char> result;
    tar->set(getStoCV("ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd03"));
    tar->set(getStoCV("abab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abcd03"));
    tar->set(getStoCV("ab0101"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd0301"));
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd03cd03"));
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00cd0300cd0300"));
    tar->set(getStoCV("ab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab02"));
    tar->set(getStoCV("ab11"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd13"));
    tar->set(getStoCV("abab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abab02"));
    tar->set(getStoCV("ab0201"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab0201"));
    tar->set(getStoCV("00ab10"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00ab10"));
    tar->set(getStoCV("ac01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ac01"));
    tar->set(getStoCV("ab"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab"));
    tar->set(getStoCV(""));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV(""));
}

TEST_CASE("case a??1, c??3 dhex", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("a??1", error) == true);
    REQUIRE(rep->set("c??3", error) == true);
    std::vector<char> result;
    tar->set(getStoCV("ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cb03"));
    tar->set(getStoCV("abab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abcb03"));
    tar->set(getStoCV("ab0101"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cb0301"));
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cb03cb03"));
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00cb0300cb0300"));
    tar->set(getStoCV("ab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab02"));
    tar->set(getStoCV("ab11"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cb13"));
    tar->set(getStoCV("abab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abab02"));
    tar->set(getStoCV("ab0201"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab0201"));
    tar->set(getStoCV("00ab10"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00ab10"));
    tar->set(getStoCV("ac01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cc03"));
    tar->set(getStoCV("ab"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab"));
    tar->set(getStoCV(""));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV(""));
}

TEST_CASE("case a??1, ?d2? dhex", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("a??1", error) == true);
    REQUIRE(rep->set("?d2?", error) == true);
    std::vector<char> result;
    tar->set(getStoCV("ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ad21"));
    tar->set(getStoCV("abab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abad21"));
    tar->set(getStoCV("ab0101"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ad2101"));
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ad21ad21"));
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00ad2100ad2100"));
    tar->set(getStoCV("ab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab02"));
    tar->set(getStoCV("ab11"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ad21"));
    tar->set(getStoCV("abab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abab02"));
    tar->set(getStoCV("ab0201"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab0201"));
    tar->set(getStoCV("00ab10"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00ab10"));
    tar->set(getStoCV("ac01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ad21"));
    tar->set(getStoCV("ab"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab"));
    tar->set(getStoCV(""));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV(""));
}

TEST_CASE("case ????, ???? dhex", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DHex);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("????", error) == true);
    REQUIRE(rep->set("????", error) == true);
    std::vector<char> result;
    tar->set(getStoCV("ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab01"));
    tar->set(getStoCV("abab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abab01"));
    tar->set(getStoCV("ab0101"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab0101"));
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab01ab01"));
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00ab0100ab0100"));
    tar->set(getStoCV("ab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab02"));
    tar->set(getStoCV("ab11"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab11"));
    tar->set(getStoCV("abab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abab02"));
    tar->set(getStoCV("ab0201"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab0201"));
    tar->set(getStoCV("00ab10"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00ab10"));
    tar->set(getStoCV("ac01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ac01"));
    tar->set(getStoCV("ab"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab"));
    tar->set(getStoCV(""));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV(""));
}

TEST_CASE("case 111111111, 11111111 dbin", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DBin);
    auto con = creater.condition();
    std::string error;
    REQUIRE(con->set("111111111", error) == false);
}

TEST_CASE("case 11111111, 111111111 dbin", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DBin);
    auto con = creater.condition();
    auto rep = creater.replacement();
    std::string error;
    REQUIRE(con->set("11111111", error) == true);
    REQUIRE(rep->set("111111111", error) == false);
}

TEST_CASE("case 1111111100000000, 11111111 dbin", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DBin);
    auto con = creater.condition();
    auto rep = creater.replacement();
    std::string error;
    REQUIRE(con->set("1111111100000000", error) == true);
    REQUIRE(rep->set("11111111", error) == true);
}

TEST_CASE("case 11111111, 11111111 dbin", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DBin);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("11111111", error) == true);
    REQUIRE(rep->set("11111111", error) == true);
    std::vector<char> result;
    tar->set(getStoCV(char256));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV(char256));
}

TEST_CASE("case 1010101100000001, 1100110100100011 dbin", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DBin);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("1010101100000001", error) == true);
    REQUIRE(rep->set("1100110100100011", error) == true);
    std::vector<char> result;
    tar->set(getStoCV("ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd23"));
    tar->set(getStoCV("abab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abcd23"));
    tar->set(getStoCV("ab0101"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd2301"));
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd23cd23"));
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00cd2300cd2300"));
    tar->set(getStoCV("ab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab02"));
    tar->set(getStoCV("ab11"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab11"));
    tar->set(getStoCV("abab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abab02"));
    tar->set(getStoCV("ab0201"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab0201"));
    tar->set(getStoCV("00ab10"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00ab10"));
    tar->set(getStoCV("ac01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ac01"));
    tar->set(getStoCV("ab"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab"));
    tar->set(getStoCV(""));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV(""));
}

TEST_CASE("case 10101011000?0001, 1100110100100011 dbin", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DBin);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("10101011000?0001", error) == true);
    REQUIRE(rep->set("1100110100100011", error) == true);
    std::vector<char> result;
    tar->set(getStoCV("ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd23"));
    tar->set(getStoCV("abab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abcd23"));
    tar->set(getStoCV("ab0101"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd2301"));
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd23cd23"));
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00cd2300cd2300"));
    tar->set(getStoCV("ab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab02"));
    tar->set(getStoCV("ab11"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd23"));
    tar->set(getStoCV("abab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abab02"));
    tar->set(getStoCV("ab0201"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab0201"));
    tar->set(getStoCV("00ab10"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00ab10"));
    tar->set(getStoCV("ac01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ac01"));
    tar->set(getStoCV("ab"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab"));
    tar->set(getStoCV(""));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV(""));
}

TEST_CASE("case 10101011000?0001, 11001101001?0011 dbin", "[replace]")
{
    bsr::Creater creater(bsr::Mode::DBin);
    auto con = creater.condition();
    auto rep = creater.replacement();
    auto tar = creater.target();
    auto chan = creater.changer();
    std::string error;
    REQUIRE(con->set("10101011000?0001", error) == true);
    REQUIRE(rep->set("11001101001?0011", error) == true);
    std::vector<char> result;
    tar->set(getStoCV("ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd23"));
    tar->set(getStoCV("abab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abcd23"));
    tar->set(getStoCV("ab0101"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd2301"));
    tar->set(getStoCV("ab01ab01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd23cd23"));
    tar->set(getStoCV("00ab0100ab0100"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00cd2300cd2300"));
    tar->set(getStoCV("ab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab02"));
    tar->set(getStoCV("ab11"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("cd33"));
    tar->set(getStoCV("abab02"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("abab02"));
    tar->set(getStoCV("ab0201"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab0201"));
    tar->set(getStoCV("00ab10"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("00ab10"));
    tar->set(getStoCV("ac01"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ac01"));
    tar->set(getStoCV("ab"));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV("ab"));
    tar->set(getStoCV(""));
    CHECK(chan->replace(*con, *rep, *tar, result, error) == true);
    CHECK(result == getStoCV(""));
}
