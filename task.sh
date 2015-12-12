#path=~/yanchunwei/retr_experiment/extract_high_pv_query/extract_common_word_between_query_bidword/all_bws
set -e -x
#path=../data/prefix.txt.ts
path=prefix.final.clean
./bin/build -bits_num 12460575891 -dbpath 2.bf.db -hash_num 7 -path $path -thread_num 10
