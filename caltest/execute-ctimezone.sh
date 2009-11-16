# /bin/sh

run-standalone.sh ./ctimezone
xmlto --skip-validation -x report.xsl html ctimezone.xml
mv ctimezone.proc ctimezone.html
