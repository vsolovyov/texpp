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

#include <texpp/command.h>
#include <texpp/parser.h>
#include <texpp/logger.h>

#include <boost/foreach.hpp>

namespace texpp {

string Command::texRepr(Parser* parser) const
{
    return Token::texReprControl(m_name, parser);
}

string Command::repr() const
{
    return "Command(" + reprString(name())
            + ", " + reprString(texRepr()) + ")";
}

string TokenCommand::texRepr(Parser* parser) const
{
    return m_token->meaning(parser);
}

bool TokenCommand::invoke(Parser&, shared_ptr<Node> node)
{
    node->setValue(m_token);
    return true;
}

Token::list_ptr Macro::stringToTokens(const string& str)
{
    Token::list_ptr returnValue(new Token::list());

    BOOST_FOREACH(char ch, str) {
        Token::CatCode catcode = (ch == ' ') ? Token::CC_SPACE :
                                               Token::CC_OTHER;
        returnValue->push_back(Token::create(Token::TOK_CHARACTER,
                                     catcode,
                                     string(1, ch)));
    }

    return returnValue;
}

bool Begingroup::invoke(Parser & parser, shared_ptr<Node>)
{
    parser.beginGroup();
    return true;
}

bool Endgroup::invoke(Parser & parser, shared_ptr<Node>)
{
    parser.endGroup();
    return true;
}

bool BeginCommand::invoke(Parser& parser, shared_ptr<Node> node)
{
    Node::ptr groupNode = parser.parseGroup(Parser::GROUP_NORMAL);
    node->appendChild("env_type",groupNode);
    string env_type = "";
    pair<string,Node::ptr> child;
    BOOST_FOREACH(child, groupNode->children())
    {
        if(child.first == "text_word"){
            env_type = child.second->valueString();
            break;
        }
    }
    parser.beginCustomGroup("environment_" + env_type);
    return true;
}

bool EndCommand::invoke(Parser &parser, shared_ptr<Node> node)
{
    Node::ptr groupNode = parser.parseGroup(Parser::GROUP_NORMAL);
    node->appendChild("env_type",groupNode);
    string env_type = "";
    pair<string,Node::ptr> child;
    BOOST_FOREACH(child, groupNode->children())
    {
        if(child.first == "text_word"){
            env_type = child.second->valueString();
            break;
        }
    }
    if(env_type == "end"){
        parser.end();
    }
    parser.endCustomGroup();
    return true;
}

bool Usepackage::invoke(Parser & parser, shared_ptr<Node> node)
{
    Node::ptr argNode = parser.parseOptionalArgs();
    Node::ptr pkgNode = parser.parseGeneralArg();
    node->appendChild("args", argNode);
    node->appendChild("package", pkgNode);
    if(pkgNode->valueString() == "inputenc"){
        node->setType("inputenc");
        node->setValue(argNode->valueString());
    }
    return true;
}

bool Section::invoke(Parser & parser, shared_ptr<Node> node)
{
    if(parser.peekToken() && parser.peekToken()->isCharacter('*')) {
        parser.nextToken(&node->tokens());
    }
    node->setType("section");
    Node::ptr title = parser.parseGeneralArg();
    node->setValue(title->valueString());
    return true;
}

bool Acknowledgments::invoke(Parser &, shared_ptr<Node> node)
{
    node->setType("section");
    node->setValue("acknowledgments");
    return true;
}

bool Newcommand::invoke(Parser & parser, shared_ptr<Node> node)
{
    node->appendChild("cmd",  parser.parseGeneralArg());
    node->appendChild("args", parser.parseOptionalArgs());
    node->appendChild("opt",  parser.parseOptionalArgs());
    node->appendChild("def",  parser.parseGeneralArg());
    return true;
}

bool Newenvironment::invoke(Parser & parser, shared_ptr<Node> node)
{
    node->appendChild("nam", parser.parseGeneralArg());
    node->appendChild("args", parser.parseOptionalArgs());
    node->appendChild("begdef", parser.parseGeneralArg());
    node->appendChild("enddef", parser.parseGeneralArg());
    return true;
}

bool Newtheorem::invoke(Parser & parser, shared_ptr<Node> node)
{
    if(parser.peekToken()->isCharacter('*')){
        node->appendChild("star", parser.parseToken());
    }
    node->appendChild("env_nam", parser.parseGeneralArg());
    node->appendChild("optional_spaces", parser.parseOptionalSpaces());
    if(parser.peekToken() && parser.peekToken()->isCharacter('['))
    {
        node->appendChild("numbered_like", parser.parseOptionalArgs());
        node->appendChild("caption", parser.parseGeneralArg());
    } else{
        node->appendChild("caption", parser.parseGeneralArg());
        node->appendChild("within", parser.parseOptionalArgs());
    }
    return true;
}

bool Newif::invoke(Parser &parser, shared_ptr<Node> node)
{
    // @ symbol is often used in newif names
    parser.lexer()->assignCatCode(0x40, Token::CC_LETTER);
    parser.setSymbol("catcode64", int(Token::CC_LETTER));

    Node::ptr newifName(parser.parseNewIf());
    node->setType("newif");
    node->setValue(newifName->valueString());

    parser.lexer()->assignCatCode(0x40, Token::CC_OTHER);
    parser.setSymbol("catcode64", int(Token::CC_OTHER));
    return true;
}

bool Documentclass::invoke(Parser & parser, shared_ptr<Node> node)
{
    node->appendChild("options", parser.parseOptionalArgs());
    node->appendChild("class", parser.parseGeneralArg());
    return true;
}

bool DefCommand::invoke(Parser & parser, shared_ptr<Node> node)
{
    node->appendChild("token", parser.parseControlSequence());
    Node::ptr args(new Node("def_args"));
    node->appendChild("args", args);
    while(parser.peekToken() &&
          !parser.peekToken()->isCharacterCat(Token::CC_BGROUP))
    {
        parser.nextToken(&args->tokens());
    }
    node->appendChild("def", parser.parseGeneralText(false));
    return true;
}

bool CaptionCommand::invoke(Parser & parser, shared_ptr<Node> node)
{
    node->setValue("caption");
    Node::ptr text = parser.parseGeneralArg();
    node->appendChild("text", text);
    return true;
}

bool ImageCommand::invoke(Parser & parser, shared_ptr<Node> node)
{
    if(parser.peekToken() && parser.peekToken()->isCharacter('*')){
        parser.nextToken(&node->tokens());
    }
    node->setValue("image");
    parser.parseOptionalArgs();
    Node::ptr filename = parser.parseGeneralArg();
    node->appendChild("filename", filename);
    return true;
}

} // namespace texpp

