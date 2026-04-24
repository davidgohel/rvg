# rvg package

`rvg` provides two graphics devices that produce Vector Graphics outputs
in DrawingML format for Microsoft PowerPoint with
[`dml_pptx()`](https://davidgohel.github.io/rvg/dev/reference/dml_pptx.md)
and for Microsoft Excel with
[`dml_xlsx()`](https://davidgohel.github.io/rvg/dev/reference/dml_xlsx.md).
These formats let users edit the graphic elements (*editable graphics*)
within PowerPoint or Excel and have a very good rendering.

These raw XML outputs cannot be used *as is*. The
[`dml()`](https://davidgohel.github.io/rvg/dev/reference/dml.md)
function is a simple wrapper that marks plot instructions as Vector
Graphics instructions, so they can be added to an Office document via
`officer`:

- in PowerPoint with
  [`officer::ph_with()`](https://davidgohel.github.io/officer/reference/ph_with.html)
- in Excel with
  [`officer::sheet_add_drawing()`](https://davidgohel.github.io/officer/reference/sheet_add_drawing.html)

``` r
library(rvg)
library(ggplot2)
library(officer)

my_vec_graph <- dml(code = barplot(1:5, col = 2:6))

doc <- read_pptx()
doc <- add_slide(doc, layout = "Title and Content", master = "Office Theme")
doc <- ph_with(doc, my_vec_graph, location = ph_location_fullsize())
print(doc, target = "my_plot.pptx")
```

For Excel workbooks, pass a
[`dml()`](https://davidgohel.github.io/rvg/dev/reference/dml.md) object
to
[`officer::sheet_add_drawing()`](https://davidgohel.github.io/officer/reference/sheet_add_drawing.html):

``` r
doc <- read_xlsx()
doc <- sheet_add_drawing(doc, sheet = "Feuil1",
  value = dml(code = barplot(1:5, col = 2:6)),
  left = 1, top = 2, width = 6, height = 6)
print(doc, target = "vg.xlsx")
```

By default, these graphics are *editable*; element edition can be
disabled with option `editable = FALSE`.

Note that
[`xl_add_vg()`](https://davidgohel.github.io/rvg/dev/reference/xl_add_vg.md)
will be deprecated in favor of
[`officer::sheet_add_drawing()`](https://davidgohel.github.io/officer/reference/sheet_add_drawing.html).

## Installation

You can get the development version from GitHub:

``` r
devtools::install_github("davidgohel/rvg")
```

Install the CRAN version:

``` r
install.packages("rvg")
```
