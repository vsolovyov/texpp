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

#ifndef __TEXPP_LOGGER_H
#define __TEXPP_LOGGER_H

#include <texpp/common.h>

namespace texpp {

class Token;
class Parser;

class Logger
{
public:
    typedef shared_ptr<Logger> ptr;

    enum Level {
        MTRACING = 5,   // put the text where cursor is
        TRACING = 10,   // put the text where cursor is but bracketed  {...}
        PLAIN = 15,     // put the text where cursor is
        MESSAGE = 20,   // add message after space
        WRITE = 25,     // write message to console in new line and begin new line
        SHOW = 30,      // begin from new line in format "> message.\n";
        ERROR = 40,
        CRITICAL = 50,
        UNIMPLEMENTED = 100
    };

    Logger() {}
    virtual ~Logger() {}

    //const string& levelName(Level level) const;

    /** return text situated after token in line
     *         if source file for parcer and token is the same
     *  return empty string if token is invalid
     */
    string tokenLines(Parser& parser, shared_ptr<Token> token) const;

    /**
     * log() interface;
     * log() shows <message> and <token>
     * text format depend on <level> variable
     */
    virtual bool log(Level level, const string& message,
                    Parser& parser, shared_ptr<Token> token) = 0;
};

class NullLogger: public Logger
{
public:
    bool log(Level, const string&, Parser&, shared_ptr<Token>) { return true; }
};

class ConsoleLogger: public Logger
{
public:
    ConsoleLogger(): m_linePos(0) {}
    ~ConsoleLogger();
    bool log(Level level, const string& message,
                Parser& parser, shared_ptr<Token> token);
protected:
    unsigned int m_linePos;
};

} // namespace texpp

#endif

