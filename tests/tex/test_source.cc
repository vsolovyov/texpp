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

#include <boost/test/included/unit_test.hpp>
#include <boost/test/parameterized_test.hpp>
#include <boost/algorithm/string.hpp>

#include <texpp/parser.h>
#include <texpp/logger.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace texpp;

#ifndef WINDOWS
#define PATH_SEP '/'
#define DEV_NULL "/dev/null"
#else
#define PATH_SEP '\\'
#define DEV_NULL "/dev/null"
#endif

void test_usage()
{
    BOOST_FAIL( "Wrong number of command line arguments\n"
                "Usage: test_tex file1 file2 ..." );
}

void test_source(const char* testfile)
{
    string jobname(testfile);

    // extract input file name(cut "path/to/file" and extestion ".tex")
    size_t n = jobname.rfind(PATH_SEP);
    if(n != jobname.npos)
        jobname = jobname.substr(n+1);
    n = jobname.rfind(".tex");
    if(n != jobname.npos)
        jobname = jobname.substr(0, n);

    shared_ptr<std::istream> file(new std::ifstream(testfile, std::fstream::in));
    BOOST_REQUIRE_MESSAGE( !file->fail(), "Can not open input file" );

    // get length of file
    file->seekg(0, std::ios::end);
    size_t fileSize = file->tellg();
    file->seekg(0, std::ios::beg);

    char* buffer = new char[fileSize+1];
    buffer[fileSize] = 0;   // add null termination at the end of file

    // read data
    file->read(buffer, fileSize);
    file->seekg(0, std::ios::beg);

    string origSource(buffer);

  vector<string> origSourceLines; // vector for record splited text
    // split origSource into lines. Move the result to origSourceLines;
    boost::split(origSourceLines, origSource, boost::is_any_of("\n"));

    Parser parser(testfile, file, "", false, false, Logger::ptr(new NullLogger));
    Node::ptr document = parser.parse();
    
    // recover source TeX text for the document
    string source = document->source(testfile);

    vector<string> sourceLines;
    boost::split(sourceLines, source, boost::is_any_of("\n"));
    
    //BOOST_CHECK_EQUAL_COLLECTIONS(
    //    sourceLines.begin(), sourceLines.end());
    //    origSourceLines.begin(), origSourceLines.end(),

    // compare collection
    BOOST_CHECK_EQUAL(source, origSource);

    // write
    std::ofstream outFileOrig((jobname+".src").c_str());
    outFileOrig.write(origSource.c_str(), origSource.size());

    std::ofstream outFile((jobname+".src.pp").c_str());
    outFile.write(source.c_str(), source.size());

    delete[] buffer;
}

boost::unit_test::test_suite*
init_unit_test_suite( int argc, char* argv[] ) 
{
    if(argc < 2) {
        boost::unit_test::framework::master_test_suite().
            add( BOOST_TEST_CASE( &test_usage ) );
        return 0;
    }

    boost::unit_test::framework::master_test_suite().
        add( BOOST_PARAM_TEST_CASE( &test_source, argv+1, argv+argc ) );

    return 0;
}


