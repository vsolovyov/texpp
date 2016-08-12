#include "hrefkeywords/_chrefliterals.h"
#include "fstream"

#include <texpp/parser.h>
#include <texpp/logger.h>

#include <texpp/base/bibliography.h>
#include <tests/testbundle.h>

using namespace boost::python;
using namespace texpp;

using std::cout;
using std::endl;


// -DCMAKE_BUILD_TYPE=Debug .
int main()
{
    string wordDictSource = "/usr/share/dict/words";

    std::cout << _isIgnoredWord("book") << std::endl;
    WordsDict wDict = WordsDict(wordDictSource, 6);
    std::cout << wDict.abbrMaxLen() << std::endl;

    Py_Initialize();
    dict pyDict = dict();
    pyDict.items();

    bool interactive = false;

    string fileName = "/home/bereziuk/lausanne/tests/1504.04854v1/chiralMHD.tex";
//    string fileName = "/home/bereziuk/lausanne/cptex/1310.3811v1/Vorticity.tex";
//    string fileName = "/home/bereziuk/lausanne/ptex/1106.0175v1/CRAS_Fermi_FP_VC_final_new_astroph.tex";
//    string fileName = "/home/bereziuk/lausanne/texpp/tests/texParts/head.tex";
//    string fileName = "/home/bereziuk/lausanne/texpp/tests/texParts/begin_document.tex";
//    string fileName = "/home/bereziuk/lausanne/texpp/tests/texParts/abstract.tex";
//    string fileName = "/home/bereziuk/lausanne/texpp/tests/texParts/bibliography.tex";
//    string fileName = "/home/bereziuk/lausanne/texpp/tests/texParts/skelet.tex";
//    string fileName = "/home/bereziuk/lausanne/texpp/tests/texParts/cite.tex";

    std::istream* file = new std::ifstream(fileName.c_str(), std::ifstream::in);

    texpp::Parser parser(
            shared_ptr<TestBundle>(new TestBundle("<test-name>", file)),
            texpp::Logger::ptr(new texpp::ConsoleLogger));
    texpp::Node::ptr document = parser.parse();

    std::cout << std::endl << "Parsed document: " << std::endl;
//    std::cout << document->treeRepr();

    std::cout<< std::endl;

    extractCiteFrequency(document);

//    dict d = extractTextInfo(document, ".*equation.*|.*eqn.*", "");
//    list l = d.items();
//    std::cout << len(l) << std::endl;

    return 0;
}
