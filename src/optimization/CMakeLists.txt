add_library(
        OP_lib STATIC
        Dominators.cpp
        Mem2Reg.cpp
        LoopSearch.cpp
        ConstPropagation.cpp
        ActiveVars.cpp
        LoopInvHoist.cpp
)
