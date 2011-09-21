#include "psi_lattice_reader.hpp"


std::string PsiLatticeReader::getFormatName() {
    return "Psi";
}

std::string PsiLatticeReader::doInfo() {
    return "Psi reader";
}

PsiLatticeReader::Worker::Worker(PsiLatticeReader& processor,
                                 std::istream& inputStream,
                                 Lattice& lattice):
    ReaderWorker(inputStream, lattice), processor_(processor) {
}

void PsiLatticeReader::Worker::doRun() {
    PsiQuoter quoter;
    PsiLRGrammar grammar;
    std::string line;
    while (std::getline(inputStream_, line)) {
        PsiLRItem item;
        std::string::const_iterator begin = line.begin();
        std::string::const_iterator end = line.end();
        if (parse(begin, end, grammar, item)) {

            item.unescape(quoter);

std::stringstream tagsSs;
BOOST_FOREACH(std::string tag, item.tags) {
if (!tagsSs.str().empty()) {
tagsSs << "+";
}
tagsSs << tag;
}

DEBUG("Reading line " << ((item.ordinal < 10)?" ":"") << item.ordinal
    << "|" << (item.beginningLoose?"@":" ")
    << "|" << (item.lengthPoint?"*":" ") << (item.lengthLoose?"@":" ")
    << "|" << tagsSs.str()
    << "|" << item.annotationText
    << "|");

        // } else {

// DEBUG("Line '" << line.substr(0,8) << "' not parsed. " << ((begin==line.begin())?"At all.":"A bit."));

        }
    }
}

