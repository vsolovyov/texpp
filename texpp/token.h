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

class Token
{
public:
    typedef shared_ptr<Token> ptr;
    typedef vector<Token::ptr> list;
    typedef shared_ptr<list> list_ptr;

    enum Type {
        TOK_SKIPPED,    // coments(%...), spaces,
        TOK_CHARACTER,  // letters,\n,\r,{,},
        TOK_CONTROL     // \par, \def, \usepakage
                        // \draft,\documentclass,\setcitestyle,...
    };

// Category for characters - separation of content
    enum CatCode {
        CC_ESCAPE = 0,      // escape character "\"
        CC_BGROUP = 1,      // Begin group: {
        CC_EGROUP = 2,      // End group: }
        CC_MATHSHIFT = 3,   // Math shift: $
        CC_ALIGNTAB = 4,    // Alignment table: &       ->   \haign,\valign
        CC_EOL = 5,         // End-of-line '\n'         ->   \endlinechar
        CC_PARAM = 6,       // Parameter for macros: #
        CC_SUPER = 7,       // Math superscript: ^
        CC_SUB = 8,         // Math subscript: _
        CC_IGNORED = 9,     // Ignored entirely         -> <null> character == 0
        CC_SPACE = 10,      // Space
        CC_LETTER = 11,     // Letters: the alphabet.
        CC_OTHER = 12,      // 'Other' character - everything else: ., 1, :, etc.
        CC_ACTIVE = 13,     // Active character - to be interpreted as control sequences: ~
        CC_COMMENT = 14,    // Start-of-comment: %
        CC_INVALID = 15,    // Invalid-in-input: [DEL]
        CC_NONE = 16
    };

    enum { npos = string::npos };

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

    bool isSkipped() const { return m_type == TOK_SKIPPED; }
    bool isControl() const { return m_type == TOK_CONTROL; }
    bool isCharacter() const { return m_type == TOK_CHARACTER; }

    bool isCharacter(char c) const {
        return m_type == TOK_CHARACTER && m_value[0] == c;
    }

    bool isCharacter(char c, CatCode cat) const {
        return m_type == TOK_CHARACTER && m_value[0] == c && m_catCode == cat;
    }

    bool isCharacterCat(CatCode cat) {
        return m_type == TOK_CHARACTER && m_catCode == cat;
    }

    bool isLastInLine() const { return m_lastInLine; }

    const string& fileName() const {
        return m_fileName ? *m_fileName : EMPTY_STRING;
    }
    shared_ptr<string> fileNamePtr() const { return m_fileName; }

    string texRepr(Parser* parser = NULL) const;
    string meaning(Parser* parser = NULL) const;

    /**
     * @return represented token string in format
     * Token(Token::typeName, Token::catCodeName, value, source,
     *      m_linePos, m_lineNo, m_charPos, m_charEnd )
     */
    string repr() const;

    Token::ptr lcopy() const {
        return Token::create(
            m_type, m_catCode, m_value, "", 0, 0, 0, 0,
            //m_lineNo, m_charEnd, m_charEnd,
            m_lastInLine, m_fileName);
    }

    static string texReprControl(const string& name,
                                Parser* parser = NULL, bool space = false);
    static string texReprList(const Token::list& tokens,
            Parser* parser = NULL, bool param = false, size_t limit = 0);

protected:
    Type        m_type;     // type of token
    CatCode     m_catCode;  // category code whitch token refer to
    string      m_value;    // described expression meaning (semantic)
    string      m_source;   // described expression

    size_t      m_linePos;  // total number of symbols above current line
    size_t      m_lineNo;   // line number (in source file)
    size_t      m_charPos;  // position of teken`s begin (on current line)
    size_t      m_charEnd;  // position of teken`s end (on current line)

    bool        m_lastInLine;   // ID: this is the last Token in the line

    shared_ptr<string> m_fileName;  // tex file name - source file for this token

    static string EMPTY_STRING;
};

} // namespace texpp

#endif

