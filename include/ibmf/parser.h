#ifndef IBMF_PARSER_H
#define IBMF_PARSER_H

#include <string>

#include "ibmf/stream-reader.h"
#include "ibmf/box.h"

namespace IBMF
{

int ParseFile(StreamReader& stream, std::vector<Box>& boxes, std::string& errorMsg);

#if defined(ENABLE_TESTS)
int StreamToHostUnitTests();
#endif

};  // namespace IBMF

#endif // IBMF_PARSER_H
