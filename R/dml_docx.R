#' @importFrom gdtools font_family_exists
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
#' @param standalone produce a standalone drawingml file? If \code{FALSE}, omits
#'   xml header and namespaces.
#' @examples
#' dml_docx( file = tempfile() )
#' plot(1:11,(-5:5)^2, type='b', main="Simple Example")
#' dev.off()
#' @keywords device
#' @seealso \code{\link{Devices}}, \code{\link{dml_docx}}, \code{\link{dsvg}}
#' @export
dml_docx <- function(file = "Rplots.dml", width = 6, height = 6,
                     bg = "white",
                     fontname_serif = "Times New Roman",
                     fontname_sans = "Calibri",
                     fontname_mono = "Courier New",
                     fontname_symbol = "Symbol",
                     pointsize = 12, editable = TRUE,
                     id = 1L,
                     next_rels_id = 1L,
                     raster_prefix = "raster_", standalone = TRUE ) {

  if( !font_family_exists(font_family = fontname_serif) )
    warning("'serif' font ", shQuote(fontname_serif), " can not be found")
  if( !font_family_exists(font_family = fontname_sans) )
    warning("'sans' font ", shQuote(fontname_sans), " can not be found")
  if( !font_family_exists(font_family = fontname_mono) )
    warning("'mono' font ", shQuote(fontname_mono), " can not be found")
  if( !font_family_exists(font_family = fontname_symbol) )
    warning("'symbol' font ", shQuote(fontname_symbol), " can not be found")

  invisible(DOCX_(file, bg, width, height,
                  pointsize = pointsize, fontname_serif = fontname_serif,
                  fontname_sans = fontname_sans, fontname_mono = fontname_mono,
                  fontname_symbol = fontname_symbol,
                  editable = editable, id = id, raster_prefix = raster_prefix,
                  next_rels_id = next_rels_id, standalone = standalone
  ))
}

