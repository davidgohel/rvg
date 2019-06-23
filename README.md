rvg package
================

[![Travis-CI Build
Status](https://travis-ci.org/davidgohel/rvg.svg?branch=master)](https://travis-ci.org/davidgohel/rvg)
[![Build
status](https://ci.appveyor.com/api/projects/status/github/davidgohel/rvg?branch=master)](https://ci.appveyor.com/project/davidgohel/rvg/branch/master)
[![version](http://www.r-pkg.org/badges/version/rvg)](https://CRAN.R-project.org/package=rvg)
![cranlogs](http://cranlogs.r-pkg.org./badges/rvg)
![Active](http://www.repostatus.org/badges/latest/active.svg)

`rvg` is providing two graphics devices that produces Vector Graphics
outputs in DrawingML format for Microsoft PowerPoint with `dml_pptx` and
for Microsoft Excel with `dml_xlsx`. Theses formats let users edit the
graphic elements (*editable graphics*) within PowerPoint or Excel and
have a very good rendering.

These raw XML outputs cannot be used *as is*. Functions `dml` and
`ph_with()` have to be used with package `officer` to add vector
graphics in PowerPoint documents (`dml` function is a simple wrapper to
mark the plot instructions as Vector Graphics instructions).

``` r
library(rvg)
library(ggplot2)
library(officer)

my_vec_graph <- dml(code = barplot(1:5, col = 2:6))

doc <- read_pptx()
doc <- add_slide(doc, layout = "Title and Content", master = "Office Theme")
doc <- ph_with(doc, my_vec_graph, location = ph_location_fullsize() )
print(doc, target = "my_plot.pptx")
```

Function `xl_add_vg()` is the equivalent for Excel workbooks.

``` r
doc <- read_xlsx()
doc <- xl_add_vg(doc, sheet = "Feuil1", code = print(gg), 
  width = 6, height = 6, left = 1, top = 2 )
print(doc, target = "vg.xlsx")
```

By default, theses graphics are *editable*, element edition can be
disabled with option `editable = FALSE`. For more details, read officer
article
[graphics](https://davidgohel.github.io/officer/articles/offcran/graphics.html).

## Installation

You can get the development version from GitHub:

``` r
devtools::install_github("davidgohel/rvg")
```

Install the CRAN version:

``` r
install.packages("rvg")
```
