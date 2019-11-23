get_img_dir <- function(){
  uid <- basename(tempfile(pattern = ""))
  img_directory = file.path(getwd(), uid )
  img_directory
}

list_raster_files <- function(img_dir){
  path_ <- dirname(img_dir)
  uid <- basename(img_dir)
  list.files(path = path_, pattern = paste0("^", uid, "(.*)\\.png$"), full.names = TRUE )
}


#' @title Wrap plot instructions for DrawingML plotting in Powerpoint
#' @description A simple wrapper to mark
#' the plot instructions as Vector Graphics instructions. It produces
#' an object of class 'dml' with a corresponding method \code{\link[officer]{ph_with}}.
#'
#' The function enable usage of any R plot with argument `code` and with
#' ggplot objects with argument `ggobj`.
#'
#' @param code plotting instructions
#' @param ggobj ggplot object to print. argument code will be ignored if this
#'   argument is supplied.
#' @param bg,fonts,pointsize,editable Parameters passed to \code{\link{dml_pptx}}
#' @param ... unused arguments
#' @examples
#' anyplot = dml(code = barplot(1:5, col = 2:6), bg = "wheat")
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
#' anyplot = dml(code = barplot(1:5, col = 2:6), bg = "wheat")
#'
#' library(officer)
#' doc <- read_pptx()
#' doc <- add_slide(doc, "Title and Content", "Office Theme")
#' doc <- ph_with(doc, anyplot, location = ph_location_fullsize())
#'
#' fileout <- tempfile(fileext = ".pptx")
#' print(doc, target = fileout)
ph_with.dml <- function( x, value, location, ... ){

  img_directory = get_img_dir()
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
  pars$raster_prefix <- img_directory
  pars$standalone <- FALSE

  do.call("dml_pptx", pars)

  tryCatch({
    if( !is.null(value$ggobj) ){
      stopifnot(inherits(value$ggobj, "ggplot"))
      print(value$ggobj)
    } else
      eval_tidy(value$code)
  }, finally = dev.off() )

  raster_files <- list_raster_files(img_dir = img_directory )
  dml_str <- scan( dml_file, what = "character", quiet = T, sep = "\n", encoding = "UTF-8" )
  if( length(raster_files) ){
    slide <- x$slide$get_slide(x$cursor)
    slide$reference_img(src = raster_files, dir_name = file.path(x$package_dir, "ppt/media"))
    unlink(raster_files, force = TRUE)
  }
  dml_str <- paste(dml_str, collapse = "")
  ph_with(x = x, value = xml2::as_xml_document(dml_str), location = location, ... )
}


#' @export
#' @title add a plot output as vector graphics into a PowerPoint object
#' @description produces a vector graphics output from R plot instructions
#' and add the result in a PowerPoint document object produced
#' by \code{\link[officer]{read_pptx}}.
#'
#' These functions will be deprecated and function \code{\link{ph_with.dml}}
#' should be used instead.
#' @param x an \code{rpptx} object produced by \code{officer::read_pptx}
#' @param code plot instructions
#' @param ggobj ggplot objet to print. argument \code{code} will
#' be ignored if this argument is supplied.
#' @param type placeholder type
#' @param index placeholder index (integer). This is to be used when a placeholder type
#' is not unique in the current slide, e.g. two placeholders with type 'body'.
#' @param ... arguments passed on to \code{\link{dml_pptx}}.
#' @importFrom xml2 xml_find_first as_xml_document
#' @importFrom officer ph_location ph_location_type
ph_with_vg <- function( x, code, ggobj = NULL, type = "body", index = 1, ... ){
  .Deprecated(new = "ph_with")
  stopifnot(inherits(x, "rpptx"))
  value <- dml(code = code, ggobj = ggobj, ...)
  ph_with(x, value, location = ph_location_type(type = type, id = index), ...)
}


#' @export
#' @rdname ph_with_vg
#' @param left,top left and top origin of the plot on the slide in inches.
#' @param height,width Height and width in inches.
ph_with_vg_at <- function( x, code, ggobj = NULL, left, top, width, height, ... ){
  .Deprecated(new = "ph_with")
  stopifnot(inherits(x, "rpptx"))
  value <- dml(code = code, ggobj = ggobj, ...)
  ph_with(x, value,
          location = ph_location(ph = "", label = "", left = left, top = top, width = width, height = height))

}
