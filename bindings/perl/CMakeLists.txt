find_package(SWIG)

set(WRAPPERS_CPP_DIRS writers)

foreach(dir ${WRAPPERS_CPP_DIRS})
    include_directories("${psi-toolkit_SOURCE_DIR}/bindings/perl/${dir}")
    link_directories("${psi-toolkit_BINARY_DIR}/bindings/perl/${dir}")
    add_subdirectory(${dir})
endforeach(dir)

add_subdirectory(PSIToolkit)
