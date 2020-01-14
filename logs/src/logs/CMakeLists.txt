set(base_SRCS
  logger.c
  )

add_library(logger ${base_SRCS})
target_link_libraries(logger pthread)

#add_library(muduo_base_cpp11 ${base_SRCS})
#target_link_libraries(muduo_base_cpp11 pthread rt)
#set_target_properties(muduo_base_cpp11 PROPERTIES COMPILE_FLAGS "-std=c++0x")

install(TARGETS logger DESTINATION lib)
#install(TARGETS muduo_base_cpp11 DESTINATION lib)

file(GLOB HEADERS "logger.h")
install(FILES ${HEADERS} DESTINATION include/logger)


