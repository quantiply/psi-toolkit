#include "tests.hpp"

#include <fstream>

#include <boost/scoped_ptr.hpp>

#include "psi_lattice_writer.hpp"
#include "writers_tests_utils.hpp"


BOOST_AUTO_TEST_SUITE( psi_lattice_writer )


BOOST_AUTO_TEST_CASE( psi_lattice_writer_simple ) {
    Lattice lattice;
    writers_tests_utils::prepareSimpleLattice(lattice);

    boost::scoped_ptr<LatticeWriter<std::ostream> > writer(new PsiLatticeWriter());

    std::ostringstream osstr;
    writer->writeLattice(lattice, osstr);

    std::string line;
    std::string contents;
    std::ifstream s(ROOT_DIR "formats/psi/t/files/pl_sample_simple.txt");
    while (getline(s, line)) {
        contents += line;
        contents += "\n";
    }

    BOOST_CHECK_EQUAL(osstr.str(), contents);

}


BOOST_AUTO_TEST_CASE( psi_lattice_writer_advanced ) {
    Lattice lattice;
    writers_tests_utils::prepareAdvancedLattice(lattice);

    boost::scoped_ptr<LatticeWriter<std::ostream> > writer(new PsiLatticeWriter());

    std::ostringstream osstr;
    writer->writeLattice(lattice, osstr);

    std::string line;
    std::string contents;
    std::ifstream s(ROOT_DIR "formats/psi/t/files/pl_sample_nocomments.txt");
    while (getline(s, line)) {
        contents += line;
        contents += "\n";
    }

    BOOST_CHECK_EQUAL(osstr.str(), contents);
}


BOOST_AUTO_TEST_SUITE_END()
