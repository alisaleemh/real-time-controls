#!/bin/csh -f
foreach i ($argv)
	echo Compiling $i
	gcc -D_REENTRANT -D__USE_POSIX199309 -D_GNU_SOURCE -o `basename $i '.c'` $i -lrt -lpthread -lm
end
echo Done.
