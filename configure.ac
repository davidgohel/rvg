# Process this file with autoconf to produce a configure script.
AC_INIT(Cairo, 1.3, david.gohel@ardata.fr)
AC_CONFIG_SRCDIR([src/rvg.cpp])
AC_CONFIG_HEADER([src/cconfig.h])

# find R home and set CC/CFLAGS
: ${R_HOME=`R RHOME`}
if test -z "${R_HOME}"; then
  echo "could not determine R_HOME"
  exit 1
fi
RBIN="${R_HOME}/bin/R"
CC=`"${RBIN}" CMD config CC`;
CFLAGS=`"${RBIN}" CMD config CFLAGS`
CPP=`"${RBIN}" CMD config CPP`;
AC_SUBST(R_HOME)

# platform check
#AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
#AC_HEADER_TIME

# Checks for library functions.
#AC_FUNC_MALLOC
#AC_TYPE_SIGNAL

AC_PATH_PROG([PKGCONF],[pkg-config],[],[$PATH:/usr/local/bin:ext/bin:ext:/sw/bin:/opt/bin])

have_cairo=no
if test "xx$CAIRO_LIBS" == "xx"; then
   if test "xx$PKGCONF" != "xx"; then
      AC_MSG_CHECKING([whether pkg-config knows about cairo])
      if "${PKGCONF}" --exists cairo; then
         AC_MSG_RESULT([yes])
	 AC_MSG_CHECKING([for configurable backends])
	 modlist="cairo"
	 for module in cairo-ft cairo-pdf cairo-png cairo-ps cairo-svn cairo-xlib cairo-xlib-xrender cairo-win32; do
	    if "${PKGCONF}" --exists ${module}; then
		modlist="${modlist} ${module}"
	    fi
	 done
	 AC_MSG_RESULT(${modlist})
	 CAIRO_CFLAGS=`"${PKGCONF}" --cflags ${modlist}`
	 CAIRO_LIBS=`"${PKGCONF}" --libs ${modlist}`
      else
	AC_MSG_RESULT([no])
      fi
   fi
fi
if test -z "${CAIRO_LIBS}"; then
   AC_MSG_NOTICE([CAIRO_LIBS is unset, attempting to guess it.])
   cpre=''
   for pre in /usr /usr/local /usr/X11 /usr/X11R6 /opt /sw; do
      if test -e "${pre}/include/cairo/cairo.h"; then
         cpre=${pre}; break
      fi
   done
   if test -n "${cpre}"; then
      CAIRO_CFLAGS="-I${cpre}/include/cairo"
      if test "${cpre}" = /usr; then
         CAIRO_LIBS="-lcairo"
      else
         CAIRO_LIBS="-L${cpre}/lib -lcairo"
      fi
   fi
fi

AC_MSG_NOTICE([CAIRO_CFLAGS=${CAIRO_CFLAGS}])


CPPFLAGS="$CPPFLAGS ${CAIRO_CFLAGS}"

AC_CHECK_HEADER(cairo.h,,AC_MSG_ERROR([Cannot find cairo.h! Please install cairo (http://www.cairographics.org/) and/or set CAIRO_CFLAGS/LIBS correspondingly.]))



LIBS="$LIBS ${CAIRO_LIBS}"

#AC_CHECK_LIB(cairo, cairo_create,,
#[AC_MSG_ERROR([Cannot link to cairo library. Please check config.log for details and set CAIRO_LIBS accordingly.])])

AC_MSG_CHECKING([whether Cairo programs can be compiled])
AC_LINK_IFELSE([
#include <cairo.h>
int main(void) {
  cairo_surface_t *cs = 0;
  cairo_t *cr;
  cr = cairo_create(cs);
  return 0;
}
],[AC_MSG_RESULT(yes)],[AC_MSG_ERROR([Cannot compile a simple Cairo program. See config.log for details.])])


dnl AC_CHECK_LIB(cairo, cairo_image_surface_get_format)
dnl if test "$ac_cv_lib_cairo_cairo_image_surface_get_format" != "yes"; then
dnl   AC_MSG_ERROR([Cairo is too old: >= 1.2 is needed.])
dnl fi
AC_CHECK_DECLS([cairo_image_surface_get_format], [#include <cairo.h>])



AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT
