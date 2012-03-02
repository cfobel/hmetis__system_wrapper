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
    typedef boost::format _;

    int vertex_count = 10;
    int edge_count = 10;
    /* Create random hypergraph to test hMetis partitioning on. */
    vector<vector<int> > graph = random_hypergraph(vertex_count, edge_count);
    /* get_partition(int vertex_count, vector<vector<int> > &hedges,
     *         int partitions_count, int balance)
     */
    /* Run hMetis on hypergraph, resulting in a list, where each entry in the
     * list is a list of hypergraph nodes belonging to the corresponding
     * partition.
     */
    vector<vector<int> > partition = HMetisWrapper::get_partition(
            vertex_count, graph, 4, 2);

    cout << "Partitioned sets:" << endl;
    for(int set_id = 0; set_id < partition.size(); set_id++) {
        vector<int> &vertices = partition[set_id];
        cout << _("  [%d] ") % set_id;
        copy(vertices.begin(), vertices.end(),
                ostream_iterator<int>(cout, ", "));
        cout << endl;
    }
    return 0;
}
