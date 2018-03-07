#!/bin/awk -f
BEGIN {
	print "unsigned long sjis[] = {"
	for(i = 0; i < 32; i++)
		printf "	0x%x,\n", i
	print "	[0x7f] = 0x7f,"
}

!/^#/ && !/^$/ {
	printf "	[%s] = %s,\n", $1, $2
}

END {
	print "};"
}
