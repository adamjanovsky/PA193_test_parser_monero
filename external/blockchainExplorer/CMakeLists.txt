cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME
        xmrblocks)


project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -std=c++14")

if (NOT MONERO_DIR)
    set(MONERO_DIR ~/monero)
endif()

message(STATUS MONERO_DIR ": ${MONERO_DIR}")

set(MONERO_SOURCE_DIR ${MONERO_DIR}
        CACHE PATH "Path to the root directory for Monero")

# set location of monero build tree
set(MONERO_BUILD_DIR ${MONERO_SOURCE_DIR}/build/release/
        CACHE PATH "Path to the build directory for Monero")

set(MY_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake"
        CACHE PATH "The path to the cmake directory of the current project")

list(APPEND CMAKE_MODULE_PATH "${MY_CMAKE_DIR}")

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${MONERO_BUILD_DIR}"
        CACHE PATH "Add Monero directory for library searching")


find_package(Monero)

# find boost
find_package(Boost COMPONENTS
        system
        filesystem
        thread
        date_time
        chrono
        regex
        serialization
        program_options
        date_time
        REQUIRED)



# include boost headers
include_directories(${Boost_INCLUDE_DIRS})


# add src/ subfolder
add_subdirectory(src/)


set(SOURCE_FILES
        main.cpp)


add_executable(${PROJECT_NAME}
        ${SOURCE_FILES})



set(LIBRARIES
        myxrm
        #wallet
        blockchain_db
        cryptonote_core
        cryptonote_protocol
        cryptonote_basic
        daemonizer
        cncrypto
        blocks
        lmdb
        ringct
        common
        mnemonics
        epee
        easylogging
        ${Boost_LIBRARIES}
        pthread
        unbound
        curl
        crypto
        ssl)

set(LIBRARIES ${LIBRARIES} atomic)

set(LIBRARIES ${LIBRARIES} dl)


target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
