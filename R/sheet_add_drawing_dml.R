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
#' @importFrom officer sheet_add_drawing
#' @importFrom xml2 xml_ns xml_new_root xml_attr<- xml_add_sibling xml_add_child xml_find_first read_xml
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

  # create media dir for rasters
  media_dir <- file.path(package_dir, "xl/media")
  dir.create(media_dir, showWarnings = FALSE, recursive = TRUE)

  # find sheet object
  sheet_idx <- which(x$worksheets$sheet_names() == sheet)
  sheet_obj <- x$sheets$get_sheet(sheet_idx)

  # render the dml to a temporary drawing file
  drawings_dir <- file.path(package_dir, "xl/drawings")
  dir.create(drawings_dir, showWarnings = FALSE, recursive = TRUE)
  dml_file <- file.path(
    drawings_dir,
    basename(tempfile(pattern = "d", fileext = ".xml"))
  )

  pars <- list(...)
  pars$file <- dml_file
  pars$id <- 0L
  pars$last_rel_id <- sheet_obj$relationship()$get_next_id() - 1
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

  # extract raster_prefix from XML comment
  xml_raw <- paste0(readLines(dml_file, warn = FALSE), collapse = "")
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

  # handle raster images
  rel <- sheet_obj$relationship()
  if (length(raster_files)) {
    file.copy(raster_files, media_dir)
    rels_dir <- file.path(drawings_dir, "_rels")
    dir.create(rels_dir, showWarnings = FALSE, recursive = TRUE)
    rel_file <- file.path(rels_dir, paste0(basename(dml_file), ".rels"))

    rid <- paste0("rId", seq_along(raster_files))
    target <- paste0("../media/", basename(raster_files))
    rel_entries <- sprintf(
      "\t<Relationship Id=\"%s\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/image\" Target=\"%s\"/>",
      rid, target
    )
    writeLines(c(
      "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>",
      "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">",
      rel_entries,
      "</Relationships>"
    ), rel_file, useBytes = TRUE)
  }

  # link sheet to drawing
  dml_rid <- rel$get_next_id()
  rel$add_drawing(src = dml_file, root_target = "../drawings")

  ns <- c(d1 = "http://schemas.openxmlformats.org/spreadsheetml/2006/main")
  ext_lst <- xml_find_first(sheet_obj$get(), "d1:extLst", ns = ns)

  xml_elt <- xml_new_root("drawing")
  xml_attr(xml_elt, "r:id") <- sprintf("rId%.0f", dml_rid)
  if (!inherits(ext_lst, "xml_missing")) {
    xml_add_sibling(.x = ext_lst, .value = xml_elt, .where = "before")
  } else {
    xml_add_child(sheet_obj$get(), xml_elt)
  }

  # content type
  override <- "application/vnd.openxmlformats-officedocument.drawing+xml"
  names(override) <- paste0("/xl/drawings/", basename(dml_file))
  x$content_type$add_override(value = override)
  sheet_obj$save()

  invisible(x)
}
