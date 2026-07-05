cmake_minimum_required(VERSION 3.10)
project(HorrorCityGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(D:/basu/include)


set(SOURCES
    Admin.cpp
    comm.cpp
    Customer.cpp
    Graph.cpp
    Models.cpp
    ProductTrie.cpp
    ProductBST.cpp
    main.cpp
    User.cpp
    WRFile.cpp
    )

set(HEADERS
    Admin.h
    comm.h
    Customer.h
    Graph.h
    Models.h
    ProductTrie.h
    ProductBST.h
    User.h
    WRFile.h
)

add_executable(BasuKala ${SOURCES} ${HEADERS})