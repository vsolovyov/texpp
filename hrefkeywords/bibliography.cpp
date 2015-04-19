#include "bibliography.h"

Bibliography::Bibliography(boost::shared_ptr<BibLib> biblib)
{
    for(BibLib::iterator it = biblib->begin(); it != biblib->end(); it++)
    {
        bibFrequency.insert(std::make_pair(it->first, BibCiteStruct(it->second)));
    }
}

void Bibliography::feed(boost::shared_ptr<CiteList> citations)
{
    for(CiteList::iterator it = citations->begin();
        it != citations->end(); it++)
    {
        string bibTag = it->bibTag;
        // single citation
        if(bibTag.find(",") == std::string::npos)
        {
            increment(*it);
        }else{
            // split sequence of sitations
            std::string delimiter = ",";
            size_t pos = 0;
            while ((pos = bibTag.find(delimiter)) != std::string::npos) {
                CiteInfo subCitation(bibTag.substr(0, pos),it->pos,it->context);
                increment(subCitation);
                bibTag.erase(0, pos + delimiter.length());
            }
            CiteInfo subCitation(bibTag,it->pos,it->context);
            increment(subCitation);
        }
    }
}

void Bibliography::increment(CiteInfo& citeInfo)
{
    if(bibFrequency.find(citeInfo.bibTag) != bibFrequency.end())
    {
        BibCiteStruct& bibSample = bibFrequency.find(citeInfo.bibTag)->second;
        bibSample.citations.push_back(std::make_pair(citeInfo.pos,citeInfo.context));
    }
    else {
        std::cout << "Error!!! wrong parsing of \""
                  << citeInfo.bibTag
                  << "\" citation" << std::endl;
    }
}

void Bibliography::show()
{
    std::cout << "______bibliography: " << std::endl;
    static int count =0;
    for(std::map<string, BibCiteStruct >::iterator it = bibFrequency.begin();
        it != bibFrequency.end(); it++)
    {
        std::cout << it->first << std::endl;

        count+= it->second.citations.size();
        std::cout << it->second.citations.size() << std::endl;
        vector<pair <int, string > > citeList = it->second.citations;
        for(size_t i = 0; i< citeList.size(); i++)
        {
            std::cout << "   position: " << std::setw(6) << citeList[i].first
                      << "  " << citeList[i].second << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << "numbur of citation: " << count << std::endl;
    std::cout << "number of bibitems: " << bibFrequency.size() << std::endl;
}


void extractCitation(const Node::ptr node,
                     shared_ptr<CiteList> citations,
                     shared_ptr<BibLib> bibliography,
                     string context)
{
    size_t childrenCount = node->childrenCount();

    if(childrenCount == 0) {
        return;
    }
//    string context; // = "context shouls be here!";

    for(size_t n = 0; n < childrenCount; ++n) {
        Node::ptr child = node->child(n);

        if(child->type() == "cite_item") {
            Token::ptr token = child->tokens()[0];

            citations->push_back(CiteInfo(child->valueString(),
                                          token->linePos() + token->charPos(),
                                          context) );
        } else if(child->type() == "bibliography_item"){
            bibliography->push_back(child->value(
                                        std::make_pair(string(),string())));
        } else if(child->type() == "group" || child->type() == "command" ) {
            extractCitation(child,citations,bibliography,context);
        } else if(child->type() == "text_character" && child->valueString() == ".")
        {
            string s = child->valueString();
//            end collection, free the context sentence.
            context.clear();
        } else if(child->type() == "text_word" || child->type() == "text_space")
        {
//            collect word to the context
            context += child->valueString();
        }
    }
}

boost::python::dict extractCiteFreequency(const Node::ptr document)
{
    shared_ptr<CiteList> citations(new CiteList());
    shared_ptr<BibLib> bibliography(new BibLib());

    extractCitation(document,citations,bibliography,"");

    Bibliography bib(bibliography);
    bib.feed(citations);
    bib.show();

    return bib.getDict();
}

/**
 * @brief toPythonList converts a C++ vector<pair<int,string> to a python list
 * @param vector sample vector<pair<int,string>
 * @return return converted from the vec list object
 */
boost::python::list toPythonList(std::vector<pair<int,string> > vec)
{
    boost::python::list list;
    std::vector<pair<int,string> >::iterator it;
    for ( it = vec.begin(); it != vec.end(); ++it)
    {
        list.append( boost::python::make_tuple(it->first,it->second) );
    }
    return list;
}

boost::python::dict Bibliography::getDict()
{
    boost::python::dict dictPy;
    typedef std::map<string,BibCiteStruct>::iterator it_type;
    for(it_type it = bibFrequency.begin(); it != bibFrequency.end(); it++)
    {
        BibCiteStruct bibItem = it->second;
        dictPy[bibItem.source] = toPythonList(bibItem.citations);
    }
    return dictPy;
}

//BOOST_PYTHON_MODULE(citation)
//{
//    def("extractCiteFreequency", &extractCiteFreequency);
//}
