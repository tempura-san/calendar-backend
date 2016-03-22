#!/bin/sh

# will be generated when the test suite is executed, clear from previous run
rm -f *.gcda

# generate test suite files
make check

rm -rf $HOME/.calendar/
rm -rf $HOME/.maesync/

# execute test suite
run-standalone.sh ./caltest

# generate test report
xmlto --skip-validation -x report.xsl html Caltest.xml
mv Caltest.proc Caltest.html

# generate test/code coverage report - indicated by GCNO/GCDA file presence
if [ -f CaltestMain.gcno -a -f CaltestMain.gcda ]
then
	lcov --directory . -c -o Caltest.info
	genhtml -o coverage Caltest.info
else
	echo "LCOV generation skipped - GCNO/GCDA files missing."
	echo "Maybe CCACHE is interfering? Recompile with CCACHE_DISABLE=1" 
fi
