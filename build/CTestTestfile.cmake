# CMake generated Testfile for 
# Source directory: /home/runner/workspace
# Build directory: /home/runner/workspace/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(test_ir_generation "python3" "/home/runner/workspace/build/test/test_ir_generation.py")
set_tests_properties(test_ir_generation PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/workspace/CMakeLists.txt;123;add_test;/home/runner/workspace/CMakeLists.txt;0;")
add_test(test_memory_mapper "python3" "/home/runner/workspace/build/test/test_memory_mapper.py")
set_tests_properties(test_memory_mapper PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/workspace/CMakeLists.txt;123;add_test;/home/runner/workspace/CMakeLists.txt;0;")
add_test(test_parser "python3" "/home/runner/workspace/build/test/test_parser.py")
set_tests_properties(test_parser PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/workspace/CMakeLists.txt;123;add_test;/home/runner/workspace/CMakeLists.txt;0;")
add_test(test_pim_backend "python3" "/home/runner/workspace/build/test/test_pim_backend.py")
set_tests_properties(test_pim_backend PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/workspace/CMakeLists.txt;123;add_test;/home/runner/workspace/CMakeLists.txt;0;")
