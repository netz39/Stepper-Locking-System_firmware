cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0048 NEW)

project(parameter_manager LANGUAGES CXX C ASM)

add_library(${PROJECT_NAME} INTERFACE)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
target_include_directories(${PROJECT_NAME} INTERFACE
        include)
target_link_libraries(${PROJECT_NAME} INTERFACE core i2c-drivers)

if (${CMAKE_TESTING_ENABLED})
    find_package(PkgConfig)
    pkg_search_module(GTEST REQUIRED gtest)
    pkg_search_module(GMOCK REQUIRED gmock)

    add_executable(${PROJECT_NAME}_test
            tests/src/FakeMemoryTest.cxx
            tests/src/main.cxx
            tests/src/SettingsContainerTest.cxx
            tests/src/SettingsEntryTest.cxx
            tests/src/SettingsIOTest.cxx
            tests/src/SettingsUserTest.cxx
            )
    target_compile_features(${PROJECT_NAME}_test PUBLIC cxx_std_17)
    target_include_directories(${PROJECT_NAME}_test PRIVATE
            tests/include)

    target_link_libraries(${PROJECT_NAME}_test PRIVATE
            ${GTEST_LDFLAGS}
            ${GMOCK_LDFLAGS}
            core
            i2c-drivers
            gcov
            ${PROJECT_NAME})

    target_link_options(${PROJECT_NAME}_test PRIVATE --coverage)
    target_compile_options(${PROJECT_NAME}_test PRIVATE ${GTEST_CFLAGS} ${GMOCK_CFLAGS} --coverage)
    add_test(${PROJECT_NAME}_test ${PROJECT_NAME}_test)

    function(DEFINE_WILL_FAIL_TESTS Name)
        add_executable(${PROJECT_NAME}_${Name}
                tests/src/will-fail/${Name}.cpp)
        target_compile_features(${PROJECT_NAME}_test PUBLIC cxx_std_17)

        target_include_directories(${PROJECT_NAME}_${Name} PRIVATE
                tests/include)

        target_link_libraries(${PROJECT_NAME}_${Name}
                ${PROJECT_NAME})
        set_target_properties(${PROJECT_NAME}_${Name} PROPERTIES
                EXCLUDE_FROM_ALL TRUE
                EXCLUDE_FROM_DEFAULT_BUILD TRUE)

        add_test(NAME ${PROJECT_NAME}_${Name}
                COMMAND ${CMAKE_COMMAND} --build . --target ${PROJECT_NAME}_${Name} --config $<CONFIGURATION>
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

        set_tests_properties(${PROJECT_NAME}_${Name} PROPERTIES WILL_FAIL TRUE)
    endfunction()

    DEFINE_WILL_FAIL_TESTS(DuplicateSettingName)
    DEFINE_WILL_FAIL_TESTS(SettingsEntryDefaultBiggerMax)
    DEFINE_WILL_FAIL_TESTS(SettingsEntryDefaultSmallerMin)
    DEFINE_WILL_FAIL_TESTS(SettingsEntryMinBiggerMax)

endif ()