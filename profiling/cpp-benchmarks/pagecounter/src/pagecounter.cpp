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

#include <iostream>
#include <iomanip>

#include <parquet/arrow/reader.h>

#include <SWParquetReader.h>
#include <timer.h>

#define PRIM_WIDTH 64

//Use standard Arrow library functions to read Arrow array from Parquet file
//Only works for Parquet version 1 style files.
std::shared_ptr<arrow::Array> readArray(std::string hw_input_file_path) {
  std::shared_ptr<arrow::io::ReadableFile> infile;
  PARQUET_THROW_NOT_OK(arrow::io::ReadableFile::Open(hw_input_file_path, arrow::default_memory_pool(), &infile));
  
  std::unique_ptr<parquet::arrow::FileReader> reader;
  PARQUET_THROW_NOT_OK(parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &reader));

  std::shared_ptr<arrow::ChunkedArray> carray;
  PARQUET_THROW_NOT_OK(reader->ReadColumn(0, &carray));
  std::shared_ptr<arrow::Array> array = carray->chunk(0);
  return array;
}

int main(int argc, char **argv) {
    int num_values;
    char* hw_input_file_path;
    char* reference_parquet_file_path;
    int iterations;
    bool verify_output;
    ptoa::encoding enc;

    Timer t;

    if (argc == 3) {
      hw_input_file_path = argv[1];
      if(!strncmp(argv[2], "delta", 5)) {
        enc = ptoa::encoding::DELTA;
      } else if (!strncmp(argv[2], "plain", 5)) {
        enc = ptoa::encoding::PLAIN;
      } else {
        std::cerr << "Invalid argument. Option \"encoding\" should be \"delta\" or \"plain\"" << std::endl;
        return 1;
      }
    } else {
      std::cerr << "Usage: prim parquet_hw_input_file_path encoding" << std::endl;
      return 1;
    }

    ptoa::SWParquetReader reader(hw_input_file_path);
    //reader.inspect_metadata(4);
    reader.count_pages(4);
}
