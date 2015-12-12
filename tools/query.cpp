#include <fstream>
#include "gflags/gflags.h"
#include "../bloom_filter.h"

class Query {
public:
    Query(const string& dbpath) {
        LOG(WARNING) << "init db from " << dbpath;
        _db.load(dbpath);
        LOG(WARNING) << "finish db from " << dbpath;
    }

    bool query(const string& key) {
        return _db.test(key.c_str());
    }

private:
    BFdb _db;
};

DEFINE_string(dbpath, "", "path of the database");
int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);
    Query query(FLAGS_dbpath);
    string line; 

    while(getline(cin, line)) {
        LOG(INFO) << "query\t" << line << "\tget\t" << query.query(line);
    }

    return 0;
}
