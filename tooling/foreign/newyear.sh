#! /bin/sh
#  Shell script to patch copyright statements in source code
#  To run, 'sh path-to-script/newyear.sh' in parent directory
#  Walks whole tree and patches all text files that match.
#
setdate() {
    for ENTRY in *; do
        if [ -d "$ENTRY" ]; then
            if [ ! "$ENTRY" = "." -a ! "$ENTRY" = ".." ]; then
                cd $ENTRY || exit
                setdate
                cd ..
            fi
        elif [ -f "$ENTRY" ]; then
            file $ENTRY | egrep -q "ASCII|XML"
            if [ $? -eq 0 ]; then
                egrep -q "Copyright.+-200.*iMatix" $ENTRY
                if [ $? -eq 0 ]; then
                    echo -n "$PWD/$ENTRY: "
                    perl -pi -w -e 'if (/Copyright.+-200.*iMatix/) { s/-200./-2009/; print STDERR $_; }' $ENTRY
                fi
            fi
        fi
    done
}

#   Restamp whole tree
setdate
