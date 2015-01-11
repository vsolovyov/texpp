/*  This file is part of texpp library.
    Copyright (C) 2009 Vladimir Kuznetsov <ks.vladimir@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "_chrefliterals.h"

using namespace boost::python;
using namespace texpp;
using std::string;

WordsDict::WordsDict(string filename, size_t abbrMaxLen)
    : _abbrMaxLen(abbrMaxLen)
{
    // open file
    std::ifstream wordsfile(filename.c_str());
    string word;
    while(wordsfile.good()) {
        std::getline(wordsfile, word);
        size_t s = word.size();
        if(s >= _abbrMinLen &&
                s <= abbrMaxLen &&
                word.substr(s-2) != "'s") {
            _words.insert(word);
        }
    }
}

inline bool _isglue(wchar_t ch) {
    return iswdigit(ch) || ch == L'-' || ch == L'/';
}

inline bool _isIgnored(wchar_t ch) {
    return ch == L'~' || ch == L'-' || ch == L'/' || iswspace(ch);
}

inline bool _isIgnoredWord(const string& word) {
    return word == "the" || word == "a" || word == "an" ||
           word == "The" || word == "A" || word == "An";
}

string wStrToStr(std::wstring wstr, bool toLower)
{
    string result("");
    char buf[MB_CUR_MAX + 1];
    std::mbstate_t state = std::mbstate_t();
    setlocale(LC_ALL, "en_US.UTF-8");
    for (size_t i=0; i < wstr.length(); i++) {
        size_t len = wcrtomb(buf, toLower? towlower(wstr[i]) : wstr[i], &state);
        if (len <= 0 || len > MB_CUR_MAX)
            break;
        buf[len] = 0;
        result += buf;
    }
    return result;
}

// TODO: refactoring "input","position"
std::wstring strToWStr(string input)
{
    setlocale(LC_ALL, "en_US.UTF-8");
    const char* position = input.c_str();
    std::mbstate_t state = std::mbstate_t();
    wchar_t wChar;
    std::wstring result;
    while (*position) {
        // Convert multibyte sequence to wide character. (position -> wChar)
        size_t len = mbrtowc(&wChar, position, MB_CUR_MAX, &state);
        if (len <= 0 || len > MB_CUR_MAX)
            break;
        position += len;
        result += wChar;
    }
    return result;
}

// TODO: refactoring "input", "input_backup"
std::wstring stem_wstring(std::wstring input, bool multilang)
{
    std::wstring input_backup(input);
    stemming::english_stem<> StemEnglish;
    StemEnglish(input);
    if (input != input_backup && input.length() != 0)
        return input;
    input = input_backup;
    if (multilang)
    {
        stemming::french_stem<> StemFrench;
        StemFrench(input);
        if (input != input_backup && input.length() != 0)
            return input;
        input = input_backup;
        stemming::german_stem<> StemGerman;
        StemGerman(input);
        if (input != input_backup && input.length() != 0)
            return input;
        input = input_backup;
        stemming::spanish_stem<> StemSpanish;
        StemSpanish(input);
        if (input != input_backup && input.length() != 0)
            return input;
        input = input_backup;
        stemming::italian_stem<> StemItalian;
        StemItalian(input);
        if (input != input_backup && input.length() != 0)
            return input;
        input = input_backup;
        stemming::portuguese_stem<> StemPortuguese;
        StemPortuguese(input);
        if (input != input_backup && input.length() != 0)
            return input;
        input = input_backup;
        stemming::dutch_stem<> StemDutch;
        StemDutch(input);
        if (input != input_backup && input.length() != 0)
            return input;
        input = input_backup;
        stemming::danish_stem<> StemDanish;
        StemDanish(input);
        if (input != input_backup && input.length() != 0)
            return input;
        input = input_backup;
        stemming::finnish_stem<> StemFinnish;
        StemFinnish(input);
        if (input != input_backup && input.length() != 0)
            return input;
        input = input_backup;
        stemming::norwegian_stem<> StemNorwegian;
        StemNorwegian(input);
        if (input != input_backup && input.length() != 0)
            return input;
        input = input_backup;
        stemming::swedish_stem<> StemSwedish;
        StemSwedish(input);
        if (input != input_backup && input.length() != 0)
            return input;
        input = input_backup;
    }
    stemming::russian_stem<> StemRussian;
    StemRussian(input);
    if (input != input_backup && input.length() != 0)
        return input;
    return input_backup;
}

// TODO refactor:
//      n(position counter),
//      clear from unused variables ?
string normLiteral(string literal,
                   const WordsDict* wordsDict,
                   const dict& whiteList,
                   bool multilang)
{
    stemming::english_stem<> StemEnglish;
    std::wstring nWLiteral;
    size_t wordStart = string::npos;
    size_t lastDot = string::npos;
    string unicodeNormLiteral;
    // work with plane text
    setlocale(LC_ALL, "en_US.UTF-8");
    /* Decompose unicode chars to the basic ones */
    UErrorCode ecode = U_ZERO_ERROR;
    // Use name="nfkc" and UNORM2_COMPOSE/UNORM2_DECOMPOSE for Unicode standard NFKC/NFKD
    const Normalizer2* normalizer = icu::Normalizer2::getInstance(NULL, "nfkc", UNORM2_DECOMPOSE, ecode);
    // In ICU, a Unicode string consists of 16-bit Unicode code units
    UnicodeString finalStr;
    UnicodeString ULiteral = icu::UnicodeString::fromUTF8(StringPiece(literal));
    UnicodeString normULiteral = normalizer->normalize(ULiteral, ecode);
    StringCharacterIterator iter(normULiteral);
    // according to unicode format, we deal with code units.
    // code units goes to the finalStr i case they correspond to BASE character
    // (letters, numbers, spacing combining marks, and enclosing marks),
    // a BLANK character ("blank" or "horizontal space") or
    // a PUNCTUATION character.
    while(iter.hasNext())
    {
        if (u_isbase(iter.current()) ||
                u_isblank(iter.current()) ||
                u_ispunct(iter.current()))
            finalStr += iter.current();
        // move iterator to the next code unit
        iter.next();
    }
    // Convert the UnicodeString final to UTF-8 and append the result
    // to a standard string unicodeNormLiteral
    finalStr.toUTF8String(unicodeNormLiteral);
    std::wstring wLiteral = strToWStr(unicodeNormLiteral);
    size_t literalLength = wLiteral.length();
    /* convert to lowercase and check the whitelist */
    // if literal in known from whiteList abbreviation, than return it
    if(whiteList.has_key(wStrToStr(wLiteral, true))) return literal;

    /* TODO: handle 's */
    for(size_t n=0; ; ++n) {
        wchar_t ch = n < literalLength ? wLiteral[n] : 0;

        if(wordStart < n) { // inside a word
            if(iswalnum(ch)) {  // if ch is alpha or digit
                // add to current word
            } else if(ch == '.') {
                // add to current word and remember the dot position
                lastDot = n;
            } else { // end of the word

                // re-read current char next time
                --n;
                
                // check for the dot
                if(lastDot < n) {
                    // dot present but not as the last char
                    n = lastDot;
                }

                // extract and lower the word
                bool isAbbr = false;    // tag - does the literal is abbreviation
                size_t lastUpper = string::npos;
                size_t firstLower = string::npos;
                std::wstring word(wLiteral.begin() + wordStart, wLiteral.begin() + n + 1);
                std::wstring word1(wLiteral.begin() + wordStart, wLiteral.begin() + n + 1);

                // reset the word
                size_t pLastDot = lastDot;
                wordStart = lastDot = string::npos;

                for(size_t k = 0; k < word.length(); ++k) {
                    if(iswupper(word1[k])) {
                        word1[k] = towlower(word1[k]);
                        lastUpper = k;
                        if(k != 0) isAbbr = true;
                    } else if(iswlower(word1[k])) {
                        if(firstLower > k) firstLower = k;
                    } else { // dot
                        isAbbr = true;
                    }
                }

                //
                if(n+1 < literalLength && _isIgnoredWord(wStrToStr(word))) {
                    // Skip articles, but not at the end
                    continue;
                }

                // if the literal still didn`t marked as an abbreviation and length in limits
                if(!isAbbr && word.length() <= wordsDict->abbrMaxLen()) {
                    // the word CAN BE by an abbreviation if word is NOT a part of wordsDict
                    isAbbr = !wordsDict->contains(wStrToStr(word));
                    if(isAbbr && lastUpper == 0)
                        isAbbr = !wordsDict->contains(wStrToStr(word1));
                }
                
                // process the word
                if(isAbbr) {
                    if(pLastDot > n) { // if no dots in abbreviation
                        // Stem plural forms for uppercase abbrevations
                        if(word.length() > 2 && firstLower == word.length()-2 &&
                                word[word.length()-2] == L'e' &&
                                word[word.length()-1] == L's') {
                            // stem "ABCes" from "es" ending
                            word.resize(word.length() - 2);
                        } else if(word.length() > 1 && firstLower == word.length()-1 &&
                                word[word.length()-1] == L's') {
                            // stem "ABCs" from "s" ending
                            word.resize(word.length() - 1);
                        }
                        // move abbreviation to upper and put dot after every letter
                        for(size_t k=0; k<word.length(); ++k) {
                            nWLiteral += iswlower(word[k]) ? towupper(word[k]) : word[k];
                            nWLiteral += (L'.');
                        }
                    } else {
                        for(size_t k=0; k<word.length(); ++k) {
                            nWLiteral += iswlower(word[k]) ? towupper(word[k]) : word[k];
                        }
                    }
                } else {
                    nWLiteral += stem_wstring(word1, multilang);
                }
            }
        } else { // not inside a word
            if(_isIgnored(ch)) {
                continue;
            } else if(iswalnum(ch)) {   // if ch a alpha or digit
                wordStart = n;          // mark the start of word
            } else if(ch == '\'' && n+1 < literalLength && (literal[n+1] == 's') &&
                    n != 0 && (iswalpha(literal[n-1]))) {
                if(n+2==literalLength) break;
                wchar_t ch2 = wLiteral[n+2];
                if(iswalnum(ch2) || ch2 == '.')
                    nWLiteral += wLiteral[n];
                else
                    ++n;
            } else if(n >= literalLength) { // end of literal has been reached
                break;
            } else {
                nWLiteral += wLiteral[n]; // use character as-is
            }
        }
    }
    return wStrToStr(nWLiteral);
}

string absolutePath(const string& initPath, const string& workDir)
{
    using boost::filesystem::path;

    path ultiPath = path(initPath);
    if(!ultiPath.is_absolute()) {
// TODO: Check is it work correctly
//        if(!workDir.empty()) {      // if workdir is NOT empty
//            path wd = path(workDir);
//            if(!wd.is_absolute())    // if wokdir is NOT absolute
//                // terminate wd, now wd is absolute path
//                wd = boost::filesystem::current_path() / wd;
//            ultiPath = wd / ultiPath;  // append work directory to the initial path
//        } else {
//            ultiPath = boost::filesystem::current_path() / ultiPath;
//        }
        path wd = workDir.empty() ? boost::filesystem::current_path() :
                      path(workDir).is_absolute() ? path(workDir) :
                             boost::filesystem::current_path() / path(workDir);
        ultiPath = wd / ultiPath;
    }

    // path can contain repetitive sequences of path delimiters and "/../"
    // expressions in path variable which mean "one directory up". So we need to
    // normalize path. For example "/root/dir1///dir2/../dir3///file.ext" boils
    // down to "/root/dir1/dir3/file.ext

#warning normalize() method is deprecated and can be not maintained in feature \
            versions of Boost library
    string retString = ultiPath.normalize().string();
    // somehow normalize() method add "/." at the end of string. Cut it:
    if(retString.size() > 2 && 0==retString.compare(retString.size()-2, 2, "/."))
        retString.resize(retString.size()-2);
    return retString;
}

/**
 * @brief isLocalFile check does str look like local file. For this purpose we check several features:
 *      if str does NOT contain absolute path --> return TRUE
 *      if str contain absolute path and workdir is part of it --> return TRUE
 *      otherwise return FALSE
 * @param fileName - file name(including path);
 * @param workdir - path to the subdirectory where the file is located;
 * @return bool value is str local or does NOT.
 */
bool isLocalFile(const string& fileName, const string& workdir)
{
    string aWorkdir = absolutePath(workdir, string());
    // if str include aWorkdir than return TRUE
    return absolutePath(fileName, aWorkdir)
            .compare(0, aWorkdir.size(), aWorkdir) == 0;
}

#define __PYTHON_NEXT(obj, iter) \
    object obj; \
    try { obj = iter.attr("next")(); } \
    catch (error_already_set const &) { \
        if(PyErr_ExceptionMatches(PyExc_StopIteration)) break; \
        throw; \
    }

struct TextTag
{
    enum Type {
        TT_OTHER = 0,
        TT_WORD,
        TT_CHARACTER,
        TT_LITERAL,
        TT_SECTION
    };

    Type   type;
    size_t start;
    size_t end;
    string value;
    std::wstring wcvalue;

    // XXX: boost::python does not support pickling of enums
    explicit TextTag(int t = TT_OTHER, size_t s = 0, size_t e = 0,
                            const string& val = string())
        : type(Type(t)), start(s), end(e), value(val), wcvalue(strToWStr(val)) {}

    bool operator==(const TextTag& o) {
        return o.type == type && o.start == start &&
               o.end == end && o.value == value;
    }

    bool operator!=(const TextTag& o) {
        return o.type != type || o.start != start ||
               o.end != end || o.value != value;
    }

    /**
     * @brief repr is observer functions that allow you to obtain the string
     * representation of a TextTag object
     * @return string representing of TextTag object in format:
     *      TextTag(type, start, end, "value")
     */
    string repr() const {
        static const char* types[] = {
            "OTHER", "WORD", "CHARACTER", "LITERAL"
        };
        std::ostringstream out;
        out << "TextTag("
            << (type <= TT_LITERAL && type >= 0 ? types[type] : "UNKNOWN")
            << ", " << start << ", " << end << ", \"" << value << "\")";
        return out.str();
    }
};

typedef std::vector<TextTag> TextTagList;

/**
 * @brief textTagListRepr is observer functions that allow you to obtain the
 *      string representation of a TextTagList object
 * @param list - TextTagList object
 * @return string representing in form
 *      "TextTagList(TextTag(type, start, end, "value"), TextTag(...), ...)"
 */
string textTagListRepr(const TextTagList& list)
{
    std::ostringstream out;
    out << "TextTagList(";
    TextTagList::const_iterator e = list.end();
    for(TextTagList::const_iterator it = list.begin(); it != e; ++it) {
        if(it != list.begin())
            out << ", ";
        out << it->repr();
    }
    out << ")";
    return out.str();
}

/**
 * @brief I don't don't know how struct based on pickle_suite class realy doing
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
    static tuple getinitargs(const TextTag& tag) {
        return make_tuple(int(tag.type), tag.start, tag.end, tag.value);
    }
};

struct TextTagListPickeSuite: pickle_suite
{
    static list getstate(const TextTagList& l) {
        list ret;
        TextTagList::const_iterator e = l.end();
        for(TextTagList::const_iterator it = l.begin(); it != e; ++it) {
            ret.append(make_tuple(int(it->type), it->start, it->end, it->value));
        }
        return ret;
    }
    static void setstate(TextTagList& l, list state) {
        l.resize(len(state));
        size_t n = 0;
        for(stl_input_iterator<tuple> it(state), e; it != e; ++it) {
            int type = extract<int>((*it)[0]);
            size_t start = extract<size_t>((*it)[1]);
            size_t end = extract<size_t>((*it)[2]);
            string value = extract<string>((*it)[3]);
            l[n++] = TextTag(type, start, end, value);
        }
    }
};


void _extractTextInfo(dict& result,
                      const Node::ptr node,
                      const boost::regex& exclude_regex,
                      const string& workdir)
{
    size_t childrenCount = node->childrenCount();

    if(childrenCount == 0) {
        return;
    }

    // append currert path to the workdir if workdir is relative path,
    // otherwise just init aWorkdir by the workdir
    string aWorkdir = absolutePath(workdir, string());
    shared_ptr<string> lastFile;
    TextTagList* tags = 0;
    for(size_t n = 0; n < childrenCount; ++n) {
        Node::ptr child = node->child(n);

        // check type
        TextTag::Type type;
        if(child->type() == "text_word") {
            type = TextTag::TT_WORD;
        } else if(child->type() == "text_character" ||
                  child->type() == "text_space") {
            type = TextTag::TT_CHARACTER;
        } else if(child->type() == "section") {
            type = TextTag::TT_SECTION;
        }
        else {
            type = TextTag::TT_OTHER;
        }

        if(type != TextTag::TT_OTHER) {
            if(child->isOneFile()) {
                shared_ptr<string> file = child->oneFile();
                if(file && (file == lastFile ||
                            isLocalFile(*file, workdir))) {
                    if(file != lastFile || !tags) {
                        lastFile = file;
                        string ffile = absolutePath(*file, aWorkdir)
                                        .substr(aWorkdir.size()+1);
                        TextTagList& tl = extract<TextTagList&>(
                            result.setdefault(ffile, TextTagList()));
                        tags = &tl;
                    }

                    // Save the node
                    std::pair<size_t, size_t> pos = child->sourcePos();
                    tags->push_back(TextTag(type, pos.first, pos.second,
                                                child->valueString()));
                }
            }
        } else if(child->type().compare(0, 12, "environment_") == 0 &&
                    !boost::regex_match(child->type(), exclude_regex)) {
            if (child->type().compare(12, 20, "abstract") == 0) {
                std::pair<size_t, size_t> pos = child->sourcePos();
                tags->push_back(TextTag(TextTag::TT_SECTION, pos.first,
                                pos.second, "abstract"));
            }
            _extractTextInfo(result, child, exclude_regex, workdir);
            tags = 0; // XXX: it it really required ?
        }
    }
}

dict extractTextInfo(const Node::ptr node, const string& exclude_regex,
                const string& workdir = string())
{
    dict result;
    boost::regex rx(exclude_regex, boost::regex::extended);
    _extractTextInfo(result, node, rx, workdir);
    return result;
}

string getDocumentEncoding(const Node::ptr node)
{
    string result;
    const Node::ChildrenList& c = node->children();
    for(Node::ChildrenList::const_iterator it = c.begin(), e = c.end();
                            it != e; ++it) {
        if(it->second->type() == "inputenc") {
            result = it->second->valueString();
            break;
        }
    }
    if(!result.empty())
        return result;
    else
        return string("ascii");
}

TextTagList findLiterals(const TextTagList& tags,
        const dict& literals, const dict& notLiterals,
        const WordsDict* wordsDict, const dict& whiteList,
        size_t maxChars = 0, bool multilang=false)
{
    TextTagList result;

    // Detemine a maximum literal length
    if(maxChars == 0) {
        for(stl_input_iterator<str> it(literals.keys()), e;
                                                it != e; ++it) {
            size_t l = len(*it);
            if(l > maxChars)
                maxChars = l;
        }
    }

    // Process the text
    size_t count = tags.size();
    for(size_t n = 0; n < count; ++n) {
        if(tags[n].type == TextTag::TT_CHARACTER) {
            // Do not start from ignored character
            if(_isIgnored(tags[n].wcvalue[0])) continue;
        } else if(tags[n].type != TextTag::TT_WORD) {
            // Ignore unknown tags
            continue;
        }

        // If previous tag is character and is adjacent,
        // then it should be a space
        if(n && tags[n-1].end == tags[n].start &&
                tags[n-1].type == TextTag::TT_CHARACTER &&
                _isglue(tags[n-1].wcvalue[0])) {
            continue;
        }

        // Do not start on an article
        if(_isIgnoredWord(tags[n].value)) {
            continue;
        }

        string text;
        size_t pos = tags[n].start;
        std::vector<boost::tuple<string, size_t, size_t> > foundLiterals;

        for(size_t k = n; k < count; ++k) {
            const TextTag& tagk = tags[k];

            // Stop if tag is not adjacent
            if(tagk.start != pos) {
                break;
            }
            pos = tagk.end;

            text += tagk.value;
            if(tagk.type == TextTag::TT_CHARACTER) {
                // Skip ignored characters
                if(_isIgnored(tagk.wcvalue[0])) continue;
            } else if(tagk.type != TextTag::TT_WORD) {
                // Stop on unknown tags
                break;
            }

            // If next tag is character and is adjacent,
            // then it should be a space
            if(k+1 < count && tags[k+1].start == tags[k].end &&
                    tags[k+1].type == TextTag::TT_CHARACTER &&
                    _isglue(tags[k+1].wcvalue[0])) {
                continue;
            }

            // Norm literal
            string literal = normLiteral(text, wordsDict, whiteList, multilang);
            if(literal.size() > maxChars) {
                break; // XXX: can normLiteral size gets smaller ?
            }

            // Lookup in dictionary
            if(literals.has_key(literal)) {
                // Skip known non-literal words
                if((!notLiterals.has_key(text)) &&
                        (k+1>=count ||
                         tags[k+1].type != TextTag::TT_CHARACTER ||
                         tags[k+1].wcvalue[0] != L'.' ||
                         !notLiterals.has_key(text+'.'))) {
                    foundLiterals.push_back(
                            boost::make_tuple(literal, tagk.end, k));
                }
            }
        }

        if(!foundLiterals.empty()) { // XXX: return all found literals !
            // Create a tag for the longest literal found
            result.push_back(TextTag(TextTag::TT_LITERAL, tags[n].start,
                                foundLiterals.back().get<1>(),
                                foundLiterals.back().get<0>()));
            n = foundLiterals.back().get<2>();
        }
    }

    return result;
}

string replaceLiterals(const string& source,
                    const TextTagList& tags)
{
    string result;
    result.reserve(source.size() + source.size()/5);
    size_t pos = 0;
    TextTagList::const_iterator end = tags.end();
    for(TextTagList::const_iterator it = tags.begin(); it != end; ++it) {
        if(it->type == TextTag::TT_LITERAL) {
            result += source.substr(pos, it->start - pos);
            result += it->value;
            pos = it->end;
        }
    }
    result += source.substr(pos, source.size() - pos);
    return result;
}

void export_TextTag()
{
    using namespace boost::python;
    scope scope_TextTag = class_<TextTag>("TextTag",
            init<int, size_t, size_t, string>())
        .def_readwrite("type", &TextTag::type)
        .def_readwrite("start", &TextTag::start)
        .def_readwrite("end", &TextTag::end)
        .def_readwrite("value", &TextTag::value)
        .def(self == self)
        .def(self != self)
        .def("__repr__", &TextTag::repr)
        .def_pickle(TextTagPickeSuite())
    ;

    enum_<TextTag::Type>("Type")
        .value("OTHER", TextTag::TT_OTHER)
        .value("WORD", TextTag::TT_WORD)
        .value("CHARACTER", TextTag::TT_CHARACTER)
        .value("LITERAL", TextTag::TT_LITERAL)
        .value("SECTION", TextTag::TT_SECTION)
    ;
}

BOOST_PYTHON_MODULE(_chrefliterals)
{
    using namespace boost::python;

    export_TextTag();
    class_<TextTagList>("TextTagList")
        .def("__repr__", &textTagListRepr)
        .def(vector_indexing_suite<TextTagList>())
        .def_pickle(TextTagListPickeSuite())
    ;

    class_<WordsDict>("WordsDict", init<string, size_t>())
        .def("insert", &WordsDict::insert)
        .def("contains", &WordsDict::contains)
    ;

    def("absolutePath", &absolutePath);
    def("isLocalFile", &isLocalFile);
    def("normLiteral", &normLiteral);
    def("extractTextInfo", &extractTextInfo);
    def("getDocumentEncoding", &getDocumentEncoding);
    def("findLiterals", &findLiterals);
    def("replaceLiterals", &replaceLiterals);
}
