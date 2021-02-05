add_executable(
    cminusfc
    cminusfc.cpp
    cminusf_builder.cpp
)

# target_compile_options(
#     cminusc
#     PRIVATE "-fno-rtti"
# )

target_link_libraries(
    cminusfc
    OP_lib
    IR_lib
    common
    syntax
)

install(
    TARGETS cminusfc
    RUNTIME DESTINATION bin
)


