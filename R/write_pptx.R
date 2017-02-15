#' @importFrom officer read_pptx add_slide
#' @title Microsoft PowerPoint Graphics Device
#'
#' @description
#' A graphical device for Microsoft PowerPoint documents.
#' @param file filename of the Microsoft PowerPoint document to produce. File
#' extension must be \code{.pptx}.
#' @param code Plotting code to execute
#' @param ... arguments passed on to \code{\link{dml_pptx}}.
#' @examples
#' \donttest{
#' write_pptx(file = "my_plot_1.pptx", code = plot(rnorm(10)))
#' write_pptx(file = "my_plot_2.pptx", code = barplot(1:7, col = 1:7))
#' }
#' @keywords device
#' @export
write_pptx <- function( file, code, ...) {

  doc <- read_pptx()
  doc <- add_slide(doc, layout = "Title and Content", master = "Office Theme")
  doc <- ph_with_vg(doc, type = "body", code = code, ... )
  print(doc, target = file )

  file
}


#' @export
#' @title pml graph code
#' @description produces the pml of a graph
#' @param x a pptx device
#' @param code plot instructions
#' @param type placeholder type
#' @param index placeholder index (integer). This is to be used when a placeholder type
#' is not unique in the current slide, e.g. two placeholders with type 'body'.
#' @param ... arguments passed on to \code{\link{dml_pptx}}.
#' @importFrom officer ph_from_xml
#' @importFrom xml2 xml_find_first as_xml_document
ph_with_vg <- function( x, code, type, index = 1, ... ){
  slide <- x$slide$get_slide(x$cursor)

  uid <- basename(tempfile(pattern = ""))
  img_directory = file.path(getwd(), uid )
  dml_file <- tempfile()

  id_xfrm <- slide$get_xfrm(type = type, index = index)
  id_xfrm <- as.list(id_xfrm[c("cx", "cy", "offx", "offy")])
  names(id_xfrm) <- c("width", "height", "offx", "offy")

  pars <- list(...)
  add_named_args <- setdiff ( names(id_xfrm), names( pars ) )
  pars[add_named_args] <- id_xfrm[add_named_args]

  pars$file <- dml_file
  pars$id <- 0L
  pars$last_rel_id <- slide$relationship()$get_next_id() - 1
  pars$raster_prefix <- img_directory
  pars$standalone <- FALSE

  do.call("dml_pptx", pars)

  tryCatch(code, finally = dev.off() )
  raster_files <- list.files(path = getwd(), pattern = paste0("^", uid, "(.*)\\.png$"), full.names = TRUE )

  dml_str <- scan( dml_file, what = "character", quiet = T, sep = "\n" )

  if( length(raster_files) ){
    slide$reference_img(src = raster_files, dir_name = file.path(x$package_dir, "ppt/media"))
    unlink(raster_files, force = TRUE)
  }

  dml_str <- gsub(pattern = "<p:spTree>",
                  replacement = paste0("<p:spTree xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" ",
                                       "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" ",
                                       "xmlns:p=\"http://schemas.openxmlformats.org/presentationml/2006/main\" ",
                                       "xmlns:pic=\"http://schemas.openxmlformats.org/drawingml/2006/picture\">"),
                  x = dml_str )
  dml_str <- as.character( xml_find_first( as_xml_document(dml_str), "//p:grpSp" ) )
  dml_str <- gsub(pattern = "<p:grpSp>",
                  replacement = paste0("<p:grpSp xmlns:a=\"http://schemas.openxmlformats.org/drawingml/2006/main\" ",
                                       "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" ",
                                       "xmlns:p=\"http://schemas.openxmlformats.org/presentationml/2006/main\" ",
                                       "xmlns:pic=\"http://schemas.openxmlformats.org/drawingml/2006/picture\">"),
                  x = dml_str )
  ph_from_xml(x = x, value = dml_str, type = type, index = index )
}
