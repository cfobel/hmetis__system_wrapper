#ifndef ___H_METIS_WRAPPER__HPP___
#define ___H_METIS_WRAPPER__HPP___

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <boost/format.hpp>
#include <boost/array.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "hmetis__parser/PartitionReader.hpp"
#include "hmetis__parser/HMetisResultParser.hpp"

namespace hmetis {

using namespace std;
typedef boost::format _;

string get_stdout_from_command(string cmd) {
    stringstream data;
    FILE *stream;
    int MAX_BUFFER = 256;
    char buffer[MAX_BUFFER];
    cmd.append(" 2>&1");
    stream = popen(cmd.c_str(), "r");
    if (!stream){
        exit(1);
    }
    while (!feof(stream)){
        if (fgets(buffer, MAX_BUFFER, stream) != NULL){
            data << buffer;
        }
    }
    pclose(stream);
    return data.str();
}


vector<vector<int> > random_hypergraph(int vertex_count,
        int hyperedge_count) {
    vector<vector<int> > hedges;

    for(int hedge_id = 0; hedge_id < hyperedge_count; hedge_id++) {
        vector<int> vertex_ids;
        int count = (rand() % 7) + 2;
        for(int i = 0; i < count; i++) {
            vertex_ids.push_back(1 + rand() % vertex_count);
        }
        hedges.push_back(vertex_ids);
    }
    return hedges;
}


string gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    string s(len, ' ');
    for(int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return s;
}


class HMetisWrapper {
public:
    static vector<vector<int> > get_partition(int vertex_count,
            vector<vector<int> > &hedges,
            int partitions_count,
            int balance) {
        namespace fs = boost::filesystem;
        using boost::format;

        /* Write graph in hMetis hypergraph input file format to temp file. */
        fs::path graph_path(tmpnam(NULL));
        fs::path parent = graph_path.parent_path();
        fs::path partition_path = graph_path.string()
            + (format(".part.%d") % partitions_count).str();
        fs::ofstream graph_stream(graph_path);
        write_hmetis_graph(graph_stream, vertex_count, hedges);
        graph_stream.close();

        cout << _("graph_path: %s") % graph_path << endl;
        cout << _("partition_path: %s") % partition_path << endl;

        /* Run hmetis on the hypergraph temp file. */
        /*
         * Usage: shmetis <GraphFile> <Nparts> <UBfactor>
         *        or
         *        shmetis <GraphFile> <FixFile> <Nparts> <UBfactor>
         * Where:
         *  GraphFile: name of the hypergraph file
         *  FixFile  : name of the file containing pre-assignment of vertices to partitions
         *  Nparts   : number of partitions desired
         *  UBfactor : balance between the partitions (e.g., use 5 for a 45-55 bisection balance)
         */
        cout << get_stdout_from_command((format("shmetis %s %d %d")
                % graph_path % partitions_count % balance).str()) << endl;

        /* Parse hmetis partition output file into vector<vector<int> > */
        fs::ifstream partition_stream(partition_path);
        PartitionReader pr(partition_stream);
        PartitionReader::partition_t p = pr.get_partition();

        /* Clean-up temp files. */
        fs::remove(graph_path);
        fs::remove(partition_path);

        /* Return partition vector. */
        return p;
    }
    
    static void write_hmetis_graph(ostream &output,
            int vertex_count, vector<vector<int> > const &graph) {
        output << graph.size() << " " << vertex_count << endl;
        for(int edge_id = 0; edge_id < graph.size(); edge_id++) {
            vector<int> const &vertices = graph[edge_id];
            if(vertices.size() <= 0) {
                throw std::runtime_error((_("Empty edge: %d")
                        % edge_id).str());
            }
            output << vertices[0];
            for(int vertex_id = 1; vertex_id < vertices.size(); vertex_id++) {
                output << " " << vertices[vertex_id];
            }
            output << endl;
        }
    }

};

}

#endif
