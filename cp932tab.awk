#!/bin/awk -f

BEGIN {
	print "Rune cp932tab[] = {"
}

!/^#/ && !/^$/ {
	if($2 ~ /^[ 	]*$/)
		$2 = "0xfffd"

	printf "	[%s] = %s,\n", $1, $2
}

END {
	print "	[0xFCFF] = 0,"
	print "};"
}
