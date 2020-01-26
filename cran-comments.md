## Test environments

- local OS X install (R 3.6.1)
- ubuntu 14.04 (on travis-ci with older, release and devel) 
- winbuilder (older, release and devel)

## R CMD check results

There were no ERROR, WARNING or NOTE.

## Reverse dependencies

There were WARNINGS related to rvg new release - functions ph_with_vg_at and ph_with_vg 
are deprecated in this new version and some packages are still using them:

* package thinkr: I have warned and also proposed a patch (https://github.com/ThinkR-open/thinkr/pull/3) 
on 23 Nov 2019 but it has not been reviewed yet. The package is calling a deprecated function 
in a dontrun macro so, there will not be any warning unless used by a user.

* export: warnings can be read as the package is using the function that I want to deprecate. 
I have warned and also proposed a patch (https://github.com/tomwenseleers/export/pull/20) 
on 23 Nov 2019 but it has not been reviewed yet.

* rrtable: warnings can be read as the package is using the function that I want to deprecate. 
I have warned and also proposed a patch (https://github.com/cardiomoon/rrtable/pull/1) 
on 23 Nov 2019 that has been accepted yesterday. The new version will probably be submitted to CRAN 
soon.

