list_raster_files <- function(img_dir) {
  path_ <- dirname(img_dir)
  uid <- basename(img_dir)
  list.files(path = path_, pattern = paste0("^", uid, "(.*)\\.png$"), full.names = TRUE)
}


#' @title Wrap plot instructions for DrawingML plotting in Powerpoint
#' @description A simple wrapper to mark
#' the plot instructions as Vector Graphics instructions. It produces
#' an object of class 'dml' with a corresponding method \code{\link[officer]{ph_with}}.
#'
#' The function enable usage of any R plot with argument `code` and with
#' ggplot objects with argument `ggobj`.
#' @section background color:
#' When dealing with a ggplot object argument `bg` will have no effect
#' because ggplot theme is specifying background color, don't forget
#' to define the colors you want in the theme:
#'
#' ```
#' theme(
#'   panel.background = element_rect(fill = "#EFEFEF"),
#'   plot.background = element_rect(fill = "wheat"))
#' ```
#' @param code plotting instructions
#' @param ggobj ggplot object to print. argument code will be ignored if this
#'   argument is supplied.
#' @param bg,fonts,pointsize,editable Parameters passed to \code{\link{dml_pptx}}
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
dml <- function(code, ggobj = NULL,
                bg = "white", fonts = list(),
                pointsize = 12, editable = TRUE, ...) {
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
#' @title add a plot output as vector graphics into a PowerPoint object
#' @description produces a vector graphics output from R plot instructions
#' stored in a \code{\link{dml}} object and add the result in an \code{rpptx}
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

  dml_str <- scan(dml_file, what = "character", quiet = T, sep = "\n", encoding = "UTF-8")

  if (dml_str == "</p:grpSp>") {
    stop("There was no plot output produced, can not add an empty plot to pptx document.")
  }

  dml_str <- paste(dml_str, collapse = "")
  ph_with(x = x, value = xml2::as_xml_document(dml_str), location = location, ...)
}
