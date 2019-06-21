#I will eventually replace this cmake lists. Its a work in progress and I know its lazy + bad
cmake_minimum_required(VERSION 2.8)
project(clatter)


set(CLAT_SRC src/cli.c src/lexer.c src/parser.c src/util.c src/clatter.c)

add_executable(clatter ${CLAT_SRC})