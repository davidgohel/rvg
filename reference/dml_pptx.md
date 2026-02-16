# DrawingML graphic device for Microsoft PowerPoint

Graphics devices for Microsoft PowerPoint DrawingML format.

## Usage

``` r
dml_pptx(
  file = "Rplots.dml",
  width = 6,
  height = 6,
  offx = 1,
  offy = 1,
  bg = "white",
  fonts = list(),
  pointsize = 12,
  editable = TRUE,
  id = 1L,
  last_rel_id = 1L,
  raster_prefix = "raster_",
  standalone = TRUE
)
```

## Arguments

- file:

  the file where output will appear.

- height, width:

  Height and width in inches.

- offx, offy:

  top and left origin of the plot

- bg:

  Default background color for the plot (defaults to "white").

- fonts:

  Named list of font names to be aliased with fonts installed on your
  system. If unspecified, the R default families `sans`, `serif`, `mono`
  and `symbol` are aliased to the families detected by
  [`gdtools::font_set_auto()`](https://davidgohel.github.io/gdtools/reference/font_set_auto.html).

  **Important:** fonts are referenced by name in the DrawingML output
  and are **not embedded**. The reader of the PowerPoint file must also
  have the same fonts installed, otherwise Office will substitute them.
  For maximum portability, prefer fonts bundled with Microsoft Office
  such as `"Arial"`, `"Times New Roman"` and `"Courier New"`.

  Fonts must be installed on your system; use
  [`gdtools::font_family_exists()`](https://davidgohel.github.io/gdtools/reference/font_family_exists.html)
  to verify availability.

  Example:
  `list(sans = "Arial", serif = "Times New Roman", mono = "Courier New")`.

- pointsize:

  default point size.

- editable:

  should vector graphics elements (points, text, etc.) be editable.

- id:

  specifies a unique identifier (integer) within the slide that will
  contain the DrawingML instructions.

- last_rel_id:

  specifies the last unique identifier (integer) within relationship
  file that will be used to reference embedded raster images if any.

- raster_prefix:

  string value used as prefix for png files produced when raster objects
  are printed on the graphical device.

- standalone:

  produce a standalone drawingml file? If `FALSE`, omits xml header and
  namespaces.

## See also

[`Devices`](https://rdrr.io/r/grDevices/Devices.html)

## Examples

``` r
# \donttest{
dml_pptx(file = tempfile())
plot(1:11, (-5:5)^2, type = "b", main = "Simple Example")
dev.off()
#> agg_record_432744511 
#>                    2 
# }
```
