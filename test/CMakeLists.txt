add_executable(test_i2c test_i2c.cpp ../src/i2c_interface.cpp)
target_include_directories(test_i2c PRIVATE ../include)
add_test(NAME test_i2c COMMAND test_i2c)

add_executable(test_ade test_ade7878a.cpp 
    ../src/ade7878a.cpp 
    ../src/i2c_interface.cpp)
target_include_directories(test_ade PRIVATE ../include)
add_test(NAME test_ade7878a COMMAND test_ade)

add_executable(test_http test_http.cpp ../src/http_client.cpp)
target_include_directories(test_http PRIVATE ../include)
target_link_libraries(test_http PRIVATE ${CURL_LIBRARIES})
add_test(NAME test_http COMMAND test_http)
