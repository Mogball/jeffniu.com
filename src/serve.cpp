#include "types.hpp"
#include "error.hpp"
#include <boost/filesystem.hpp>
#include <vector>
#include <fstream>

using namespace std;

static void readAndSend(const ResponsePtr &res, const shared_ptr<ifstream> &ifs) {
    static vector<char> buffer(1 << 17);
    streamsize readLength = ifs->read(
        buffer.data(),
        static_cast<streamsize>(buffer.size())
    ).gcount();
    if (readLength > 0) {
        res->write(buffer.data(), readLength);
        if (buffer.size() == readLength) {
            res->send([res, ifs](const errcode_t &ec) {
                if (!ec) {
                    readAndSend(res, ifs);
                } else {
                    cerr << "Connection interrupted" << endl;
                }
            });
        }
    }
}

Expected<bool> serveResource(const ResponsePtr &res, string resPath) {
    Header header;
    auto ifs = make_shared<ifstream>();
    ifs->open(resPath, ifstream::in | ios::binary | ios::ate);
    if (!*ifs) {
        return {"could not read file: " + resPath};
    }
    auto len = ifs->tellg();
    ifs->seekg(0, ios::beg);
    header.emplace("Content-Length", to_string(len));
    res->write(header);
    readAndSend(res, ifs);
    return true;
}
