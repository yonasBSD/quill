set(EXAMPLE_TARGETS
        quill_example_sbe_logging
)

# Add example executables
foreach (example_target ${EXAMPLE_TARGETS})
    # Determine the source file from the target name
    string(REPLACE "quill_example_" "" source_name ${example_target})
    set(source_file "${source_name}.cpp")

    add_executable(${example_target} ${source_file})
    set_common_compile_options(${example_target})
    target_link_libraries(${example_target} quill)

    if (MSVC)
        target_compile_options(${example_target} PRIVATE /wd4245 /WX-)
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${example_target} PRIVATE -Wno-sign-conversion -Wno-error)
    endif ()

endforeach ()

install(TARGETS ${EXAMPLE_TARGETS}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
