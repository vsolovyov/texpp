#ifndef BIBLIOGRAPHY_H
#define BIBLIOGRAPHY_H

#include <vector>
#include <string>
#include <utility>
#include <map>
#include <iostream>
#include <iomanip>

#include <texpp/parser.h>

#include <boost/python.hpp>

// TODO replace by c++11
#include <boost/tuple/tuple.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/algorithm/string.hpp>



using namespace texpp;
using std::string;
using std::vector;
using std::pair;
using boost::shared_ptr;


class CiteInfo
{
public:
    CiteInfo(std::string _bibTag, int _pos, std::string _context)
        :pos(_pos), bibTag(_bibTag), context(_context)
    {}
    int pos;
    string bibTag;
    string context;
};

class BibCiteStruct
{
public:
    BibCiteStruct(string _source)
        :source(_source)
    {}
    string source;
    vector<pair<int, string> > citations;  // <position, context>
};

typedef std::vector<CiteInfo> CiteList;
typedef std::vector<pair<string,string> > BibLib;

class Bibliography
{
public:
    explicit Bibliography(boost::shared_ptr<BibLib> biblib);
    void feed(boost::shared_ptr<CiteList> citations);
    void show();

    std::map<string, BibCiteStruct > bibFrequency;
    boost::python::dict getDict();
private:
    void increment(CiteInfo& citeInfo);
};

void extractCitation(const Node::ptr node,
                     boost::shared_ptr<CiteList> citations,
                     boost::shared_ptr<BibLib> bibliography,
                     std::string context);

boost::python::dict extractCiteFreequency(const Node::ptr document);



#endif // BIBLIOGRAPHY_H
