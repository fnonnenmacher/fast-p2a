#!/bin/python3

import os
import pyarrow.parquet
import pyarrow.parquet as pq
import numpy as np
import pandas as pd
import pyarrow as pa
import time

df = pd.DataFrame({"str": ["Apple", "thing", "boat", "house", "Guy", "art", "home", "later"]})
table = pa.Table.from_pandas(df)
pq.write_table(table, 'str.parquet')

#force writing the file
os.fsync

ding = pq.read_table('str.parquet')
sink = pa.BufferOutputStream()
writer = pa.RecordBatchFileWriter(sink, ding.schema)
writer.write_batch(ding.to_batches()[0])
writer.close()
buf = sink.getvalue()
file = open("str.arrow", "wb")
file.write(buf)
file.flush()
file.close()

