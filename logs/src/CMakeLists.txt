set(base_SRCS
  main.c
  )

add_library(lsp_demo ${base_SRCS})
target_link_libraries(lsp_demo pthread)
target_link_libraries(lsp_demo logger)
target_link_libraries(lsp_demo datetime)
#add_library(muduo_base_cpp11 ${base_SRCS})
#target_link_libraries(muduo_base_cpp11 pthread rt)
#set_target_properties(muduo_base_cpp11 PROPERTIES COMPILE_FLAGS "-std=c++0x")



add_executable(lsp_main main.c)
install(TARGETS lsp_main RUNTIME DESTINATION bin)
