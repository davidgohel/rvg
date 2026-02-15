# rvg 0.4.1

## Issues

- Text no longer inherits bold/italic style from PowerPoint/Excel templates;
  `b` and `i` attributes are now always set explicitly in run properties.
- Shapes without fill now emit explicit `<a:noFill/>` instead of omitting
  the fill element, preventing unintended theme fill inheritance in
  PowerPoint/Excel.

## changes

- Implement `xlsx_path` callback for the XLSX device, enabling
  `geom_path` and path-based geometries in Excel output.
- Compound paths (`pptx_path`/`xlsx_path`) now combine all
  sub-polygons into a single `<a:path>` element within `<a:custGeom>`,
  so holes (donuts, cut-out polygons) render correctly via winding
  rule.
- Bump `deviceVersion` from v13 to v14 (`R_GE_deviceClip`) on
  R >= 4.2, avoiding redundant double-clipping by the graphics
  engine.
- Font resolution now uses `gdtools::font_set_auto()` to detect
  system fonts for `sans`, `serif`, `mono` and `symbol` aliases.
  Minimum gdtools version bumped to 0.5.0.
- `validate_fonts()` now warns when user-supplied font families
  are not found on the system.
- Documentation clarifies that fonts are not embedded in DrawingML
  output and must be available on the reader's system.

# rvg 0.4.0

- migrate from gdtools to systemfonts

# rvg 0.3.5

## Issues

- Use png from the system/Rtools

# rvg 0.3.4

## Issues

* throw more informative error message when no plot is generated

# rvg 0.3.3

## Issues

* fix issue with raster images that get deleted before being integrated by officer.
* drop cpp11 specification

# rvg 0.3.2

## Issues

* use cstdint as required by the CRAN to let the package work 
with GCC-13.

# rvg 0.3.1

## Issues

* add field `SystemRequirements` in DESCRIPTION file 
so that software dependancies can be managed automatically.

## Changes

* use new mechanisms of officer 0.5 and drop complex
treatments of raster/images for pptx.

# rvg 0.3.0

## Issues

* replace call to sprintf by call to snprintf

## Changes

* use png for rasters
* refactor cpp file tree

# rvg 0.2.5

## Changes

* update for changes in the R graphics engine (thanks to Dr Paul Murrell)
* function `body_add_vg` is defunct.
* functions `ph_with_vg` and `ph_with_vg_at` have been removed.

# rvg 0.2.4

## Changes

* functions `ph_with_vg` and `ph_with_vg_at` are defunct and will be removed in the next release.
* rvg web site is now indicated in DESCRIPTION file.
* functions `ph_with_vg` and `ph_with_vg_at` are defunct.


# rvg 0.2.3

## Deprecations

* functions `ph_with_vg` and `ph_with_vg_at` are deprecated.

# rvg 0.2.2

## Fixes

* fix issues with default font

# rvg 0.2.1

## Enhancement

* New function `ph_with.dml` and `dml`, it will replace functions `ph_with_vg` and
`ph_with_vg_at` (they will be deprecated in the next release).

# rvg 0.2.0

## Changes

* dsvg is now living in ggiraph package

# rvg 0.1.9

## Enhancement

* add path method to PowerPoint device. As an effect
  `ggplot2::geom_sf` is now supported.
* argument `type` of function `ph_with_vg`
  now default to "body".

## Changes

* `dml_docx` is deprecated and will be removed in later versions. It
  was maintained for compatibility reasons with package ReporteRs.
  As ReporteRs will be removed from CRAN 2018-07-16, this function
  has no more reason to exist.

# rvg 0.1.8

* fix encoding issues with Windows platforms.

# rvg 0.1.7

* add argument `ggobj` to `ph_with_vg()` and `ph_with_vg_at()`.
It can be used as replacement to `code=print(gg)` when gg is
a ggplot object.

* new function `xl_add_vg()` to add vector graphics produced
from R into Excel workbooks. This requires `officer >= 0.2.0`.

# rvg 0.1.6

* update with officer 1.8

# rvg 0.1.5

* update with gdtools 1.6
* raster ids are now registered

# rvg 0.1.4

* raster images dpi has been increased
* new 'ph_with_vg_at' methods for package officer

# rvg 0.1.3

* new 'body_add_vg' and 'ph_with_vg' methods for package officer
* deletion of write_* functions, these are now available in officer (functions print)

# rvg 0.1.2

* add donttest tags in all examples to avoid long runs due to fontconfig initialisation
* change textbox in Word graphics so that text auto-adjusts.

# rvg 0.1.1

* adapt code to gdtools 0.1.1

# rvg 0.1.0

* adapt code to xml2 1.0 (use xml_find_first instead of deprecated xml_find_one)

# rvg 0.0.9

## New features

* new device driver for Microsoft Excel documents
* add function write_xlsx

## Bug fixes

* make cliping region id unique in dsvg

# rvg 0.0.8

## Bug fixes

* fix rectangle clipping
* stop wrapping text in DrawingML devices

# rvg 0.0.7

## New features

* implementation of clipping (algorithms by Francois Brunetti).
* new function set_attr to set attributes to graphical elements
  without dependency to jquery nor bootstrap (suggested by
  Bob Rudis).

# rvg 0.0.6

## Bug fixes

* plot size was truncated to integers

* lines with opacity were not displayed

## New features

* grid support for interactive points,
  polygons, polylines, rectangles,
  segments and text


