#include <boost/algorithm/string/join.hpp>

#include "html_help_formatter.hpp"

#include "sundown/cpp/stringwrapper.hpp"

void HtmlHelpFormatter::doFormatOneProcessorHelp(
    std::string processorName,
    std::string description,
    boost::program_options::options_description options,
    std::vector<TestBatch> usingExamples,
    std::ostream& output) {

    output << "<div class=\"help-item\">"
        << "<h2><a name=\"" << processorName << "\">" << processorName << "</a></h2>" << std::endl;

    if (description.size() != 0) {
        output << "<div class=\"help-desc\">" << markdownString2String(description)
            << "</div>" << std::endl;
    }

    if (!usingExamples.empty()) {
        formatUsingExamples_(usingExamples, output);
    }

    formatAllowedOptions_(options, output);
    output << "</div>" << std::endl;
}

void HtmlHelpFormatter::formatUsingExamples_(std::vector<TestBatch> batches, std::ostream& output) {
    output << "<div class=\"help-example\">"
        << "<h3>" << EXAMPLES_HEADER << "</h3>" << std::endl;

    for (unsigned int i = 0; i < batches.size(); i++) {
       output << "<code class=\"example-pipe\">"
            << batches[i].getPipeline() << "</code>" << std::endl;

       output << "<div class=\"example-desc\">"
            << markdownString2String(batches[i].getDescription()) << "</div>" << std::endl;

        std::vector<TestRun> inOuts = batches[i].getTestRuns();
        for (unsigned int j = 0; j < inOuts.size(); j++) {
            output << "<div class=\"in-out\">" << std::endl;

            std::string fileContent = getFileContent(inOuts[j].getInputFilePath());
            output << "<div class=\"in\">in:</div>"
                << "<pre><code>" << fileContent << "</code></pre>" << std::endl;

            fileContent = getFileContent(inOuts[j].getExpectedOutputFilePath());
            output << "<div class=\"out\">out:</div>"
                << "<pre><code>" << fileContent << "</code></pre>" << std::endl;
            output << "</div>" << std::endl;
        }
    }

    output << "</div>" << std::endl;
}

void HtmlHelpFormatter::doFormatHelpIntroduction(std::string text, std::ostream& output) {
    output << markdownString2String(text) << std::endl;
}

void HtmlHelpFormatter::formatAllowedOptions_(boost::program_options::options_description options,
    std::ostream& output) {

    output << "<div class=\"help-opts\">"
        << "<h3>" << OPTIONS_HEADER << "</h3>" << std::endl
        << "<pre>" << options << "</pre>" << std::endl
        << "</div>" << std::endl;
}

void HtmlHelpFormatter::doFormatOneAlias(
    std::string aliasName,
    std::list<std::string> processorNames,
    std::ostream& output) {

    if (processorNames.empty()) return;

    output << "<div class=\"alias-item\">" << aliasName << " &rarr; ";

    unsigned int i = 0;
    BOOST_FOREACH(std::string processorName, processorNames) {
        output << "<a href=\"#" << processorName << "\">" << processorName << "</a>";
        if (++i != processorNames.size()) output << ", ";
    }
    output << "</div>" << std::endl;
}

HtmlHelpFormatter::~HtmlHelpFormatter() { }
