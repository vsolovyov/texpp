//Link to Boost
#define BOOST_TEST_DYN_LINK

#include <Python.h>
//Define our Module name (prints at testing)
#define BOOST_TEST_MODULE "inner_texpp_test"

//VERY IMPORTANT - include this last
#include <boost/test/included/unit_test.hpp>
#include "hrefkeywords/_chrefliterals.h"

WordsDict* wordsDict = new WordsDict ("/usr/share/dict/words",4);
boost::python::dict whiteList;
#define normLiteral(x) normLiteral(x,wordsDict,whiteList,false)

// ------------- Tests Follow -------------

BOOST_AUTO_TEST_CASE( testFirst)
{
    Py_Initialize();
    BOOST_CHECK_EQUAL(normLiteral("electrons"), "electron");
    BOOST_CHECK_EQUAL(normLiteral("xxxxxxxxx"), "xxxxxxxxx");
    BOOST_CHECK_EQUAL(normLiteral("eLectrons"), "E.L.E.C.T.R.O.N.S.");
}

BOOST_AUTO_TEST_CASE(testShortWords)
{
    BOOST_CHECK_EQUAL(normLiteral("or"), "or");
    BOOST_CHECK_EQUAL(normLiteral("set"), "set");
    BOOST_CHECK_EQUAL(normLiteral("sets"), "set");
    BOOST_CHECK_EQUAL(normLiteral("SET"), "S.E.T.");
    BOOST_CHECK_EQUAL(normLiteral("S.E.T."), "S.E.T.");
    BOOST_CHECK_EQUAL(normLiteral("s.e.t."), "S.E.T.");
    BOOST_CHECK_EQUAL(normLiteral("seT"), "S.E.T.");
}

BOOST_AUTO_TEST_CASE(testShortNonWords)
{
    BOOST_CHECK_EQUAL(normLiteral("dof"), "D.O.F.");
    BOOST_CHECK_EQUAL(normLiteral("Dof"), "D.O.F.");
    BOOST_CHECK_EQUAL(normLiteral("DoF"), "D.O.F.");
}

BOOST_AUTO_TEST_CASE(testShortNames)
{
    BOOST_CHECK_EQUAL(normLiteral("Ada"), "ada");
    BOOST_CHECK_EQUAL(normLiteral("ada"), "A.D.A.");
}

BOOST_AUTO_TEST_CASE(testOneLetter)
{
    BOOST_CHECK_EQUAL(normLiteral("I"), "i");
    BOOST_CHECK_EQUAL(normLiteral("i"), "I.");
    BOOST_CHECK_EQUAL(normLiteral("A"), "a");
    BOOST_CHECK_EQUAL(normLiteral("a"), "a");
    BOOST_CHECK_EQUAL(normLiteral("T"), "T.");
    BOOST_CHECK_EQUAL(normLiteral("t"), "T.");
}

BOOST_AUTO_TEST_CASE(testSAbbr)
{
    BOOST_CHECK_EQUAL(normLiteral("Dr.A.B."), "DR.A.B.");
    BOOST_CHECK_EQUAL(normLiteral("Dr.A.Boy."), "DR.A.BOY.");
    BOOST_CHECK_EQUAL(normLiteral("Dr.A.Boy"), "DR.A.boi");
    BOOST_CHECK_EQUAL(normLiteral("Dr.A.boy"), "DR.A.boi");
    BOOST_CHECK_EQUAL(normLiteral("Dr.A.Ada"), "DR.A.ada");
    BOOST_CHECK_EQUAL(normLiteral("Dr.A.ada"), "DR.A.A.D.A.");
}

BOOST_AUTO_TEST_CASE(testDigits)
{
    BOOST_CHECK_EQUAL(normLiteral("Fig.1"), "FIG.1.");
    BOOST_CHECK_EQUAL(normLiteral("SU(2)"), "S.U.(2.)");
}

BOOST_AUTO_TEST_CASE(testIgnoredSymbols)
{
    BOOST_CHECK_EQUAL(normLiteral("g-factor"), "G.factor");
    BOOST_CHECK_EQUAL(normLiteral("g/factor"), "G.factor");
    BOOST_CHECK_EQUAL(normLiteral("g.factor"), "G.factor");
    BOOST_CHECK_EQUAL(normLiteral("g factor"), "G.factor");
    BOOST_CHECK_EQUAL(normLiteral("T-J model"), "T.J.model");
}

BOOST_AUTO_TEST_CASE(testPluralAbbr)
{
    BOOST_CHECK_EQUAL(normLiteral("SETs"), "S.E.T.");
    BOOST_CHECK_EQUAL(normLiteral("SETes"), "S.E.T.");
    BOOST_CHECK_EQUAL(normLiteral("SETS"), "S.E.T.S.");
    BOOST_CHECK_EQUAL(normLiteral("SETES"), "S.E.T.E.S.");
    BOOST_CHECK_EQUAL(normLiteral("S.E.T.s"), "S.E.T.S.");
    BOOST_CHECK_EQUAL(normLiteral("dofs"), "D.O.F.S.");
}

BOOST_AUTO_TEST_CASE(testApostrophe)
{
    BOOST_CHECK_EQUAL(normLiteral("world\'s"), "world");
    BOOST_CHECK_EQUAL(normLiteral("\'s x"), "\'S.X.");
    BOOST_CHECK_EQUAL(normLiteral(" \'s x"), "\'S.X.");
}

BOOST_AUTO_TEST_CASE(testArticle)
{
    BOOST_CHECK_EQUAL(normLiteral("The word"), "word");
    BOOST_CHECK_EQUAL(normLiteral("a word"), "word");
    BOOST_CHECK_EQUAL(normLiteral("an apple"), "appl");
    BOOST_CHECK_EQUAL(normLiteral("2 the word"), "2.word");
    BOOST_CHECK_EQUAL(normLiteral("word a"), "worda");
}
