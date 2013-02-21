#include "tests.hpp"

#include <boost/assign/list_of.hpp>

#include "unumsunt.hpp"


BOOST_AUTO_TEST_SUITE( unumsunt )


BOOST_AUTO_TEST_CASE( unumsunt_simple ) {

    Unumsunt converter("pl", ROOT_DIR "tools/converters/unumsunt/t/files/test.u");

    AnnotationItemManager aim;
    Lattice lattice(aim);
    std::string paragraph = "text";
    lattice.appendStringWithSymbols(paragraph);

    LayerTagCollection tags1(lattice.getLayerTagManager().createTagCollectionFromList(
        boost::assign::list_of("source-tagset")("tag1")));
    LayerTagCollection tags2(lattice.getLayerTagManager().createTagCollectionFromList(
        boost::assign::list_of("source-tagset")("tag2")));
    LayerTagCollection tags3(lattice.getLayerTagManager().createTagCollectionFromList(
        boost::assign::list_of("source-tagset")("tag3")));

    LayerTagMask mask1 = lattice.getLayerTagManager().getMaskFromList(
        boost::assign::list_of("target-tagset")("tag1"));
    LayerTagMask mask2 = lattice.getLayerTagManager().getMaskFromList(
        boost::assign::list_of("target-tagset")("tag2"));
    LayerTagMask mask3 = lattice.getLayerTagManager().getMaskFromList(
        boost::assign::list_of("target-tagset")("tag3"));

    Lattice::VertexDescriptor first = lattice.getFirstVertex();
    Lattice::VertexDescriptor last = lattice.getLastVertex();

    AnnotationItem ai1("c", paragraph);
    AnnotationItem ai2("category", paragraph);
    AnnotationItem ai3("category", paragraph);
    lattice.getAnnotationItemManager().setValue(ai3, "attribute", "value");

    Lattice::EdgeDescriptor edge1 = lattice.addEdge(first, last, ai1, tags1);
    Lattice::EdgeSequence::Builder builder1(lattice);
    builder1.addEdge(edge1);
    Lattice::EdgeDescriptor edge2 = lattice.addEdge(first, last, ai2, tags2, builder1.build());
    Lattice::EdgeSequence::Builder builder2(lattice);
    builder2.addEdge(edge2);
    lattice.addEdge(first, last, ai3, tags3, builder2.build());

    converter.convertTags(lattice);

    Lattice::EdgesSortedByTargetIterator ei1 = lattice.edgesSortedByTarget(mask1);
    BOOST_CHECK(ei1.hasNext());
    AnnotationItem tai1 = lattice.getEdgeAnnotationItem(ei1.next());
    BOOST_CHECK_EQUAL(tai1.getCategory(), "c");

    Lattice::EdgesSortedByTargetIterator ei2 = lattice.edgesSortedByTarget(mask2);
    BOOST_CHECK(ei2.hasNext());
    AnnotationItem tai2 = lattice.getEdgeAnnotationItem(ei2.next());
    BOOST_CHECK_EQUAL(tai2.getCategory(), "Kategoria");

    Lattice::EdgesSortedByTargetIterator ei3 = lattice.edgesSortedByTarget(mask3);
    BOOST_CHECK(ei3.hasNext());
    AnnotationItem tai3 = lattice.getEdgeAnnotationItem(ei3.next());
    BOOST_CHECK_EQUAL(tai3.getCategory(), "Kategoria");
    BOOST_CHECK_EQUAL(
        lattice.getAnnotationItemManager().getValueAsString(tai3, "Atrybut"),
        "Wartość");

}


BOOST_AUTO_TEST_SUITE_END()
