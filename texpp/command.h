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

#ifndef __TEXPP_COMMAND_H
#define __TEXPP_COMMAND_H

#include <texpp/common.h>

#include <set>
#include <boost/lexical_cast.hpp>

namespace texpp {

class Token;
class Node;
class Parser;

class Command
{
public:
    typedef shared_ptr<Command> ptr;

    Command(const string& name = string()): m_name(name) {}
    virtual ~Command() {}

    const string& name() const { return m_name; }

    virtual string repr() const;

    /**
     * @brief represent command name m_name in string
     * @param parser - using to obtain certain escape symbol
     * @return - name of command in string format
     */
    virtual string texRepr(Parser* parser = NULL) const;

    virtual bool invoke(Parser&, shared_ptr<Node>) { return true; }
    virtual bool invokeWithPrefixes(Parser&, shared_ptr<Node>,
                                std::set<string>&) { return false; }

    virtual bool presetMode(Parser&) { return false; }

protected:
    string m_name;
};

// \par
class TokenCommand: public Command
{
public:
    typedef shared_ptr<TokenCommand> ptr;

    TokenCommand(shared_ptr<Token> token)
        : Command("token_command"), m_token(token) {}

    const shared_ptr<Token>& token() const { return m_token; }

    string texRepr(Parser* parser = NULL) const;
    bool invoke(Parser& parser, shared_ptr<Node> node);

protected:
    shared_ptr<Token> m_token;
};

// class witch describe behavior of macro define command "\macro"
class Macro: public Command
{
public:
    typedef shared_ptr<Macro> ptr;

    Macro(const string& name = string()): Command(name) {}

    bool invokeWithPrefixes(Parser&, shared_ptr<Node>,
                                std::set<string>&) { return true; }
    virtual bool expand(Parser&, shared_ptr<Node>) { return false; }

    static shared_ptr<vector<shared_ptr<Token> > >
                        stringToTokens(const string& str);
};

class ConditionalBegin: public Macro
{
public:
    typedef shared_ptr<ConditionalBegin> ptr;
    ConditionalBegin(const string& name = string()): Macro(name) {}
    virtual bool evaluate(Parser&, shared_ptr<Node>) { return true; }
};

class ConditionalOr: public Macro
{
public:
    typedef shared_ptr<ConditionalOr> ptr;
    ConditionalOr(const string& name = string()): Macro(name) {}
};

class ConditionalElse: public Macro
{
public:
    typedef shared_ptr<ConditionalElse> ptr;
    ConditionalElse(const string& name = string()): Macro(name) {}
};

class ConditionalEnd: public Macro
{
public:
    typedef shared_ptr<ConditionalEnd> ptr;
    ConditionalEnd(const string& name = string()): Macro(name) {}
};

/**
 * @brief handler of left curve bracket '{' symbol. '{' mean begin new group.
 */
class Begingroup: public Command
{
public:
    Begingroup(const string& name = string()): Command(name) {}
    bool invoke(Parser &, shared_ptr<Node>);
};

/**
 * @brief handler of right curve bracket '}' symbol. '}' mean end current group.
 */
class Endgroup: public Command
{
public:
    Endgroup(const string& name = string()): Command(name) {}
    bool invoke(Parser &, shared_ptr<Node>);
};

/**
 * @brief handler of \begin{...} command
 */
class BeginCommand: public Command
{
public:
    BeginCommand(const string& name = string()): Command(name) {}
    bool invoke(Parser &parser, shared_ptr<Node> node);
};

/**
 * @brief handler of \end{...} command
 */
class EndCommand: public Command
{
public:
    EndCommand(const string& name = string()): Command(name) {}
    bool invoke(Parser &parser, shared_ptr<Node> node);
};

class Usepackage: public Command
{
public:
    Usepackage(const string& name = string()): Command(name) {}
    bool invoke(Parser & parser, shared_ptr<Node> node);
};

class Section: public Command
{
public:
    Section(const string& name = string()): Command(name) {}
    bool invoke(Parser & parser, shared_ptr<Node> node);
};

class Acknowledgments: public Command
{
public:
    Acknowledgments(const string& name = string()): Command(name) {}
    bool invoke(Parser &, shared_ptr<Node> node);
};

class Newcommand: public Command
{
public:
    Newcommand(const string& name = string()): Command(name) {}
    bool invoke(Parser &parser, shared_ptr<Node> node);
};

class Newenvironment: public Command
{
public:
    Newenvironment(const string& name = string()): Command(name) {}
    bool invoke(Parser &parser, shared_ptr<Node> node);
};

class Newtheorem: public Command
{
public:
    Newtheorem(const string& name = string()): Command(name) {}
    bool invoke(Parser &parser, shared_ptr<Node> node);
};

class Newif: public Command
{
public:
    Newif(const string& name = string()): Command(name) {}
    bool invoke(Parser &parser, shared_ptr<Node> node);
};

class Documentclass: public Command
{
public:
    Documentclass(const string& name = string()): Command(name) {}
    bool invoke(Parser &parser, shared_ptr<Node> node);
};

class DefCommand: public Command
{
public:
    DefCommand(const string& name = string()): Command(name) {}
    bool invoke(Parser &parser, shared_ptr<Node> node);
    bool checkPrefixes(Parser&){ return true; }
};

class CaptionCommand: public Command
{
public:
    CaptionCommand(const string& name = string()): Command(name) {}
    bool invoke(Parser &parser, shared_ptr<Node> node);
};

class ImageCommand: public Command
{
public:
    ImageCommand(const string& name = string()): Command(name) {}
    bool invoke(Parser &parser, shared_ptr<Node> node);
};
} // namespace texpp

#endif


