
#include "lattice_wrapper.hpp"

namespace poleng {

    namespace bonsai {
        namespace puddle {

            namespace lattice {

//            LatticeWrapper::LatticeWrapper() { //TagsetPtr tagset) {
// //               initAnnotationItemManager(tagset);
//            }

                    struct SentenceToken {
                        std::string orth;
                        std::string category;
                        int start;
                        int end;
                        std::vector<std::string> baseForms;
                        std::vector<std::string> morphology;
                    };
            //ParseGraphPtr LatticeWrapper::readInputLattice(Lattice &lattice,
                std::string readInputLattice(Lattice &lattice) {
                    std::string sentenceString = "";
//                ParseGraphPtr pg = ParseGraphPtr(new ParseGraph());

//                std::map<int, TransitionInfo*> edgesMap;
                std::map<int, SentenceToken> edgesMap;

                LayerTagCollection token_tag
                    = lattice.getLayerTagManager().createSingletonTagCollection("token");
                LayerTagCollection lemma_tag
                    = lattice.getLayerTagManager().createSingletonTagCollection("lemma");
                LayerTagMask lemmaMask = lattice.getLayerTagManager().getMask(lemma_tag);

                Lattice::EdgesSortedBySourceIterator edgeIterator =
                    lattice.edgesSortedBySource(lemmaMask);
                while (edgeIterator.hasNext()) {
                    Lattice::EdgeDescriptor edge = edgeIterator.next();
                    int start = lattice.getEdgeBeginIndex(edge);
                    int end = start + lattice.getEdgeLength(edge);
                    std::string orth = lattice.getEdgeText(edge);
                    AnnotationItem ai = lattice.getEdgeAnnotationItem(edge);
                    //std::string partOfSpeech = ai.getCategory();
                    std::string category = ai.getCategory();
                    std::list< std::pair<std::string, std::string> > av
                        = lattice.getAnnotationItemManager().getValues(ai);
                    if (lattice.getAnnotationItemManager().getValue(
                                ai, "discard") == "1")
                        continue; //skip discarded edges
                    std::string base = lattice.getAnnotationItemManager().getValue(
                            ai, "base");
                    std::string morphology = lattice.getAnnotationItemManager().getValue(
                            ai, "morphology");
//                    for (std::list< std::pair<std::string, std::string> >
//                            ::iterator avi = av.begin();
//                            avi != av.end(); avi ++) {
//                        if (avi->first == "base")
//                            base = avi->second;
//                        else {
//                            if (morphology != "")
//                                morphology += ":";
//                            morphology += avi->second;
//                        }
//                    }

                    //std::string morphoString = partOfSpeech;
                    //if (morphology != "")
                    //    morphoString += ":" + morphology;
                    //PosInfo pi(base, morphoString, 1);
//                    PosInfo pi(base, morphology, 1);

//                    std::map<int, TransitionInfo*>::iterator edgesMapIt =
                    std::map<int, SentenceToken>::iterator edgesMapIt =
                        edgesMap.find(start);
                    if (edgesMapIt != edgesMap.end()) {
                        edgesMapIt->second.baseForms.push_back(base);
                        edgesMapIt->second.morphology.push_back(morphology);
                    } else {
                        SentenceToken edge;
                        edge.orth = orth;
                        edge.category = category;
                        edge.start = start;
                        edge.end = end;
                        edge.baseForms.push_back(base);
                        edge.morphology.push_back(morphology);
//                        TransitionInfo *edge = new TransitionInfo("token");
//                        edge->setStart(start);
//                        edge->setEnd(end);
//                        edge->setDepth(0);
//                        edge->setLabel(category);
//                        edge->setOrth(orth);
//                        edge->addMorphology(pi);
                        //edgesMap.insert(std::pair<int, TransitionInfo*>(
                        edgesMap.insert(std::pair<int, SentenceToken>(
                                    start, edge));
                    }

                }

                int start = 0;
                int end = 0;
                std::stringstream ss;
                ss << "<<s<" << start << "<" << start << "<sb<>";
                //for (std::map<int, TransitionInfo*>::iterator edgesMapIt =
                for (std::map<int, SentenceToken>::iterator edgesMapIt =
                        edgesMap.begin();
                        edgesMapIt != edgesMap.end(); edgesMapIt ++) {
                    start = edgesMapIt->second.start;
                    end = edgesMapIt->second.end;
//                    pg->add_edge(start, end, *(edgesMapIt->second));
                    ss << "<<t" << "<" << start << "<" << end << "<" <<
                        "TOKEN" << "<" << edgesMapIt->second.orth;
                    std::vector<std::string>::iterator baseIt =
                        edgesMapIt->second.baseForms.begin();
                    std::vector<std::string>::iterator morphIt =
                        edgesMapIt->second.morphology.begin();
                    while (baseIt != edgesMapIt->second.baseForms.end()) {
                        ss << "<" << *baseIt << "<" << *morphIt;
                        baseIt ++;
                        morphIt ++;
                    }
//                    std::vector<PosInfo> vars = edgesMapIt->second->variants_;
//                    for (std::vector<PosInfo>::iterator varIt = vars.begin();
//                            varIt != vars.end(); varIt ++ )
//                        ss << "<" << boost::get<0>(*varIt) << "<"
//                            << boost::get<1>(*varIt);
                    ss << ">";
                }
                ss << "<<s<" << end << "<" << end << "<se<>";
                sentenceString = ss.str();

//                std::cerr << "TAKI LUJ: "<< pg->write_graphviz() << std::endl;
                return sentenceString;;
            }

#ifdef _WITH_BONSAI_PARSEGRAPH
            //ParseGraphPtr LatticeWrapper::readOutputLattice(Lattice &lattice) {
            ParseGraphPtr convertToBonsaiGraph(Lattice &lattice) {
                ParseGraphPtr pg = ParseGraphPtr(new ParseGraph());

                std::multimap<int, TransitionInfo*> edgesMap;
                std::map<int, int> depthsMap;

                LayerTagMask mask = lattice.getLayerTagManager().getMask(
                        createUnion(
                            lattice.getLayerTagManager().
                                createSingletonTagCollection("lemma") //@todo: czy "token"?
                                ,
                                lattice.getLayerTagManager().
                                    createSingletonTagCollection("parse")
                            )
                        );

                Lattice::EdgesSortedBySourceIterator edgeIterator =
                    lattice.edgesSortedBySource(mask);
                while (edgeIterator.hasNext()) {
                    Lattice::EdgeDescriptor edge = edgeIterator.next();
                    int start = lattice.getEdgeBeginIndex(edge);
                    int end = start + lattice.getEdgeLength(edge);
                    std::string orth = lattice.getEdgeText(edge);
                    AnnotationItem ai = lattice.getEdgeAnnotationItem(edge);
                    //std::string partOfSpeech = ai.getCategory();
                    std::string category = ai.getCategory();
                    if (lattice.getAnnotationItemManager().getValue(
                                ai, "discard") == "1")
                        continue; //skip discarded edges
                    std::string base = lattice.getAnnotationItemManager().getValue(
                            ai, "base");
                    std::string morphology = lattice.getAnnotationItemManager().getValue(
                            ai, "morphology");
//                    std::string base;
//                    std::string morphology = "";
//                    std::list< std::pair<std::string, std::string> > av
//                        = lattice.getAnnotationItemManager().getValues(ai);
//                    for (std::list< std::pair<std::string, std::string> >
//                            ::iterator avi = av.begin();
//                            avi != av.end(); avi ++) {
//                        if (avi->first == "base")
//                            base = avi->second;
//                        else {
//                            if (morphology != "")
//                                morphology += ":";
//                            morphology += avi->second;
//                        }
//                    }

                    //std::string morphoString = partOfSpeech;
                    //if (morphology != "")
                    //    morphoString += ":" + morphology;
                    //PosInfo pi(base, morphoString, 1);
                    PosInfo pi(base, morphology, 1);

                    std::map<int, int>::iterator depthsMapIt =
                        depthsMap.find(start);
                    if (depthsMapIt != depthsMap.end()) {
                        std::pair<
                            std::map<int, TransitionInfo*>::iterator,
                            std::map<int, TransitionInfo*>::iterator
                                > edgesMapIt = edgesMap.equal_range(start);
                        int max_depth = 0;
                        while (edgesMapIt.first != edgesMapIt.second) {
                            if (edgesMapIt.first->second->getDepth() > max_depth)
                                max_depth = edgesMapIt.first->second->getDepth();

                            if (edgesMapIt.first->second->getLabel() == category) {
                                edgesMapIt.first->second->addMorphology(pi);
                                break;
                            }
                            edgesMapIt.first ++;
                        }
                        if (edgesMapIt.first == edgesMapIt.second) {
                            TransitionInfo *edge = new TransitionInfo("group");
                            edge->setStart(start);
                            edge->setEnd(end);
                            edge->setDepth(max_depth + 1);
                            edge->setLabel(category);
                            edge->setOrth(orth);
                            edge->addMorphology(pi);
                            edgesMap.insert(std::pair<int, TransitionInfo*>(
                                        start, edge));
                            depthsMap.insert(std::pair<int, int>(start, max_depth + 1));
                        }
                    } else {
                        TransitionInfo *edge = new TransitionInfo("token");
                        edge->setStart(start);
                        edge->setEnd(end);
                        edge->setDepth(0);
                        edge->setLabel(category);
                        edge->setOrth(orth);
                        edge->addMorphology(pi);
                        edgesMap.insert(std::pair<int, TransitionInfo*>(
                                    start, edge));
                        depthsMap.insert(std::pair<int, int>(start, 0));
                    }

                }
                for (std::map<int, TransitionInfo*>::iterator edgesMapIt =
                        edgesMap.begin();
                        edgesMapIt != edgesMap.end(); edgesMapIt ++) {
                    int start = edgesMapIt->second->getStart();
                    int end = edgesMapIt->second->getEnd();
                    pg->add_edge(start, end, *(edgesMapIt->second));
                }

                return pg;
            }
#endif

//            void LatticeWrapper::initAnnotationItemManager(TagsetPtr tagset) {
//                AnnotationItem ai("base");
//                annotationItemManager.setValue(ai, "base", "xxx");
//                std::vector<std::string> attributes = tagset->getAttributes();
//                std::vector<std::string> partsOfSpeech =
//                    tagset->getPartsOfSpeech();
//                for (std::vector<std::string>::iterator it = attributes.begin();
//                        it != attributes.end(); it ++) {
//                    annotationItemManager.setValue(ai, *it, "aaa");
//                }
//            }

            //std::list<Lattice::EdgeDescriptor> LatticeWrapper::getTopEdges(
            std::list<Lattice::EdgeDescriptor> getTopEdges(
                    Lattice &lattice, Lattice::VertexDescriptor start) {
                LayerTagMask mask = lattice.getLayerTagManager().getMask(
                        createUnion(
                            lattice.getLayerTagManager().
                                createSingletonTagCollection("lemma") //@todo: czy "token"?
                                ,
                                lattice.getLayerTagManager().
                                    createSingletonTagCollection("parse")
                            )
                        );
                return getTopEdges(lattice, start, mask);
            }

//            bool LatticeWrapper::isNonTop(
//                    std::list<Lattice::EdgeDescriptor> nontopEdges,
//                    const Lattice::EdgeDescriptor &value) {
//                for (std::list<Lattice::EdgeDescriptor>::iterator refIt =
//                        nontopEdges.begin(); refIt != nontopEdges.end();
//                        refIt ++) {
//                    if (value == *refIt)
//                        return true;
//                }
//                return false;
//            }

            //std::list<Lattice::EdgeDescriptor> LatticeWrapper::getTopEdges(
            std::list<Lattice::EdgeDescriptor> getTopEdges(
                    Lattice &lattice, Lattice::VertexDescriptor start,
                    LayerTagMask mask) {
                std::list<Lattice::EdgeDescriptor> edges;
                std::list<Lattice::EdgeDescriptor> nontopEdges;
                Lattice::InOutEdgesIterator edgeIt =
                    lattice.outEdges(start, mask);
                while (edgeIt.hasNext()) {
                    Lattice::EdgeDescriptor edge = edgeIt.next();
                    AnnotationItem ai = lattice.getEdgeAnnotationItem(edge);
                    if (lattice.getAnnotationItemManager().getValue(
                                ai, "discard") == "1")
                        continue;

                    edges.push_back(edge);
                    std::list<Lattice::Partition> partitions =
                        lattice.getEdgePartitions(edge);
                    for (std::list<Lattice::Partition>::iterator partIt =
                            partitions.begin();
                            partIt != partitions.end(); partIt ++) {
                        nontopEdges.insert(nontopEdges.end(),
                                partIt->begin(),
                                partIt->end()
                                );
                    }
                }
                nontopEdges.unique(EdgeUnique());
                edges.remove_if(EdgeNonTop(nontopEdges));
                return edges;
            }

            //std::list<Lattice::EdgeSequence> LatticeWrapper::getEdgesRange(Lattice &lattice,
            std::list<Lattice::EdgeSequence> getEdgesRange(Lattice &lattice,
                    Lattice::VertexDescriptor start,
                    Lattice::VertexDescriptor end,
                    LayerTagMask mask) {
                std::list<Lattice::EdgeSequence> ranges;
//                std::list<VertexDescriptor> vertexStack;
                Lattice::VertexDescriptor vertex = start;
                while (vertex <= end) {
                    std::list<Lattice::EdgeDescriptor> edges = getTopEdges(lattice,
                            vertex, mask);
                    if (edges.size() == 0) {
                        vertex ++;
                        continue;
                    }
                    std::list<Lattice::EdgeSequence> tmpRanges;
                    if (ranges.size() > 0) {
                        for (std::list<Lattice::EdgeDescriptor>::iterator edgeIt =
                                edges.begin(); edgeIt != edges.end(); edgeIt ++) {
                            for (std::list<Lattice::EdgeSequence>::iterator seqIt =
                                    ranges.begin(); seqIt != ranges.end(); seqIt ++) {
                                Lattice::EdgeSequence::Builder rangeBuilder;
                                    for (Lattice::EdgeSequence::Iterator rangeIt =
                                            seqIt->begin(); rangeIt != seqIt->end();
                                            rangeIt ++ )
                                        rangeBuilder.addEdge(*rangeIt);
                                rangeBuilder.addEdge(*edgeIt);
                                tmpRanges.push_back(rangeBuilder.build());
                            }
                        }
                    } else {
                        for (std::list<Lattice::EdgeDescriptor>::iterator edgeIt =
                                edges.begin(); edgeIt != edges.end(); edgeIt ++) {
                            Lattice::EdgeSequence::Builder rangeBuilder;
                            rangeBuilder.addEdge(*edgeIt);
                            tmpRanges.push_back(rangeBuilder.build());
                        }
                    }
                    ranges = tmpRanges;
                    vertex += lattice.getEdgeLength(edges.front());
                }
                return ranges;
            }
//                Ladtice::InOutEdgesIterator edgeIt =
//                    lattice.outEdges(start, mask);
//                while (edgeIt.hasNext()) {
//                    Lattice::EdgeDescriptor edge = edgeIt.next();
//                    int edgeEnd = lattice.getEdgeBeginIndex(edge) +
//                        lattice.getEdgeLength();
//                    if (edgeEnd != end) {
//                        std::list<Partition> tmpRanges = getEdgesRange(
//                                edgeEnd, end, mask);
//                    } else {
//                        if (ranges.size() > 0) {
//                        } else {
//                            ranges.push_back
//
//                        }
//                    }
//                    edges.push_back(edge);
//                }
//                return ranges;
//            }
//
            //std::list<Lattice::EdgeSequence> LatticeWrapper::getEdgesRange(Lattice &lattice,
            std::list<Lattice::EdgeSequence> getEdgesRange(Lattice &lattice,
                    Lattice::VertexDescriptor start,
                    Lattice::VertexDescriptor end) {
                LayerTagMask mask = lattice.getLayerTagManager().getMask(
                        createUnion(
                            lattice.getLayerTagManager().
                            createSingletonTagCollection("lemma") //@todo: czy "token"?
                            ,
                            lattice.getLayerTagManager().
                            createSingletonTagCollection("parse")
                            )
                        );
                return getEdgesRange(lattice, start, end, mask);
            }

            //void LatticeWrapper::addParseEdges(Lattice &lattice,
            void addParseEdges(Lattice &lattice,
                    std::list<Lattice::EdgeDescriptor> startEdges,
                    std::list<Lattice::EdgeDescriptor> endEdges,
                    std::string &parseCategory,
                    std::list<Lattice::EdgeDescriptor> headEdges,
                    std::list<Lattice::EdgeSequence> groupSequences,
                    Lattice::Score score) {
                Lattice::VertexDescriptor startVertex =
                    lattice.getEdgeBeginIndex(startEdges.front());
                Lattice::VertexDescriptor endVertex =
                    lattice.getEdgeBeginIndex(endEdges.front()) +
                    lattice.getEdgeLength(endEdges.front());
                LayerTagCollection tags =
                    lattice.getLayerTagManager().createSingletonTagCollection("parse");
                for (std::list<Lattice::EdgeDescriptor>::iterator edgeIt =
                        headEdges.begin(); edgeIt != headEdges.end(); edgeIt ++) {
                    AnnotationItem annotationItem(parseCategory);
                    std::list< std::pair<std::string, std::string> > values =
                        lattice.getAnnotationItemManager().getValues(
                                lattice.getEdgeAnnotationItem(*edgeIt) );
                    for (std::list< std::pair<std::string, std::string> >::iterator valueIt =
                            values.begin(); valueIt != values.end(); valueIt ++) {
                        lattice.getAnnotationItemManager().setValue(
                                annotationItem, valueIt->first, valueIt->second);
//                        std::cerr << "ustawiam: " << valueIt->first << " na " << valueIt->second << std::endl;
                    }
//                    std::cerr << "categoria: " << lattice.getAnnotationItemManager().getCategory(annotationItem) << std::endl;

                    for (std::list<Lattice::EdgeSequence>::iterator seqIt =
                            groupSequences.begin();
                            seqIt != groupSequences.end();
                            seqIt ++) {
//                        std::cerr << "partycja: ";
//                        for (Lattice::EdgeSequence::Iterator eIt = seqIt->begin();
//                                eIt != seqIt->end(); eIt ++) {
//                            std::cerr << " " << lattice.getEdgeText(*eIt);
//                            std::list< std::pair<std::string, std::string> > values666 =
//                                lattice.getAnnotationItemManager().getValues(
//                                        lattice.getEdgeAnnotationItem(*eIt) );
//                            std::cerr << "(";
//                            for (std::list< std::pair<std::string, std::string> >::iterator valueIt =
//                                    values666.begin(); valueIt != values666.end(); valueIt ++)
//                                std::cerr << ":" << valueIt->second;
//                            std::cerr << ")";
//                        }
//                        std::cerr << std::endl;
//                        std::cerr << "chce dodac to od: " << startVertex << " do: " << endVertex << std::endl;
                        bool firstPartition = true;
                        Lattice::InOutEdgesIterator outEdgesIt = lattice.outEdges(
                                startVertex, lattice.getLayerTagManager().getMask(tags));
                        if (outEdgesIt.hasNext()) {
                            Lattice::EdgeDescriptor newEdge = outEdgesIt.next();
//                            std::cerr << "dodawany pierun: " << annotationItem.getCategory() << std::endl;
//                            std::cerr << "z kratry wziety: " << lattice.getEdgeAnnotationItem(newEdge).getCategory() << std::endl;
                            if (annotationItem == lattice.getEdgeAnnotationItem(newEdge)) {
                                //there is already such an edge. add another partition
                                //@todo: to tak nie dziala. nowa partition powinno dodawac samo addEdge, ale nie robi tego. na razie jest wiec jedna partition zawsze
                                //std::cerr << "PRZED: " << lattice.getEdgePartitions(newEdge).size() << std::endl;

//                        lattice.addEdge(
//                                startVertex,
//                                endVertex,
//                                annotationItem,
//                                tags,
//                                *seqIt
//                                );

//                                lattice.getEdgePartitions(newEdge).push_back(*seqIt);
                                //std::cerr << "PO: " << lattice.getEdgePartitions(newEdge).size() << std::endl;
                                firstPartition = false;
                            }
                        }
                        if (firstPartition) {
//                            std::cerr << "daje nowego" << std::endl;
                            //there is no such an edge yet. add a new one
                        lattice.addEdge(
                                startVertex,
                                endVertex,
                                annotationItem,
                                tags,
                                *seqIt
                                );
                        }

                    }
                }
            }

            //void LatticeWrapper::addSyntokEdges(Lattice &lattice,
            void addSyntokEdges(Lattice &lattice,
                    std::list<Lattice::EdgeDescriptor> startEdges,
                    std::list<Lattice::EdgeDescriptor> endEdges,
                    std::string &syntokCategory,
                    std::vector<std::string> baseForms,
                    std::vector<std::string> morphology,
                    std::list<Lattice::EdgeSequence> edgeSequences,
                    LayerTagCollection tags,
                    Lattice::Score score) {

                Lattice::VertexDescriptor startVertex =
                    lattice.getEdgeBeginIndex(startEdges.front());
                Lattice::VertexDescriptor endVertex =
                    lattice.getEdgeBeginIndex(endEdges.front()) +
                    lattice.getEdgeLength(endEdges.front());

                for (std::vector<std::string>::iterator baseIt =
                        baseForms.begin();
                        baseIt != baseForms.end(); baseIt ++) {
                    for (std::vector<std::string>::iterator morphIt =
                            morphology.begin();
                            morphIt != morphology.end();
                            morphIt ++) {
                        AnnotationItem annotationItem(syntokCategory);

                        lattice.getAnnotationItemManager().setValue(
                                annotationItem, "base", *baseIt);
                        lattice.getAnnotationItemManager().setValue(
                                annotationItem, "morphology", *morphIt);
                        lattice.getAnnotationItemManager().setValue(
                                annotationItem, "discard", "0");

                        for (std::list<Lattice::EdgeSequence>::iterator sequenceIt =
                                edgeSequences.begin();
                                sequenceIt != edgeSequences.end();
                                sequenceIt ++) {

                            lattice.addEdge(
                                    startVertex,
                                    endVertex,
                                    annotationItem,
                                    tags,
                                    *sequenceIt
                                    );
                        }
                    }
                }

            }

            //void LatticeWrapper::addNewVariantEdges(Lattice &lattice,
            void addNewVariantEdges(Lattice &lattice,
                    Lattice::EdgeDescriptor edge,
                    std::vector<std::string> baseForms,
                    std::vector<std::string> morphology) {

                Lattice::VertexDescriptor startVertex =
                    lattice.getEdgeBeginIndex(edge);
                Lattice::VertexDescriptor endVertex =
                    startVertex + lattice.getEdgeLength(edge);
                AnnotationItem annotationItem =
                    lattice.getEdgeAnnotationItem(edge);
                LayerTagCollection tags = lattice.getEdgeLayerTags(edge);
                std::list<Lattice::Partition> partitions =
                    lattice.getEdgePartitions(edge);
                Lattice::Score score = lattice.getEdgeScore(edge);

                for (std::vector<std::string>::iterator baseIt =
                        baseForms.begin();
                        baseIt != baseForms.end();
                        baseIt ++) {
                    for (std::vector<std::string>::iterator morphIt =
                            morphology.begin();
                            morphIt != morphology.end();
                            morphIt ++) {
                        lattice.getAnnotationItemManager().setValue(
                                annotationItem, "base", *baseIt);
                        lattice.getAnnotationItemManager().setValue(
                                annotationItem, "morphology", *morphIt);
                        lattice.getAnnotationItemManager().setValue(
                                annotationItem, "discard", "0");

                        for (std::list<Lattice::Partition>::iterator partIt =
                                partitions.begin();
                                partIt != partitions.end();
                                partIt ++) {
                            Lattice::EdgeSequence sequence = partIt->getSequence();
                            lattice.addEdge(
                                    startVertex,
                                    endVertex,
                                    annotationItem,
                                    tags,
                                    sequence
                                    );
                        }
                    }
                }
            }

            //void LatticeWrapper::removeParseEdges(Lattice &lattice,
            void removeParseEdges(Lattice &lattice,
                    Lattice::VertexDescriptor start,
                    Lattice::VertexDescriptor end ) {
                LayerTagMask mask = lattice.getLayerTagManager().getMask(
                                lattice.getLayerTagManager().
                                    createSingletonTagCollection("parse")
                        );
                Lattice::VertexDescriptor vertex = start;
                while (vertex <= end) {
                    std::list<Lattice::EdgeDescriptor> edges = getTopEdges(
                            lattice, vertex, mask);
                    if (edges.size() == 0) {
                        vertex ++;
                        continue;
                    }
                    for (std::list<Lattice::EdgeDescriptor>::iterator edgeIt =
                            edges.begin(); edgeIt != edges.end(); edgeIt ++) {
                        AnnotationItem ai = lattice.getEdgeAnnotationItem(*edgeIt);
                        lattice.getAnnotationItemManager().setValue(
                                ai, "discard", "1");
                    }
                    vertex += lattice.getEdgeLength(edges.front());
                }
            }

            //void LatticeWrapper::addPosEdges(Lattice &lattice) {
/*            void addPosEdges(Lattice &lattice) {
                LayerTagMask mask = lattice.getLayerTagManager().getMask(
                            lattice.getLayerTagManager().
                            createSingletonTagCollection("lemma") //@todo: czy "token"?
                        );

                std::multimap<Lattice::EdgeDescriptor, std::string> posEdgesMap;
                Lattice::EdgesSortedBySourceIterator edgeIterator =
                    lattice.edgesSortedBySource(mask);
                while (edgeIterator.hasNext()) {
                    Lattice::EdgeDescriptor edge = edgeIterator.next();
                    int start = lattice.getEdgeBeginIndex(edge);
                    int end = start + lattice.getEdgeLength(edge);
                    AnnotationItem ai = lattice.getEdgeAnnotationItem(edge);
                    std::string morphology =
                        lattice.getAnnotationItemManager().getValue(ai, "morphology");
                    std::string partOfSpeech = morphology.substr(0, morphology.find(":"));
//                    std::list< std::pair<std::string, std::string> > av
//                        = lattice.getAnnotationItemManager().getValues(ai);
//                    for (std::list< std::pair<std::string, std::string> >
//                            ::iterator avi = av.begin();
//                            avi != av.end(); avi ++) {
//                        if (avi->first == "pos")
//                            partOfSpeech = avi->second;
//                    }
                    std::pair<
                        std::multimap<Lattice::EdgeDescriptor, std::string>::iterator,
                        std::multimap<Lattice::EdgeDescriptor, std::string>::iterator
                            > posEdgesMapIt =
                        posEdgesMap.equal_range(edge);
                    bool alreadyAdded = false;
                    if (posEdgesMapIt.first != posEdgesMapIt.second) {
                        while (posEdgesMapIt.first != posEdgesMapIt.second) {
                            if (posEdgesMapIt.first->second ==
                                    partOfSpeech) {
                                alreadyAdded = true;
                                break;
                            }
                            posEdgesMapIt.first ++;
                        }
                    }
                    if (! alreadyAdded) {
                        AnnotationItem ai(partOfSpeech);
                        lattice.getAnnotationItemManager().setValue(ai, "discard", "0");
                        Lattice::EdgeSequence::Builder seqBuilder;
                        seqBuilder.addEdge(edge);
                        lattice.addEdge(
                                start,
                                end,
                                ai,
                                lattice.getLayerTagManager().
                                createSingletonTagCollection("parse"), //@todo: czy cos innego jeszcze? a moze lemma?
//                                0.0, //@todo: jaki tu score?
                                seqBuilder.build()
                                );
                    }
                }
            }*/

//
//            void LatticeWrapper::addParseEdge(Lattice &lattice,
//                    Lattice::VertexDescriptor start, Lattice::VertexDescriptor end,
//                    std::string &parseLabel) {
//
//            }
//
//            std::list<EdgeDescriptor> edgesRangeBSF(lattice, start, end, mask) {
//                Lattice::InOutEdgesIterator edgeIt =
//                    lattice.outEdges(start, mask);
//                std::list<EdgeDescriptor> tmpList;
//                while (edgeIt.hasNext()) {
//                    Lattice::EdgeDescriptor edge = edgeIt.next();
//                    int edgeEnd = lattice.getEdgeBeginIndex(edge) +
//                        lattice.getEdgeLength();
//                    if (edgeEnd != end) {
//                        tmpRanges = edgesRangeBSF(edgeEnd, end, mask);
//                    }
//                }
//
//            }

            }

        }
    }
}