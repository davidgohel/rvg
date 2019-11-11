#' @title DrawingML graphic device for Microsoft PowerPoint
#'
#' @description
#' Graphics devices for Microsoft PowerPoint DrawingML format.
#'
#' @param file the file where output will appear.
#' @param height,width Height and width in inches.
#' @param offx,offy top and left origin of the plot
#' @param bg Default background color for the plot (defaults to "white").
#' @param fonts Named list of font names to be aliased with
#'   fonts installed on your system. If unspecified, the R default
#'   families \code{sans}, \code{serif}, \code{mono} and \code{symbol}
#'   are aliased to the family returned by \code{\link[gdtools]{match_family}()}.
#'
#' When you use specific fonts, you will need that font installed on your system.
#' This can be check with package `gdtools` and function `gdtools::font_family_exists()`.
#'
#' An example: `list(sans = "Roboto", serif = "Times", mono = "Courier")`.
#'
#' @param pointsize default point size.
#' @param editable should vector graphics elements (points, text, etc.) be editable.
#' @param id specifies a unique identifier (integer) within the slide
#' that will contain the DrawingML instructions.
#' @param last_rel_id specifies the last unique identifier (integer)
#' within relationship file that will be used to reference embedded
#' raster images if any.
#' @param raster_prefix string value used as prefix for png
#' files produced when raster objects are printed on the
#' graphical device.
#' @param standalone produce a standalone drawingml file? If \code{FALSE}, omits
#'   xml header and namespaces.
#' @examples
#' \donttest{
#' dml_pptx( file = tempfile() )
#' plot(1:11,(-5:5)^2, type='b', main="Simple Example")
#' dev.off()
#' }
#' @keywords device
#' @seealso \code{\link{Devices}}
#' @export
dml_pptx <- function(file = "Rplots.dml", width = 6, height = 6,
                     offx = 1, offy = 1,
                     bg = "white",
                     fonts = list(),
                     pointsize = 12, editable = TRUE,
                     id = 1L, last_rel_id = 1L,
                     raster_prefix = "raster_", standalone = TRUE ) {

  system_fonts <- validate_fonts( fonts )
  invisible(PPTX_(file, bg, width, height, offx = offx, offy = offy,
              pointsize = pointsize,
              aliases = list(system = system_fonts, user = list()),
              editable = editable, id = id, raster_prefix = raster_prefix,
              last_rel_id = last_rel_id, standalone = standalone
            ))
}

