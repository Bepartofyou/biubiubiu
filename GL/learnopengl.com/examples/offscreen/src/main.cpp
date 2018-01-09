#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "glfwWin.h"
#include "cmdline.h"

int main(int argc, char** argv)
{
	struct gengetopt_args_info args_info;
	/* let's call our cmdline parser */
	if (cmdline_parser(argc, argv, &args_info) != 0)
		exit(1);

	CglfwWin hander(args_info.winwidth_arg, args_info.winheight_arg, args_info.backimg_arg, args_info.frontimg_arg);
	hander.Create();
	hander.Generate(args_info.outpath_arg, args_info.framerate_arg, args_info.duration_arg, args_info.direction_flag, args_info.start_arg);
	hander.Destory();

	return 0;
}