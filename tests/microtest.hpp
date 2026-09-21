#pragma once

// Minimal test runner, so the suite builds with the plain g++ command from the
// handout and needs no third-party dependency.

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace microtest
{

    struct Case
    {
        std::string name;
        std::function<void()> fn;
    };

    inline std::vector<Case> &registry()
    {
        static std::vector<Case> cases;
        return cases;
    }

    inline int &failureCount()
    {
        static int failures = 0;
        return failures;
    }

    inline std::string &currentCase()
    {
        static std::string name;
        return name;
    }

    struct Registrar
    {
        Registrar(const std::string &name, std::function<void()> fn)
        {
            registry().push_back({name, std::move(fn)});
        }
    };

    inline void reportFailure(const std::string &expr, const std::string &why,
                              const char *file, int line)
    {
        ++failureCount();
        std::cout << "  FAIL  " << currentCase() << '\n'
                  << "        " << file << ':' << line << '\n'
                  << "        " << why << '\n'
                  << "        failing expression: " << expr << '\n';
    }

    template <typename A, typename B>
    void reportUnequal(const A &actual, const B &expected, const std::string &expr,
                       const std::string &why, const char *file, int line)
    {
        std::ostringstream detail;
        detail << why << " (expected " << expected << ", got " << actual << ')';
        reportFailure(expr, detail.str(), file, line);
    }

    inline int run(const char *suiteName)
    {
        std::cout << "running " << registry().size() << " cases in " << suiteName << '\n';
        for (const Case &c : registry())
        {
            currentCase() = c.name;
            c.fn();
        }
        const int failures = failureCount();
        std::cout << (failures == 0 ? "OK" : "FAILED") << " - "
                  << registry().size() << " cases, " << failures << " failed\n";
        return failures == 0 ? 0 : 1;
    }

} // namespace microtest

// Assertions take a message so a failure names the broken rule, not just a line.
#define TEST(name)                                                   \
    static void name();                                              \
    static microtest::Registrar microtest_registrar_##name(#name, name); \
    static void name()

#define CHECK(cond, why)                                                    \
    do                                                                      \
    {                                                                       \
        if (!(cond))                                                        \
            microtest::reportFailure(#cond, (why), __FILE__, __LINE__);     \
    } while (0)

#define CHECK_EQ(actual, expected, why)                                          \
    do                                                                           \
    {                                                                            \
        const auto microtest_a = (actual);                                       \
        const auto microtest_b = (expected);                                     \
        if (!(microtest_a == microtest_b))                                       \
            microtest::reportUnequal(microtest_a, microtest_b,                   \
                                     #actual " == " #expected, (why),            \
                                     __FILE__, __LINE__);                        \
    } while (0)
