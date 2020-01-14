
add_subdirectory(datetime)
add_subdirectory(logs)

set(base_SRCS
  main.c
  )
add_executable(log_demo main.c)
target_link_libraries(log_demo pthread)
target_link_libraries(log_demo logger)
target_link_libraries(log_demo datetimeutil)
#add_library(muduo_base_cpp11 ${base_SRCS})
#target_link_libraries(muduo_base_cpp11 pthread rt)
#set_target_properties(muduo_base_cpp11 PROPERTIES COMPILE_FLAGS "-std=c++0x")



install(TARGETS log_demo RUNTIME DESTINATION bin)
