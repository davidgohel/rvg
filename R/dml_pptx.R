#' @title DrawingML graphic device for Microsoft PowerPoint
#'
#' @description
#' Graphics devices for Microsoft PowerPoint DrawingML format.
#'
#' @param file the file where output will appear.
#' @param height,width Height and width in inches.
#' @param offx,offy top and left origin of the plot
#' @param bg Default background color for the plot (defaults to "white").
#' @param fontname_serif,fontname_sans,fontname_mono,fontname_symbol font names for font faces
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
#' @examples
#' dml_pptx( file = tempfile() )
#' plot(1:11,(-5:5)^2, type='b', main="Simple Example")
#' dev.off()
#' @keywords device
#' @export
#' @export
dml_pptx <- function(file = "Rplots.dml", width = 10, height = 8,
                     offx = 1, offy = 1,
                     bg = "white",
                     fontname_serif = getOption("rvg_fonts")$fontname_serif,
                     fontname_sans = getOption("rvg_fonts")$fontname_sans,
                     fontname_mono = getOption("rvg_fonts")$fontname_mono,
                     fontname_symbol = getOption("rvg_fonts")$fontname_symbol,
                     pointsize = 12, editable = TRUE,
                     id = 1L,
                     next_rels_id = 1L,
                     raster_prefix = "raster_" ) {
  invisible(PPTX_(file, bg, width, height, offx = offx, offy = offy,
              pointsize = pointsize, fontname_serif = fontname_serif,
              fontname_sans = fontname_sans, fontname_mono = fontname_mono,
              fontname_symbol = fontname_symbol,
              editable = editable, id = id, raster_prefix = raster_prefix,
              next_rels_id = next_rels_id
            ))
}

