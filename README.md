[![Travis-CI Build Status](https://travis-ci.org/davidgohel/rvg.svg?branch=master)](https://travis-ci.org/davidgohel/rvg)
[![Build status](https://ci.appveyor.com/api/projects/status/github/davidgohel/rvg?branch=master)](https://ci.appveyor.com/project/davidgohel/rvg/branch/master)
[![Coverage Status](https://img.shields.io/codecov/c/github/davidgohel/rvg/master.svg)](https://codecov.io/github/davidgohel/rvg?branch=master)

# rvg

rvg is a set of graphics devices that produces svg and DrawingML outputs. 

This package implements `write_docx` and `write_pptx` functions. The first one is producing 
a Microsoft Word document and the last one a Microsoft PowerPoint document containing R plot 
results in a vector graphics format. By default, theses graphics are *editable*, element 
edition can be disabled with option `editable = FALSE`.

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

