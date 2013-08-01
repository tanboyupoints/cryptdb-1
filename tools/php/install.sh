#!/bin/bash
#
# CryptDB onion tool installer

#   Carlos Carvalho
#

apache2="/etc/apache2/sites-enabled/000-default"
declare -a files=(./index.php ./favicon.ico)

function checkenv() {

    # be faithful
    if [ $UID != 0 ] ; then
        echo "Root?"
        exit
    fi

    ps ax |grep apache2 >/dev/null 2>&1
    if [ $? -ne 0 ] ; then
        echo "* warning: apache2 is not running"
    else
        echo "apache2 running on pid(s): `pidof apache2`"
    fi
    php=$(ls /etc/apache2/mods-enabled/php*.load |cut -d "." -f1)
    if [ $? -ne 0 ] ; then
        echo "* warning: php disabled"
    else
        echo "`basename $php` seems to be enabled"
    fi
    
    # Installing...
    total=$(ls -l $1 |head -1| awk '{print $2}')
    if [ $total -gt 0 ] ; then

        # Even after backup, keep all data there, but files with
        # the same name will be overwritten.
        echo -n "[!] DocumentRoot directory $1 is not empty. Backup data? [Y/n]: "
        read opt

        if [ "$opt" != "n" ] ; then
            currdir=$(pwd)
            cd `dirname $1`
            currtime=$(date +"%m-%d-%Y_%H:%M")
            tar -vcjf $currdir/www-bkp-$currtime.tar.bz2 `basename $1`
            cd - >/dev/null
        fi
    fi

    a=0
    while : ; do 
        if [ -z "${files[$a]}" ] ; then
            break;
        fi

        cp -Rpv ${files[$a]} $1/
        a=$(expr $a + 1)
    done
    echo "Done."

}

function install
{
    if [ ! -f $apache2 ] ; then
        echo "Install script defined to use apache2"
        echo "Directory $apache2 not found."
        exit
    fi

    docroot=$(grep DocumentRoot $apache2 |awk '{print $2}')

    if [ -z $docroot ] ; then
        echo "DocumentRoot not found"
        exit
    fi

    checkenv $docroot
}

#do it
install


