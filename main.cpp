#define STRIP_FLAG_HELP 1    // this must go before the #include!
#include <gflags/gflags.h>
#include "bloom_filter.h"

DEFINE_int32(bits_num, 0, "number of bits");
DEFINE_int32(hash_num, 0, "number of hash fuctions to use");

int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    if (FLAGS_bits_num <= 0 || FLAGS_hash_num <= 0) {
        LOG(ERROR) << "both bits_num and hash_num should be positive integer";
        exit(-1);
    }
    LOG(INFO) << "bits_num:\t" << FLAGS_bits_num;
    LOG(INFO) << "hash_num:\t" << FLAGS_hash_num;


    return 0;
}
