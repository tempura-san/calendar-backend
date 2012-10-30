AC_PREREQ([2.59])

#Package name
AC_INIT(calendar-backend, CALENDAR_BACKEND_VERSION, 0, 0, 5, 0, WERROR="no", WERROR="yes")
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)

AM_PROG_LIBTOOL
AM_CONFIG_HEADER(config.h)
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CXX
AM_PROG_AS
AC_SUBST(ERROR_CFLAGS)
AC_HEADER_STDC([])
AC_C_INLINE

pkg_modules="sqlite3 libical alarm dbus-1 "

PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)
AC_DEFINE(NO_DEBUG_MESSAGES_PLEASE,1,[always defined])

dnl checking check
#AM_PATH_CHECK(,[have_check="yes"],
#  AC_MSG_WARN([Check not found; cannot run unit tests!])
#  [have_check="no"])
#AM_CONDITIONAL(HAVE_CHECK, test x"$have_check" = "xyes")

#Generate Makefile 

AC_OUTPUT( calendar-backend.pc
Makefile \
src/Makefile \
caltest/Makefile \
tools/Makefile \
docs/Makefile \
include/Makefile 
)

