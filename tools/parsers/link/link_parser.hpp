#ifndef LINK_PARSER_HDR
#define LINK_PARSER_HDR


#include <boost/program_options.hpp>

#include "annotator.hpp"
#include "language_dependent_annotator_factory.hpp"
#include "lang_specific_processor_file_fetcher.hpp"


class Gobio : public Annotator {

public:

    class Factory : public LanguageDependentAnnotatorFactory {
        virtual Annotator* doCreateAnnotator(
            const boost::program_options::variables_map& options);

        virtual void doAddLanguageIndependentOptionsHandled(
            boost::program_options::options_description& optionsDescription);

        virtual std::string doGetName() const;
        virtual boost::filesystem::path doGetFile() const;

        virtual std::list<std::list<std::string> > doRequiredLayerTags();

        virtual std::list<std::list<std::string> > doOptionalLayerTags();

        virtual std::list<std::string> doProvidedLayerTags();
    };

    Gobio();

    void parse(Lattice &lattice);

private:

    class Worker : public LatticeWorker {
    public:
        Worker(Gobio& processor, Lattice& lattice);
    private:
        virtual void doRun();
        Gobio& processor_;
    };

    virtual LatticeWorker* doCreateLatticeWorker(Lattice& lattice);

    virtual std::string doInfo();

};


#endif
