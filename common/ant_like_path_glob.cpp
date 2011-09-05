#include "ant_like_path_glob.hpp"

#include "logging.hpp"

AntLikePathGlob::AntLikePathGlob(const std::string& globSpec) {
    boost::filesystem::path globAsPath(globSpec);

    fixedPrefix_ = globAsPath.root_path();
    boost::filesystem::path rest = globAsPath.relative_path();

    bool inPrefix = true;
    bool lastStarStar = false;

    for (boost::filesystem::path::iterator iter = rest.begin();
        iter != rest.end();
        ++iter) {

        SegmentGlob segmentGlob((*iter).string());

        if (inPrefix && segmentGlob.isFixed())
            fixedPrefix_ /= (*iter);
        else {
            inPrefix = false;

            segmentGlobs_.push_back(segmentGlob);

            lastStarStar = (segmentGlob.asString() == "**");
        }
    }

    if (lastStarStar) {
        SegmentGlob anyFile("*");
        segmentGlobs_.push_back(anyFile);
    }
}

void AntLikePathGlob::allMatchingFiles(boost::filesystem::path currentPath,
                                       std::set<boost::filesystem::path>& matchedFiles) const {

    boost::filesystem::path fixedPart =
        (fixedPrefix_.has_root_path()
         ? fixedPrefix_
         : currentPath / fixedPrefix_);

    if (segmentGlobs_.empty())
        checkFile_(fixedPart, matchedFiles, true);
    else if (boost::filesystem::is_directory(fixedPart))
        findMatchingFiles_(fixedPart, segmentGlobs_.begin(), matchedFiles);
}

void AntLikePathGlob::findMatchingFiles_(
    boost::filesystem::path currentPath,
    std::list<SegmentGlob>::const_iterator globIter,
    std::set<boost::filesystem::path>& matchedFiles) const {

    if (globIter == segmentGlobs_.end())
        return;

    INFO("looking in: `" << currentPath << "' " <<
         (globIter == segmentGlobs_.end()
          ? "END"
          : (*globIter).asString()));

    std::list<SegmentGlob>::const_iterator globIterPlusOne = globIter;
    ++globIterPlusOne;

    if ((*globIter).asString() == "**") {
        findMatchingFiles_(currentPath, globIterPlusOne, matchedFiles);

        boost::filesystem::recursive_directory_iterator end_itr;
        for (boost::filesystem::recursive_directory_iterator fiter(currentPath);
             fiter != end_itr;
             ++fiter)
            if (boost::filesystem::is_directory(fiter->path()))
                findMatchingFiles_(fiter->path(), globIterPlusOne, matchedFiles);
    }
    else {
        boost::filesystem::directory_iterator end_iter;
        for (boost::filesystem::directory_iterator fiter(currentPath);
             fiter != end_iter;
             ++fiter)
            if (globIter->matches(fiter->path().filename().string())) {
                if (boost::filesystem::is_directory(fiter->path()))
                    findMatchingFiles_(*fiter, globIterPlusOne, matchedFiles);
                else {
                    if (boost::filesystem::is_regular_file(fiter->path())
                        && globIterPlusOne == segmentGlobs_.end())
                        checkFile_(fiter->path(), matchedFiles, false);
                }
            }
    }
}

void AntLikePathGlob::checkFile_(boost::filesystem::path filePath,
                                 std::set<boost::filesystem::path>& matchedFiles,
                                 bool printWarning) const {
    if (is_regular_file(filePath))
        matchedFiles.insert(filePath);
    else if(printWarning)
        WARN("expected regular file `" << filePath.string() << "'");
}
