#' @title DrawingML graphic device for Microsoft Word
#'
#' @description
#' Graphics devices for Microsoft Word DrawingML format.
#'
#' @param file DrawingML file.
#' @param height,width Height and width in inches.
#' @param bg Default background color for the plot (defaults to "white").
#' @param fonts Named list of font names to be aliased with
#'   fonts installed on your system. If unspecified, the R default
#'   families \code{sans}, \code{serif}, \code{mono} and \code{symbol}
#'   are aliased to the family returned by \code{\link[gdtools]{match_family}()}.
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
                     fonts = list(),
                     pointsize = 12, editable = TRUE,
                     id = 1L,
                     next_rels_id = 1L,
                     raster_prefix = "raster_", standalone = TRUE ) {

  system_fonts <- validate_fonts( fonts )

  invisible(DOCX_(file, bg, width, height,
                  pointsize = pointsize,
                  aliases = list(system = system_fonts, user = list()),
                  editable = editable, id = id, raster_prefix = raster_prefix,
                  next_rels_id = next_rels_id, standalone = standalone
  ))
}

