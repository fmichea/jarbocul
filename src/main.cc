#include "graph/graph.hh"
#include "processors/gb_z80/cpu.hh"

int main(int argc, const char** argv) {
    (void) argc;
    (void) argv;

    Graph<GB_Z80>("../../cpu-dump.txt").generate_graph();

    return 0;
}
