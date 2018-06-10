## Test environments

- local OS X install (R 3.4.3)
- ubuntu 14.04 (on travis-ci with older, release and devel) 
- winbuilder (older, release and devel)

## R CMD check results

There were no ERROR or WARNING.

There were 2 notes:

* on travis-ci/ubuntu:

checking installed package size ... NOTE
  installed size is  5.6Mb
  sub-directories of 1Mb or more:
    libs   5.5Mb

* on winbuilder with R version 3.3.3 (but these URL are valid):

Found the following (possibly) invalid URLs:
  URL: https://davidgohel.github.io/ggiraph
    From: README.md
    Status: Error
    Message: libcurl error code 35
    	error:1407742E:SSL routines:SSL23_GET_SERVER_HELLO:tlsv1 alert protocol version
  URL: https://davidgohel.github.io/officer/articles/offcran/graphics.html
    From: README.md
    Status: Error
    Message: libcurl error code 35
    	error:1407742E:SSL routines:SSL23_GET_SERVER_HELLO:tlsv1 alert protocol version



## Reverse dependencies

There were no ERROR, WARNING or NOTE. 
  
Checked ggiraph  : 0 errors | 0 warnings | 0 notes
Checked ReporteRs: 0 errors | 0 warnings | 0 notes
