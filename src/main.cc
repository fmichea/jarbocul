#include "graph/graph.hh"
#include "processors/gb_z80/cpu.hh"
#include "writers/dotwriter.hh"

int main(int argc, const char** argv) {
    (void) argc;
    (void) argv;

    Graph<GB_Z80> graph("../cpu-dump.txt");
    graph.generate_graph();

    DotWriter<GB_Z80> writer;
    writer.output(graph);

    return 0;
}
