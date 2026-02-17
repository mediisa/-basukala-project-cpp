cmake_minimum_required(VERSION 3.10)
project(HorrorCityGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(D:/basukala/-basukala-project-cpp/include)


set(SOURCES
    admin.cpp
    common.cpp
    customer.cpp
    main.cpp
    WRFile.cpp
    )

set(HEADERS
    admin.h
    common.h
    customer.h
    WRFile.h
)

add_executable(BasuKala ${SOURCES} ${HEADERS})