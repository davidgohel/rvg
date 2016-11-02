#' @title DrawingML graphic device for Microsoft PowerPoint
#'
#' @description
#' Graphics devices for Microsoft PowerPoint DrawingML format.
#'
#' @param file the file where output will appear.
#' @param height,width Height and width in inches.
#' @param offx,offy top and left origin of the plot
#' @param bg Default background color for the plot (defaults to "white").
#' @param fontname_serif,fontname_sans,fontname_mono,fontname_symbol font
#' names for font faces. Used fonts should be available in the operating system.
#' These arguments are deprecated in favor of the \code{fonts} argument.
#' @param fonts Named list of font names to be aliased with
#'   fonts installed on your system. If unspecified, the R default
#'   families \code{sans}, \code{serif}, \code{mono} and \code{symbol}
#'   are aliased to the family returned by \code{\link[gdtools]{match_family}()}.
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
#' dml_pptx( file = tempfile() )
#' plot(1:11,(-5:5)^2, type='b', main="Simple Example")
#' dev.off()
#' @keywords device
#' @seealso \code{\link{Devices}}, \code{\link{dml_docx}}, \code{\link{dsvg}}
#' @export
dml_pptx <- function(file = "Rplots.dml", width = 6, height = 6,
                     offx = 1, offy = 1,
                     bg = "white",
                     fonts = list(),
                     pointsize = 12, editable = TRUE,
                     id = 1L,
                     next_rels_id = 1L,
                     raster_prefix = "raster_", standalone = TRUE,
                     fontname_serif = NULL,
                     fontname_sans = NULL,
                     fontname_mono = NULL,
                     fontname_symbol = NULL) {

  if( !is.null(fontname_serif) || !is.null(fontname_sans) ||
      !is.null(fontname_mono) || !is.null(fontname_symbol) ){
    warning("arguments fontname_(sans|serif|mono|symbol) are deprecated in favor of the fonts argument")
    fonts <- list()

    if( !is.null(fontname_serif) )
      fonts$serif <- fontname_serif
    if( !is.null(fontname_sans) )
      fonts$sans <- fontname_sans
    if( !is.null(fontname_mono) )
      fonts$mono <- fontname_mono
    if( !is.null(fontname_symbol) )
      fonts$symbol <- fontname_symbol
  }

  system_fonts <- validate_fonts( fonts )
  invisible(PPTX_(file, bg, width, height, offx = offx, offy = offy,
              pointsize = pointsize,
              aliases = list(system = system_fonts, user = list()),
              editable = editable, id = id, raster_prefix = raster_prefix,
              next_rels_id = next_rels_id, standalone = standalone
            ))
}

