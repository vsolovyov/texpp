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

#define BOOST_TEST_MODULE parser_test_suite
#include <boost/test/included/unit_test.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include <texpp/parser.h>
#include <texpp/logger.h>
#include <texpp/command.h>
#include <iostream>
#include <sstream>

using namespace texpp;

class TestLogger: public Logger
{
public:
    bool log(Level, const string& message,
                Parser&, shared_ptr<Token> token) {
        logMessages.push_back(message);
        logPositions.push_back(
            token ? std::make_pair(token->lineNo(), token->charPos())
                  : std::make_pair(size_t(0), size_t(0)));
        return true;
    }
    vector<string> logMessages;
    vector<std::pair<size_t, size_t> > logPositions;
};

shared_ptr<Parser> create_parser(const string& input)
{
    shared_ptr<std::istream> ifile(new std::istringstream(input));
    return shared_ptr<Parser>(
        new Parser("", ifile, "", false, false,
                    shared_ptr<Logger>(new TestLogger)));
}

// "parser token test" handles letters and commands in the input.
// Here we test behavior of the parser based on peekToken(), lastToken() and
// nextToken() methods. See description of methods in parser.h file.
BOOST_AUTO_TEST_CASE( parser_tokens )
{
    shared_ptr<Parser> parser = create_parser("  a %  \n  \\bb\\ \\c");
    BOOST_CHECK_EQUAL(parser->lastToken(), Token::ptr());

    BOOST_CHECK_EQUAL(parser->peekToken()->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 0, 1, 2, 3).repr());
    BOOST_CHECK_EQUAL(parser->peekToken()->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 0, 1, 2, 3).repr());

    BOOST_CHECK_EQUAL(parser->lastToken()->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 0, 1, 2, 3).repr());
    /*BOOST_CHECK_EQUAL(parser->peekToken(1)->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 0, 1, 2, 3).repr());
    BOOST_CHECK_EQUAL(parser->peekToken(2)->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_SPACE, " ", " ", 0, 1, 3, 4).repr());
    BOOST_CHECK_EQUAL(parser->peekToken(3)->repr(),
        Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\bb", "\\bb", 8, 2, 2, 5).repr());
    BOOST_CHECK_EQUAL(parser->peekToken()->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 0, 1, 2, 3).repr());*/

    vector< Token::ptr > tokens_t;
    Token::ptr token0 = parser->nextToken(&tokens_t);
    BOOST_CHECK_EQUAL(token0->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 0, 1, 2, 3).repr());

    BOOST_CHECK_EQUAL(parser->peekToken()->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_SPACE, " ", " ", 0, 1, 3, 4).repr());

    // copy content of m_tokenSource and tokens_t to m_tokenQueue
    // clean m_tokenSource
    // last Token stay the same
    // the result should be as if we didn't read tokens in m_tokenQueue yet
    parser->pushBack(&tokens_t);
    BOOST_CHECK_EQUAL(parser->lastToken()->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_SPACE, " ", " ", 0, 1, 3, 4).repr());
    BOOST_CHECK_EQUAL(parser->peekToken()->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 0, 1, 2, 3).repr());
    BOOST_CHECK_EQUAL(parser->lastToken()->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_SPACE, " ", " ", 0, 1, 3, 4).repr());

    tokens_t.clear();
    // m_tokenSource is not empty -> return parser.m_token and free m_tokenSource
    token0 = parser->nextToken(&tokens_t);
    BOOST_CHECK_EQUAL(token0->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 0, 1, 2, 3).repr());

    // now m_tokenSource empty -> read nex token, free m_tokenSource
    token0 = parser->nextToken(&tokens_t);
    BOOST_CHECK_EQUAL(token0->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_SPACE, " ", " ", 0, 1, 3, 4).repr());

    BOOST_CHECK_EQUAL(parser->peekToken(false)->repr(),
        Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\bb", "\\bb", 8, 2, 2, 5).repr());

    parser->pushBack(&tokens_t);

    BOOST_CHECK_EQUAL(parser->lastToken()->repr(),
        Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\bb", "\\bb", 8, 2, 2, 5).repr());
    BOOST_CHECK_EQUAL(parser->peekToken()->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 0, 1, 2, 3).repr());
    BOOST_CHECK_EQUAL(parser->lastToken()->repr(),
        Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\bb", "\\bb", 8, 2, 2, 5).repr());

    // not skipped tokens only!!!!
    Token tokens[] = {
        Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 0, 1, 2, 3),
        Token(Token::TOK_CHARACTER, Token::CC_SPACE, " ", " ", 0, 1, 3, 4),
        Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\bb", "\\bb", 8, 2, 2, 5),
        Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\ ", "\\ ", 8, 2, 5, 7),
        Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\c", "\\c", 8, 2, 7, 9),
    };

    // sequence of tokens which correspond to "get next real token" rules.
    // peekToken() command proces input text till NOT_SKIPPED token;
    // skipped tokens including the last real token go to the m_tokenSource
    // buffer. So this multidimention array is sequence of m_tokenSource
    // collections in fact
    Token tokens_all[][5] = {
        {
            Token(Token::TOK_SKIPPED, Token::CC_SPACE, " ", "  ", 0, 1, 0, 2),
            Token(Token::TOK_CHARACTER, Token::CC_LETTER, "a", "a", 0, 1, 2, 3)
        },{
            Token(Token::TOK_CHARACTER, Token::CC_SPACE, " ", " ", 0, 1, 3, 4)
        },{
            Token(Token::TOK_SKIPPED, Token::CC_COMMENT, "%", "%  \n", 0, 1, 4, 8),
            Token(Token::TOK_SKIPPED, Token::CC_SPACE, " ", "  ", 8, 2, 0, 2),
            Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\bb", "\\bb", 8, 2, 2, 5)
        },{
            Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\ ", "\\ ", 8, 2, 5, 7)
        },{
            Token(Token::TOK_CONTROL, Token::CC_ESCAPE, "\\c", "\\c", 8, 2, 7, 9)
        },
    };
    int tokens_all_counts[] = {2,1,3,1,1};

    vector<string> output_repr;
    Token::ptr token, token1, token2;

    size_t n = 0;
    while(token = parser->peekToken(false)) {
        vector<Token::ptr> output_all;
        // first time n=0, parser lat token is "\\bb"
        BOOST_CHECK_EQUAL(parser->lastToken()->repr(),
            n < 3 ? Token(Token::TOK_CONTROL, Token::CC_ESCAPE,
                        "\\bb", "\\bb", 8, 2, 2, 5).repr()
                    : token->repr());

        //token1 = parser->peekToken(2);
        // move tokens from parser.m_tokenSource to output_all (buffer cleaning)
        token2 = parser->nextToken(&output_all, false);

        BOOST_CHECK_EQUAL(token->repr(), token2->repr());
        BOOST_CHECK_EQUAL(parser->lastToken()->repr(),
            n < 3 ? Token(Token::TOK_CONTROL, Token::CC_ESCAPE,
                        "\\bb", "\\bb", 8, 2, 2, 5).repr()
                    : token->repr());

        /*if(token1)
            BOOST_CHECK_EQUAL(token1->repr(), parser->peekToken()->repr());
        else
            BOOST_CHECK_EQUAL(Token::ptr(), parser->peekToken());*/

        // n - "counter" for the tokens array {0,1,...tokens.length()}
        if(n < sizeof(tokens)/sizeof(Token)) {
            vector<string> output_all_repr(output_all.size());
            vector<string> tokens_all_repr(tokens_all_counts[n]);

            // make text representing for every token in output_all, put the
            // result into output_all_repr
            std::transform(output_all.begin(), output_all.end(),
                    output_all_repr.begin(),
                    boost::lambda::bind(&Token::repr, *boost::lambda::_1));

            // make the same for tokens_all
            std::transform(tokens_all[n], tokens_all[n] + tokens_all_counts[n],
                    tokens_all_repr.begin(),
                    boost::lambda::bind(&Token::repr, boost::lambda::_1));

            // check is they the same as was predicted
            BOOST_CHECK_EQUAL_COLLECTIONS(
                    tokens_all_repr.begin(), tokens_all_repr.end(),
                    output_all_repr.begin(), output_all_repr.end());
        }
        
        // represent all real tokens in output_repr vector
        output_repr.push_back(token->repr());
        ++n;
    }
    
    // create vector with the same dimention as the tokens array
    vector<string> tokens_repr(sizeof(tokens)/sizeof(Token));
    // represent all prediction for real tokens in tokens_repr vector
    std::transform(tokens, tokens + tokens_repr.size(), tokens_repr.begin(),
                    boost::lambda::bind(&Token::repr, boost::lambda::_1));
    // compare TeXpp real token collection vs predicted token collection
    BOOST_CHECK_EQUAL_COLLECTIONS(tokens_repr.begin(), tokens_repr.end(),
                                  output_repr.begin(), output_repr.end());
}

// scope test
// Local settings should be true only within actual scope.
// Global settings valid everywhere no matter where they was published
BOOST_AUTO_TEST_CASE( parser_symbols )
{
    shared_ptr<Parser> parser = create_parser("");
    
    parser->setSymbol("a", 1);
    parser->setSymbol("b", string("2"));
    parser->setSymbol("c", double(2.2));

    BOOST_CHECK_EQUAL(1, parser->symbol("a", 0));
    BOOST_CHECK_EQUAL(string("2"), parser->symbol("b", string()));
    BOOST_CHECK_EQUAL(double(2.2), parser->symbol("c", double(0.0)));

    BOOST_CHECK_EQUAL(string("x"), parser->symbol("c", string("x")));
    BOOST_CHECK_EQUAL(2, parser->symbol("b", 2));

    BOOST_CHECK_EQUAL(1, parser->symbol("a", 0));

    parser->beginGroup();

    BOOST_CHECK_EQUAL(1, parser->symbol("a", 0));
    
    parser->setSymbol("a", 2);
    BOOST_CHECK_EQUAL(2, parser->symbol("a", 0));

    parser->setSymbol("a", 3, true);    // global assignment
    BOOST_CHECK_EQUAL(3, parser->symbol("a", 0));

    parser->setSymbol("d", 5);
    BOOST_CHECK_EQUAL(5, parser->symbol("d", 0));

    parser->setSymbol("e", 7, true);    // global assignment
    BOOST_CHECK_EQUAL(7, parser->symbol("e", 0));

    parser->setSymbol("e", 8);
    BOOST_CHECK_EQUAL(8, parser->symbol("e", 0));

    parser->endGroup();

    BOOST_CHECK_EQUAL(3, parser->symbol("a", 0));

    BOOST_CHECK_EQUAL(0, parser->symbol("d", 0));

    BOOST_CHECK_EQUAL(7, parser->symbol("e", 0));
}


#include <texpp/base/bibliography.h>
#include <texpp/token.h>
// here we can see an example of simple node tree representing
BOOST_AUTO_TEST_CASE( parser_parse )
{
//    std::string str = "abc{def}gh ";
//    std::string str = "\\vrule width 4pt height 6pt depth 6pt ";
//    std::string str = "\\input /home/bereziuk/lausanne/texpp/build/anotherFile.tex abc{def}gh ";
    //    std::string str = "\\bibitem{knuth-84 } ";
    std::string str = "\\begin{thebibliography}{99}\n"
                      "\\bibitem{knuth-84} \n"
                      "Donald E. Knuth: {\\it The \\TeX Book}. Addison-Wesley PC. 1984.\n"
                      "\\bibitem{knuth-meta}\n"
                      "Donald E.Knuth: {\\it The METAFONTbook}. Addison-Wesley PC. 1984.\n"
                      "\\end{thebibliography}";
    // std::cout<< str << std::endl;
    shared_ptr<Parser> parser = create_parser(str);
//    parser->lexer()->assignCatCode('{', Token::CC_BGROUP);
//    parser->lexer()->assignCatCode('}', Token::CC_EGROUP);
    
//    std::cout << texpp::Token::texReprControl("\\", parser.get(), true) << std::endl;

    Node::ptr document = parser->parse();
    std::cout << document->treeRepr();
//    texpp::base::Bibliography::showBibliography();
}

// this macro command is examle of macro definition whitch we test in
// downstream BOOST_TEST. The expansion algorithm is main target to test here.
// in short: \macro command with two next numbers exchange to "89".
// So "\\macro1234" converting into "8934"
class TestMacro: public Macro
{
public:
    explicit TestMacro(const string& name): Macro(name) {}

    // here expand() takes two forthcoming letters and store them in
    // token1, token1a, token2, token2a tokens. With next distribution:
    // 1st argument: -> token1, token1a;
    // 2nd argument: -> token2, token2a;
    // Concerning "89" - they move to the m_tokenQueue as real tokens.
    bool expand(Parser& parser, shared_ptr<Node> node)
    {
        Node::ptr child(new Node("args"));
        node->appendChild("args", child);

        token1 = parser.peekToken();
        token1a = parser.nextToken(&child->tokens());
        token2 = parser.peekToken();
        token2a = parser.nextToken(&child->tokens());

        parser.peekToken();
        parser.peekToken();

        node->setValue(stringToTokens("89"));
        return true;
    }

    Token::ptr token1, token1a;
    Token::ptr token2, token2a;
};

BOOST_AUTO_TEST_CASE( parser_expansion )
{
    // create parser for "\\macro1234" input TeX text
    shared_ptr<Parser> parser = create_parser("\\macro1234");
    // bind macro command functionality to "\\macro" string
    shared_ptr<TestMacro> macro(new TestMacro("\\macro"));
    // register the macro command for the parser
    parser->setSymbol("\\macro", Command::ptr(macro));

    // nothing yet processed
    BOOST_CHECK_EQUAL(parser->lastToken(), Token::ptr());

    Token::list tokens;
    Token token  = Token(Token::TOK_CHARACTER, Token::CC_OTHER, "8", "", 0, 0, 0, 0);
    Token token3 = Token(Token::TOK_CHARACTER, Token::CC_OTHER, "3", "3", 0, 1, 8, 9);

    BOOST_CHECK_EQUAL(parser->peekToken()->repr(), token.repr());
    BOOST_CHECK_EQUAL(parser->lastToken()->repr(), token3.repr());
    BOOST_CHECK_EQUAL(parser->nextToken(&tokens)->repr(), token.repr());
    BOOST_CHECK_EQUAL(parser->lastToken()->repr(), token3.repr());

    BOOST_CHECK_EQUAL(tokens.size(), 2);    // "\macro12", "8"
    if(tokens.size() == 2) {
        BOOST_CHECK_EQUAL(tokens[0]->repr(),
            Token(Token::TOK_SKIPPED, Token::CC_ESCAPE,
                "\\macro", "\\macro12", 0, 0, 0).repr());
        BOOST_CHECK_EQUAL(tokens[1]->repr(), token.repr());
    }

    BOOST_CHECK(macro->token1); BOOST_CHECK(macro->token1a);
    BOOST_CHECK(macro->token2); BOOST_CHECK(macro->token2a);

    BOOST_CHECK_EQUAL(macro->token1, macro->token1a);
    BOOST_CHECK_EQUAL(macro->token2, macro->token2a);

    BOOST_CHECK_EQUAL(macro->token1->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_OTHER, "1", "1", 0, 1, 6, 7).repr());
    BOOST_CHECK_EQUAL(macro->token2->repr(),
        Token(Token::TOK_CHARACTER, Token::CC_OTHER, "2", "2", 0, 1, 7, 8).repr());

    tokens.clear();
    token = Token(Token::TOK_CHARACTER, Token::CC_OTHER, "9", "", 0, 0, 0, 0);
    BOOST_CHECK_EQUAL(parser->peekToken()->repr(), token.repr());
    BOOST_CHECK_EQUAL(parser->lastToken()->repr(), token3.repr());  // "3"
    BOOST_CHECK_EQUAL(parser->nextToken(&tokens)->repr(), token.repr());
    BOOST_CHECK_EQUAL(parser->lastToken()->repr(), token3.repr());  // "3"

    BOOST_CHECK_EQUAL(tokens.size(), 1);
    if(tokens.size() == 1)
        BOOST_CHECK_EQUAL(tokens[0]->repr(), token.repr());

    tokens.clear();
    token = Token(Token::TOK_CHARACTER, Token::CC_OTHER, "3", "3", 0, 1, 8, 9);
    BOOST_CHECK_EQUAL(parser->peekToken()->repr(), token.repr());
    BOOST_CHECK_EQUAL(parser->lastToken()->repr(), token.repr());
    BOOST_CHECK_EQUAL(parser->nextToken(&tokens)->repr(), token.repr());
    BOOST_CHECK_EQUAL(parser->lastToken()->repr(), token.repr());

    BOOST_CHECK_EQUAL(tokens.size(), 1);
    if(tokens.size() == 1)
        BOOST_CHECK_EQUAL(tokens[0]->repr(), token.repr());

    tokens.clear();
    token = Token(Token::TOK_CHARACTER, Token::CC_OTHER, "4", "4", 0, 1, 9, 10);
    BOOST_CHECK_EQUAL(parser->peekToken()->repr(), token.repr());
    BOOST_CHECK_EQUAL(parser->lastToken()->repr(), token.repr());
    BOOST_CHECK_EQUAL(parser->nextToken(&tokens)->repr(), token.repr());
    BOOST_CHECK_EQUAL(parser->lastToken()->repr(), token.repr());

    BOOST_CHECK_EQUAL(tokens.size(), 1);
    if(tokens.size() == 1)
        BOOST_CHECK_EQUAL(tokens[0]->repr(), token.repr());

    tokens.clear();
    token = Token(Token::TOK_CHARACTER, Token::CC_SPACE, " ", "", 0, 1, 10, 10);
    BOOST_CHECK_EQUAL(parser->peekToken()->repr(), token.repr());
    BOOST_CHECK_EQUAL(parser->lastToken()->repr(), token.repr());
    BOOST_CHECK_EQUAL(parser->nextToken(&tokens)->repr(), token.repr());
    BOOST_CHECK_EQUAL(parser->lastToken()->repr(), token.repr());

    BOOST_CHECK_EQUAL(tokens.size(), 1);
    if(tokens.size() == 1)
        BOOST_CHECK_EQUAL(tokens[0]->repr(), token.repr());
}
