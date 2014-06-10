#! /bin/sh
#
#   Gets documentation from Xitami wiki, for packaging
#
wget -nv http://xitami.wikidot.com/doc:xitami-5 -O webpages/default.html
wget -nv http://xitami.wikidot.com/local--theme/custom/style.css -O webpages/default.css
perl -pi -w -e 's/\/local--theme\/custom\/style.css/default.css/' webpages/default.html