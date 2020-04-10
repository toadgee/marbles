#include "precomp.h"
#include "ConsoleUtilities.h"

std::string ConsoleGetInput(const char *input)
{
	fprintf(stdout, "%s: ", input);
	
	std::string lineString;
	std::getline(std::cin, lineString);
	
	return lineString;
}

int ConsoleGetInputNumber(const char *message, int invalidNumber, int minimumValue, int maximumValue)
{
	std::string result = ConsoleGetInput(message);
	int val = invalidNumber;
#ifdef WIN32
	int scanned = sscanf_s(result.c_str(), "%d", &val);
#else
	int scanned = sscanf(result.c_str(), "%d", &val);
#endif
	if (scanned != 1)
	{
		return invalidNumber;
	}
	
	if (val < minimumValue) return invalidNumber;
	if (val > maximumValue) return invalidNumber;
	return val;
}
