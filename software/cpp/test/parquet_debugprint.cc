// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <arrow/io/file.h>
#include <arrow/memory_pool.h>
#include <arrow/status.h>
#include <arrow/util/compression.h>
#include <parquet/file_reader.h>
#include <parquet/printer.h>
#include <parquet/properties.h>
#include <parquet/types.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <list>
#include <memory>
#include <string>


int main(int argc, char **argv) {
	if (argc != 2) {
		printf("usage: %s <parquet file>\n\n");
		exit(-1);
	}

	std::list<int> collist({0});
	std::unique_ptr<parquet::ParquetFileReader> reader = parquet::ParquetFileReader::OpenFile(argv[1]);;
	std::stringstream ss;
	std::list<int> columns;
	parquet::ParquetFilePrinter printer(reader.get());
	printer.DebugPrint(ss, collist, true, true, true,
			argv[1]);
	std::cout << ss.str();
  return 0;
}
