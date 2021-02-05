flex_target(lex lexical_analyzer.l ${CMAKE_CURRENT_BINARY_DIR}/lexical_analyzer.c)
bison_target(syntax syntax_analyzer.y ${CMAKE_CURRENT_BINARY_DIR}/syntax_analyzer.c)
add_flex_bison_dependency(lex syntax)
add_library(syntax STATIC
  ${BISON_syntax_OUTPUTS}
  ${FLEX_lex_OUTPUTS}
)
