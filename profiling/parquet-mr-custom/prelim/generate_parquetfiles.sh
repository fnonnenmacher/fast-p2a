#!/bin/bash

# Generate parquet files with
# DataTypes int32, int64, str
# Encodings plain & delta for ints, deltalen for str
# Varying page sizes at 1 GB total data size

outdir=./parquetfiles
fulldatasize=$((10**8)) #1 GBytes of data
parquetwriter=~/workspaces/openCAPI/fast-p2a/software/cpp/test/parquetwriter_test

rm *.prq
mkdir $outdir

for datatype in int32 int64 str; do
	if [ "$datatype" == "int32" ]; then
		fullsize_entries=$((fulldatasize/4))
	elif [ "$datatype" == "int64" ]; then
		fullsize_entries=$((fulldatasize/8))
	else
		fullsize_entries=$((fulldatasize/64))
	fi
	$parquetwriter $fullsize_entries 1
	for exp in $(seq 3 8); do
		size_bytes=$((10**exp))
		echo "Generating $datatype files of size $fulldatasize bytes and pagesize $size_bytes bytes ($size_entries entries)"
		echo "./run.sh test_${datatype}.prq test_${datatype}_ps${size_bytes}_plain.prq $size_bytes plain"
		echo "./run.sh test_${datatype}.prq test_${datatype}_ps${size_bytes}_delta.prq $size_bytes delta"
		./run.sh test_${datatype}.prq test_${datatype}_ps${size_bytes}_plain.prq $size_bytes plain &
		./run.sh test_${datatype}.prq test_${datatype}_ps${size_bytes}_delta.prq $size_bytes delta
		mv test_${datatype}_ps${size_bytes}_plain.prq $outdir
		mv test_${datatype}_ps${size_bytes}_delta.prq $outdir
	done
done



