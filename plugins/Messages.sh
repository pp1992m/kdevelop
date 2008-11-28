#! /usr/bin/env bash
for plugin in `find -maxdepth 1 -type d -not -name . -not -name .svn -printf "%f\n"` ; do

    rcfiles="`find $plugin -name \*.rc`"
    uifiles="`find $plugin -name \*.ui`"
    if [[ "$rcfiles" != "" ]] ; then
        $EXTRACTRC $rcfiles >> rc.cpp || exit 11
    fi
    if [[ "$uifiles" != "" ]] ; then
        $EXTRACTRC $uifiles >> rc.cpp || exit 12
    fi
    if [ -f rc.cpp ] ; then
        $XGETTEXT -kaliasLocal `find $plugin -name \*.cc -o -name \*.cpp -o -name \*.h` rc.cpp -o $podir/kdev${plugin}.pot
        rm -f rc.cpp
    else
        $XGETTEXT -kaliasLocal `find $plugin -name \*.cc -o -name \*.cpp -o -name \*.h` -o $podir/kdev${plugin}.pot
    fi
done
