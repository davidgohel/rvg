# Wrap plot instructions for DrawingML plotting in Powerpoint

A simple wrapper to mark the plot instructions as Vector Graphics
instructions. It produces an object of class 'dml' with a corresponding
method
[`ph_with`](https://davidgohel.github.io/officer/reference/ph_with.html).

The function enables usage of any R plot with argument `code` and with
ggplot objects with argument `ggobj`.

**Note:** the output is a vector graphic, not a native Microsoft Office
chart. The underlying data is not embedded in the document.

## Usage

``` r
dml(
  code,
  ggobj = NULL,
  bg = "white",
  fonts = list(),
  pointsize = 12,
  editable = TRUE,
  ...
)
```

## Arguments

- code:

  plotting instructions

- ggobj:

  ggplot object to print. argument code will be ignored if this argument
  is supplied.

- bg, fonts, pointsize, editable:

  Parameters stored and later passed to
  [`dml_pptx`](https://davidgohel.github.io/rvg/dev/reference/dml_pptx.md)
  by
  [`ph_with.dml`](https://davidgohel.github.io/rvg/dev/reference/ph_with.dml.md).

- ...:

  unused arguments

## background color

When dealing with a ggplot object, the `bg` parameter sets the device
background but the ggplot theme's `plot.background` will typically draw
over it. To control the background appearance, set it in the theme
instead:

    theme(
      panel.background = element_rect(fill = "#EFEFEF"),
      plot.background = element_rect(fill = "wheat"))

## Limitations

The DrawingML format does not support the following R graphics features:

- **Patterns and gradients**: grid patterns (`GridPattern`), linear and
  radial gradients used as fill are not translated into DrawingML.
  ggplot2 will emit a warning when these are used.

- **Rich text (ggtext/gridtext)**: packages such as `ggtext` render
  HTML/Markdown into composite grid grobs. The rvg device can only
  capture low-level primitives (text strings, lines, polygons), so the
  rich formatting (inline images, coloured spans, HTML markup) is lost.
  Use standard ggplot2 text functions or apply formatting after export
  in PowerPoint.

- **Plotmath expressions**: R's graphics engine decomposes
  [`expression()`](https://rdrr.io/r/base/expression.html) into
  individual text primitives before the device sees them. Compound
  expressions (e.g. `expression("a" * "b")`, superscripts, subscripts)
  are rendered as separate text boxes, causing vertical misalignment.

- **Clipping paths, masks, compositing and transformations**: these R
  \>= 4.1 features have no DrawingML equivalent and are silently
  ignored.

## See also

[`ph_with.dml`](https://davidgohel.github.io/rvg/dev/reference/ph_with.dml.md)

## Examples

``` r
anyplot <- dml(code = barplot(1:5, col = 2:6), bg = "wheat")

library(officer)
doc <- read_pptx()
doc <- add_slide(doc, "Title and Content", "Office Theme")
doc <- ph_with(doc, anyplot, location = ph_location_fullsize())
fileout <- tempfile(fileext = ".pptx")
# fileout <- "vg.pptx"
print(doc, target = fileout)
```
