# Copyright (c) 2005-2026, University of Oxford.
# All rights reserved.
#
# University of Oxford means the Chancellor, Masters and Scholars of the
# University of Oxford, having an administrative office at Wellington
# Square, Oxford OX1 2JD, UK.
#
# This file is part of Chaste.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#  * Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#  * Neither the name of the University of Oxford nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
# GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
# OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

if(NOT Chaste_ENABLE_PYCHASTE)
    return()
endif()

################################
####  Generate Python wrappers
################################

# Get includes from the project's chaste target
get_target_property(PROJECT_INCLUDE_DIRS chaste_project_template_project INCLUDE_DIRECTORIES)

# Inherit the include configuration that pychaste exposes for
# VTK, PETSc4Py and the pybind11 typecasters.
get_target_property(PYCHASTE_INTERFACE_INCLUDE_DIRS chaste_pychaste INTERFACE_INCLUDE_DIRECTORIES)
if(NOT PYCHASTE_INTERFACE_INCLUDE_DIRS)
    message(FATAL_ERROR
        "Cannot find chaste_pychaste INTERFACE include directories; update your Chaste source tree.")
endif()
foreach(_dir IN LISTS PYCHASTE_INTERFACE_INCLUDE_DIRS)
    if(_dir MATCHES "^\\$<BUILD_INTERFACE:(.+)>$")
        list(APPEND PROJECT_INCLUDE_DIRS "${CMAKE_MATCH_1}")
    elseif(NOT _dir MATCHES "^\\$<")
        list(APPEND PROJECT_INCLUDE_DIRS "${_dir}")
    endif()
endforeach()

# Add the project's own source directory so cppwg can find project headers
list(APPEND PROJECT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../src)

list(REMOVE_DUPLICATES PROJECT_INCLUDE_DIRS)

# To parse C++ headers, castxml mimics a compiler. PyChaste searches for clang,
# which has better castxml support, and caches the result in Chaste_CASTXML_COMPILER,
# which this project inherits. The fallback is set to the normal compiler below.
# The library itself is built with the normal compiler either way.
if(NOT Chaste_CASTXML_COMPILER)
    set(Chaste_CASTXML_COMPILER ${CMAKE_CXX_COMPILER})
    message(WARNING "clang not found. Using ${CMAKE_CXX_COMPILER} as the castxml"
                    " compiler. Wrapper generation may fail")
endif()

# Command for generating wrappers.
#  - gnu++17 (rather than c++17) and -Wno-missing-template-arg-list-after-template-kw
#    silence a castxml/LLVM error on use of the template keyword without a
#    following argument list, which appears in some boost headers.
set(WRAPPER_GENERATION_COMMAND
    ${chaste_python3_venv}/cppwg ${CMAKE_CURRENT_SOURCE_DIR}/..
        -w ${CMAKE_CURRENT_BINARY_DIR}/wrappers
        -p ${CMAKE_CURRENT_SOURCE_DIR}/config.yaml
        -i ${PROJECT_INCLUDE_DIRS}
        -l ${CMAKE_CURRENT_BINARY_DIR}/cppwg.log
        --std "gnu++17"
        --castxml_cflags="-Wno-missing-template-arg-list-after-template-kw"
        --castxml_compiler ${Chaste_CASTXML_COMPILER}
)

# Generate wrappers. Unlike pychaste, which generates once, a user project
# regenerates on every configure so that edits to config.yaml and to the
# project's own classes are picked up.
execute_process(
    COMMAND ${WRAPPER_GENERATION_COMMAND}
    COMMAND_ERROR_IS_FATAL ANY
)

# Target for manually regenerating wrappers
add_custom_target(template_project_wrappers
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/wrappers
    COMMAND ${WRAPPER_GENERATION_COMMAND}
)

# Add generated wrapper headers to includes
header_dirs("${CMAKE_CURRENT_BINARY_DIR}/wrappers" _include_dirs)
list(APPEND PROJECT_INCLUDE_DIRS ${_include_dirs})

################################
####  Build Python module
################################
# Creates a `_template_project_all` library from the Python wrappers.
# When built, it can be used in Python as `from template_project._template_project_all import *`.

file(GLOB_RECURSE WRAPPER_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/wrappers/all/*.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/wrappers/all/*.hpp
)

if(NOT WRAPPER_SOURCES)
    message(FATAL_ERROR
        "No Python wrapper sources were generated for the project. "
        "Check the classes listed in bindings/config.yaml and the cppwg log at: "
        "  ${CMAKE_CURRENT_BINARY_DIR}/cppwg.log")
endif()

pybind11_add_module(_template_project_all OPT_SIZE ${WRAPPER_SOURCES})

target_link_libraries(_template_project_all PRIVATE
    Chaste_COMMON_DEPS
    chaste_project_template_project
    chaste_pychaste
)

target_include_directories(_template_project_all PRIVATE ${PROJECT_INCLUDE_DIRS})

# -g0: Disable debug symbols in the Python modules as they are not needed.
target_compile_options(_template_project_all PRIVATE -Wno-unused-local-typedefs -g0)

# Perform link-time optimization for Release builds only.
set_target_properties(_template_project_all PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # clang ThinLTO needs lld.
    target_link_options(_template_project_all PRIVATE -fuse-ld=lld)
endif()

set_target_properties(_template_project_all PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/package/template_project
)

add_dependencies(_template_project_all pychaste)
add_dependencies(project_template_project _template_project_all)

################################
####  Copy Python source
################################

file(GLOB_RECURSE _py_sources
    CONFIGURE_DEPENDS
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/package
    ${CMAKE_CURRENT_SOURCE_DIR}/package/*
)
list(FILTER _py_sources EXCLUDE REGEX "\\.(dll|dylib|pyc|so)$")

foreach(_src ${_py_sources})
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/package/${_src}
        ${CMAKE_CURRENT_BINARY_DIR}/package/${_src}
        COPYONLY
    )
endforeach()
