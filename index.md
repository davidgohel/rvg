# rvg package

`rvg` provides two graphics devices that produce Vector Graphics outputs
in DrawingML format for Microsoft PowerPoint with `dml_pptx` and for
Microsoft Excel with `dml_xlsx`. These formats let users edit the
graphic elements (*editable graphics*) within PowerPoint or Excel and
have a very good rendering.

These raw XML outputs cannot be used *as is*. Functions `dml` and
[`ph_with()`](https://davidgohel.github.io/officer/reference/ph_with.html)
have to be used with package `officer` to add vector graphics in
PowerPoint documents (`dml` function is a simple wrapper to mark the
plot instructions as Vector Graphics instructions).

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

Function
[`xl_add_vg()`](https://davidgohel.github.io/rvg/reference/xl_add_vg.md)
is the equivalent for Excel workbooks.

``` r
doc <- read_xlsx()
doc <- xl_add_vg(doc, sheet = "Feuil1", code = print(gg), 
  width = 6, height = 6, left = 1, top = 2 )
print(doc, target = "vg.xlsx")
```

By default, these graphics are *editable*, element edition can be
disabled with option `editable = FALSE`.

## Installation

You can get the development version from GitHub:

``` r
devtools::install_github("davidgohel/rvg")
```

Install the CRAN version:

``` r
install.packages("rvg")
```
