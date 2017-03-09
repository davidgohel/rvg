rvg package
================

[![Travis-CI Build Status](https://travis-ci.org/davidgohel/rvg.svg?branch=master)](https://travis-ci.org/davidgohel/rvg) [![Build status](https://ci.appveyor.com/api/projects/status/github/davidgohel/rvg?branch=master)](https://ci.appveyor.com/project/davidgohel/rvg/branch/master) [![Coverage Status](https://img.shields.io/codecov/c/github/davidgohel/rvg/master.svg)](https://codecov.io/github/davidgohel/rvg?branch=master) [![version](http://www.r-pkg.org/badges/version/rvg)](http://www.r-pkg.org/pkg/rvg) ![cranlogs](http://cranlogs.r-pkg.org./badges/rvg)

`rvg` is a set of graphics devices that produces Vector Graphics outputs. This package is providing:

-   SVG (*Scalable Vector Graphics*) with `dsvg`. The graphic can be made interactive (tooltip, onclick and id attribute manipulation). To make that work in an HTML document, check for dependencies `d3.js` in the document.
-   DrawingML for Microsoft Word (&gt;=2007) with `dml_docx`, for Microsoft PowerPoint with `dml_docx` and for Microsoft Excel with `dml_xlsx`. Theses formats let users edit the graphic elements (*editable graphics*) and have a very good rendering.

These raw XML outputs cannot be used *as is*. Function `body_add_vg` can be used with package `officer` to add vector graphics in Word documents. Function `ph_with_vg` can be used with package `officer` to add vector graphics in PowerPoint documents. By default, theses graphics are *editable*, element edition can be disabled with option `editable = FALSE`.

``` r
library(rvg)
library(ggplot2)
library(officer)

doc <- read_pptx()
doc <- add_slide(doc, layout = "Title and Content", master = "Office Theme")
doc <- ph_with_vg(doc, code = barplot(1:10), type = "body")
print(doc, target = "my_plot.pptx")
```

Installation
------------

Install the development version:

``` r
devtools::install_github("davidgohel/rvg")
```

Install the CRAN version:

``` r
install.packages("rvg")
```
