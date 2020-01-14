set(base_SRCS
  datetimeutil.c
  )

add_library(datetimeutil ${base_SRCS})
#target_link_libraries(muduo_base pthread rt)

#add_library(muduo_base_cpp11 ${base_SRCS})
#target_link_libraries(muduo_base_cpp11 pthread rt)
#set_target_properties(muduo_base_cpp11 PROPERTIES COMPILE_FLAGS "-std=c++0x")

install(TARGETS datetimeutil DESTINATION lib)

file(GLOB HEADERS "datetimeutil.h")
install(FILES ${HEADERS} DESTINATION include/datetime)

#if(MUDUO_BUILD_EXAMPLES)
#  add_subdirectory(tests)
#endif()
