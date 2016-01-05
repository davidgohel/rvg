#' @title SVG Graphics Driver
#'
#' @description This function produces SVG files (compliant to the current w3 svg XML standard)
#' where elements can be made interactive.
#'
#' @details Graphic elements are made interactive with \code{jQuery} code. To embed
#' an interactive svg file into a web page, check that \code{jQuery} javascript file
#' is included.
#'
#' @param file the file where output will appear.
#' @param height,width Height and width in inches.
#' @param bg Default background color for the plot (defaults to "white").
#' @param pointsize default point size.
#' @param standalone Produce a stand alone svg file? If \code{FALSE}, omits
#'   xml header and default namespace.
#' @param canvas_id svg id within HTML page.
#' @param fontname_serif,fontname_sans,fontname_mono,fontname_symbol font
#' names for font faces
#' @seealso \code{\link{Devices}}, \code{\link{dml_docx}}, \code{\link{dml_pptx}}
#' @examples
#' dsvg()
#' plot(rnorm(10), main="Simple Example", xlab = "", ylab = "")
#' dev.off()
#' @keywords device
#' @useDynLib rvg
#' @importFrom Rcpp sourceCpp
#' @importFrom gdtools raster_view
#' @export
dsvg <- function(file = "Rplots.svg", width = 10, height = 8, bg = "white",
                pointsize = 12, standalone = TRUE, canvas_id = 1,
                fontname_serif = "Times New Roman",
                fontname_sans = "Calibri",
                fontname_mono = "Courier New",
                fontname_symbol = "Symbol" ) {

  if( !font_family_exists(font_family = fontname_serif) )
    warning("'serif' font ", shQuote(fontname_serif), " can not be found")
  if( !font_family_exists(font_family = fontname_sans) )
    warning("'sans' font ", shQuote(fontname_sans), " can not be found")
  if( !font_family_exists(font_family = fontname_mono) )
    warning("'mono' font ", shQuote(fontname_mono), " can not be found")
  if( !font_family_exists(font_family = fontname_symbol) )
    warning("'symbol' font ", shQuote(fontname_symbol), " can not be found")

  invisible(DSVG_(file=file, width=width, height=height, bg=bg, pointsize=pointsize, standalone=standalone,
                  canvas_id=canvas_id,
                  fontname_serif = fontname_serif,
                  fontname_sans = fontname_sans,
                  fontname_mono = fontname_mono,
                  fontname_symbol = fontname_symbol)
            )
}

