#include "openfst_adapter_impl.hpp"

#include <sstream>

#include "exceptions.hpp"
#include "fst_path_finder.hpp"


const size_t OpenFSTAdapterImpl::MAX_LINE_LENGTH = 99999;


OpenFSTAdapterImpl::OpenFSTAdapterImpl() :
    filter_(NULL)
{
}


OpenFSTAdapterImpl::~OpenFSTAdapterImpl()
{
}


void OpenFSTAdapterImpl::init()
{
}


void OpenFSTAdapterImpl::init(const std::string& far, const std::string& fst)
{
    automata_.push_back(loadFstFromFar_(far, fst));
}


void OpenFSTAdapterImpl::init(std::vector< std::pair<std::string, std::string> > spec)
{
    for (std::vector< std::pair<std::string, std::string> >::iterator iter = spec.begin();
            iter != spec.end();
            ++iter) {
        automata_.push_back(loadFstFromFar_(iter->first, iter->second));
    }
}


std::string OpenFSTAdapterImpl::normalize(const std::string& input)
{
    fst::StdVectorFst* input_fst = compileByte_(input);
    fst::StdVectorFst* output_fst;

    output_fst = input_fst;

    size_t n = automata_.size();
    for (size_t i = 0; i < n; ++i) {
        fst::StdVectorFst* intermediate_fst = new fst::StdVectorFst;
        fst::Compose(*output_fst, *automata_[i], intermediate_fst);
        delete output_fst;
        output_fst = intermediate_fst;
    }

    fst::Project(output_fst, fst::PROJECT_OUTPUT);
    fst::RmEpsilon(output_fst);

    fst::StdVectorFst output_fst_det;
    fst::Determinize(*output_fst, &output_fst_det);
    fst::Minimize(&output_fst_det);

    std::string output(fstToString_(output_fst_det));
    delete output_fst;
    return output;
}


std::string OpenFSTAdapterImpl::normalize(
        const std::string& far,
        const std::string& fst,
        const std::string& input)
{
    const fst::Fst<fst::StdArc>* automaton = loadFstFromFar_(far, fst);
    fst::StdVectorFst* input_fst = compileByte_(input);
    fst::StdVectorFst* output_fst = new fst::StdVectorFst;

    fst::Compose(*input_fst, *automaton, output_fst);

    fst::Project(output_fst, fst::PROJECT_OUTPUT);
    fst::RmEpsilon(output_fst);

    fst::StdVectorFst output_fst_det;
    fst::Determinize(*output_fst, &output_fst_det);
    fst::Minimize(&output_fst_det);

    std::string output(fstToString_(output_fst_det));
    delete automaton;
    delete input_fst;
    delete output_fst;
    return output;
}


fst::StdVectorFst* OpenFSTAdapterImpl::compileByte_(const std::string& s)
{
    int state;
    const char* cstr = s.c_str();
    fst::StdVectorFst* automaton = new fst::StdVectorFst;
    state = automaton->AddState();
    automaton->SetStart(state);
    while (*cstr) {
        state = automaton->AddState();
        int symbol = (unsigned char)(*cstr);
        automaton->AddArc(state - 1, fst::StdArc(symbol, symbol, 0, state));
        ++cstr;
    }
    automaton->SetFinal(state, 0);
    return automaton;
}


const fst::Fst<fst::StdArc>* OpenFSTAdapterImpl::loadFstFromFar_(
            const std::string& far,
            const std::string& fst)
{
    fst::FarReader<fst::StdArc>* r = fst::FarReader<fst::StdArc>::Open(far);
    if (!r || r->Error() || !(r->Find(fst))) {
        std::stringstream errorSs;
        errorSs << "Iayko normalizer: failed to load FST \"" << fst << "\""
            << " from FAR \"" << far << "\"";
        throw FileFormatException(errorSs.str());
    }
    return r->GetFst();
}


std::string OpenFSTAdapterImpl::fstToString_(fst::StdVectorFst& automaton)
{
    std::stringstream resultSs;
    FstPathFinder finder;
    finder.extract_all_paths(automaton);
    if (finder.paths.size() == 0) {
        return "";
    }
    char buffer[MAX_LINE_LENGTH];
    size_t len = finder.paths[0].path.size();
    for (size_t i = 0; i < len; ++i) {
        buffer[i] = (char)(finder.paths[0].path[i]);
    }
    buffer[len] = '\0';
    return buffer;
}


// ==============================================

extern "C" OpenFSTAdapterImpl* create()
{
    return new OpenFSTAdapterImpl;
}


extern "C" void destroy(OpenFSTAdapterImpl* Tl)
{
    delete Tl ;
}
