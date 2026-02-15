list_raster_files <- function(img_dir) {
  path_ <- dirname(img_dir)
  uid <- basename(img_dir)
  list.files(
    path = path_,
    pattern = paste0("^", uid, "(.*)\\.png$"),
    full.names = TRUE
  )
}


#' @title Wrap plot instructions for DrawingML plotting in Powerpoint
#' @description A simple wrapper to mark
#' the plot instructions as Vector Graphics instructions. It produces
#' an object of class 'dml' with a corresponding method \code{\link[officer]{ph_with}}.
#'
#' The function enables usage of any R plot with argument `code` and with
#' ggplot objects with argument `ggobj`.
#'
#' **Note:** the output is a vector graphic, not a native Microsoft Office
#' chart. The underlying data is not embedded in the document.
#' @section background color:
#' When dealing with a ggplot object, the `bg` parameter sets the
#' device background but the ggplot theme's `plot.background` will
#' typically draw over it. To control the background appearance,
#' set it in the theme instead:
#'
#' ```
#' theme(
#'   panel.background = element_rect(fill = "#EFEFEF"),
#'   plot.background = element_rect(fill = "wheat"))
#' ```
#' @section Limitations:
#' The DrawingML format does not support the following R graphics features:
#'
#' - **Patterns and gradients**: grid patterns (`GridPattern`),
#'   linear and radial gradients used as fill are not translated into
#'   DrawingML. ggplot2 will emit a warning when these are used.
#' - **Rich text (ggtext/gridtext)**: packages such as `ggtext` render
#'   HTML/Markdown into composite grid grobs. The rvg device can only
#'   capture low-level primitives (text strings, lines, polygons), so
#'   the rich formatting (inline images, coloured spans, HTML markup)
#'   is lost. Use standard ggplot2 text functions or apply formatting
#'   after export in PowerPoint.
#' - **Plotmath expressions**: R's graphics engine decomposes
#'   `expression()` into individual text primitives before the device
#'   sees them. Compound expressions (e.g. `expression("a" * "b")`,
#'   superscripts, subscripts) are rendered as separate text boxes,
#'   causing vertical misalignment.
#' - **Clipping paths, masks, compositing and transformations**:
#'   these R >= 4.1 features have no DrawingML equivalent and are
#'   silently ignored.
#'
#' @param code plotting instructions
#' @param ggobj ggplot object to print. argument code will be ignored if this
#'   argument is supplied.
#' @param bg,fonts,pointsize,editable Parameters stored and
#'   later passed to \code{\link{dml_pptx}} by \code{\link{ph_with.dml}}.
#' @param ... unused arguments
#' @examples
#' anyplot <- dml(code = barplot(1:5, col = 2:6), bg = "wheat")
#'
#' library(officer)
#' doc <- read_pptx()
#' doc <- add_slide(doc, "Title and Content", "Office Theme")
#' doc <- ph_with(doc, anyplot, location = ph_location_fullsize())
#' fileout <- tempfile(fileext = ".pptx")
#' # fileout <- "vg.pptx"
#' print(doc, target = fileout)
#' @export
#' @seealso \code{\link{ph_with.dml}}
dml <- function(
  code,
  ggobj = NULL,
  bg = "white",
  fonts = list(),
  pointsize = 12,
  editable = TRUE,
  ...
) {
  out <- list()
  out$code <- enquo(code)
  out$ggobj <- ggobj
  out$bg <- bg
  out$fonts <- fonts
  out$pointsize <- pointsize
  out$editable <- editable
  class(out) <- "dml"
  return(out)
}

#' @importFrom xml2 xml_find_first as_xml_document
#' @importFrom officer ph_location ph_location_type
#' @importFrom officer ph_with fortify_location
#' @importFrom rlang eval_tidy enquo
#' @export
#' @method ph_with dml
#' @title add a plot output as vector graphics into a PowerPoint object
#' @description Produces a vector graphics output from R plot instructions
#' stored in a \code{\link{dml}} object and adds the result in an \code{rpptx}
#' object produced by \code{\link[officer]{read_pptx}}.
#' @param x a pptx device
#' @param value \code{\link{dml}} object
#' @param location a location for a placeholder.
#' @param ... Arguments to be passed to methods
#' @examples
#' anyplot <- dml(code = barplot(1:5, col = 2:6), bg = "wheat")
#'
#' library(officer)
#' doc <- read_pptx()
#' doc <- add_slide(doc, "Title and Content", "Office Theme")
#' doc <- ph_with(doc, anyplot, location = ph_location_fullsize())
#'
#' fileout <- tempfile(fileext = ".pptx")
#' print(doc, target = fileout)
ph_with.dml <- function(x, value, location, ...) {
  img_directory <- tempfile()
  dir.create(img_directory, recursive = TRUE, showWarnings = FALSE)

  dml_file <- tempfile()

  pars <- list(...)

  loc <- fortify_location(location, doc = x)

  pars$offx <- loc$left
  pars$offy <- loc$top
  pars$width <- loc$width
  pars$height <- loc$height

  pars$bg <- value$bg
  pars$fonts <- value$fonts
  pars$pointsize <- value$pointsize
  pars$editable <- value$editable

  pars$file <- dml_file
  pars$id <- 0L
  pars$last_rel_id <- 1
  pars$raster_prefix <- paste0(img_directory, "/raster-")
  pars$standalone <- FALSE

  do.call("dml_pptx", pars)

  tryCatch(
    {
      if (!is.null(value$ggobj)) {
        stopifnot(inherits(value$ggobj, "ggplot"))
        print(value$ggobj)
      } else {
        eval_tidy(value$code)
      }
    },
    finally = dev.off()
  )

  dml_str <- scan(
    dml_file,
    what = "character",
    quiet = T,
    sep = "\n",
    encoding = "UTF-8"
  )

  if (dml_str == "</p:grpSp>") {
    stop(
      "There was no plot output produced, cannot add an empty plot to pptx document."
    )
  }

  dml_str <- paste(dml_str, collapse = "")
  ph_with(
    x = x,
    value = xml2::as_xml_document(dml_str),
    location = location,
    ...
  )
}
