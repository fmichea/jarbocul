#include "graph/graph.hh"

int main(int argc, const char** argv) {
    (void) argc;
    (void) argv;

    Graph("../../cpu-dump.txt").generate_graph();

    return 0;
}
