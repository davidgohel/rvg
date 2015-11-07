#' A Microsoft PowerPoint DrawingML file
#'
#' @param file the file where output will appear.
#' @param height,width Height and width in inches.
#' @param offx,offy top and left origin of the plot
#' @param bg Default background color for the plot (defaults to "white").
#' @param fontname_serif,fontname_sans,fontname_mono,fontname_symbol font names for font faces
#' @param pointsize default point size.
#' @param editable should vector graphics elements (points, text, etc.) be editable.
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
                     pointsize = 12, editable = TRUE ) {

  invisible(devDML_(file, bg, width, height, offx = offx, offy = offy,
                    pointsize = pointsize, fontname_serif = fontname_serif,
                    fontname_sans = fontname_sans, fontname_mono = fontname_mono,
                    fontname_symbol = fontname_symbol,
                    type = "p", editable = editable, id = 0 ))
}
