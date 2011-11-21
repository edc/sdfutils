#How to use

	sdfcut input.sdf 100

This will read in input.sdf and generate a series of output file each with 100
molecules (or less for the last one).

You can also read from stdin:

	gzcat mysdf.gz | sdfcut - 1000


