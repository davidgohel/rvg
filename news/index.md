# Changelog

## rvg 0.4.1

### Issues

- Text no longer inherits bold/italic style from PowerPoint/Excel
  templates; `b` and `i` attributes are now always set explicitly in run
  properties.
- Shapes without fill now emit explicit `<a:noFill/>` instead of
  omitting the fill element, preventing unintended theme fill
  inheritance in PowerPoint/Excel.
- Circles (points) outside the clipping region are now skipped,
  preventing data points from appearing beyond plot limits.
- Text is no longer vertically shifted in PowerPoint/Excel output;
  removed explicit line spacing that conflicted with the centered anchor
  of text boxes.

### changes

- [`dml_xlsx()`](https://davidgohel.github.io/rvg/reference/dml_xlsx.md)
  no longer exposes the `raster_prefix` argument; raster files are now
  written to an internal temporary directory. The prefix is embedded in
  the output as an XML comment (`<!-- rvg_raster_prefix:... -->`), which
  callers can read after
  [`dev.off()`](https://rdrr.io/r/grDevices/dev.html) to locate the PNG
  files.
- Implement `xlsx_path` callback for the XLSX device, enabling
  `geom_path` and path-based geometries in Excel output.
- Compound paths (`pptx_path`/`xlsx_path`) now combine all sub-polygons
  into a single `<a:path>` element within `<a:custGeom>`, so holes
  (donuts, cut-out polygons) render correctly via winding rule.
- Bump `deviceVersion` to v15 (`R_GE_group`) on R \>= 4.2, avoiding
  redundant double-clipping by the graphics engine and enabling the
  `capabilities` callback so that
  [`dev.capabilities()`](https://rdrr.io/r/grDevices/dev.capabilities.html)
  reports explicitly unsupported features (patterns, masks, clipping
  paths, compositing, transformations). This eliminates the “Unable to
  check capabilities” warning from ggplot2.
- Font resolution now uses
  [`gdtools::font_set_auto()`](https://davidgohel.github.io/gdtools/reference/font_set_auto.html)
  to detect system fonts for `sans`, `serif`, `mono` and `symbol`
  aliases. Minimum gdtools version bumped to 0.5.0.
- `validate_fonts()` now warns when user-supplied font families are not
  found on the system.
- Documentation clarifies that fonts are not embedded in DrawingML
  output and must be available on the reader’s system.

## rvg 0.4.0

CRAN release: 2025-10-06

- migrate from gdtools to systemfonts

## rvg 0.3.5

CRAN release: 2025-03-27

### Issues

- Use png from the system/Rtools

## rvg 0.3.4

CRAN release: 2024-08-27

### Issues

- throw more informative error message when no plot is generated

## rvg 0.3.3

CRAN release: 2023-05-10

### Issues

- fix issue with raster images that get deleted before being integrated
  by officer.
- drop cpp11 specification

## rvg 0.3.2

CRAN release: 2023-01-07

### Issues

- use cstdint as required by the CRAN to let the package work with
  GCC-13.

## rvg 0.3.1

CRAN release: 2022-12-21

### Issues

- add field `SystemRequirements` in DESCRIPTION file so that software
  dependancies can be managed automatically.

### Changes

- use new mechanisms of officer 0.5 and drop complex treatments of
  raster/images for pptx.

## rvg 0.3.0

CRAN release: 2022-12-08

### Issues

- replace call to sprintf by call to snprintf

### Changes

- use png for rasters
- refactor cpp file tree

## rvg 0.2.5

CRAN release: 2020-06-30

### Changes

- update for changes in the R graphics engine (thanks to Dr Paul
  Murrell)
- function `body_add_vg` is defunct.
- functions `ph_with_vg` and `ph_with_vg_at` have been removed.

## rvg 0.2.4

CRAN release: 2020-02-17

### Changes

- functions `ph_with_vg` and `ph_with_vg_at` are defunct and will be
  removed in the next release.
- rvg web site is now indicated in DESCRIPTION file.
- functions `ph_with_vg` and `ph_with_vg_at` are defunct.

## rvg 0.2.3

CRAN release: 2020-01-26

### Deprecations

- functions `ph_with_vg` and `ph_with_vg_at` are deprecated.

## rvg 0.2.2

CRAN release: 2019-11-11

### Fixes

- fix issues with default font

## rvg 0.2.1

CRAN release: 2019-06-25

### Enhancement

- New function `ph_with.dml` and `dml`, it will replace functions
  `ph_with_vg` and `ph_with_vg_at` (they will be deprecated in the next
  release).

## rvg 0.2.0

CRAN release: 2019-04-06

### Changes

- dsvg is now living in ggiraph package

## rvg 0.1.9

CRAN release: 2018-06-10

### Enhancement

- add path method to PowerPoint device. As an effect `ggplot2::geom_sf`
  is now supported.
- argument `type` of function `ph_with_vg` now default to “body”.

### Changes

- `dml_docx` is deprecated and will be removed in later versions. It was
  maintained for compatibility reasons with package ReporteRs. As
  ReporteRs will be removed from CRAN 2018-07-16, this function has no
  more reason to exist.

## rvg 0.1.8

CRAN release: 2018-02-13

- fix encoding issues with Windows platforms.

## rvg 0.1.7

CRAN release: 2017-12-03

- add argument `ggobj` to `ph_with_vg()` and `ph_with_vg_at()`. It can
  be used as replacement to `code=print(gg)` when gg is a ggplot object.

- new function
  [`xl_add_vg()`](https://davidgohel.github.io/rvg/reference/xl_add_vg.md)
  to add vector graphics produced from R into Excel workbooks. This
  requires `officer >= 0.2.0`.

## rvg 0.1.6

CRAN release: 2017-10-05

- update with officer 1.8

## rvg 0.1.5

CRAN release: 2017-09-05

- update with gdtools 1.6
- raster ids are now registered

## rvg 0.1.4

CRAN release: 2017-06-23

- raster images dpi has been increased
- new ‘ph_with_vg_at’ methods for package officer

## rvg 0.1.3

CRAN release: 2017-03-10

- new ‘body_add_vg’ and ‘ph_with_vg’ methods for package officer
- deletion of write\_\* functions, these are now available in officer
  (functions print)

## rvg 0.1.2

CRAN release: 2017-01-03

- add donttest tags in all examples to avoid long runs due to fontconfig
  initialisation
- change textbox in Word graphics so that text auto-adjusts.

## rvg 0.1.1

CRAN release: 2016-11-02

- adapt code to gdtools 0.1.1

## rvg 0.1.0

CRAN release: 2016-06-27

- adapt code to xml2 1.0 (use xml_find_first instead of deprecated
  xml_find_one)

## rvg 0.0.9

CRAN release: 2016-04-29

### New features

- new device driver for Microsoft Excel documents
- add function write_xlsx

### Bug fixes

- make cliping region id unique in dsvg

## rvg 0.0.8

CRAN release: 2016-03-04

### Bug fixes

- fix rectangle clipping
- stop wrapping text in DrawingML devices

## rvg 0.0.7

CRAN release: 2016-02-25

### New features

- implementation of clipping (algorithms by Francois Brunetti).
- new function set_attr to set attributes to graphical elements without
  dependency to jquery nor bootstrap (suggested by Bob Rudis).

## rvg 0.0.6

CRAN release: 2016-01-26

### Bug fixes

- plot size was truncated to integers

- lines with opacity were not displayed

### New features

- grid support for interactive points, polygons, polylines, rectangles,
  segments and text
