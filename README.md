[![Travis-CI Build Status](https://travis-ci.org/davidgohel/rvg.svg?branch=master)](https://travis-ci.org/davidgohel/rvg)
[![Build status](https://ci.appveyor.com/api/projects/status/github/davidgohel/rvg?branch=master)](https://ci.appveyor.com/project/davidgohel/rvg/branch/master)
[![Coverage Status](https://img.shields.io/codecov/c/github/davidgohel/rvg/master.svg)](https://codecov.io/github/davidgohel/rvg?branch=master)
[![version](http://www.r-pkg.org/badges/version/rvg)](http://www.r-pkg.org/pkg/rvg)

# rvg

`rvg` is a set of graphics devices that produces Vector Graphics outputs. This package 
is providing three formats:

* SVG (*Scalable Vector Graphics*) with `dsvg`. The graphic can be made interactive (tooltip, onclick 
  and id attribute manipulation). To make that work in an HTML document, check for 
  dependencies `jquery` and `bootstrap` in the document.
* DrawingML for Microsoft Word (>=2007) with `dml_docx` and DrawingML for Microsoft PowerPoint 
  with `dml_docx`. Theses formats let users edit the graphic elements (*editable graphics*) 
  and have a very good rendering. 

Theses are producing raw XML outputs. To let users get easelly their plot into a MS Word or 
PowerPoint file, two functions have been implements `write_docx` and `write_pptx`. 
By default, theses graphics are *editable*, element edition can be disabled with 
option `editable = FALSE`.

```
library(rvg)
library(ggplot2)

gg = ggplot( iris, aes(Sepal.Length, Sepal.Width, 
    color = Petal.Width)) + geom_point()

write_docx(file = "my_plot.docx", code = print( gg ))
write_pptx(file = "my_plot.pptx", code = print( gg ))
```

## Installation

Install the development version:

    devtools::install_github("davidgohel/rvg")

