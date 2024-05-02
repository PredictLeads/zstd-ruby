
# sample data
FROM http://www.cl.ecei.tohoku.ac.jp/~matsuda/LRE_corpus/

# usage

```
bundle exec ruby compress.rb city.json
bundle exec ruby decompress.rb city.json
```


# Result
## 2024/03/29
https://github.com/SpringMT/zstd-ruby/commit/53ab279a0db4125dfdc646f638a81f2625c720b5

```
% system_profiler SPHardwareDataType
Hardware:

    Hardware Overview:

      Model Name: MacBook Pro
      Model Identifier: Mac14,7
      Chip: Apple M2
      Total Number of Cores: 8 (4 performance and 4 efficiency)
      Memory: 24 GB
      System Firmware Version: 8422.100.650
      OS Loader Version: 7459.141.1
      Activation Lock Status: Disabled
```
### Compression
```
% bundle exec ruby compress.rb city.json
ruby 3.3.0 (2023-12-25 revision 5124f9ac75) [arm64-darwin21]
Warming up --------------------------------------
              snappy    12.000 i/100ms
                gzip     5.000 i/100ms
                  xz     1.000 i/100ms
                 lz4    43.000 i/100ms
                zstd    34.000 i/100ms
Calculating -------------------------------------
              snappy    124.315 (± 3.2%) i/s -    624.000 in   5.024270s
                gzip     51.185 (± 5.9%) i/s -    255.000 in   5.000801s
                  xz      3.358 (± 0.0%) i/s -     17.000 in   5.074521s
                 lz4    443.950 (± 4.1%) i/s -      2.236k in   5.045395s
                zstd    352.064 (± 6.5%) i/s -      1.768k in   5.044874s

```
#### Data Size
##### before
```
% ls -alh samples/city.json
-rw-r--r--  1 springmt  staff   1.7M  9  3  2022 samples/city.json
```
##### after
```
% ls -alh results
total 2816
drwxr-xr-x   7 springmt  staff   224B  4  9  2023 .
drwxr-xr-x  13 springmt  staff   416B  4  9  2023 ..
-rw-r--r--   1 springmt  staff   219K  3 29 14:46 city.json.gzip
-rw-r--r--   1 springmt  staff   384K  3 29 14:46 city.json.lz4
-rw-r--r--   1 springmt  staff   358K  3 29 14:46 city.json.snappy
-rw-r--r--   1 springmt  staff   166K  3 29 14:46 city.json.xz
-rw-r--r--   1 springmt  staff   225K  3 29 14:46 city.json.zstd
```
### Decompression
```
% bundle exec ruby decompress.rb city.json
ruby 3.3.0 (2023-12-25 revision 5124f9ac75) [arm64-darwin21]
Warming up --------------------------------------
              snappy    45.000 i/100ms
                gzip    44.000 i/100ms
                  xz     7.000 i/100ms
                 lz4   121.000 i/100ms
                zstd    95.000 i/100ms
Calculating -------------------------------------
              snappy    454.245 (± 3.1%) i/s -      2.295k in   5.057263s
                gzip    440.680 (± 3.4%) i/s -      2.244k in   5.098470s
                  xz     77.378 (± 2.6%) i/s -    392.000 in   5.070707s
                 lz4      1.224k (± 5.1%) i/s -      6.171k in   5.056241s
                zstd    932.464 (± 3.2%) i/s -      4.750k in   5.099694s
```

## 2022/06/20
https://github.com/SpringMT/zstd-ruby/commit/77a9e704747ad24761bcc8194884270f983538c1
```
MacBook Pro (13-inch, 2020, Four Thunderbolt 3 ports)
2 GHz クアッドコアIntel Core i5
16 GB 3733 MHz LPDDR4X
```
### Compression
```
% bundle exec ruby compress.rb city.json
Warming up --------------------------------------
              snappy    40.000  i/100ms
                gzip     3.000  i/100ms
                  xz     1.000  i/100ms
                 lz4    29.000  i/100ms
                zstd    18.000  i/100ms
Calculating -------------------------------------
              snappy    402.381  (± 8.2%) i/s -      2.040k in   5.112554s
                gzip     32.348  (±12.4%) i/s -    162.000  in   5.109498s
                  xz      1.777  (± 0.0%) i/s -      9.000  in   5.116790s
                 lz4    291.582  (±12.3%) i/s -      1.450k in   5.076016s
                zstd    174.981  (±12.6%) i/s -    864.000  in   5.032567s
```
#### Data Size
##### before
```
% ls -alh samples/city.json
-rw-r--r--  1 springmt  staff   1.7M 12 27 15:42 samples/city.json
```
##### after
```
% ls -alh results
total 2784
drwxr-xr-x   7 springmt  staff   224B 12 27 15:42 .
drwxr-xr-x  11 springmt  staff   352B 12 27 15:42 ..
-rw-r--r--   1 springmt  staff   219K  6 20 10:06 city.json.gzip
-rw-r--r--   1 springmt  staff   365K  6 20 10:06 city.json.lz4
-rw-r--r--   1 springmt  staff   358K  6 20 10:06 city.json.snappy
-rw-r--r--   1 springmt  staff   166K  6 20 10:06 city.json.xz
-rw-r--r--   1 springmt  staff   225K  6 20 10:06 city.json.zstd
```
### Decompression
```
% bundle exec ruby decompress.rb city.json
Warming up --------------------------------------
              snappy    85.000  i/100ms
                gzip    20.000  i/100ms
                  xz     4.000  i/100ms
                 lz4    70.000  i/100ms
                zstd    54.000  i/100ms
Calculating -------------------------------------
              snappy    862.639  (± 2.6%) i/s -      4.335k in   5.028667s
                gzip    207.820  (± 6.7%) i/s -      1.040k in   5.026189s
                  xz     41.649  (± 4.8%) i/s -    208.000  in   5.004779s
                 lz4    680.900  (± 3.5%) i/s -      3.430k in   5.043917s
                zstd    541.291  (± 7.4%) i/s -      2.700k in   5.037102s
```

## 2017/02/16
https://github.com/SpringMT/zstd-ruby/commit/0ca4b99e4ffaf18b39b2cdb102b5b5bc31a18071
### Compression
```
% bundle exec ruby compress.rb city.json
Warming up --------------------------------------
              snappy    18.000  i/100ms
                gzip     2.000  i/100ms
                  xz     1.000  i/100ms
                 lz4    24.000  i/100ms
                zstd    17.000  i/100ms
Calculating -------------------------------------
              snappy    189.588  (±16.9%) i/s -    918.000  in   5.072385s
                gzip     27.703  (± 7.2%) i/s -    138.000  in   5.032686s
                  xz      1.621  (± 0.0%) i/s -      9.000  in   5.560271s
                 lz4    282.316  (±14.5%) i/s -      1.368k in   5.008697s
                zstd    195.722  (±14.3%) i/s -    952.000  in   5.027488s
```

#### Data Size
##### before

```
% ls -alh samples/city.json
-rw-r--r--@ 1 makoto  staff   1.7M  2  5 16:07 samples/city.json
```

##### after

```
% ls -alh results
total 2712
-rw-r--r--   1 makoto  staff   219K  2  5 16:08 city.json.gzip
-rw-r--r--   1 makoto  staff   365K  2  5 16:08 city.json.lz4
-rw-r--r--   1 makoto  staff   358K  2  5 16:08 city.json.snappy
-rw-r--r--   1 makoto  staff   166K  2  5 16:08 city.json.xz
-rw-r--r--   1 makoto  staff   238K  2  5 16:08 city.json.zstd
```

### Decompression

```
Warming up --------------------------------------
              snappy    59.000  i/100ms
                gzip    19.000  i/100ms
                  xz     5.000  i/100ms
                 lz4    51.000  i/100ms
                zstd    31.000  i/100ms
Calculating -------------------------------------
              snappy    583.211  (± 9.1%) i/s -      2.891k in   5.002226s
                gzip    195.468  (±10.7%) i/s -    969.000  in   5.028082s
                  xz     53.501  (± 7.5%) i/s -    270.000  in   5.083982s
                 lz4    511.275  (± 9.4%) i/s -      2.550k in   5.036539s
                zstd    302.455  (±16.5%) i/s -      1.488k in   5.070354s
```

## YYYY/MM/DD
### Compression
#### Data Size
##### before
##### after
### Decompression
