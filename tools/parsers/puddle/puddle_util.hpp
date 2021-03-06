#ifndef PUDDLE_UTIL_HDR
#define PUDDLE_UTIL_HDR

#include <string>
#include <sstream>
#include "puddle_types.hpp"
#include <boost/algorithm/string/join.hpp>

namespace poleng {
    namespace bonsai {
        namespace puddle {
            namespace util {

                std::string escapeSpecialChars(std::string &s);
                std::string unescapeSpecialChars(std::string &s);

                std::string getMorphologyString(Morphology morphology);

                bool getGroupActionParams(RuleTokenSizes &ruleTokenSizes,
                        unsigned int start, unsigned int head, unsigned int end,
                        int &realStart, int &realHead, int &realEnd);
                bool getSyntokActionParams(RuleTokenSizes &ruleTokenSizes,
                        unsigned int start, unsigned int end,
                        int &realStart, int &realEnd);
                int getDeleteActionParams(RuleTokenSizes &ruleTokenSizes,
                        int tokenIndex);
                int getAddActionParams(RuleTokenSizes &ruleTokenSizes,
                        int tokenIndex);
                int getTransformActionParams(RuleTokenSizes &ruleTokenSizes,
                        int element);
                bool getUnifyActionTokenParams(RuleTokenSizes &ruleTokenSizes,
                        int index, int &count, int &before);
                bool getRuleBoundaries(RuleTokenSizes &ruleTokenSizes,
                        unsigned int start, unsigned int end,
                        int &realStart, int &realEnd);

            }
        }
    }
}


#endif
