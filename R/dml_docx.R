#' @title DrawingML graphic device for Microsoft Word
#'
#' @description
#' Graphics devices for Microsoft Word DrawingML format.
#'
#' @param file DrawingML file.
#' @param height,width Height and width in inches.
#' @param bg Default background color for the plot (defaults to "white").
#' @param fontname_serif,fontname_sans,fontname_mono,fontname_symbol font
#' names for font faces
#' @param pointsize default point size.
#' @param editable should vector graphics elements (points, text, etc.)
#' be editable.
#' @param id specifies a unique identifier (integer) within the document
#' that will contain the DrawingML instructions.
#' @param next_rels_id specifies the next unique identifier (integer)
#' within relationship file that will be used to reference embedded
#' raster images if any.
#' @param raster_prefix string value used as prefix for png
#' files produced when raster objects are printed on the
#' graphical device.
#' @examples
#' dml_docx( file = tempfile() )
#' plot(1:11,(-5:5)^2, type='b', main="Simple Example")
#' dev.off()
#' @keywords device
#' @export
#' @export
dml_docx <- function(file = "Rplots.dml", width = 10, height = 8,
                     bg = "white",
                     fontname_serif = getOption("rvg_fonts")$fontname_serif,
                     fontname_sans = getOption("rvg_fonts")$fontname_sans,
                     fontname_mono = getOption("rvg_fonts")$fontname_mono,
                     fontname_symbol = getOption("rvg_fonts")$fontname_symbol,
                     pointsize = 12, editable = TRUE,
                     id = 1L,
                     next_rels_id = 1L,
                     raster_prefix = "raster_" ) {

  invisible(DOCX_(file, bg, width, height,
                  pointsize = pointsize, fontname_serif = fontname_serif,
                  fontname_sans = fontname_sans, fontname_mono = fontname_mono,
                  fontname_symbol = fontname_symbol,
                  editable = editable, id = id, raster_prefix = raster_prefix,
                  next_rels_id = next_rels_id
  ))
}

