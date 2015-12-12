#include <mutex>
#include <vector>
#include <thread>
#include "gflags/gflags.h"
#include "../bloom_filter.h"

class Builder {
public:
    Builder(string path, string dbpath, int num_threads, size_t n_bits, size_t n_hashes) :
        _num_threads {num_threads},
        _db{n_bits, n_hashes}

    {
        _file.open(path.c_str());
        CHECK(_file.is_open()) << "path " << path << " can not be opened";

    }
    void start() {
        size_t lineno {0};
        auto task = [this, &lineno] {
            LOG(WARNING) << "start thread " << std::this_thread::get_id();
            string line;
            while (true) {
                if (lineno++ % 10000 == 0) LOG(INFO) << "trained\t" << lineno << "\tlines";
                { std::lock_guard<std::mutex> lk(_read_mut);
                if (_file.eof()) break;
                getline(_file, line);
                }
                _db.set(line.c_str());
            }
            LOG(WARNING) << "thread " << std::this_thread::get_id() << " exit!";
        };
        vector<std::thread> threads;
        for (int i = 0; i < _num_threads; i++) {
            std::thread t(task);
            threads.push_back(std::move(t));
        }
        for (auto& t: threads) {
            t.join();
        }
    }
    BFdb &db() {
        return _db;
    }

private:
    std::mutex _read_mut;
    ifstream _file;
    int _num_threads { 0 };
    BFdb _db;
    
};

DEFINE_int32(thread_num, 1, "number of thread");
DEFINE_int64(bits_num, 0, "number of bits");
DEFINE_int32(hash_num, 0, "number of hash fuctions to use");
DEFINE_string(path, "", "path of the file");
DEFINE_string(dbpath, "", "path of the database");

int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    if (FLAGS_bits_num <= 0 || FLAGS_hash_num <= 0) {
        LOG(ERROR) << "both bits_num and hash_num should be positive integer";
        exit(-1);
    }
    if (FLAGS_path.empty() || FLAGS_dbpath.empty()) {
        LOG(ERROR) << "either path or dbpath is empty";
        exit(-1);
    }
    LOG(INFO) << "bits_num:\t" << FLAGS_bits_num;
    LOG(INFO) << "hash_num:\t" << FLAGS_hash_num;
    LOG(INFO) << "path:\t" << FLAGS_path;
    LOG(INFO) << "dbpath:\t" << FLAGS_dbpath;

    Builder builder(FLAGS_path, FLAGS_dbpath, FLAGS_thread_num, FLAGS_bits_num, FLAGS_hash_num);
    LOG(WARNING) << "start works";
    builder.start();
    LOG(WARNING) << "saving database to " << FLAGS_dbpath;
    builder.db().save(FLAGS_dbpath, true);
    LOG(WARNING) << "finish task!";

    return 0;
}
