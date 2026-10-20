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

// Writes the content byte for byte, so the line endings are not translated.
class TempIniBytes
{
   public:
    explicit TempIniBytes(const std::string& content)
    {
        std::ofstream ofs(m_path, std::ios::binary);
        ofs << content;
    }
    ~TempIniBytes() { std::remove(m_path.c_str()); }

    std::string path() const { return m_path; }

   private:
    std::string m_path = "rtr2014_tmp_test_bytes.ini";
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

TEST_CASE("IniFile parses a file with windows line endings", "[io]")
{
    TempIniBytes crlfFile("[group]\r\nkey=value\r\n");
    TempIniBytes lfFile("[group]\nkey=value\n");
    IniFile crlfIni;
    IniFile lfIni;

    REQUIRE(crlfIni.load(crlfFile.path()));
    REQUIRE(lfIni.load(lfFile.path()));
    REQUIRE(crlfIni.getValue("group", "key", "fallback") ==
            lfIni.getValue("group", "key", "fallback"));
    REQUIRE(crlfIni.getValue("group", "key", "fallback") == "value");
}

TEST_CASE("IniFile ignores whitespace around a key and a value", "[io]")
{
    TempIniBytes file("[group]\r\n  key  =  value  \r\n");
    IniFile ini;

    REQUIRE(ini.load(file.path()));
    REQUIRE(ini.hasKey("group", "key"));
    REQUIRE(ini.getValue("group", "key", "fallback") == "value");
}

TEST_CASE("IniFile keeps the precision of a float value", "[io]")
{
    TempIni file("[group]\n");
    IniFile ini;

    REQUIRE(ini.load(file.path()));

    const float value = 1.2345678f;
    ini.setValue("group", "key", value);
    REQUIRE(ini.save(file.path()));

    IniFile reloaded;
    REQUIRE(reloaded.load(file.path()));
    REQUIRE(reloaded.getValue("group", "key", 0.f) == value);
}

TEST_CASE("IniFile reads the material files shipped with the project", "[io]")
{
    // The shipped ini files use windows line endings, the paths they contain
    // must not carry a carriage return.
    IniFile ini;

    REQUIRE(ini.load(std::string(RTR_PROJECT_DATA_DIR) + "/material/brick_1_diffuse.ini"));
    REQUIRE(ini.getValue("diffuse", "file", "") == "data/images/brick_1_diffuse.png");
}