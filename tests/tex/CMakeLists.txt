include_directories(${Boost_INCLUDE_DIR})

add_executable(test_tex test_tex.cc)

add_executable(test_source test_source.cc)
target_link_libraries(test_source libtexpp)

# find all .tex files in current derectory. Put full names(/path/to/files.tex)
# into the TEX_TEST_FILES variable
file(GLOB TEX_TEST_FILES "*.tex")
# for every .tex file
foreach(_tex_file ${TEX_TEST_FILES})
    # get the file name without directory or longest extension
    # and put the name into _tex_file_name variable
    get_filename_component(_tex_file_name ${_tex_file} NAME_WE)

    add_test(test_tex_${_tex_file_name}
         env TEXINPUTS=${CMAKE_CURRENT_SOURCE_DIR}
         ${CMAKE_CURRENT_BINARY_DIR}/test_tex
         ${TEX_EXECUTABLE} ${TEXPP_EXECUTABLE} ${_tex_file})
    set_property(TEST test_tex_${_tex_file_name} PROPERTY TIMEOUT 30)

    add_test(test_source_${_tex_file_name}
         env TEXINPUTS=${CMAKE_CURRENT_SOURCE_DIR}
         ${CMAKE_CURRENT_BINARY_DIR}/test_source
         ${_tex_file})
    set_property(TEST test_source_${_tex_file_name} PROPERTY TIMEOUT 30)
endforeach(_tex_file ${TEX_TEST_FILES})

