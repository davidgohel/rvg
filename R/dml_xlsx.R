#' @title DrawingML graphic device for Microsoft Excel
#'
#' @description
#' Graphics devices for Microsoft Excel DrawingML format.
#'
#' @param file the file where output will appear.
#' @param height,width Height and width in inches.
#' @param offx,offy top and left origin of the plot
#' @param bg Default background color for the plot (defaults to "white").
#' @param fontname_serif,fontname_sans,fontname_mono,fontname_symbol font names for font faces.
#' Used fonts should be available in the operating system.
#' @param pointsize default point size.
#' @param editable should vector graphics elements (points, text, etc.) be editable.
#' @param id specifies a unique identifier (integer) within the slide
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
#' dml_xlsx( file = tempfile() )
#' plot(1:11,(-5:5)^2, type='b', main="Simple Example")
#' dev.off()
#' @keywords device
#' @seealso \code{\link{Devices}}, \code{\link{dml_docx}}, \code{\link{dml_pptx}}, \code{\link{dsvg}}
#' @export
dml_xlsx <- function(file = "Rplots.dml", width = 6, height = 6,
                     offx = 1, offy = 1,
                     bg = "white",
                     fontname_serif = "Times New Roman",
                     fontname_sans = "Calibri",
                     fontname_mono = "Courier New",
                     fontname_symbol = "Symbol",
                     pointsize = 12, editable = TRUE,
                     id = 1L,
                     next_rels_id = 1L,
                     raster_prefix = "raster_", standalone = TRUE ) {

    invisible(XLSX_(file, bg, width, height, offx = offx, offy = offy,
              pointsize = pointsize, fontname_serif = fontname_serif,
              fontname_sans = fontname_sans, fontname_mono = fontname_mono,
              fontname_symbol = fontname_symbol,
              editable = editable, id = id, raster_prefix = raster_prefix,
              next_rels_id = next_rels_id, standalone = standalone
            ))
}

