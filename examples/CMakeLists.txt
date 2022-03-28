add_executable(stack-dark stack-dark.c)
target_link_libraries(stack-dark spica png)

add_executable(compose-rgb compose-rgb.c)
target_link_libraries(compose-rgb spica png m)

add_executable(align align.c)
target_link_libraries(align spica png m)
