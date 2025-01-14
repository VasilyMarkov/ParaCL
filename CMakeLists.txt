cmake_minimum_required(VERSION 3.5)

project(grammar LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

option(DEBUG_GRAMMAR "Debug" OFF)

set(GRAMMAR "lang/grammar.y" CACHE STRING "file with grammar" FORCE)

flex_target(scanner
  lang/lexer.l
  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

bison_target(parser
  ${GRAMMAR}
  ${CMAKE_CURRENT_BINARY_DIR}/parser.cc
  COMPILE_FLAGS "--defines=${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.hh")

add_flex_bison_dependency(scanner parser)

add_compile_options(${CMAKE_CXX_FLAGS_RELEASE})

message("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
message("CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS_RELEASE}")



add_executable(${PROJECT_NAME} 
  app/main.cpp  
  ast/ast.cpp  
  ${BISON_parser_OUTPUTS}
  ${FLEX_scanner_OUTPUTS}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ast app)