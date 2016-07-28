include_directories(${Boost_INCLUDE_DIR})
include_directories(${PYTHON_INCLUDE_DIR})

add_executable(texpp texpp.cc)
target_link_libraries(texpp libtexpp)
set(TEXPP_EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/texpp)

add_executable(test_lexer test_lexer.cc)
target_link_libraries(test_lexer libtexpp)
add_test(test_lexer ${EXECUTABLE_OUTPUT_PATH}/test_lexer)

add_executable(test_parser test_parser.cc)
target_link_libraries(test_parser libtexpp)
add_test(test_parser ${EXECUTABLE_OUTPUT_PATH}/test_parser)

# add_executable(test_literal test_literal.cpp)
# target_link_libraries(test_literal _chrefliterals ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${ICU_LIBRARIES})
# add_test(test_literal ${EXECUTABLE_OUTPUT_PATH}/test_literal)

add_executable(test_citations test_citations.cc)
target_link_libraries(test_citations libtexpp _chrefliterals ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
add_test(test_citations ${EXECUTABLE_OUTPUT_PATH}/test_citations)

if(TEX_FOUND)
    add_subdirectory(tex)
endif(TEX_FOUND)

#if(BOOST_PYTHON_FOUND AND PYTHONINTERP_FOUND AND TEX_FOUND)
#    add_subdirectory(hrefkeywords)
#endif(BOOST_PYTHON_FOUND AND PYTHONINTERP_FOUND AND TEX_FOUND)


# somehow the BOOST_PYTHON_FOUND is empty while Boost python3 included corectly
# that`s why we should include manualy hrefkeywords directory till clarify what
# is wrong with BOOST_PYTHON_FOUND flag
if(PYTHONINTERP_FOUND AND TEX_FOUND)
    add_subdirectory(hrefkeywords)
endif(PYTHONINTERP_FOUND AND TEX_FOUND)
