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

#ifndef __TEXPP_TOKEN_H
#define __TEXPP_TOKEN_H

#include <texpp/common.h>
#include <boost/pool/singleton_pool.hpp>

namespace texpp {

class Parser;
class Token;

/**
 * @brief The Token class designet to store single semantic objects used in
 *  TeX text. Token class object contain all nesessary information about token
 *  needed to be used for parsing TeX text:
 * - type of token
 * - category code
 * - meaning of token
 * - origin section of the text in the source text file;
 * - location in file
 */
class Token
{
public:
    typedef shared_ptr<Token> ptr;
    typedef vector<Token::ptr> list;
    typedef shared_ptr<list> list_ptr;

    enum Type {
        TOK_SKIPPED,    /*!< symbols whitch do not affect the outcome, skipped
                            during processing: comments, extra spaces, single
                            endline characters, ... */
        TOK_CHARACTER,  /*!< characters whitch shown as is: letters, numbers,
                            punctuations, spaces */
        TOK_CONTROL     //!< control sequences: commands, begin/end group, EOL,...
    };

// Category for characters - separation of content
    enum CatCode {
        CC_ESCAPE = 0,      //!< escape character '\'
        CC_BGROUP = 1,      //!< Begin group: '{'
        CC_EGROUP = 2,      //!< End group: '}'
        CC_MATHSHIFT = 3,   //!< Math shift: '$'
        CC_ALIGNTAB = 4,    //!< Alignment table: &
        CC_EOL = 5,         //!< End-of-line character
        CC_PARAM = 6,       //!< Parameter for macros: '#'
        CC_SUPER = 7,       //!< Math superscript: '^'
        CC_SUB = 8,         //!< Math subscript: '_'
        CC_IGNORED = 9,     //!< Ignored entirely
        CC_SPACE = 10,      //!< Space
        CC_LETTER = 11,     //!< Letters: the alphabet.
        CC_OTHER = 12,      //!< 'Other' character:
        CC_ACTIVE = 13,     //!< Active character: ~
        CC_COMMENT = 14,    //!< Start-of-comment: %
        CC_INVALID = 15,    //!< Invalid-in-input: [DEL]
        CC_NONE = 16
    };

    enum { npos = string::npos };

    /**
     * @brief Token class constructor
     */
    Token(Type type = TOK_SKIPPED, CatCode catCode = CC_INVALID,
            const string& value = string(), const string& source = string(),
            size_t linePos = 0, size_t lineNo = 0,
            size_t charPos = 0, size_t charEnd = 0,
            bool lastInLine = false,
            shared_ptr<string> fileName = shared_ptr<string>())
        : m_type(type), m_catCode(catCode), m_value(value), m_source(source),
          m_linePos(linePos), m_lineNo(lineNo),
          m_charPos(charPos), m_charEnd(charEnd),
          m_lastInLine(lastInLine), m_fileName(fileName) {}

    /**
     * @brief Token pointer constructor. Create Token object and return shared
     *      pointer on object
     */
    static Token::ptr create(Type type = TOK_SKIPPED,
            CatCode catCode = CC_INVALID,
            const string& value = string(), const string& source = string(),
            size_t linePos = 0, size_t lineNo = 0,
            size_t charPos = 0, size_t charEnd = 0,
            bool lastInLine = false,
            shared_ptr<string> fileName = shared_ptr<string>()) {
        return Token::ptr(new Token(type, catCode, value, source,
                linePos, lineNo, charPos, charEnd, lastInLine, fileName)
                );
    }

    //! @page Setters and Getters

    Type type() const { return m_type; }
    void setType(Type type) { m_type = type; }

    CatCode catCode() const { return m_catCode; }
    void setCatCode(CatCode catCode) { m_catCode = catCode; }

    const string& value() const { return m_value; }
    void setValue(const string& value) { m_value = value; }

    const string& source() const { return m_source; }
    void setSource(const string& source) { m_source = source; }

    size_t linePos() const { return m_linePos; }
    void setLinePos(size_t linePos) { m_linePos = linePos; }

    size_t lineNo() const { return m_lineNo; }
    void setLineNo(size_t lineNo) { m_lineNo = lineNo; }

    size_t charPos() const { return m_charPos; }
    void setCharPos(size_t charPos) { m_charPos = charPos; }

    size_t charEnd() const { return m_charEnd; }
    void setCharEnd(size_t charEnd) { m_charEnd = charEnd; }

    /**
     * @return TRUE if token's m_type set to TOK_SKIPPED. FALSE otherwise
     */
    bool isSkipped() const { return m_type == TOK_SKIPPED; }

    /**
     * @return TRUE if token's m_type set to TOK_CONTROL. FALSE otherwise
     */
    bool isControl() const { return m_type == TOK_CONTROL; }

    /**
     * @return TRUE if token's m_type set to TOK_CHARACTER; FALSE otherwise
     */
    bool isCharacter() const { return m_type == TOK_CHARACTER; }

    /**
     * @brief check does token is checkChar
     * @return TRUE if token's m_type set to TOK_CHARACTER and m_value is
     *      checkChar; FALSE otherwise
     */
    bool isCharacter(char checkChar) const {
        return m_type == TOK_CHARACTER &&
                m_value[0] == checkChar;
    }

    /**
     * @brief check does token is character checkChar with category code cat;
     * @return TRUE if token's m_type set to TOK_CHARACTER, m_value set to
     *      checkChar and m_catCode set to cat; FALSE otherwise
     */
    bool isCharacter(char checkChar, CatCode cat) const {
        return m_type == TOK_CHARACTER &&
                m_value[0] == checkChar &&
                m_catCode == cat;
    }

    /**
     * @brief check is token character with category code cat
     * @return TRUE if m_type set to TOK_CHARACTER and m_catCode set to cat.
     *      FALSE otherwise.
     */
    bool isCharacterCat(CatCode cat) {
        return m_type == TOK_CHARACTER && m_catCode == cat;
    }

    /**
     * @brief Getter for m_lastInLine variable;
     * @return m_lastInLine;
     */
    bool isLastInLine() const { return m_lastInLine; }

    /**
     * @brief return source file name for token
     * @return address of source file name
     */
    const string& fileName() const {
        return m_fileName ? *m_fileName : EMPTY_STRING;
    }

    /**
     * @brief return pointer to name of token's source file. Getter for
     *      m_fileName varable
     */
    shared_ptr<string> fileNamePtr() const { return m_fileName; }

    /**
     * @brief represent m_value of token
     * @param parser - needed in case token can be a control command
     */
    string texRepr(Parser* parser = NULL) const;

    /**
     * @brief represent meaning of the token:
     *      if token is character - return string "<catCode> <m_value>"
     *      if token is control - represent token by texRepr(parser) method
     *      if token set to skipped - return "skipped characters" string
     * @param parser needed in case token is a control command
     */
    string meaning(Parser* parser = NULL) const;

    /**
     * @brief represent token in format Token(typeName, catCodeName, value,
     *       source, m_linePos, m_lineNo, m_charPos, m_charEnd )
     * @return string of representation
     */
    string repr() const;

    /**
     * @brief lcopy - method return copy of actual token with all inner settings
     *  responsible for token origin skipped to default value:
     *  - source
     *  - linePos
     *  - lineNo
     *  - charPos
     *  - charEnd
     * @return pointer to the token
     */
    Token::ptr lcopy() const {
        return Token::create(m_type, m_catCode, m_value, "", 0, 0, 0, 0,
            //m_lineNo, m_charEnd, m_charEnd,
            m_lastInLine, m_fileName);
    }

    /**
     * @brief represent commandName. Assumed that string name is name of command.
     *  In general this method change string name. Algorithm is next:
     *  - if commandName begins from "`" - truncate commandName from "`" and
     *      return the result (example texReprControl("`abc") -> "abc")
     *  - if the parser is not a NULL and commandName contain only "\\" symbol,
     *      than return string: escapechar + "csname" + escapechar + "endcsname"
     *  - if the parser is not a NULL, commandName is "\\word" or "\\x" (where
     *      x is LETTER). Method return escapechar + "word" or escapechar + "x"
     *      if space set to FALSE. If space set to TRUE - append space at the end.
     *      So it will return escapechar + "word " and escapechar + "x " respectively.
     *  - otherwise return commandName without changing.
     * @param parser need to get escape character and checking is "x" is letter;
     * @param space - set to TRUE if need space at the end of command
     */
    static string texReprControl(const string& commandName,
                                Parser* parser = NULL, bool space = false);

    /**
     * @brief represent list of Tokens. This method construct string from separate
     *  consecutive Token's represention. Ultimate string filling till it string
     *  length will reach #limit. Then return the result.
     *  - for Charater token the representation is m_value; If the token have
     *      CC_PARAM category code and #param set to FALSE the m_value goes to the
     *      final string-representation twice;
     *  - for Control token the representation forms via texReprControl() method
     *      with "add space at the end" option.
     *  Afer string of consecutive token's representation reach length #limit the
     *  string ends by "\\ETC." and perpesentation loop brokes. But if the limit
     *  set to 0 the loop run till the end of token's list.
     * @param tokens - list of Tokens
     * @param parser - required for control token representation in
     *  texReprControl() method
     * @param param -
     * @param limit - upper limit for ultimate string-representation.
     * @return - string-representation
     */
    // TODO: clarify meaning of #param and finish description above
    static string texReprList(const Token::list& tokens,
            Parser* parser = NULL, bool param = false, size_t limit = 0);

protected:
    Type        m_type;     //!< type of token
    CatCode     m_catCode;  //!< category code for token
    string      m_value;    //!< meaning(semantic) of token
    string      m_source;   //!< token's origin text

    size_t      m_linePos;  //!< total number of symbols above current line
    size_t      m_lineNo;   //!< current line number in source file
    size_t      m_charPos;  //!< position of tekon`s begin (on current line)
    size_t      m_charEnd;  //!< position of token`s end (on current line)

    bool        m_lastInLine;   //!< ID: is this Token the last in the line

    shared_ptr<string> m_fileName;  //!< tex file name - source file for this token

    static string EMPTY_STRING;
};

} // namespace texpp

#endif
