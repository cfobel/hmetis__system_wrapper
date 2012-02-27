#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <boost/format.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#include "hmetis__parser/PartitionReader.hpp"
#include "HMetisWrapper.hpp"

using namespace std;

int main(int argc, char **argv) {
    using namespace hmetis;

    int vertex_count = 10;
    int edge_count = 10;
    vector<vector<int> > graph = random_hypergraph(vertex_count, edge_count);
    vector<vector<int> > partition = HMetisWrapper::get_partition(
            vertex_count, graph, 10, 2);
    HMetisWrapper::write_hmetis_graph(cout, vertex_count, graph);
    
    cout << get_stdout_from_command("shmetis --help") << endl;
    exit(0);

    using boost::format;
    if(argc != 2) {
        cerr << format("usage: %s <hmetis_output_path>")
            % argv[0] << endl;
        exit(-1);
    }
    string input_path(argv[1]);
    ifstream input(input_path.c_str());
    PartitionReader pr(input);
    PartitionReader::partition_t p = pr.get_partition();

    for(int set_id = 0; set_id < p.size(); set_id++) {
        PartitionReader::vertex_list_t &vertices = p[set_id];
        cout << boost::format("[%d] ") % set_id;
        copy(vertices.begin(), vertices.end(),
                ostream_iterator<int>(cout, ", "));
        cout << endl;
    }
    return 0;
}
