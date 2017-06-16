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


extract_group_sp <- function(dml_str){
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
  dml_str
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
#' @examples
#' \donttest{
#' library(officer)
#' doc <- read_pptx()
#' doc <- add_slide(doc, "Title and Content", "Office Theme")
#' doc <- ph_with_vg(doc, code = barplot(1:5, col = 2:6), type = "body")
#' doc <- add_slide(doc, "Title and Content", "Office Theme")
#' doc <- ph_with_vg_at(doc, code = barplot(1:5, col = 2:6),
#'   left = 1, top = 2, width = 6, height = 4)
#' print(doc, target = "vg.pptx")
#' }
ph_with_vg <- function( x, code, type, index = 1, ... ){
  stopifnot(inherits(x, "rpptx"))
  slide <- x$slide$get_slide(x$cursor)

  img_directory = get_img_dir()
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
  raster_files <- list_raster_files(img_dir = img_directory )
  dml_str <- scan( dml_file, what = "character", quiet = T, sep = "\n" )

  if( length(raster_files) ){
    slide$reference_img(src = raster_files, dir_name = file.path(x$package_dir, "ppt/media"))
    unlink(raster_files, force = TRUE)
  }

  dml_str <- extract_group_sp( dml_str )
  ph_from_xml(x = x, value = dml_str, type = type, index = index )
}



#' @export
#' @rdname ph_with_vg
#' @param left,top left and top origin of the plot on the slide in inches.
#' @param height,width Height and width in inches.
#' @importFrom officer ph_from_xml_at
ph_with_vg_at <- function( x, code, left, top, width, height, ... ){
  stopifnot(inherits(x, "rpptx"))
  slide <- x$slide$get_slide(x$cursor)

  img_directory = get_img_dir()
  dml_file <- tempfile()

  dml_pptx(file = dml_file, width = width,
           height = height, offx = left,
           offy = top, id = 0L, raster_prefix = img_directory, standalone = FALSE,
           last_rel_id = slide$relationship()$get_next_id() - 1, ...)

  tryCatch(code, finally = dev.off() )
  raster_files <- list_raster_files(img_dir = img_directory )
  dml_str <- scan( dml_file, what = "character", quiet = T, sep = "\n" )

  if( length(raster_files) ){
    slide$reference_img(src = raster_files, dir_name = file.path(x$package_dir, "ppt/media"))
    unlink(raster_files, force = TRUE)
  }

  dml_str <- extract_group_sp( dml_str )
  ph_from_xml_at(x = x, value = dml_str, left, top, width, height )
}
