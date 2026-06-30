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

# Add VTK 9+ includes
if(TARGET VTK::CommonCore)
    get_target_property(_include_dirs VTK::CommonCore INTERFACE_INCLUDE_DIRECTORIES)
    foreach(_dir IN LISTS _include_dirs)
        if(_dir MATCHES "^\\$<BUILD_INTERFACE:(.+)>$")
            # VTK 9.5+ have $<BUILD_INTERFACE:path> generator expressions in their includes
            list(APPEND PROJECT_INCLUDE_DIRS "${CMAKE_MATCH_1}")
        elseif(NOT _dir MATCHES "^\\$<")
            # VTK 9.0 to 9.4 have plain include paths, no generator expressions
            list(APPEND PROJECT_INCLUDE_DIRS "${_dir}")
        endif()
    endforeach()
endif()

# Add PETSc4Py includes
list(APPEND PROJECT_INCLUDE_DIRS ${PETSC4PY_INCLUDES})

# Add the project's own source directory so cppwg can find project headers
list(APPEND PROJECT_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Add pychaste typecaster includes
header_dirs("${CMAKE_SOURCE_DIR}/pychaste/dynamic" _include_dirs)
list(APPEND PROJECT_INCLUDE_DIRS ${_include_dirs})

# Command for generating wrappers
set(WRAPPER_GENERATION_COMMAND
    ${chaste_python3_venv}/cppwg ${CMAKE_CURRENT_SOURCE_DIR}/..
        -w ${CMAKE_CURRENT_BINARY_DIR}/wrappers
        -p ${CMAKE_CURRENT_SOURCE_DIR}/config.yaml
        -i ${PROJECT_INCLUDE_DIRS}
        -l ${CMAKE_CURRENT_BINARY_DIR}/cppwg.log
        --std c++17
)

# Generate wrappers if not already present
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/wrappers")
    execute_process(
        COMMAND ${WRAPPER_GENERATION_COMMAND}
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()

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

file(GLOB_RECURSE WRAPPER_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/wrappers/all/*.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/wrappers/all/*.hpp
)

pybind11_add_module(_template_project_all OPT_SIZE ${WRAPPER_SOURCES})

target_link_libraries(_template_project_all PRIVATE
    ${Python3_LIBRARIES}
    Chaste_COMMON_DEPS
    chaste_project_template_project
    chaste_pychaste
)

target_include_directories(_template_project_all PRIVATE ${PROJECT_INCLUDE_DIRS})

target_compile_options(_template_project_all PRIVATE -Wno-unused-local-typedefs)

set_target_properties(_template_project_all PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)

add_dependencies(_template_project_all pychaste)
add_dependencies(project_template_project _template_project_all)

set_target_properties(_template_project_all PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/package/template_project
)

################################
####  Copy Python source
################################

file(GLOB_RECURSE _py_sources
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/py/*
)
list(FILTER _py_sources EXCLUDE REGEX "\\.(dll|dylib|pyc|so)$")

foreach(_src ${_py_sources})
    file(RELATIVE_PATH _rel ${CMAKE_CURRENT_SOURCE_DIR}/../src/py ${_src})
    configure_file(${_src} ${CMAKE_CURRENT_BINARY_DIR}/package/${_rel} COPYONLY)
endforeach()
