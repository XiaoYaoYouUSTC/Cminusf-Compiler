flex_target(lex lexical_analyzer.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c)
add_library(flex STATIC
	lex.yy.c
)






