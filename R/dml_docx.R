#' @title DrawingML graphic device for Microsoft Word
#'
#' @description
#' Graphics devices for Microsoft Word DrawingML format.
#'
#' @param file DrawingML file.
#' @param height,width Height and width in inches.
#' @param bg Default background color for the plot (defaults to "white").
#' @param fontname_serif,fontname_sans,fontname_mono,fontname_symbol font
#' names for font faces. Used fonts should be available in the operating system.
#' These arguments are deprecated in favor of the \code{fonts} argument.
#' @param fonts Named list of font names to be aliased with
#'   fonts installed on your system. If unspecified, the R default
#'   families \code{sans}, \code{serif}, \code{mono} and \code{symbol}
#'   are aliased to the family returned by \code{\link[gdtools]{match_family}()}.
#' @param pointsize default point size.
#' @param editable should vector graphics elements (points, text, etc.)
#' be editable.
#' @param id specifies a unique identifier (integer) within the document
#' that will contain the DrawingML instructions.
#' @param last_rel_id specifies the last unique identifier (integer)
#' within relationship file that will be used to reference embedded
#' raster images if any.
#' @param next_rels_id deprecated. Use last_rel_id instead.
#' @param raster_prefix string value used as prefix for png
#' files produced when raster objects are printed on the
#' graphical device.
#' @param standalone produce a standalone drawingml file? If \code{FALSE}, omits
#'   xml header and namespaces.
#' @note
#' Text rendering is not optimal, this device should not be considered as a
#' valid R graphical device.
#'
#' The DrawingML implementation for 'Microsoft Word' is different from standard
#' DrawingML particulary with text boxes. The major point is that the exact size
#' and position of text boxes cannot be exactly defined regarding to text widths
#' and heights.
#'
#' Autofit option has been set as a workaround, this moves text slightly on the produced graphic when
#' edited in 'Microsoft Word' but this makes sure the text can be read.
#' @examples
#' \donttest{
#' dml_docx( file = tempfile() )
#' plot(1:11,(-5:5)^2, type='b', main="Simple Example")
#' dev.off()
#' }
#' @keywords device
#' @seealso \code{\link{Devices}}
#' @export
dml_docx <- function(file = "Rplots.dml", width = 6, height = 6,
                     bg = "white",
                     fonts = list(),
                     pointsize = 12, editable = TRUE,
                     id = 1L,
                     last_rel_id = 1L,
                     next_rels_id,
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

  if(!missing(next_rels_id)){
    last_rel_id <- next_rels_id
  }

  system_fonts <- validate_fonts( fonts )

  invisible(DOCX_(file, bg, width, height,
                  pointsize = pointsize,
                  aliases = list(system = system_fonts, user = list()),
                  editable = editable, id = id, raster_prefix = raster_prefix,
                  last_rel_id = last_rel_id, standalone = standalone
  ))
}

