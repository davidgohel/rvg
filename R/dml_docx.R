#' A Microsoft Word DrawingML file
#'
#' @param file the file where output will appear.
#' @param height,width Height and width in inches.
#' @param bg Default background color for the plot (defaults to "white").
#' @param fontname_serif,fontname_sans,fontname_mono,fontname_symbol font names for font faces
#' @param pointsize default point size.
#' @param init_id this id must will be used as the first unique id of graphical elements
#' to produce within a word document.
#' @param editable should vector graphics elements (points, text, etc.) be editable.
#' @examples
#' dml_docx( file = tempfile() )
#' plot(1:11,(-5:5)^2, type='b', main="Simple Example")
#' dev.off()
#' @keywords device
#' @export
#' @export
dml_docx <- function(file = "Rplots.dml", width = 10, height = 8, bg = "white",
                 fontname_serif = getOption("rvg_fonts")$fontname_serif,
                 fontname_sans = getOption("rvg_fonts")$fontname_sans,
                 fontname_mono = getOption("rvg_fonts")$fontname_mono,
                 fontname_symbol = getOption("rvg_fonts")$fontname_symbol,
                 pointsize = 12, init_id = 1, editable = TRUE ) {

  invisible(devDML_(file, bg, width, height, offx = 0, offy = 0,
          pointsize = pointsize, fontname_serif = fontname_serif,
          fontname_sans = fontname_sans, fontname_mono = fontname_mono,
          fontname_symbol = fontname_symbol,
          type = "wps", editable = editable, id = init_id ))
}
