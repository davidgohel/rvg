# rvg package

`rvg` provides two graphics devices that produce Vector Graphics outputs
in DrawingML format for Microsoft PowerPoint with
[`dml_pptx()`](https://davidgohel.github.io/rvg/reference/dml_pptx.md)
and for Microsoft Excel with
[`dml_xlsx()`](https://davidgohel.github.io/rvg/reference/dml_xlsx.md).
These formats let users edit the graphic elements (*editable graphics*)
within PowerPoint or Excel and have a very good rendering.

These raw XML outputs cannot be used *as is*. The
[`dml()`](https://davidgohel.github.io/rvg/reference/dml.md) function is
a simple wrapper that marks plot instructions as Vector Graphics
instructions, so they can be added to an Office document via `officer`:

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
[`dml()`](https://davidgohel.github.io/rvg/reference/dml.md) object to
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
[`xl_add_vg()`](https://davidgohel.github.io/rvg/reference/xl_add_vg.md)
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

## Contributing to the package

### Bug reports

Bugs are reported through [GitHub
issues](https://github.com/davidgohel/rvg/issues/new/choose). The issue
form requires a reproducible example (a
[reprex](https://reprex.tidyverse.org/)) and the output of
[`sessionInfo()`](https://rdrr.io/r/utils/sessionInfo.html); reports
missing these elements are closed without further response. The more
reproducible your report, the more time can go into investigating the
bug rather than reconstructing it.

For usage questions (“how do I do X with rvg”), please read
<https://davidgohel.github.io/rvg/> or ask on [Stack
Overflow](https://stackoverflow.com/questions/tagged/r) with the `[r]`
tag instead.

### Pull requests

Before writing any code, please open an issue describing the change you
have in mind and the reason for it. The proposal needs to be discussed
and explicitly validated by the maintainer before work starts; pull
requests for features that have not gone through this step will not be
considered, no matter how well crafted. This isn’t meant to discourage
contributions, only to make sure the time you invest goes into a change
that will actually land.

Pull requests are accepted on a case-by-case basis: once a proposal has
been agreed in an issue, the maintainer will invite you to open a PR for
that specific change. Unsolicited PRs that bypass this step will be
closed.

When invited to open a PR, please include:

- the new function(s) with code and roxygen tags, including runnable
  examples
- documentation updates where relevant
- corresponding tests under `tests/testthat`

Run `rhub::check_for_cran()` locally before submitting; CI will run the
full check suite on the PR.
