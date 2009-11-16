# /bin/sh

rm *.gcda
make check
rm -rf $HOME/.calendar/
rm -rf $HOME/.maesync/
run-standalone.sh ./caltest
xmlto --skip-validation -x report.xsl html Caltest.xml
lcov --directory . -c -o Caltest.info
genhtml -o coverage Caltest.info
mv Caltest.proc Caltest.html
