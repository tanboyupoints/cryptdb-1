Building CryptDB
    + These instructions were tested on Ubuntu 13.10

#########################################
#########################################
############### Get CrypDB ##############
#########################################
#########################################

    > sudo apt-get install git
    > git clone -b public git://g.csail.mit.edu/cryptdb

#########################################
#########################################
########## Install Dependencies #########
#########################################
#########################################

    > sudo apt-get install liblua5.1-0-dev libntl-dev libmysqlclient-dev \
                           libssl-dev libbsd-dev libevent-dev            \
                           libglib2.0-dev libgmp-dev mysql-server        \
                           libaio-dev automake gtk-doc-tools flex cmake  \
                           libncurses5-dev bison g++ bzr make
    > During the installation of mysql-server it may ask you to provide
      a password for the root user. For the purpose of this document we
      will use the password 'letmein' (without the single quotes).

#########################################
#########################################
######### Install MySQL Proxy ###########
#########################################
#########################################

    > bzr branch lp:mysql-proxy
    > cd mysql-proxy
    > automake --version
    > If your autotools version >= 1.12
        + Edit configure.in by following the instructions pertaining to
          de-ANSI-fication/ansi2knr at this URL: 
            * https://www.flameeyes.eu/autotools-mythbuster/forwardporting/automake.html
    > sh ./autogen.sh
    > ./configure --enable-maintainer-mode --with-lua=lua5.1
    > make
    > make install
    > If your build fails with an error about g_debug_hexdump being
      undefined, try applying the following patch and re-doing the above
      steps starting with 'sh ./autogen.sh'

    % patch -p0 <<"EOM"$
=== modified file 'src/Makefile.am'$
--- src/Makefile.am 2010-10-28 11:58:51 +0000$
+++ src/Makefile.am 2012-01-10 15:49:27 +0000$
@@ -23,7 +23,7 @@$ 
 $
 BUILD_CPPFLAGS = $(LUA_CFLAGS) $(MYSQL_CFLAGS) $(GLIB_CFLAGS) $(GMODULE_CFLAGS) $(GTHREAD_CFLAGS)$
 BUILD_CFLAGS   = -DLUAEXTDIR="\"$(luaextdir)\""  -DPLUGINDIR="\"$(plugindir)\""$
-BUILD_LDADD    = $(GLIB_LIBS) $(GMODULE_LIBS) libmysql-chassis.la $(GTHREAD_LIBS) libmysql-proxy.la$
+BUILD_LDADD    = $(GLIB_LIBS) $(GMODULE_LIBS) libmysql-chassis.la libmysql-chassis-glibext.la $(GTHREAD_LIBS) libmysql-proxy.la$
 $
 BUILT_SOURCES =$
 if USE_WRAPPER_SCRIPT$
EOM$

#########################################
#########################################
############## gcc Version ##############
#########################################
#########################################

    > gcc --version
    > If version < 4.6
        + apt-add-repository ppa:ubuntu-toolchain-r/test
        + apt-get update
        + apt-get install gcc-4.6 g++-4.6

#########################################
#########################################
####### Build MySQL version 5.5.14 ######
#########################################
#########################################

    > wget http://es.csail.mit.edu/mysql-5.5.14.tar.gz
    > tar zxf mysql-5.5.14.tar.gz
    > cp -R /path/to/cryptdb/parser/mysql_mods/* mysql-5.5.14/
    > rm mysql-5.5.14/sql/sql_yacc.{cc,h}
    > cd mysql-5.5.14
    > mkdir build
    > cd build
    > cmake -DWITH_EMBEDDED_SERVER=on ..
    > make
    > If you get compilation errors,
        + sql/log.h : Add using declarations to line 351 and 352.
        + sql/msqld.cc : Add reinterpret_cast<socklen_t *>(variable) to
          line 5267 and 5334. Where 'variable' is the appropriate
          identifier.

#########################################
#########################################
########### Building CryptDB ############
#########################################
#########################################

    > cd /path/to/cryptdb
    > cp conf/config.mk.sample conf/config.mk
    > Open conf/config.mk in your vimitor and set the MYSRC path.
    > make
    > sudo make install
    > mkdir /var/lib/shadow-mysql
    > sudo chown <account-running-mysql> /var/lib/shadow-mysql


Now go to doc/README for information on how to use CryptDB.
