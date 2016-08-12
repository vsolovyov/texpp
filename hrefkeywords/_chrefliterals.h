#ifndef _CHREFLITERALS_H
#define _CHREFLITERALS_H

#include <boost/python.hpp>
#include <boost/python/stl_iterator.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "stem_include/english_stem.h"
#include "stem_include/french_stem.h"
#include "stem_include/german_stem.h"
#include "stem_include/finnish_stem.h"
#include "stem_include/swedish_stem.h"
#include "stem_include/spanish_stem.h"
#include "stem_include/dutch_stem.h"
#include "stem_include/danish_stem.h"
#include "stem_include/italian_stem.h"
#include "stem_include/norwegian_stem.h"
#include "stem_include/portuguese_stem.h"
#include "stem_include/russian_stem.h"

// exclude deprecated methods. They can be not maintained in the feature
//#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <boost/tuple/tuple.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <set>
#include <string>
#include <fstream>
#include <sstream>

#include <cstring>
#include <cctype>

#include <iostream>
#include <memory>
#include <utility>
#include "bibliography.h"

#include <texpp/parser.h>
#include <unicode/stringpiece.h>
#include <unicode/unistr.h>
#include <unicode/normalizer2.h>
#include <unicode/schriter.h>

#include <locale.h>
#include <wchar.h>

using namespace boost::python;
using namespace texpp;
using std::string;
using boost::shared_ptr;

/**
 * @brief The WordsDict is class of string dictionary.
 */
class WordsDict {
public:
    /**
     * @brief WordsDict collect words from newline-delimited list of words.
     *  Constructor record all newline-delimited letter combinations longer than
     *  _abbrMinLen and shorter than abbrMaxLen value. Also skip words with "'s"
     *  completion.
     * @param filename - source file name which should be newline-delimited list
     *  of dictionary words. For example "/usr/share/dict/words".
     * @param abbrMaxLen - upper limit of word's length
     */
    WordsDict(std::string filename, size_t abbrMaxLen);

    size_t abbrMaxLen() const { return _abbrMaxLen; }

    void insert(std::string word) { _words.insert(word); }
    bool contains(std::string word) const { return _words.count(word); }

protected:
    std::set<std::string> _words;           //!< words dictionary
    size_t _abbrMaxLen;                     //!< max length of words
    static const size_t _abbrMinLen = 2;    //!< min length of words
};

/**
 * @return true if ch is '-', '/' digit. Otherwise return false
 */
inline bool _isglue(wchar_t ch) {
    return iswdigit(ch) || ch == L'-' || ch == L'/';
}

/**
 * @return true if ch is '~', '/', '-' or space. Otherwise return false
 */
inline bool _isIgnored(wchar_t ch) {
    return ch == L'~' || ch == L'-' || ch == L'/' || iswspace(ch);
}

/**
 * @return true if word is an article. Otherwise return false
 */
inline bool _isIgnoredWord(const string& word) {
    return word == "the" || word == "a" || word == "an" ||
           word == "The" || word == "A" || word == "An";
}

/**
 * @brief wStrToStr - decompose word wstr from wide(multibyte) characters
 *      String format (basic_string<wchar_t>) into narrow(1 byte) character
 *      array (basic_string<char>)
 * @param wstr - string of wide characters
 * @param toLower - case convertor trigger
 *      if true - translate to lower case all letters in wstr
 *      if false - leave the word register as is
 * @return narrow character array where the multibyte characters are stored
 *      consistently and separated by '\0' delimiter
 */
std::string wStrToStr(std::wstring wstr, bool toLower=false);

/**
 * @brief strToWStr - compose wide characters String from narrow character array
 * @param input - narrow(1 byte) character array (basic_string<char>)
 * @return translated wide(multibyte) characters String (basic_string<wchar_t>)
 */
std::wstring strToWStr(std::string input);

/**
 * @brief stem_wstring make stemming of wide string
 * @param input - target for stemming, wide character string
 * @param multilang - bool trigger to support multilanguage stemming
 *      if true - support english, french, german, spanish, italian, portuguese,
 *           dutch, danish, finnish_stem, norwegian, swedish and russian
 *           languages
 *      if false(by default) - support only english and russian languages
 * @return stemmed word if steming passed successful.
 *      return the input word otherwise
 */
std::wstring stem_wstring(std::wstring input, bool multilang=false);

/**
 * @brief normLiteral - separate input literal from unpredicted kind of symbols,
 *  then determines "whether the input literal an abbreviation or not?"
 *      If no - make stemming of input literal, return the result
 *      if yes - return input literal in upper case when every letter separated
 *          by dots.
 * @param literal - input string variable
 * @param wordsDict - words dictionary. Dictionary contain all words which can`t
 *      be an abbreviation
 * @param whiteList - dictionary with known abbreviation
 * @param multilang - bool variable. Will support multilingualism?
 * @return normalized string
 */
std::string normLiteral(std::string literal,
                        const WordsDict* wordsDict,
                        const boost::python::dict& whiteList,
                        bool multilang);

/**
 * @brief absolutePath - return absolute path string assembled from the arguments.
 *  return initPath if initPath is absolute path. Otherwise fill workDir by
 *  current system path if workDir is relative. Append workDir to the initPath
 *  and return the result.
 * @param initPath - initial path
 * @param workDir - additional path for appending right to the initPath
 * @return absolute path string "[[system path /] workDir /] initPath"
 */
std::string absolutePath(const std::string& initPath, const std::string& workDir);

/**
 * @brief isLocalFile check does str look like local file. For this purpose we
 *  check several features:
 *      if str does NOT contain absolute path --> return TRUE
 *      if str contain absolute path with workdir as part of it --> return TRUE
 *      otherwise return FALSE
 * @param fileName - file name(including path);
 * @param workdir - path to the subdirectory where the file is located;
 * @return bool value is str local or does NOT.
 */
bool isLocalFile(const std::string& fileName, const std::string& workdir);

struct TextTag
{
    enum Type {
        TT_OTHER = 0,
        TT_WORD,
        TT_CHARACTER,
        TT_LITERAL,
        TT_SECTION,
        TT_COMMAND
    };

    Type   type;    // type of node to be tagged
    size_t start;   // start positin of node on it`s source file
    size_t end;     // end positin  of node on it`s source file
    string value;   // m_value of node
    std::wstring wcvalue;

    // XXX: boost::python does not support pickling of enums
    explicit TextTag(int t = TT_OTHER, size_t s = 0, size_t e = 0,
                            const string& val = string())
        : type(Type(t)), start(s), end(e), value(val), wcvalue(strToWStr(val)) {}

    bool operator==(const TextTag& o);

    bool operator!=(const TextTag& o);

    /**
     * @brief observer functions that allow you to obtain the string
     *  representation of a TextTag object
     * @return string representing of TextTag object in format:
     *  TextTag(type, start, end, "value")
     */
    std::string repr() const;
};

typedef std::vector<TextTag> TextTagList;

/**
 * @brief textTagListRepr is observer functions that allow you to obtain the
 *      string representation of a TextTagList object
 * @param list - TextTagList object
 * @return string representing in form
 *      "TextTagList(TextTag(type, start, end, "value"), TextTag(...), ...)"
 */
string textTagListRepr(const TextTagList& list);

/**
 * @brief I don't know how struct based on pickle_suite class realy doing
 *  in Python but have some hints from boost.org.
 *  It is often necessary to save and restore the contents of an object to a file.
 *  One approach to this problem is to write a pair of functions that read and
 *  write data from a file in a special format. A powerful alternative approach
 *  is to use Python's pickle module. Exploiting Python's ability for introspection,
 *  the pickle module recursively converts nearly arbitrary Python objects into
 *  a stream of bytes that can be written to a file.
 */
struct TextTagPickeSuite: pickle_suite
{
    static boost::python::tuple getinitargs(const TextTag& tag) {
        return boost::python::make_tuple(int(tag.type), tag.start, tag.end, tag.value);
    }
};

struct TextTagListPickeSuite: pickle_suite
{
    static list getstate(const TextTagList& l);
    static void setstate(TextTagList& l, list state);
};

/**
 * @brief _extractTextInfo екстрактор інформації про abstract секцію в документі
 *  node: положення у файлі, положення сусідніх вузлів, та інформацію, про верхні
 *  вузли всередині Node abstract ..........................
 * @param result -
 * @param node - the node to be explored
 * @param exclude_regex - regular expression to exclude undesirable nodes from
 *  processing
 * @param workdir - work directory of the node's source file
 */
void _extractTextInfo(boost::python::dict& result,
                      const texpp::Node::ptr node,
                      const boost::regex& exclude_regex,
                      const std::string& workdir = std::string());

dict extractTextInfo(const Node::ptr node,
                     const string& exclude_regex,
                     const string& workdir = string());

/**
 * @brief getDocumentEncoding - extract document encoding from node
 * @param node - parsing node object
 * @return encoding string extracted from the node, and "ascii" in case encoding
 *  declaration was not found
 */
string getDocumentEncoding(const Node::ptr node);


/**
 * @brief findLiterals
 * @param tags
 * @param literals
 * @param notLiterals - known non-literal words
 * @param wordsDict - dictionary of all words which is not an abbreviation
 * @param whiteList - dictionary of known abbreviation
 * @param maxChars - maximum length of literas from @tags to be treated ...
 *  by default set to length of longest literal from the @literals dictionary
 * @param multilang - multilanguage support
 * @return
 */
TextTagList findLiterals(const TextTagList& tags,
                         const dict& literals,
                         const dict& notLiterals,
                         const WordsDict* wordsDict,
                         const dict& whiteList,
                         size_t maxChars = 0,
                         bool multilang=false);

#endif // _CHREFLITERALS_H
