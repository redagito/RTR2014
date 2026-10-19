#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <fstream>
#include <string>

#include "io/IniFile.h"

namespace
{
class TempIni
{
   public:
    explicit TempIni(const std::string& content)
    {
        std::ofstream ofs(m_path);
        ofs << content;
    }
    ~TempIni() { std::remove(m_path.c_str()); }

    std::string path() const { return m_path; }

   private:
    std::string m_path = "rtr2014_tmp_test.ini";
};
} // namespace

TEST_CASE("IniFile parses groups and key value pairs", "[io]")
{
    TempIni file("[group]\nkey=value\n");
    IniFile ini;

    REQUIRE(ini.load(file.path()));
    REQUIRE(ini.hasGroup("group"));
    REQUIRE(ini.hasKey("group", "key"));
    REQUIRE(ini.getValue("group", "key", "fallback") == "value");
}

TEST_CASE("IniFile rejects a key line with no separator", "[io]")
{
    TempIni file("[group]\nkey_without_separator\n");
    IniFile ini;

    REQUIRE_FALSE(ini.load(file.path()));
}

TEST_CASE("IniFile rejects a group line that is never closed", "[io]")
{
    TempIni file("[unterminated\nkey=value\n");
    IniFile ini;

    REQUIRE_FALSE(ini.load(file.path()));
}

TEST_CASE("IniFile returns the default for a missing key", "[io]")
{
    TempIni file("[group]\nkey=value\n");
    IniFile ini;

    REQUIRE(ini.load(file.path()));
    REQUIRE(ini.getValue("group", "absent", "fallback") == "fallback");
    REQUIRE(ini.getValue("nogroup", "key", "fallback") == "fallback");
}

TEST_CASE("IniFile parses int and float values", "[io]")
{
    TempIni file("[group]\ncount=7\nscale=1.5\n");
    IniFile ini;

    REQUIRE(ini.load(file.path()));
    REQUIRE(ini.getValue("group", "count", 0) == 7);
    REQUIRE(ini.getValue("group", "scale", 0.f) == Catch::Approx(1.5f));
}