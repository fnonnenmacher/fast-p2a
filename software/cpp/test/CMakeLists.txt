# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

cmake_minimum_required(VERSION 3.10)

project(parquetwriter_test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -fPIC -Ofast -march=native -I/usr/local/include --std=c++11 -mmacosx-version-min=10.9")

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_library(LIB_ARROW arrow)
find_library(LIB_PARQUET parquet)

add_executable(parquetwriter_test "./parquetwriter_test.cc")
add_executable(parquet_debugprint "./parquet_debugprint.cc")
target_link_libraries(parquetwriter_test ${LIB_PARQUET} ${LIB_ARROW})
target_link_libraries(parquet_debugprint ${LIB_PARQUET} ${LIB_ARROW})

cmake_minimum_required(VERSION 3.10)

project(main)
