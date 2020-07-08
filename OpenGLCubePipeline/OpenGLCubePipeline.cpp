// OpenGLCubePipeline.cpp : Defines the entry point for the application.
//

#include "OpenGLCubePipeline.h"

using namespace std;

int main()
{
	auto pipeline = OpenGLPipelineService();
	pipeline.launch();
	return 0;
}
