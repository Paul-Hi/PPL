project(compiler)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/command_line_parser.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/token.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ast.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ast_visitor.hpp
)

if(WIN32 AND MSVC)
    add_executable(
        compiler WIN32
            ${SOURCES}
    )
else()
        add_executable(
            compiler
            ${SOURCES}
    )
endif()

set_target_properties(compiler
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>$<$<CONFIG:Release>:/release>/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>$<$<CONFIG:Release>:/release>/lib
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>$<$<CONFIG:Release>:/release>/bin
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

target_include_directories(compiler
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(compiler
PUBLIC
        $<$<CONFIG:Debug>:DEBUG>
        $<$<CXX_COMPILER_ID:MSVC>:WINMAIN>
        $<$<CXX_COMPILER_ID:Clang>:$<$<BOOL:{CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"}>:WINMAIN>>
        $<$<BOOL:${BUILD_TESTS}>:TEST>
        $<$<BOOL:${PROFILE}>:PROFILE>
PRIVATE
        $<$<BOOL:${WIN32}>:WIN32>
        $<$<BOOL:${LINUX}>:LINUX>
        $<$<CXX_COMPILER_ID:MSVC>: _CRT_SECURE_NO_WARNINGS>
    )

target_compile_options(compiler
    PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>: /W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>: -Wall -Wextra>
    $<$<CXX_COMPILER_ID:MSVC>:$<$<BOOL:${ENABLE_HARD_WARNINGS}>: /WX>>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:$<$<BOOL:${ENABLE_HARD_WARNINGS}>: -pedantic -Werror -Wconversion -pedantic-errors>>
)

install(TARGETS compiler DESTINATION bin)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/res/ DESTINATION bin/res)
