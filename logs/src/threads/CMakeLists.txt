set(base_SRCS
  thread.c
  )

add_library(common_thread ${base_SRCS})
target_link_libraries(common_thread pthread)

#add_library(muduo_base_cpp11 ${base_SRCS})
#target_link_libraries(muduo_base_cpp11 pthread rt)
#set_target_properties(muduo_base_cpp11 PROPERTIES COMPILE_FLAGS "-std=c++0x")

install(TARGETS common_thread DESTINATION lib)
#install(TARGETS muduo_base_cpp11 DESTINATION lib)


file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include)

