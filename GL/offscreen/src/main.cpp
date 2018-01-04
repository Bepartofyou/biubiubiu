#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "offscreen.h"
#include "cmdline.h"

int main(int argc, char** argv)
{

	struct gengetopt_args_info args_info;
	/* let's call our cmdline parser */
	if (cmdline_parser(argc, argv, &args_info) != 0)
		exit(1);

	CoffScreen handle(args_info.backimg_arg, args_info.frontimg_arg);
	handle.InitWin(args_info.winwidth_arg, args_info.winheight_arg);
	handle.GenPic(args_info.outpath_arg, args_info.framerate_arg, args_info.duration_arg, args_info.direction_flag, args_info.start_arg);
	handle.DestoryWin();

	return 1;
}
