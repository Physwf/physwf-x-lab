#include "api.h"

static void usage(const char *msg = nullptr) {
	if (msg)
		fprintf(stderr, "pbrt: %s\n\n", msg);

	fprintf(stderr, R"(usage: pbrt [<options>] <filename.pbrt...>
Rendering options:
  --cropwindow <x0,x1,y0,y1> Specify an image crop window.
  --help               Print this help text.
  --nthreads <num>     Use specified number of threads for rendering.
  --outfile <filename> Write the final image to the given filename.
  --quick              Automatically reduce a number of quality settings to
                       render more quickly.
  --quiet              Suppress all text output other than error messages.

Logging options:
  --logdir <dir>       Specify directory that log files should be written to.
                       Default: system temp directory (e.g. $TMPDIR or /tmp).
  --logtostderr        Print all logging messages to stderr.
  --minloglevel <num>  Log messages at or above this level (0 -> INFO,
                       1 -> WARNING, 2 -> ERROR, 3-> FATAL). Default: 0.
  --v <verbosity>      Set VLOG verbosity.

Reformatting options:
  --cat                Print a reformatted version of the input file(s) to
                       standard output. Does not render an image.
  --toply              Print a reformatted version of the input file(s) to
                       standard output and convert all triangle meshes to
                       PLY files. Does not render an image.
)");
	exit(msg ? 1 : 0);
}

int main(int argc, char** argv)
{
	Options options;
	std::vector<std::string> filenames;
	// Process command-line arguments
	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "--nthreads") || !strcmp(argv[i], "-nthreads")) {
			if (i + 1 == argc)
				usage("missing value after --nthreads argument");
			options.nThreads = atoi(argv[++i]);
		}
		else if (!strncmp(argv[i], "--nthreads=", 11)) {
			options.nThreads = atoi(&argv[i][11]);
		}
		else if (!strcmp(argv[i], "--outfile") || !strcmp(argv[i], "-outfile")) {
			if (i + 1 == argc)
				usage("missing value after --outfile argument");
			options.imageFile = argv[++i];
		}
		else if (!strcmp(argv[i], "--cropwindow") || !strcmp(argv[i], "-cropwindow")) {
			if (i + 4 >= argc)
				usage("missing value after --cropwindow argument");
			options.cropWindow[0][0] = atof(argv[++i]);
			options.cropWindow[0][1] = atof(argv[++i]);
			options.cropWindow[1][0] = atof(argv[++i]);
			options.cropWindow[1][1] = atof(argv[++i]);
		}
		else if (!strncmp(argv[i], "--outfile=", 10)) {
			options.imageFile = &argv[i][10];
		}
		else if (!strcmp(argv[i], "--logdir") || !strcmp(argv[i], "-logdir")) {
			if (i + 1 == argc)
				usage("missing value after --logdir argument");
			//FLAGS_log_dir = argv[++i];
		}
		else if (!strncmp(argv[i], "--logdir=", 9)) {
			//FLAGS_log_dir = &argv[i][9];
		}
		else if (!strcmp(argv[i], "--minloglevel") ||
			!strcmp(argv[i], "-minloglevel")) {
			if (i + 1 == argc)
				usage("missing value after --minloglevel argument");
			//FLAGS_minloglevel = atoi(argv[++i]);
		}
		else if (!strncmp(argv[i], "--minloglevel=", 14)) {
			//FLAGS_minloglevel = atoi(&argv[i][14]);
		}
		else if (!strcmp(argv[i], "--quick") || !strcmp(argv[i], "-quick")) {
			options.quickRender = true;
		}
		else if (!strcmp(argv[i], "--quiet") || !strcmp(argv[i], "-quiet")) {
			options.quiet = true;
		}
		else if (!strcmp(argv[i], "--cat") || !strcmp(argv[i], "-cat")) {
			options.cat = true;
		}
		else if (!strcmp(argv[i], "--toply") || !strcmp(argv[i], "-toply")) {
			options.toPly = true;
		}
		else if (!strcmp(argv[i], "--v") || !strcmp(argv[i], "-v")) {
			if (i + 1 == argc)
				usage("missing value after --v argument");
			//FLAGS_v = atoi(argv[++i]);
		}
		else if (!strncmp(argv[i], "--v=", 4)) {
			//FLAGS_v = atoi(argv[i] + 4);
		}
		else if (!strcmp(argv[i], "--logtostderr")) {
			//FLAGS_logtostderr = true;
		}
		else if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-help") ||
			!strcmp(argv[i], "-h")) {
			usage();
			return 0;
		}
		else
			filenames.push_back(argv[i]);
	}
	pbrtInit(options);
	// Process scene description
	if (filenames.empty()) {
		// Parse scene from standard input
		pbrtParseFile("-");
	}
	else {
		// Parse scene from input files
		for (const std::string &f : filenames)
			pbrtParseFile(f);
	}
	pbrtCleanup();
	return 0;
}