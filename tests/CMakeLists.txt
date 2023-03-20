add_custom_target(
        test_archiver
        WORKING_DIRECTORY
        DEPENDS archiver
        COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/test.py ${CMAKE_BINARY_DIR}/archiver ${CMAKE_CURRENT_SOURCE_DIR}/data
)
