#' @importFrom officer docx
#' @title Microsoft Word Graphics Device
#'
#' @description
#' A graphical device for Microsoft Word documents.
#' @param file filename of the Microsoft Word document to produce. File
#' extension must be \code{.docx}.
#' @param code Plotting code to execute
#' @param ... arguments for \code{fun} (passed on to \code{\link{dml_docx}}.)
#' @examples
#' \donttest{
#' write_docx(file = "my_plot_1.docx", code = plot(rnorm(10)) )
#' write_docx(file = "my_plot_2.docx", code = barplot(1:7, col = 1:7))
#' }
#' @keywords device
#' @export
write_docx <- function( file, code, ...) {

  doc <- docx()
  doc <- docx_add_vgplot(doc, code = code, ...)
  print(doc, target = file )
}





#' @export
#' @title vml graph code
#' @description produces the vml of a graph
#' @param x a docx device
#' @param code plot instructions
#' @param pos where to add the new element relative to the cursor,
#' one of "after", "before", "on".
#' @param ... arguments passed on to \code{\link{dml_docx}}.
#' @importFrom officer add_xml_node docx_reference_img wml_link_images
#' @importFrom xml2 xml_find_first as_xml_document
docx_add_vgplot <- function( x, code, pos = "after", ... ){

  uid <- basename(tempfile(pattern = ""))
  img_directory = file.path(getwd(), uid )
  dml_file <- tempfile()

  pars <- list(...)

  pars$file <- dml_file
  pars$id <- 0L
  pars$last_rel_id <- x$rels$get_next_id() - 1
  pars$raster_prefix <- img_directory
  pars$standalone <- FALSE

  do.call("dml_docx", pars)

  tryCatch(code, finally = dev.off() )
  raster_files <- list.files(path = getwd(), pattern = paste0("^", uid, "(.*)\\.png$"), full.names = TRUE )
  xml_elt <- scan( dml_file, what = "character", quiet = T, sep = "\n" )
  xml_elt <- gsub(pattern = "<w:drawing>",
                  replacement = paste0("<w:p ",
                                       "xmlns:w=\"http://schemas.openxmlformats.org/wordprocessingml/2006/main\" ",
                                       "xmlns:wp=\"http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing\" ",
                                       "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\" ",
                                       "xmlns:wps=\"http://schemas.microsoft.com/office/word/2010/wordprocessingShape\" ",
                                       "xmlns:pic=\"http://schemas.openxmlformats.org/drawingml/2006/picture\" ",
                                       "xmlns:wpg=\"http://schemas.microsoft.com/office/word/2010/wordprocessingGroup\">",
                                       "<w:pPr/><w:r>",
                                       "<w:drawing>"),
                  x = xml_elt )
  xml_elt <- paste0(xml_elt, "</w:r></w:p>")

  if( length(raster_files)){
    x <- docx_reference_img(x, raster_files)
    xml_elt <- wml_link_images( x, xml_elt )
    unlink(raster_files)
  }

  add_xml_node(x = x, str = xml_elt, pos = pos)

}


