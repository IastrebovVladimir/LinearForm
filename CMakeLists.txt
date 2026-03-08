cmake_minimum_required(VERSION 3.29)
project(Linear_Form C)

set(CMAKE_C_STANDARD 11)

add_executable(Linear_Form main.c
        linearform.c
        types.c
        menu.c
)

add_executable(main_test
        test/main_test.c
        test/tests.c
        linearform.c
        types.c
)
