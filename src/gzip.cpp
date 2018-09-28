#include "gzip.hpp"
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <string>
#include <sstream>

using namespace std;
using namespace boost::iostreams;

string compress(const string &src) {
    stringstream compressed;
    stringstream origin(src);
    filtering_streambuf<input> out;
    out.push(gzip_compressor());
    out.push(origin);
    copy(out, compressed);
    return compressed.str();
}

string decompress(const string &src) {
    stringstream decompress;
    stringstream origin(src);
    filtering_streambuf<input> out;
    out.push(gzip_decompressor());
    out.push(origin);
    copy(out, decompress);
    return decompress.str();
}
