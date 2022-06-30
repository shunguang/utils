#include "libUtil/DataTypes.h"
#include "libUtil/UtilFuncs.h"

int test_mat(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	//const std::string logFilename = "./log-" + pnt::getPrettyTimeStamp().substr(0, 20) + ".txt";
	//app::startLogThread(logFilename, true);

	test_mat(argc, argv);

	//app::endLogThread();
	return 0;
}




