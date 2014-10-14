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

    // return next Token
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

    int getCatCode(int ch) const { return m_catCodeTable[ch]; }
    void assignCatCode(int ch, int code) { m_catCodeTable[ch] = code; }

protected:
    /**
     * @brief initialisation of m_catCodeTable lookup table
     */
    void init();

    Token::ptr newToken(Token::Type type,
                    const string& value = string());
    /**
     * @brief read new line from source
     * reset position counters m_charPos, m_charEnd
     * increase counter m_linePos by length of current line
     * fill m_lineOrig string by string from line or console
     * fill m_lineTex string by string from line or console with modified endlinechar
     * increment line number counter m_lineNo
     * @return false if end of file; true otherwise
     */
    bool nextLine();

    /** read next symbol from m_lineTex following to the m_charEnd.
     *  Determines category code for this symbol
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

    std::istream*   m_file;         // .tex file source
    shared_ptr<string> m_fileName;  // source file name

    string  m_lineOrig; // current line as in source
    string  m_lineTex;  // current line with modified the last char ('/n' -> '/r')

    size_t  m_linePos;  // the total number of characters above current line
    size_t  m_lineNo;   // current line number
    size_t  m_charPos;  // actual position of next char in line
    size_t  m_charEnd;  // position of the last char in line

    State   m_state;    // processing state
    int     m_char;     // buffer for symbol, next to parsing
    Token::CatCode m_catCode;   // category code for m_char

    int     m_endlinechar;  // symbol witch replace last symol in line in plane text
    short int m_catCodeTable[256];  // lookup table for finding char->CatCode

    bool    m_interactive;  // true in interactive mode( run program without argument
                            //  (path to .tex source file))
    bool    m_saveLines;    // trigger save/don`t save processed lines into m_lines

    vector<string> m_lines; // massive of already cinsidered text lines
};

} // namespace

#endif

