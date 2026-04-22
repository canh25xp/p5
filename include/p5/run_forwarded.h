#pragma once

#include <string>
#include <vector>

class P5;

void run_forwarded(P5 &p5, const char *command, const std::vector<std::string> &args);

void run_p4_passthrough(const char *command, const std::vector<std::string> &args);
