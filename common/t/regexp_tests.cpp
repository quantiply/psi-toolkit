#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

#include "regexp.hpp"


BOOST_AUTO_TEST_CASE( regexp_utf8 ) {
    RegExp re("(ż+)");

    std::string arg;
    BOOST_CHECK(RegExp::FullMatch("żżż", re, &arg));
    BOOST_CHECK_EQUAL(arg, "żżż");

    PerlRegExp perlRe("(ż+)");

    std::string perlArg;
    BOOST_CHECK(PerlRegExp::FullMatch("żżż", perlRe, &perlArg));
    BOOST_CHECK_EQUAL(perlArg, "żżż");
}

