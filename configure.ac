AC_INIT([simkit],[0.1])
AC_CONFIG_SRCDIR([src/clocking_test.cc])
AM_INIT_AUTOMAKE([subdir-objects])

#Autoconf will make CXXFLAGS/CFLAGS "-g -O2" if no user-specified CXXFLAGS/CFLAGS are available.
#This is a decent default for development, but we want at least an option with all optis and no debug symbols.
#The below lines explicitly set flags to blank if the user hasn't specified any.
#The badness happens in the AC_PROG_CXX and AC_PROG_CC macros, so the explicit blanking has to be done before them.
test ".$CFLAGS" = "." && CFLAGS=""
test ".$CXXFLAGS" = "." && CXXFLAGS=""

AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB
AC_CONFIG_FILES([Makefile])

#config.h
#AC_CONFIG_HEADERS([config.h])

#Arguments
#AC_ARG_ENABLE([couchdb],
#              AS_HELP_STRING([--enable-couchdb], [Enable CouchDB support]))

#AM_CONDITIONAL(ENABLE_COUCHDB, test x$enable_couchdb = xyes)

#if test "x$enable_couchdb" = "xyes"
#then
#        AC_SUBST(COUCHDB_LIBS, ["-luuid -L/usr/local/lib -lcurlpp -lstdc++ -lcurl -lgssapi_krb5"])
#	AC_DEFINE([ENABLE_COUCHDB], [1], [Define if CouchDB support is enabled.])
#fi

AC_OUTPUT
