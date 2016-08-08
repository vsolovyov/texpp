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

#ifndef __TEXPP_LEXER_H
#define __TEXPP_LEXER_H

#include <texpp/common.h>
#include <texpp/token.h>

#include <istream>

namespace texpp {

class Lexer;
class Context;

class Lexer
{
public:
    Lexer(const string& fileName, std::istream* file,
                bool interactive = false, bool saveLines = false);
    Lexer(const string& fileName, shared_ptr<std::istream> file,
                bool interactive = false, bool saveLines = false);
    ~Lexer();

    /**
     * @brief nextToken is handler which allow as to get next token from
     *      the source file(m_file) corresponding to the current state of Lexer.
     * @return pointer of the next token from the source file(m_file)
     *
     * behaviour of nextToken() method depends of current state(m_state value).
     * 1st stage:
     *  Check m_state value:
     *  - if ST_EOF(end of file) than return empty token pointer.
     *  - if not EOF than begin "extract token from source" loop. Go to stage 2.
     * 2nd stage:
     *  Check does EOL achieved?
     *  - If yes - to the 3rd stage;
     *  - If no - read next character and gogo to the 4th stage
     * 3rd stage:
     *  Read next line from source file (m_file);
     *  - If no more lines go to ST_EOF state and return empty token pointer;
     *  - If the next line reading passed succesfuly go to ST_NEW_LINE. Go to
     *      4th stage
     *
     * (below we find out meaning of the character)
     * 4th stage:
     *  Check m_state:
     *  - if m_state is ST_NEW_LINE or ST_SKIP_SPACES than go to the 5th stage;
     *  - if m_state is ST_MIDDLE than go to the 6th stage
     *
     * 5th stage:
     *  handle ST_NEW_LINE and ST_SKIP_SPACES states.
     *  - if m_char is endlinechar than return control command "\\par"
     *  - if m_char is CC_IGNORED of CC_IGNORED than return SKIPPED token;
     *  - if m_char is CC_SPACE than create TOK_SKIPPED token whitch include all
     *      subsequent spaces from source file and return this token. So spaces
     *      at the begin of line treated as skipped.
     *  - for all other values m_char we set current character as not read yet,
     *      set m_state to ST_MIDDLE and go to the stage 4.
     *
     * 6th stage: (middle of line)
     *  M state is most common state. In this stage process forking by m_catCode.
     *      if m_catCode is ...
     *  - CC_ESCAPE: it is mean that we have control sequence. Then we build
     *      control token with backslash and next character which belong to this
     *      token. If the next character after backslach is letter than include
     *      all subsequent letters into control token(till no letter character).
     *      Then init token by this control world. Set m_state to ST_SKIP_SPACES
     *      if next to the backslash is leter of space. Return constucted
     *      CONTROL token.
     *  - CC_ACTIVE: return CONTROL token with ("`" + current character) m_value
     *  - CC_SPACE: one or next sequence spaces presents as one visible space
     *      token.
     *  - CC_EOL: EOL presenting as visible space token. Set state to EOL.
     *      return token.
     *  - CC_COMMENT: Set state to EOL. Set state to EOL. move to 4th stage.
     *  - CC_IGNORED or CC_INVALID: return SKIPPED token.
     *  - CC_BGROUP CC_EGROUP CC_MATHSHIFT CC_ALIGNTAB CC_PARAM CC_SUPER CC_SUB
     *      CC_LETTER CC_OTHER: actual letter treated as letter. return
     *      CHARACTER token for this m_char value.
     */
    Token::ptr nextToken();

    bool interactive() const { return m_interactive; }

    string jobName() const;

    // fileName valude getter
    const string& fileName() const { return *m_fileName; }

    // fileName pointer getter
    shared_ptr<string> fileNamePtr() const { return m_fileName; }

    size_t linePos() const { return m_linePos; }
    size_t lineNo() const { return m_lineNo; }
    const string& line() const { return m_lineOrig; }
    const string& line(size_t n) const;

    int endlinechar() const { return m_endlinechar; }
    void setEndlinechar(int endlinechar) { m_endlinechar = endlinechar; }

    int getCatCode(int ch) const { return m_catCodeTable[(unsigned char) ch]; }
    void assignCatCode(int ch, int code) { m_catCodeTable[ch] = code; }

protected:
    /**
     * @brief initialisation of m_catCodeTable lookup table
     */
    void init();

    Token::ptr newToken(Token::Type type,
                    const string& value = string());

    /**
     * @brief read new line from source file <m_file>;
     *      reset position counters m_charPos, m_charEnd;
     *      increase counter m_linePos by length of current line;
     *      fill m_lineOrig string by string from line or console;
     *      fill m_lineTex string by string from line or console with modified
     *          endlinechar;
     *      increment line number counter m_lineNo
     * @return false if end of file; true otherwise
     */
    bool nextLine();

    /**
     * @brief read next symbol from m_lineTex following to the m_charEnd.
     *      Determines category code for this symbol
     * @return false if end of line; true - otherwise
     */
    bool nextChar();

protected:
    enum State {
        ST_EOF = 0,         // end of file
        ST_EOL = 1,         // end of line
        ST_NEW_LINE = 2,    // new line
        ST_SKIP_SPACES = 3, // skip spaces
        ST_MIDDLE = 4       // middle of line
    };

    shared_ptr<std::istream> m_fileShared;

    std::istream*   m_file;         // TeX source file
    shared_ptr<string> m_fileName;  // source file name

    string  m_lineOrig; // current line as in source
    string  m_lineTex;  // current line with modified the last char. Here '/n'
                        // '/r' globbing by m_endlinechar,

    size_t  m_linePos;  // the total number of characters above current line
    size_t  m_lineNo;   // current line number
    size_t  m_charPos;  // actual position of next char in line
    size_t  m_charEnd;  // position of the last char in line

    State   m_state;    // processing state
    int     m_char;     // buffer for symbol, next to parsing
    Token::CatCode m_catCode;   // category code for m_char

    int     m_endlinechar;  // symbol witch replace last symol in line in plane
                            // text
    int     m_catCodeTable[256];    // lookup table for finding char->CatCode

    bool    m_interactive;  // true in interactive mode( run program without
                            // arguments)
    bool    m_saveLines;    // trigger save/don`t processed lines into m_lines

    vector<string> m_lines; // massive of already cinsidered text lines
};

} // namespace

#endif

