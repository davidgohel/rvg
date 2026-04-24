# Add a dml drawing to an Excel sheet

Add a [`dml()`](https://davidgohel.github.io/rvg/dev/reference/dml.md)
object to a sheet in an xlsx workbook created with
[`officer::read_xlsx()`](https://davidgohel.github.io/officer/reference/read_xlsx.html).
The plot is rendered as editable vector graphics (DrawingML).

## Usage

``` r
# S3 method for class 'dml'
sheet_add_drawing(
  x,
  value,
  sheet,
  left = 1,
  top = 1,
  width = 6,
  height = 6,
  ...
)
```

## Arguments

- x:

  an rxlsx object (created by
  [`officer::read_xlsx()`](https://davidgohel.github.io/officer/reference/read_xlsx.html))

- value:

  a [`dml()`](https://davidgohel.github.io/rvg/dev/reference/dml.md)
  object

- sheet:

  sheet name (must already exist)

- left, top:

  left and top origin of the plot on the sheet in inches

- width, height:

  width and height in inches

- ...:

  additional arguments passed to
  [`dml_xlsx()`](https://davidgohel.github.io/rvg/dev/reference/dml_xlsx.md)

## Value

the rxlsx object (invisibly)

## Examples

``` r
# \donttest{
library(officer)
library(rvg)

x <- read_xlsx()
x <- sheet_add_drawing(x,
  value = dml(code = barplot(1:5, col = 2:6)),
  sheet = "Feuil1",
  left = 1, top = 2, width = 6, height = 6
)
print(x, target = tempfile(fileext = ".xlsx"))
# }
```
