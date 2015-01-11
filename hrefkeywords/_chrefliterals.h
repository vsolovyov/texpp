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

#include <texpp/parser.h>
#include <unicode/stringpiece.h>
#include <unicode/unistr.h>
#include <unicode/normalizer2.h>
#include <unicode/schriter.h>

#include <locale.h>
#include <wchar.h>

/**
 * @brief The WordsDict is class of string dictionary.
 */
class WordsDict {
public:
    /**
     * @brief WordsDict collect words from newline-delimited list of words.
     *  Constructor record all newline-delimited letter combinations longer than
     *  _abbrMinLen and shorter than abbrMaxLen value. Letter combinations ending
     *  by "'s" skips too.
     * @param filename - source file name which should be newline-delimited list
     *  of dictionary words.
     * @param abbrMaxLen - word's upper length limit value
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
inline bool _isglue(wchar_t ch);

/**
 * @return true if ch is '~', '/', '-' or space. Otherwise return false
 */
inline bool _isIgnored(wchar_t ch);

/**
 * @return true if word is article. Otherwise return false
 */
inline bool _isIgnoredWord(const std::string& word);

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
 *  determines "is the input literal an abbreviation or not"
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

void _extractTextInfo(boost::python::dict& result,
                      const texpp::Node::ptr node,
                      const boost::regex& exclude_regex,
                      const std::string& workdir = std::string());

#endif // _CHREFLITERALS_H
