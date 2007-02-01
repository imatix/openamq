@echo off
echo %OPENAMQ_LOGIN%>response
echo %OPENAMQ_PASSWD%>>response
echo cd doc>>response
echo prompt >>response
echo bin >>response
echo mput *.html>>response
echo mput *.jpg>>response
echo mput *.png>>response
echo mput *.css>>response
echo put concepts.pdf>>response
echo put protocol.pdf>>response
echo put server.pdf>>response
echo put concepts.txt>>response
echo put protocol.txt>>response
echo put server.txt>>response
echo mput *.html>>response
echo quit>>response
ftp -v -s:response www.openamq.org
del response


