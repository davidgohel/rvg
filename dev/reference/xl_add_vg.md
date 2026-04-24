# add a plot output as vector graphics into an Excel object

Deprecated, use
[`officer::sheet_add_drawing()`](https://davidgohel.github.io/officer/reference/sheet_add_drawing.html)
with a [`dml()`](https://davidgohel.github.io/rvg/dev/reference/dml.md)
object instead.

## Usage

``` r
xl_add_vg(x, sheet, code, left, top, width, height, ...)
```

## Arguments

- x:

  an `rxlsx` object produced by
  [`officer::read_xlsx`](https://davidgohel.github.io/officer/reference/read_xlsx.html)

- sheet:

  sheet label/name

- code:

  plot instructions

- left, top:

  left and top origin of the plot on the sheet in inches.

- height, width:

  Height and width in inches.

- ...:

  arguments passed on to
  [`dml_xlsx`](https://davidgohel.github.io/rvg/dev/reference/dml_xlsx.md).

## Examples

``` r
# \donttest{
library(officer)
my_ws <- read_xlsx()
my_ws <- sheet_add_drawing(my_ws,
  value = dml(code = barplot(1:5, col = 2:6)),
  sheet = "Feuil1",
  width = 6, height = 6, left = 1, top = 2
)
fileout <- tempfile(fileext = ".xlsx")
print(my_ws, target = fileout)
# }
```
