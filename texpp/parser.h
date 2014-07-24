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

#ifndef __TEXPP_PARSER_H
#define __TEXPP_PARSER_H

#include <texpp/common.h>
#include <texpp/lexer.h>
#include <texpp/command.h>
#include <texpp/command.h>

#include <deque>
#include <set>
#include <cassert>
#include <climits>

#include <boost/any.hpp>

namespace texpp {

using boost::any;
using boost::any_cast;
using boost::unsafe_any_cast;

class Lexer;
class Logger;
class Parser;

namespace base {
    class ExpandafterMacro;
} // namespace base

class Node
{
public:
    typedef shared_ptr<Node> ptr;
    typedef vector< pair< string, Node::ptr > > ChildrenList;

    Node(const string& type): m_type(type) {}

    string source(const string& fileName = string()) const;
    unordered_map<shared_ptr<string>, string> sources() const;
    std::set<shared_ptr<string> > files() const;
    shared_ptr<string> oneFile() const;
    bool isOneFile() const;

    // Returns a pair (start_pos, end_pos)
    std::pair<size_t, size_t> sourcePos() const;

    const string& type() const { return m_type; }
    void setType(const string& type) { m_type = type; }

    void setValue(const any& value) { m_value = value; }

    const any& valueAny() const { return m_value; }

    template<typename T>
    T value(T def) const {
        if(m_value.type() != typeid(T)) return def;
        else return *unsafe_any_cast<T>(&m_value);
    }

    // XXX: The following is a horrible hack required to
    // XXX: overcome problems with RTTI across shared objects
    // XXX: It will never work good and should be removed !
    // XXX: The only solution is to NOT use boost::any
    const string& valueString() const;

    const vector< Token::ptr >& tokens() const { return m_tokens; }
    vector< Token::ptr >& tokens() { return m_tokens; }

    const ChildrenList& children() const { return m_children; }
    ChildrenList& children() { return m_children; }

    size_t childrenCount() const { return m_children.size(); }
    Node::ptr child(int num) { return m_children[num].second; }
    Node::ptr child(const string& name);

    /**
     * @brief append node to m_children list with tag "name"
     * @param name - tag of node
     * @param node
     */
    void appendChild(const string& name, Node::ptr node) {
        m_children.push_back(make_pair(name, node));
    }

    Token::ptr lastToken();

    string repr() const;
    string treeRepr(size_t indent = 0) const;

protected:
    string                  m_type;
    any                     m_value;
    vector< Token::ptr >    m_tokens;

    ChildrenList            m_children;
};

class Parser
{
public:
    enum Interaction { ERRORSTOPMODE, SCROLLMODE,
                       NONSTOPMODE, BATCHMODE };
// list of modes for execution processor
    enum Mode { NULLMODE,
                VERTICAL,   // vertical lists are broken into pages
                HORIZONTAL, // horizontal lists are broken into paragraphs
                RVERTICAL,
                RHORIZONTAL,
                MATH,       // formulas are built out of math lists
                DMATH };
    enum GroupType { GROUP_DOCUMENT,
                     GROUP_NORMAL, GROUP_SUPER,
                     GROUP_MATH, GROUP_DMATH,
                     GROUP_CUSTOM };

    Parser(const string& fileName, std::istream* file,
            const string& workdir = string(),
            bool interactive = false, bool ignoreEmergency = false,
            shared_ptr<Logger> logger = shared_ptr<Logger>());

    Parser(const string& fileName, shared_ptr<std::istream> file,
            const string& workdir = string(),
            bool interactive = false, bool ignoreEmergency = false,
            shared_ptr<Logger> logger = shared_ptr<Logger>());

    Interaction interaction() const { return m_interaction; }
    void setInteraction(Interaction intr) { m_interaction = intr; }

    const string& workdir() const { return m_workdir; }
    void setWorkdir(const string& workdir) { m_workdir = workdir; }

    bool ignoreEmergency() const { return m_ignoreEmergency; }
    void setIgnoreEmergency(bool ignoreEmergency) {
        m_ignoreEmergency = ignoreEmergency;
    }
   
    ///////// Parse 
    Node::ptr parse();

    const string& modeName() const;
    Mode mode() const { return m_mode; }
    void setMode(Mode mode) { m_mode = mode; }

    bool hasOutput() const { return m_hasOutput; }

    void traceCommand(Token::ptr token, bool expanding = false);

    //////// Tokens
    Token::ptr lastToken();

    //
    // return actual real (no TOK_SKIPPED) token
    /**
     * @brief read and put all next tokens to m_tokenSourse list untill NoSkipped token
     * @param expand
     * @return next real(no skipped) token
     */
    Token::ptr peekToken(bool expand = true);

    /**
     * @brief insert tokens from m_tokenSource to tokenVector
     * clean m_tokenSource and m_token
     * @param expand
     * @return actual real (no TOK_SKIPPED) token
     */
    Token::ptr nextToken(vector< Token::ptr >* tokenVector = NULL,
                         bool expand = true);

    // insert tokens from m_tokenSource to tokenVector
    //
    /**
     * @brief insert tokens from m_tokenSource to tokenVector
     * clean m_tokenSource and m_token
     */
    void pushBack(vector< Token::ptr >* tokenVector);

    // void setNoexpand(Token::ptr token) { m_noexpandToken = token; }
    void addNoexpand(Token::ptr token) { m_noexpandTokens.insert(token); }

    void resetNoexpand() { m_noexpandTokens.clear(); pushBack(NULL); }

    void input(const string& fileName, const string& fullName);
    void end() { m_end = true; }
    void endinput() { m_endinput = true; }

    Command::ptr currentCommand() const {
        return m_commandStack.empty() ? Command::ptr() : m_commandStack.back();
    }

    Command::ptr prevCommand(size_t n = 1) const {
        return m_commandStack.size() <= n ? Command::ptr() :
            m_commandStack[m_commandStack.size()-1-n];
    }

    void lockToken(Token::ptr token) { m_lockToken = token; }
    void setAfterassignmentToken(Token::ptr t) { m_afterassignmentToken = t; }
    void addAftergroupToken(Token::ptr t) {
        if(m_groupLevel > 0) m_aftergroupTokensStack.back().push_back(t);
    }

    //////// Parse helpers
    bool helperIsImplicitCharacter(Token::CatCode catCode,
                                        bool expand = true);

    Node::ptr parseGroup(GroupType groupType);

    Node::ptr parseCommand(Command::ptr command);

    Node::ptr parseToken(bool expand = true);
    Node::ptr parseDMathToken();
    Node::ptr parseControlSequence(bool expand = true);

    Node::ptr parseOptionalSpaces();

    Node::ptr parseKeyword(const vector<string>& keywords);
    Node::ptr parseOptionalKeyword(const vector<string>& keywords);

    Node::ptr parseOptionalEquals();
    Node::ptr parseOptionalSigns();
    Node::ptr parseNormalInteger();
    Node::ptr parseNumber();
    Node::ptr parseDimenFactor();
    Node::ptr parseNormalDimen(bool fil = false, bool mu = false);
    Node::ptr parseDimen(bool fil = false, bool mu = false);
    Node::ptr parseGlue(bool mu = false);

    Node::ptr parseFiller(bool expand);
    Node::ptr parseBalancedText(bool expand, int paramCount = -1,
                                    Token::ptr nameToken = Token::ptr());
    Node::ptr parseGeneralText(bool expand, bool implicitLbrace = true);

    Node::ptr parseFileName();

    /**
     * @brief read word untill non letter symbol
     * @return node with the word inside
     */
    Node::ptr parseTextWord();
    Node::ptr parseTextCharacter();

    /**
     * @brief set the HORIZONTAL mode. Insert "ch" to the to m_symbols table
     * @param ch - symbol
     * @param token
     */
    void processTextCharacter(char ch, Token::ptr token);
    void resetParagraphIndent();

    //////// Symbols
    /// insert symbol to m_symbols SymbolTable
    void setSymbol(const string& name, const any& value, bool global = false);
    void setSymbol(Token::ptr token, const any& value, bool global = false) {
        if(token && token->isControl())
            setSymbol(token->value(), value, global);
    }

    void setSymbolDefault(const string& name, const any& defaultValue);
    
    const any& symbolAny(const string& name) const;
    const any& symbolAny(Token::ptr token) const {
        if(!token || !token->isControl()) return EMPTY_ANY;
        else return symbolAny(token->value());
    }
    
    template<typename T>
    T symbol(const string& name, T def) const {
        const any& v = symbolAny(name);
        if(v.type() != typeid(T)) return def;
        else return *unsafe_any_cast<T>(&v);
    }

    template<typename T>
    T symbol(Token::ptr token, T def) const {
        const any& v = symbolAny(token);
        if(v.type() != typeid(T)) return def;
        else return *unsafe_any_cast<T>(&v);
    }

    template<typename T>
    shared_ptr<T> symbolCommand(Token::ptr token) const {
        return dynamic_pointer_cast<T>(
                symbol(token, Command::ptr()));
    }

    void beginGroup();
    void endGroup();
    int groupLevel() const { return m_groupLevel; }

    void beginCustomGroup(const string& type) {
        m_customGroupBegin = true; m_customGroupType = type; beginGroup(); }
    void endCustomGroup() { endGroup(); m_customGroupEnd = true; }

    string escapestr() const {
        int e = symbol("escapechar", int(0));
        return e >= 0 && e <= 255 ? string(1, e) : string();
    }

    //////// Others
    shared_ptr<Logger> logger() { return m_logger; }
    shared_ptr<Lexer> lexer() { return m_lexer; }

    static const string& banner() { return BANNER; }

protected:
    void endinputNow();
    Node::ptr rawExpandToken(Token::ptr token);

    // read and return next token be it skipped or no
    Token::ptr rawNextToken(bool expand = true);
    Node::ptr parseFalseConditional(size_t level,
                          bool sElse = false, bool sOr = false);
    void setSpecialSymbol(const string& name, const any& value);
    void init();

    typedef std::deque<
        Token::ptr
    > TokenQueue;

    typedef std::set<
        Token::ptr
    > TokenSet;

    typedef std::vector<
        Command::ptr
    > CommandStack;

    typedef std::vector<
        pair<shared_ptr<Lexer>, TokenQueue>
    > InputStack;

    string          m_workdir;
    bool            m_ignoreEmergency;

    shared_ptr<Lexer>   m_lexer;
    shared_ptr<Logger>  m_logger;

    Token::ptr      m_token;
    Token::list     m_tokenSource;

    Token::ptr      m_lastToken;
    TokenSet        m_noexpandTokens;
    TokenQueue      m_tokenQueue;

    int             m_groupLevel;
    bool            m_end;
    bool            m_endinput;
    bool            m_endinputNow;

    struct ConditionalInfo {
        bool parsed;
        bool ifcase;
        bool active;
        int  value;
        int  branch;
    };
    vector< ConditionalInfo >
                    m_conditionals;

    typedef unordered_map<
        string, pair< int, any >
    > SymbolTable;

    typedef vector<
        pair<string, pair<int, any> >
    > SymbolStack;

    SymbolTable     m_symbols;
    SymbolStack     m_symbolsStack;
    vector<size_t>  m_symbolsStackLevels;

    size_t          m_lineNo;   // current linie in file
    Mode            m_mode;
    Mode            m_prevMode;

    bool            m_hasOutput;

    GroupType       m_currentGroupType;

    string  m_customGroupType;
    bool    m_customGroupBegin;
    bool    m_customGroupEnd;

    CommandStack m_commandStack;

    Interaction m_interaction;
    
    Token::ptr          m_lockToken;
    Token::ptr          m_afterassignmentToken;
    vector<Token::list> m_aftergroupTokensStack;

    InputStack m_inputStack;

    static any EMPTY_ANY;
    static string BANNER;

    friend class base::ExpandafterMacro;
};

} // namespace texpp

#endif

