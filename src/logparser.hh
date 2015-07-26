#ifndef LOGPARSER_HH_
# define LOGPARSER_HH_

# include <string>

struct LogEntry {
    uint16_t pc;    // program counter
    uint8_t opcode; // operation code
    uint8_t data[2];  // data after opcode
};

class LogParser {
public:
    LogParser();

    LogEntry* parse_line(std::string line);
};


#endif /* !LOGPARSER_HH_ */
