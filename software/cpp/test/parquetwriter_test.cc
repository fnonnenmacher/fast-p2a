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


#include <arrow/array/builder_binary.h>
#include <arrow/array/builder_primitive.h>
#include <arrow/io/file.h>
#include <arrow/memory_pool.h>
#include <arrow/status.h>
#include <arrow/table.h>
#include <arrow/type.h>
#include <arrow/util/compression.h>
#include <parquet/arrow/writer.h>
#include <parquet/properties.h>
#include <parquet/types.h>
#include <cstdlib>
#include <memory>
#include <string>

#define NAMEBUFSIZE 64

std::string gen_random_string(const int length) {
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    std::string result(length, 0);

    for (int i = 0; i < length; ++i) {
        result[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return result;
}

std::shared_ptr<arrow::Table> generate_int64_table(int num_values, int nCols, bool sequential) {
    //Create the schema
	std::vector<std::shared_ptr<arrow::Field>> fields;
    for (int c = 0; c < nCols; c++) {
    	char name[NAMEBUFSIZE];
    	snprintf(name, NAMEBUFSIZE, "int%d", c);
    	fields.push_back(arrow::field(name, arrow::int64(), false));
    }
    std::shared_ptr<arrow::Schema> schema = arrow::schema(fields);

    //Generate the values
    std::vector<std::shared_ptr<arrow::Array>> arrays;
    for (int c = 0; c < nCols; c++) {
		arrow::Int64Builder i64builder;
		for (int i = 0; i < num_values; i++) {
			int number;
			if (sequential) {
				number = i;
			} else {
				number = rand();
			}
			PARQUET_THROW_NOT_OK(i64builder.Append(number));

		}
		std::shared_ptr<arrow::Array> i64array;
		PARQUET_THROW_NOT_OK(i64builder.Finish(&i64array));
		arrays.push_back(i64array);
    }

    return arrow::Table::Make(schema, arrays);
}

std::shared_ptr<arrow::Table> generate_int32_table(int num_values, int nCols, bool sequential) {
	//Create the schema
	std::vector<std::shared_ptr<arrow::Field>> fields;
    for (int c = 0; c < nCols; c++) {
    	char name[NAMEBUFSIZE];
    	snprintf(name, NAMEBUFSIZE, "int%d", c);
    	fields.push_back(arrow::field(name, arrow::int32(), false));
    }
    std::shared_ptr<arrow::Schema> schema = arrow::schema(fields);

    //Generate the values
    std::vector<std::shared_ptr<arrow::Array>> arrays;
    for (int c = 0; c < nCols; c++) {
		arrow::Int32Builder i32builder;
		for (int i = 0; i < num_values; i++) {
			int number;
			if (sequential) {
				number = i;
			} else {
				number = rand();
			}
			PARQUET_THROW_NOT_OK(i32builder.Append(number));

		}
		std::shared_ptr<arrow::Array> i32array;
		PARQUET_THROW_NOT_OK(i32builder.Finish(&i32array));
		arrays.push_back(i32array);
    }

    return arrow::Table::Make(schema, arrays);
}

std::shared_ptr<arrow::Table> generate_str_table(int num_values, int nCols, int min_length, int max_length) {
	//Create the schema
	std::vector<std::shared_ptr<arrow::Field>> fields;
    for (int c = 0; c < nCols; c++) {
    	char name[NAMEBUFSIZE];
    	snprintf(name, NAMEBUFSIZE, "str%d", c);
    	fields.push_back(arrow::field(name, arrow::utf8(), false));
    }
    std::shared_ptr<arrow::Schema> schema = arrow::schema(fields);

    //Generate the values
    std::vector<std::shared_ptr<arrow::Array>> arrays;
    for (int c = 0; c < nCols; c++) {
		arrow::StringBuilder strbuilder;
		for (int i = 0; i < num_values; i++) {
			int length = rand() % (max_length - min_length + 1) + min_length;
			PARQUET_THROW_NOT_OK(strbuilder.Append(gen_random_string(length)));
		}
		std::shared_ptr<arrow::Array> strarray;
		PARQUET_THROW_NOT_OK(strbuilder.Finish(&strarray));
		arrays.push_back(strarray);
    }


    return arrow::Table::Make(schema, arrays);
}

void write_parquet(std::shared_ptr<arrow::Table> table, std::string name) {
	for (bool dict : { false, true }) {
		for (arrow::Compression::type comptype : {arrow::Compression::type::UNCOMPRESSED,
												  arrow::Compression::type::SNAPPY}) {
			std::shared_ptr<arrow::io::FileOutputStream> outfile;
			std::string filename = name
					+ (dict ? "_dict" : "")
					+ (comptype == arrow::Compression::type::SNAPPY ? "_snappy" : "")
					+ ".prq";
			arrow::io::FileOutputStream::Open(filename, false, &outfile);
			parquet::WriterProperties::Builder propbuilder = parquet::WriterProperties::Builder{};
			propbuilder.compression(comptype)->encoding(
					parquet::Encoding::type::PLAIN)->disable_statistics()->version(
					parquet::ParquetVersion::PARQUET_1_0);
			if (!dict) {
				propbuilder.disable_dictionary();
			}
			parquet::arrow::WriteTable(*table, arrow::default_memory_pool(), outfile,
					INT_MAX, propbuilder.build());
			outfile->Flush();
			outfile->Close();
		}
	}
}

int main(int argc, char **argv) {
	int nRows = 100;
	int nCols = 1;
	if (argc >= 2) {
		nRows = strtol(argv[1], 0, 10);
	}
	if (argc >= 3) {
		nCols = strtol(argv[2], 0, 10);
	}

	printf("Generating parquet files with %d rows, %d columns\n", nRows, nCols);

  std::shared_ptr<arrow::Table> test_int64rtable = generate_int64_table(nRows, nCols, false);
  std::shared_ptr<arrow::Table> test_int64stable = generate_int64_table(nRows, nCols, true);
  std::shared_ptr<arrow::Table> test_int32rtable = generate_int32_table(nRows, nCols, false);
  std::shared_ptr<arrow::Table> test_int32stable = generate_int32_table(nRows, nCols, true);
  std::shared_ptr<arrow::Table> test_strtable = generate_str_table(nRows, nCols, 2, 128);

  write_parquet(test_int64stable, "./test_int64s");
  write_parquet(test_int64rtable, "./test_int64r");
  write_parquet(test_int32stable, "./test_int32s");
  write_parquet(test_int32rtable, "./test_int32r");
  write_parquet(test_strtable, "./test_str");

  return 0;
}
