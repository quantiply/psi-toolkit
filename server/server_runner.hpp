#ifndef SERVER_RUNNER_HDR
#define SERVER_RUNNER_HDR

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <sstream>

class ServerRunner {

public:
    ServerRunner(int argc, char* argv[]);
    int run();

private:
    boost::program_options::variables_map options;
    std::vector<std::string> annotatorOptions;

    boost::program_options::options_description optionsDescription;

    boost::program_options::variables_map parseOptions(int argc, char * argv[]);
    void setOptionsDescription();
    std::string annotatorOptionsAsString();

    int executeOptions();

    void daemonize_(bool leaveStandardDescriptors);
    int setRootDirectory_();

    boost::filesystem::path rootDir_;

    static const std::string DEFAULT_PIPE;
};

#endif
