#define BOOST_TEST_NO_MAIN
#include <boost/test/unit_test.hpp>

#include "xml_property_tree.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/version.hpp>

#include "config.h"

BOOST_AUTO_TEST_CASE( xml_property_tree_simple ) {
    XmlPropertyTree xmlTree(ROOT_DIR "common/t/simple.xml");

    BOOST_CHECK_EQUAL(
        xmlTree.get<std::string>("root.foo.bar"),
        "Źdźbło & źdźbło");
}

BOOST_AUTO_TEST_CASE( xml_property_tree_broken ) {
    XmlPropertyTree xmlTree(ROOT_DIR "common/t/broken.xml");

    BOOST_CHECK_EQUAL(
        xmlTree.get<std::string>("root.foo"),
        "Broken XML File.");
}


BOOST_AUTO_TEST_CASE( xml_property_tree_nonexisting ) {
    boost::scoped_ptr<XmlPropertyTree> xmlTree;

    BOOST_CHECK_THROW(
        xmlTree.reset(new XmlPropertyTree(ROOT_DIR "common/t/nonexisting.xml")),
        std::exception);
}


BOOST_AUTO_TEST_CASE( xml_property_tree_spaces ) {
    XmlPropertyTree xmlTree(ROOT_DIR "common/t/spaces.xml");

    // in Boost 1.42 all spaces are trimmed (a bug??)
#if BOOST_VERSION != 14200
    BOOST_CHECK_EQUAL(
        xmlTree.get<std::string>("root.foo"), "  ");
#endif
}


BOOST_AUTO_TEST_CASE( xml_property_tree_space_entity ) {
    XmlPropertyTree xmlTree(ROOT_DIR "common/t/space-entity.xml");

    BOOST_CHECK_EQUAL(
        xmlTree.get<std::string>("root.foo"), "   ");
}
