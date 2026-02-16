# add a plot output as vector graphics into an Excel object

Produces a vector graphics output from R plot instructions and adds the
result in an Excel sheet.

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
  [`dml_xlsx`](https://davidgohel.github.io/rvg/reference/dml_xlsx.md).

## Examples

``` r
# \donttest{
library(officer)
my_ws <- read_xlsx()
my_ws <- xl_add_vg(my_ws,
  sheet = "Feuil1",
  code = barplot(1:5, col = 2:6), width = 6, height = 6, left = 1, top = 2
)
fileout <- tempfile(fileext = ".xlsx")
print(my_ws, target = fileout)
# }
```
