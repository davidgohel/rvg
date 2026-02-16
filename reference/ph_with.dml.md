# add a plot output as vector graphics into a PowerPoint object

Produces a vector graphics output from R plot instructions stored in a
[`dml`](https://davidgohel.github.io/rvg/reference/dml.md) object and
adds the result in an `rpptx` object produced by
[`read_pptx`](https://davidgohel.github.io/officer/reference/read_pptx.html).

## Usage

``` r
# S3 method for class 'dml'
ph_with(x, value, location, ...)
```

## Arguments

- x:

  a pptx device

- value:

  [`dml`](https://davidgohel.github.io/rvg/reference/dml.md) object

- location:

  a location for a placeholder.

- ...:

  Arguments to be passed to methods

## Examples

``` r
anyplot <- dml(code = barplot(1:5, col = 2:6), bg = "wheat")

library(officer)
doc <- read_pptx()
doc <- add_slide(doc, "Title and Content", "Office Theme")
doc <- ph_with(doc, anyplot, location = ph_location_fullsize())

fileout <- tempfile(fileext = ".pptx")
print(doc, target = fileout)
```
