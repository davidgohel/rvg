#' @export
#' @title Add a dml drawing to an Excel sheet
#' @description Add a [dml()] object to a sheet in an xlsx workbook
#' created with [officer::read_xlsx()]. The plot is rendered as
#' editable vector graphics (DrawingML).
#' @param x an rxlsx object (created by [officer::read_xlsx()])
#' @param value a [dml()] object
#' @param sheet sheet name (must already exist)
#' @param left,top left and top origin of the plot on the sheet in inches
#' @param width,height width and height in inches
#' @param ... additional arguments passed to [dml_xlsx()]
#' @return the rxlsx object (invisibly)
#' @importFrom officer sheet_add_drawing xlsx_drawing
#' @importFrom xml2 xml_add_child xml_find_all read_xml
#' @method sheet_add_drawing dml
#' @examples
#' \donttest{
#' library(officer)
#' library(rvg)
#'
#' x <- read_xlsx()
#' x <- sheet_add_drawing(x,
#'   value = dml(code = barplot(1:5, col = 2:6)),
#'   sheet = "Feuil1",
#'   left = 1, top = 2, width = 6, height = 6
#' )
#' print(x, target = tempfile(fileext = ".xlsx"))
#' }
sheet_add_drawing.dml <- function(
  x, value, sheet,
  left = 1, top = 1, width = 6, height = 6,
  ...
) {
  stopifnot(inherits(x, "rxlsx"))

  package_dir <- x$package_dir

  # ensure media dir exists (rasters referenced by the dml go there)
  media_dir <- file.path(package_dir, "xl/media")
  dir.create(media_dir, showWarnings = FALSE, recursive = TRUE)

  # find sheet object
  sheet_idx <- which(x$worksheets$sheet_names() == sheet)
  sheet_obj <- x$sheets$get_sheet(sheet_idx)

  # Get (or create) the shared drawing part for this sheet. Anchors
  # produced below will be appended to it so that a sheet never ends
  # up with more than one <drawing> reference.
  drawing <- officer::xlsx_drawing$new(
    package_dir, sheet_obj, x$content_type
  )

  # Render the dml to a throw-away file; we only want its <xdr:*Anchor>
  # children and the list of raster files it produced.
  tmp_dml_file <- tempfile(pattern = "rvg_dml_", fileext = ".xml")

  pars <- list(...)
  pars$file <- tmp_dml_file
  pars$id <- 0L
  # Raster rIds written inside the dml XML must slot into the shared
  # drawing's rels, so start numbering from the drawing's next id.
  pars$last_rel_id <- drawing$relationship()$get_next_id() - 1
  pars$standalone <- TRUE
  pars$width <- width
  pars$height <- height
  pars$offx <- left
  pars$offy <- top
  if (!is.null(value$bg)) pars$bg <- value$bg
  if (!is.null(value$fonts) && length(value$fonts) > 0) {
    pars$fonts <- value$fonts
  }
  if (!is.null(value$pointsize)) pars$pointsize <- value$pointsize
  if (!is.null(value$editable)) pars$editable <- value$editable

  do.call("dml_xlsx", pars)

  # execute the plotting code
  if (!is.null(value$ggobj)) {
    print(value$ggobj)
  } else {
    tryCatch(
      eval(rlang::quo_get_expr(value$code), envir = rlang::quo_get_env(value$code)),
      finally = NULL
    )
  }
  dev.off()

  # Extract the raster file list (if any) from the marker comment.
  xml_raw <- paste0(readLines(tmp_dml_file, warn = FALSE), collapse = "")
  m <- regmatches(
    xml_raw,
    regexpr("<!-- rvg_raster_prefix:(.+?) -->", xml_raw, perl = TRUE)
  )
  raster_files <- character(0)
  if (length(m) == 1L) {
    raster_prefix <- sub(
      "<!-- rvg_raster_prefix:(.+?) -->", "\\1", m, perl = TRUE
    )
    raster_files <- list_raster_files(img_dir = raster_prefix)
  }

  # Copy rasters into xl/media/ and register them as rels on the
  # shared drawing. rIds are assigned in the same order dml_xlsx
  # allocated them (incrementing from `last_rel_id`), so the
  # `r:embed="rIdN"` attributes inside the anchors resolve correctly.
  if (length(raster_files)) {
    file.copy(raster_files, media_dir)
    for (rf in basename(raster_files)) {
      drawing$add_image_rel(rf)
    }
  }

  # Append every anchor produced by dml_xlsx to the shared drawing.
  dml_doc <- read_xml(tmp_dml_file)
  anchors <- xml_find_all(
    dml_doc,
    "xdr:absoluteAnchor | xdr:oneCellAnchor | xdr:twoCellAnchor",
    ns = c(
      xdr = "http://schemas.openxmlformats.org/drawingml/2006/spreadsheetDrawing"
    )
  )
  for (a in anchors) {
    xml_add_child(drawing$get(), a)
  }
  drawing$save()

  unlink(tmp_dml_file)

  invisible(x)
}
